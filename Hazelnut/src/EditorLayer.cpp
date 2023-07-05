#include "EditorLayer.h"
#include"Hazel/Scene/ScenceSerializer.h"
#include"imgui/imgui.h"
#include<glm/gtc/matrix_transform.hpp>
#include<glm/gtc/type_ptr.hpp>
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
			m_ActiveScene->OnViewportResize((uint32_t)m_ViewportSize.x, (uint32_t)m_ViewportSize.y);//resize ������SetViewPortSize
		}
		if(m_ViewportFocused)//���ѡ�в����ƶ�����ͷ
			m_CameraController.OnUpdate(ts);

		Renderer2D::ResetStats();
		m_Framebuffer->Bind();//�󶨻�����֮�����л��Ķ������ử����framebuffer����
		RenderCommand::Clear();
		RenderCommand::SetClearColor({ 0.2f, 0.2f, 0.2f, 1 });
		m_ActiveScene->OnUpdata(ts);//scene�ĸ���,Ҫ��beginscene����
		
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
					if (ImGui::MenuItem("Serialize"))
					{
						SceneSerializer serializer(m_ActiveScene);
						serializer.Serialize("asset/scenes/Example.hazel");//�ļ��������ǰscene�������ʲô����
					}
					if (ImGui::MenuItem("Deserialize"))
					{
						SceneSerializer serializer(m_ActiveScene);
						serializer.Deserialize("asset/scenes/Example.hazel");//�ļ��������ǰscene�������ʲô����
					}

					if (ImGui::MenuItem("Exit")) Application::Get().Close();

					ImGui::EndMenu();
				}
				ImGui::EndMenuBar();
			}
			
		}

		m_SceneHierarchyPanel.OnImGuiRender();

		ImGui::Begin("status");
		auto stats = Renderer2D::GetStats();
		ImGui::Text("Renderer2D Stats:");
		ImGui::Text("Draw Calls:%d", stats.DrawCalls);//��ʾdrawcalls������ش���
		ImGui::Text("Quad:%d", stats.QuadCount);
		ImGui::Text("Vertices: %d", stats.GetTotalVertexCount());
		ImGui::Text("Indices: %d", stats.GetTotalIndexCount());
	

		ImGui::End();
		ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0, 0));//ȡ���߽�

		ImGui::Begin("Viewport");
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
		uint32_t textureID = m_Framebuffer->GetColorAttachment();//�������ΪID���ؾ��У���ֵһ���������ǵĵ�ַ�ǲ�һ����
		ImGui::Image((void*)textureID, ImVec2{ m_ViewportSize.x, m_ViewportSize.y }, { 0, 1 }, { 1,0 });//��֡����imgui����,{1,0,0,1}���淴��
		ImGui::End();
		ImGui::PopStyleVar();
		ImGui::End();
	}

	void EditorLayer::OnEvent(Event& e)
	{
		m_CameraController.OnEvent(e);
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
		fbSpec.Width = 1280;
		fbSpec.Height = 720;
		m_Framebuffer = Framebuffer::Create(fbSpec);//Framebuffer�Ĵ���

		m_ActiveScene = CreateRef<Scene>();
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
}