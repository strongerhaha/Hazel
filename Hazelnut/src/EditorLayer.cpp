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
			m_ActiveScene->OnViewportResize((uint32_t)m_ViewportSize.x, (uint32_t)m_ViewportSize.y);//resize 里面有SetViewPortSize
		}
		if(m_ViewportFocused)//如果选中才能移动摄像头
			m_CameraController.OnUpdate(ts);

		Renderer2D::ResetStats();
		m_Framebuffer->Bind();//绑定画布，之后所有画的东西都会画在这framebuffer里面
		RenderCommand::Clear();
		RenderCommand::SetClearColor({ 0.2f, 0.2f, 0.2f, 1 });
		m_ActiveScene->OnUpdata(ts);//scene的更新,要在beginscene后面
		
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
			style.WindowMinSize.x = minWinSizeX;//后面的都会是这样？的意思
			if (ImGui::BeginMenuBar())
			{
				if (ImGui::BeginMenu("File"))
				{
					if (ImGui::MenuItem("Serialize"))
					{
						SceneSerializer serializer(m_ActiveScene);
						serializer.Serialize("asset/scenes/Example.hazel");//文件输出，当前scene的输出有什么东东
					}
					if (ImGui::MenuItem("Deserialize"))
					{
						SceneSerializer serializer(m_ActiveScene);
						serializer.Deserialize("asset/scenes/Example.hazel");//文件输出，当前scene的输出有什么东东
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
		ImGui::Text("Draw Calls:%d", stats.DrawCalls);//显示drawcalls个数相关窗口
		ImGui::Text("Quad:%d", stats.QuadCount);
		ImGui::Text("Vertices: %d", stats.GetTotalVertexCount());
		ImGui::Text("Indices: %d", stats.GetTotalIndexCount());
	

		ImGui::End();
		ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0, 0));//取消边界

		ImGui::Begin("Viewport");
		m_ViewportFocused = ImGui::IsWindowFocused();//是否选中
		m_ViewportHovered = ImGui::IsWindowHovered();//鼠标是否在上面
		Application::Get().GetImGuiLayer()->BlockEvents(m_ViewportFocused&&m_ViewportHovered);//如果在Viewport窗口或者在那个上面就停止那个e的event获取

		ImVec2 viewportPanelSize=ImGui::GetContentRegionAvail();
		m_ViewportSize = { viewportPanelSize.x, viewportPanelSize.y };
		/*if (m_ViewportSize != *((glm::vec2*)&viewportPanelSize) && viewportPanelSize.x>0 && viewportPanelSize.y>0)
		{
			m_Framebuffer->Resize((uint32_t)viewportPanelSize.x, (uint32_t)viewportPanelSize.y);//重置frambuffer的大小
			m_ViewportSize = { viewportPanelSize.x,viewportPanelSize.y };
			m_CameraController.OnResize(viewportPanelSize.x, viewportPanelSize.y);//重置摄像机的大小
		}*/
		uint32_t textureID = m_Framebuffer->GetColorAttachment();//把这个作为ID返回就行，数值一样但是他们的地址是不一样的
		ImGui::Image((void*)textureID, ImVec2{ m_ViewportSize.x, m_ViewportSize.y }, { 0, 1 }, { 1,0 });//将帧画在imgui里面,{1,0,0,1}画面反了
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
		m_Framebuffer = Framebuffer::Create(fbSpec);//Framebuffer的创建

		m_ActiveScene = CreateRef<Scene>();
#if 0
		//entity
		auto square=m_ActiveScene->CreateEntity("Square"); //entity belong to scene//创建
		//把entt：：entity和scene传进去，自己建立一个新的api控制entity
		square.AddComponent<SpriteRendererComponent>(glm::vec4{ 0.0f,1.0f,0.0f,1.0f });//添加颜色
		//m_SquareEntity = square;//copy

		m_CameraEntity = m_ActiveScene->CreateEntity("Camera A");//z在这里创建了照相机绑定了
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
				translation.x = rand() % 10 - 5.0f;//随机x坐标
				//GetComponent<TransformComponent>();
			}
			void OnDestroy()
			{

			}
			void OnUpdate(Timestep ts)//这里重写了ScriptableEntity的virtual函数 ，覆盖了原来的
			{
				auto& translation = GetComponent<TransformComponent>().Translation;
				float speed = 5.0f;
				if (Input::IsKeyPressed(HZ_KEY_A))
					translation.x -= speed * ts;//transform是那个矩阵吧[3][0]是x，[3][1]是y,[3][2]是z [3][3]是w用来透视
				if (Input::IsKeyPressed(HZ_KEY_D))
					translation.x += speed * ts;//transform是那个矩阵吧
				if (Input::IsKeyPressed(HZ_KEY_W))
					translation.y += speed * ts;//transform是那个矩阵吧
				if (Input::IsKeyPressed(HZ_KEY_S))
					translation.y -= speed * ts;//transform是那个矩阵吧
			}
		};
		m_CameraEntity.AddComponent<NativeScriptComponent>().Bind<CameraController>();
		m_SecondCamera.AddComponent<NativeScriptComponent>().Bind<CameraController>();//绑定控制，通过Component系统，可以通过改变绑定更换可以控制的摄像机
#endif // 0
		m_SceneHierarchyPanel.SetContext(m_ActiveScene);//Panel,把当前的Scene传入作为context，通信？程序之间通讯。

		

	}

	void EditorLayer::OnDetach()
	{
	}
}