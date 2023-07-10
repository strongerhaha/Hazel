#include "EditorLayer.h"
#include"imgui/imgui.h"
#include"Hazel/Scene/ScenceSerializer.h"
#include<glm/gtc/matrix_transform.hpp>
#include<glm/gtc/type_ptr.hpp>
#include"Hazel/Utils/PlatformUtils.h"
#include"ImGuizmo/ImGuizmo.h"
#include"Hazel/Math/Math.h"
#include"Hazel/Renderer/Editorcamera.h"
#include<chrono>
namespace Hazel {
	EditorLayer::EditorLayer()
		:Layer("EditorLayer"), m_CameraController(1280.0f / 720.0f, true)
	{

	}

	void EditorLayer::OnUpdate(Timestep ts)
	{
		HZ_PROFILE_FUNCTION();
		if (FramebufferSpecification spec = m_Framebuffer->GetSpecification();
			m_ViewportSize.x > 0.0f && m_ViewportSize.y > 0.0f && // zero sized framebuffer is invalid
			(spec.Width != m_ViewportSize.x || spec.Height != m_ViewportSize.y))
		{
			m_Framebuffer->Resize((uint32_t)m_ViewportSize.x, (uint32_t)m_ViewportSize.y);
			m_CameraController.OnResize(m_ViewportSize.x, m_ViewportSize.y);
			m_EditorCamera.SetViewportSize(m_ViewportSize.x, m_ViewportSize.y);//��editorcamera�ı��СʱҲҪ��
			m_ActiveScene->OnViewportResize((uint32_t)m_ViewportSize.x, (uint32_t)m_ViewportSize.y);//resize ������SetViewPortSize
		}
		if(m_ViewportFocused)//���ѡ�в����ƶ�����ͷ
			m_CameraController.OnUpdate(ts);
		m_EditorCamera.OnUpdate(ts);

		Renderer2D::ResetStats();
		m_Framebuffer->Bind();//�󶨻�����֮�����л��Ķ������ử����framebuffer����
		RenderCommand::Clear();
		RenderCommand::SetClearColor({ 0.2f, 0.2f, 0.2f, 1 });//ȫ��attachment������

		//clear our entity id attachment to -1
		m_Framebuffer->ClearAttachment(1, -1);//index =1,�ڶ���framebuffer

		m_ActiveScene->OnUpdataEditor(ts,m_EditorCamera);//scene�ĸ���,Ҫ��beginscene����
		
		auto [mx, my] = ImGui::GetMousePos();
		mx -= m_ViewportBounds[0].x;
		my -= m_ViewportBounds[0].y;
		glm::vec2 viewportSize = m_ViewportSize;
		//glm::vec2 viewportSize = m_ViewportBounds[1] - m_ViewportBounds[0];
		my = viewportSize.y - my;//��Ϊopengl��0��0�������½ǡ�Ϊ�˶�Ӧopengl
		int mouseX = (int)mx;
		int mouseY = (int)my;
		if (mouseX >= 0 && mouseY >= 0 && mouseX < (int)viewportSize.x && mouseY < (int)viewportSize.y)
		{
			int pixelData =m_Framebuffer->ReadPixel(1, mouseX, mouseY);//ReadPixel����ȡattachment����Ķ��� �ڶ���framebufferҲ���ǣ�1
			HZ_CORE_WARN("Pixeldata={0},{1}", mouseX, mouseY);
			//HZ_CORE_WARN("Pixeldata={0}", pixelData);
			m_HoveredEntity = pixelData == -1 ? Entity() : Entity((entt::entity)pixelData, m_ActiveScene.get());//������ͣ����entity
		}
		m_Framebuffer->Unbind();

	}

	void EditorLayer::OnImGuiRender()
	{
		HZ_PROFILE_FUNCTION();
		static bool dockingEnabled = false;
		if (!dockingEnabled)
		{
			static bool dockspaceOpen = true;
			static bool opt_fullscreen_persistant = true;
			static ImGuiDockNodeFlags opt_flags = ImGuiDockNodeFlags_None;
			bool opt_fullscreen = opt_fullscreen_persistant;

			ImGuiWindowFlags window_flags = ImGuiWindowFlags_MenuBar | ImGuiWindowFlags_NoDocking;
			if (opt_fullscreen)
			{
				ImGuiViewport* viewport = ImGui::GetMainViewport();
				ImGui::SetNextWindowPos(viewport->Pos);
				ImGui::SetNextWindowSize(viewport->Size);
				ImGui::SetNextWindowViewport(viewport->ID);
				ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 0.0f);
				ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0.0f);
				window_flags |= ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove;
				window_flags |= ImGuiWindowFlags_NoBringToFrontOnFocus | ImGuiWindowFlags_NoNavFocus;
			}

			// When using ImGuiDockNodeFlags_PassthruDockspace, DockSpace() will render our background and handle the pass-thru hole, so we ask Begin() to not render a background.
			if (opt_flags & ImGuiDockNodeFlags_PassthruDockspace)
				window_flags |= ImGuiWindowFlags_NoBackground;

			ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0.0f, 0.0f));
			ImGui::Begin("DockSpace Demo", &dockspaceOpen, window_flags);
			ImGui::PopStyleVar();

			if (opt_fullscreen)
				ImGui::PopStyleVar(2);

			// Dockspace
			ImGuiIO& io = ImGui::GetIO();
			ImGuiStyle& style = ImGui::GetStyle();
			float minWinSizeX = style.WindowMinSize.x;
			style.WindowMinSize.x = 370.0f;
			if (io.ConfigFlags & ImGuiConfigFlags_DockingEnable)
			{
				ImGuiID dockspace_id = ImGui::GetID("MyDockspace");
				ImGui::DockSpace(dockspace_id, ImVec2(0.0f, 0.0f), opt_flags);
			}
			style.WindowMinSize.x = minWinSizeX;//����Ķ���������������˼
			if (ImGui::BeginMenuBar())
			{
				if (ImGui::BeginMenu("File"))
				{
					if (ImGui::MenuItem("New","Ctrl+N"))
					{
						NewScene();
					}
					if (ImGui::MenuItem("Open...","Ctrl+O"))
					{
						OpenScene();
					}
					if (ImGui::MenuItem("Save As...","Ctrl+Shift+S"))
					{
						SaveSceneAs();
					}

					if (ImGui::MenuItem("Exit")) Application::Get().Close();

					ImGui::EndMenu();
				}
				ImGui::EndMenuBar();
			}
			
		}

		m_SceneHierarchyPanel.OnImGuiRender();

		ImGui::Begin("status");

		std::string name = "None";
		if (m_HoveredEntity)
			name = m_HoveredEntity.GetComponent<TagComponent>().Tag;
		ImGui::Text("Hovered Entity: %s", name.c_str());

		auto stats = Renderer2D::GetStats();
		ImGui::Text("Renderer2D Stats:");
		ImGui::Text("Draw Calls:%d", stats.DrawCalls);//��ʾdrawcalls������ش���
		ImGui::Text("Quad:%d", stats.QuadCount);
		ImGui::Text("Vertices: %d", stats.GetTotalVertexCount());
		ImGui::Text("Indices: %d", stats.GetTotalIndexCount());
	

		ImGui::End();
		ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0, 0));//ȡ���߽�

		ImGui::Begin("Viewport");
		auto viewportOffset = ImGui::GetCursorPos();//0,24?Ϊɶ��tab bar ��С

		m_ViewportFocused = ImGui::IsWindowFocused();//�Ƿ�ѡ��
		m_ViewportHovered = ImGui::IsWindowHovered();//����Ƿ�������
		Application::Get().GetImGuiLayer()->BlockEvents(m_ViewportFocused&&m_ViewportHovered);//�����Viewport���ڻ������Ǹ������ֹͣ�Ǹ�e��event��ȡ

		ImVec2 viewportPanelSize=ImGui::GetContentRegionAvail();
		m_ViewportSize = { viewportPanelSize.x, viewportPanelSize.y };
		/*if (m_ViewportSize != *((glm::vec2*)&viewportPanelSize) && viewportPanelSize.x>0 && viewportPanelSize.y>0)
		{
			m_Framebuffer->Resize((uint32_t)viewportPanelSize.x, (uint32_t)viewportPanelSize.y);//����frambuffer�Ĵ�С
			m_ViewportSize = { viewportPanelSize.x,viewportPanelSize.y };
			m_CameraController.OnResize(viewportPanelSize.x, viewportPanelSize.y);//����������Ĵ�С
		}*/
		uint32_t textureID = m_Framebuffer->GetColorAttachmentRendererID();//�������ΪID���ؾ��У���ֵһ���������ǵĵ�ַ�ǲ�һ����,0/1��ͬ��colorbuffer���ĸ�
		ImGui::Image((void*)textureID, ImVec2{ m_ViewportSize.x, m_ViewportSize.y }, { 0, 1 }, { 1,0 });//��֡����imgui����,{1,0,0,1}���淴��
		
		auto windowSize = ImGui::GetWindowSize();//1041,876  ���ڴ�С
		ImVec2 minBound = ImGui::GetWindowPos();//viewport ���Ͻǵĵ�
		minBound.x += viewportOffset.x;
		minBound.y += viewportOffset.y;

		ImVec2 maxBound = { minBound.x + windowSize.x,minBound.y + windowSize.y };//���ķ�Χ=���Ͻǵĵ�+���ڴ�С
		m_ViewportBounds[0] = { minBound.x,minBound.y };
		m_ViewportBounds[1] = { maxBound.x,maxBound.y };


		//Gizmos
		Entity selectedEntity = m_SceneHierarchyPanel.GetSelectedEntity();
		if (selectedEntity&& m_GizmoType!=-1)
		{
			ImGuizmo::SetOrthographic(false);
			ImGuizmo::SetDrawlist();

			float windowWidth = (float)ImGui::GetWindowWidth();
			float windowHeight = (float)ImGui::GetWindowHeight();
			ImGuizmo::SetRect(ImGui::GetWindowPos().x, ImGui::GetWindowPos().y, windowWidth, windowHeight);

			// camera,��component��õ�runtimecamera
			// auto cameraEntity = m_ActiveScene->GetPrimaryCameraEntity();
			// const auto& camera = cameraEntity.GetComponent<CameraComponent>().Camera;
			// const glm::mat4& cameraProjection = camera.GetProjection();
			// glm::mat4 cameraView = glm::inverse(cameraEntity.GetComponent<TransformComponent>().GetTransform());
			
			// editor camera 
			const glm::mat4& cameraProjection = m_EditorCamera.GetProjection();
			glm::mat4 cameraView = m_EditorCamera.GetViewMatrix();


			auto& tc = selectedEntity.GetComponent<TransformComponent>();
			glm::mat4 transform = tc.GetTransform();

			//Snapping,һ�α�45.0f,
			bool snap = Input::IsKeyPressed(HZ_KEY_LEFT_CONTROL);
			float snapValue = 0.5f;//0.5f���� translation/scale
			if (m_GizmoType == ImGuizmo::OPERATION::ROTATE)
				snapValue = 45.0f;
			float snapValues[3] = { snapValue,snapValue ,snapValue };


			ImGuizmo::Manipulate(glm::value_ptr(cameraView), glm::value_ptr(cameraProjection),
				(ImGuizmo::OPERATION)m_GizmoType, ImGuizmo::LOCAL, glm::value_ptr(transform), nullptr, snap ? snapValues : nullptr);//��ʾ���Ը��ĵ���Щ������entity���ġ�ͨ��cameraView����cameraprojection�����λ��,snap�Ƿ���true˵�Ļ���snapValues����
		if (ImGuizmo::IsUsing())//ͨ����������λ��ת��
		{
			glm::vec3 translation, rotation, scale;
			Math::DecomposeTransform(transform, translation, rotation, scale);
			glm::vec3 deltaRotation = rotation - tc.Rotation;
			tc.Translation = translation;
			tc.Rotation += deltaRotation;
			tc.Scale = scale;

		}
		}
		
		ImGui::End();
		ImGui::PopStyleVar();
		ImGui::End();
	}

	void EditorLayer::OnAttach()
	{
		HZ_PROFILE_FUNCTION();
		m_Texture = (Texture2D::Create("asset/textures/awesomeface.png"));
		m_HazelTexture = (Texture2D::Create("asset/textures/container.jpg"));
		m_SheetTexture = (Texture2D::Create("asset/game/textures/spritesheet_objects.png"));

		//m_SheetTestTexture = SubTexture2D::CreteFromCoords(m_SheetTexture, { 0,0 }, {128,128});
		m_SheetTestTexture = SubTexture2D::CreteFromCoords(m_SheetTexture, { 0,0 }, { 128,128 }, { 1,2 });
	

		FramebufferSpecification fbSpec;
		fbSpec.Attachments = { FramebufferTextureFormat::RGBA8,FramebufferTextureFormat::RED_INTEGER,FramebufferTextureFormat::Depth };//����������framebuffer��RED_INTEGER������mouse piking
		fbSpec.Width = 1280;
		fbSpec.Height = 720;
		m_Framebuffer = Framebuffer::Create(fbSpec);//Framebuffer�Ĵ���

		m_ActiveScene = CreateRef<Scene>();
		m_EditorCamera = EditorCamera(30.0f,1.787f,0.1f,1000.0f);

#if 0
		//entity
		auto square=m_ActiveScene->CreateEntity("Square"); //entity belong to scene//����
		//��entt����entity��scene����ȥ���Լ�����һ���µ�api����entity
		square.AddComponent<SpriteRendererComponent>(glm::vec4{ 0.0f,1.0f,0.0f,1.0f });//�����ɫ
		//m_SquareEntity = square;//copy

		m_CameraEntity = m_ActiveScene->CreateEntity("Camera A");//z�����ﴴ�������������
		m_CameraEntity.AddComponent<CameraComponent>();

		m_SecondCamera = m_ActiveScene->CreateEntity("Camera B");
		auto& cc=m_SecondCamera.AddComponent<CameraComponent>();
		cc.Primary = false;

		class CameraController :public ScriptableEntity
		{
		public:
			void OnCreate()
			{
				auto& translation = GetComponent<TransformComponent>().Translation;
				translation.x = rand() % 10 - 5.0f;//���x����
				//GetComponent<TransformComponent>();
			}
			void OnDestroy()
			{

			}
			void OnUpdate(Timestep ts)//������д��ScriptableEntity��virtual���� ��������ԭ����
			{
				auto& translation = GetComponent<TransformComponent>().Translation;
				float speed = 5.0f;
				if (Input::IsKeyPressed(HZ_KEY_A))
					translation.x -= speed * ts;//transform���Ǹ������[3][0]��x��[3][1]��y,[3][2]��z [3][3]��w����͸��
				if (Input::IsKeyPressed(HZ_KEY_D))
					translation.x += speed * ts;//transform���Ǹ������
				if (Input::IsKeyPressed(HZ_KEY_W))
					translation.y += speed * ts;//transform���Ǹ������
				if (Input::IsKeyPressed(HZ_KEY_S))
					translation.y -= speed * ts;//transform���Ǹ������
			}
		};
		m_CameraEntity.AddComponent<NativeScriptComponent>().Bind<CameraController>();
		m_SecondCamera.AddComponent<NativeScriptComponent>().Bind<CameraController>();//�󶨿��ƣ�ͨ��Componentϵͳ������ͨ���ı�󶨸������Կ��Ƶ������
#endif // 0
		m_SceneHierarchyPanel.SetContext(m_ActiveScene);//Panel,�ѵ�ǰ��Scene������Ϊcontext��ͨ�ţ�����֮��ͨѶ��

	}

	void EditorLayer::OnDetach()
	{
	}

	void EditorLayer::OnEvent(Event& e)
	{
		m_CameraController.OnEvent(e);
		m_EditorCamera.OnEvent(e);
		EventDispatcher dispatcher(e);
		dispatcher.Dispatch<KeyPressedEvent>(HZ_BIND_EVENT_FN(EditorLayer::OnKeyPressed));

	}
	bool EditorLayer::OnKeyPressed(KeyPressedEvent& e)
	{
		if (e.GetRepeatCount() > 0)
			return false;
		bool control = Input::IsKeyPressed(HZ_KEY_LEFT_CONTROL) || Input::IsKeyPressed(HZ_KEY_RIGHT_CONTROL);
		bool shift = Input::IsKeyPressed(HZ_KEY_LEFT_SHIFT) || Input::IsKeyPressed(HZ_KEY_RIGHT_SHIFT);
		switch (e.GetKeyCode())
		{
		case HZ_KEY_N:
		{
			if (control)
				NewScene();
			break;
		}
		case HZ_KEY_O:
		{
			if (control)
				OpenScene();
			break;
		}
		case HZ_KEY_S:
		{
			if (control&&shift)
				SaveSceneAs();
			break;
		}
		case HZ_KEY_Q:
			m_GizmoType = -1;
			break;
		case HZ_KEY_E:
			m_GizmoType = ImGuizmo::OPERATION::TRANSLATE;
			break;
		case HZ_KEY_R:
			m_GizmoType = ImGuizmo::OPERATION::ROTATE;
			break;
		case HZ_KEY_T:
			m_GizmoType = ImGuizmo::OPERATION::SCALE;
			break;

		}

		return false;
	}

	void EditorLayer::NewScene()
	{
		m_ActiveScene = CreateRef<Scene>();
		m_ActiveScene->OnViewportResize((uint32_t)m_ViewportSize.x, (uint32_t)m_ViewportSize.y);
		m_SceneHierarchyPanel.SetContext(m_ActiveScene);
	}

	void EditorLayer::OpenScene()
	{
		std::string filepath = FileDialogs::OpenFile("Hazel Scene(*.hazel)\0*.hazel\0");//���ļ�����filer
		if (!filepath.empty())
		{
			m_ActiveScene = CreateRef<Scene>();
			m_ActiveScene->OnViewportResize((uint32_t)m_ViewportSize.x, (uint32_t)m_ViewportSize.y);
			m_SceneHierarchyPanel.SetContext(m_ActiveScene);
			SceneSerializer serializer(m_ActiveScene);
			serializer.Deserialize(filepath);//�ļ��������ǰscene�������ʲô����

		}
	}

	void EditorLayer::SaveSceneAs()
	{
		std::string filepath = FileDialogs::SaveFile("Hazel Scene(*.hazel)\0*.hazel\0");//���ļ�����filer
		if (!filepath.empty())
		{
			SceneSerializer serializer(m_ActiveScene);
			serializer.Serialize(filepath);//�ļ��������ǰscene�������ʲô����
		}
	}
}