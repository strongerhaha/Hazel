#include "EditorLayer.h"
#include"imgui/imgui.h"
#include<glm/gtc/matrix_transform.hpp>
#include<glm/gtc/type_ptr.hpp>

#include<chrono>
namespace Hazel {
	EditorLayer::EditorLayer()
		:Layer("EditorLayer"), m_CameraController(1280.0f / 720.0f, true), m_SquareColor({ 0.2f, 0.3f, 0.8f, 1.0f })
	{

	}

	void EditorLayer::OnUpdate(Hazel::Timestep ts)
	{
		HZ_PROFILE_FUNCTION();

		m_CameraController.OnUpdate(ts);
		Hazel::Renderer2D::ResetStats();

		{
			HZ_PROFILE_SCOPE("Renderer Prep");
			m_Framebuffer->Bind();//绑定画布，之后所有画的东西都会画在这framebuffer里面
			Hazel::RenderCommand::Clear();
			Hazel::RenderCommand::SetClearColor({ 0.2f, 0.2f, 0.2f, 1 });
		}
		{
			HZ_PROFILE_SCOPE("Renderer Draw");


			Hazel::Renderer2D::BeginScene(m_CameraController.GetCamera());

			for (float y = -5.0f; y < 5.0f; y += 0.5f)
			{
				for (float x = -5.0f; x < 5.0f; x += 0.5f)
				{
					glm::vec4 color = { (x + 5.0f) / 10.0f, 0.4f, (y + 5.0f) / 10.0f, 0.7f };
					Hazel::Renderer2D::DrawQuad({ x, y }, { 0.3f, 0.3f }, color);
				}
			}
			Hazel::Renderer2D::EndScene();

			Hazel::Renderer2D::BeginScene(m_CameraController.GetCamera());
			Hazel::Renderer2D::DrawRotationQuad({ -1.0f,0.0f }, { 0.5f,0.5f }, glm::radians(45.0f), { 0.0f,0.5f,0.9f,1.0f });
			Hazel::Renderer2D::DrawQuad({ -1.0f,0.0f }, { 0.5f,0.5f }, { 0.0f,0.5f,0.9f,1.0f });
			Hazel::Renderer2D::DrawRotationQuad({ 1.0f,-1.0f }, { 0.5f,0.5f }, glm::radians(45.0f), m_HazelTexture, 1.0f);
			Hazel::Renderer2D::DrawQuad({ 1.0f,-1.0f }, { 0.5f,0.5f }, m_HazelTexture, 1.0f, { 0.0f,0.5f,0.9f,1.0f });
			Hazel::Renderer2D::EndScene();
			Hazel::Renderer2D::BeginScene(m_CameraController.GetCamera());
			Hazel::Renderer2D::DrawQuad({ 0.0f,0.0f }, { 10.0f,10.0f }, m_SheetTestTexture, 1.0f, m_SquareColor);
			Hazel::Renderer2D::EndScene();

			m_Framebuffer->Unbind();
		}

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
			if (io.ConfigFlags & ImGuiConfigFlags_DockingEnable)
			{
				ImGuiID dockspace_id = ImGui::GetID("MyDockspace");
				ImGui::DockSpace(dockspace_id, ImVec2(0.0f, 0.0f), opt_flags);
			}


			if (ImGui::BeginMenuBar())
			{
				if (ImGui::BeginMenu("File"))
				{
					if (ImGui::MenuItem("Exit")) Hazel::Application::Get().Close();
					ImGui::EndMenu();
				}
				ImGui::EndMenuBar();
			}
			ImGui::End();
		}

		ImGui::Begin("Settings");
		auto stats = Hazel::Renderer2D::GetStats();
		ImGui::Text("Renderer2D Stats:");
		ImGui::Text("Draw Calls:%d", stats.DrawCalls);
		ImGui::Text("Quad:%d", stats.QuadCount);
		ImGui::Text("Vertices: %d", stats.GetTotalVertexCount());
		ImGui::Text("Indices: %d", stats.GetTotalIndexCount());
		ImGui::ColorEdit4("Square Color", glm::value_ptr(m_SquareColor));

		uint32_t textureID = m_Framebuffer->GetColorAttachment();//把这个作为ID返回就行，数值一样但是他们的地址是不一样的
		ImGui::Image((void*)textureID, ImVec2{ 1280.0f, 720.0f }, { 1,0 }, { 0, 1 });//将帧画在imgui里面
		ImGui::End();
	}

	void EditorLayer::OnEvent(Hazel::Event& e)
	{
		m_CameraController.OnEvent(e);
	}

	void EditorLayer::OnAttach()
	{
		HZ_PROFILE_FUNCTION();
		m_Texture = (Hazel::Texture2D::Create("asset/textures/awesomeface.png"));
		m_HazelTexture = (Hazel::Texture2D::Create("asset/textures/container.jpg"));
		m_SheetTexture = (Hazel::Texture2D::Create("asset/game/textures/spritesheet_objects.png"));

		//m_SheetTestTexture = Hazel::SubTexture2D::CreteFromCoords(m_SheetTexture, { 0,0 }, {128,128});
		m_SheetTestTexture = Hazel::SubTexture2D::CreteFromCoords(m_SheetTexture, { 0,0 }, { 128,128 }, { 1,2 });
	

		Hazel::FramebufferSpecification fbSpec;
		fbSpec.Width = 1280;
		fbSpec.Height = 720;
		m_Framebuffer = Hazel::Frambuffer::Create(fbSpec);

	}

	void EditorLayer::OnDetach()
	{
	}
}