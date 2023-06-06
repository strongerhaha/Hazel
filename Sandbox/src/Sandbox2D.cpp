#include "Sandbox2D.h"
#include"imgui/imgui.h"
#include<glm/gtc/matrix_transform.hpp>
#include<glm/gtc/type_ptr.hpp>
#include<chrono>


Sandbox2D::Sandbox2D()
	:Layer("Sandbox2D"), m_CameraController(1280.0f / 720.0f, true)
{

}

void Sandbox2D::OnUpdate(Hazel::Timestep ts)
{
	HZ_PROFILE_FUNCTION();

	m_CameraController.OnUpdate(ts);
	Hazel::Renderer2D::ResetStats();
	
	{
	HZ_PROFILE_SCOPE("Renderer Prep");
	Hazel::RenderCommand::SetClearColor({ 0.2f, 0.2f, 0.2f, 1 });
	Hazel::RenderCommand::Clear();

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
		Hazel::Renderer2D::DrawRotationQuad({ -1.0f,0.0f }, { 0.5f,0.5f }, 45.0f, { 0.0f,0.5f,0.9f,1.0f });
		Hazel::Renderer2D::DrawQuad({ -1.0f,0.0f }, { 0.5f,0.5f }, { 0.0f,0.5f,0.9f,1.0f });
		Hazel::Renderer2D::DrawRotationQuad({ 1.0f,-1.0f }, { 0.5f,0.5f }, 45.0f, m_HazelTexture,1.0f);
		Hazel::Renderer2D::DrawQuad({ 1.0f,-1.0f }, { 0.5f,0.5f }, m_HazelTexture,1.0f, { 0.0f,0.5f,0.9f,1.0f });
		Hazel::Renderer2D::EndScene();

		

		
	}

}

void Sandbox2D::OnImGuiRender()
{
	HZ_PROFILE_FUNCTION();

	ImGui::Begin("Settings");
	auto stats = Hazel::Renderer2D::GetStats();	
	ImGui::Text("Renderer2D Stats:");
	ImGui::Text("Draw Calls:%d", stats.DrawCalls);
	ImGui::Text("Quad:%d", stats.QuadCount);
	ImGui::Text("Vertices: %d", stats.GetTotalVertexCount());
	ImGui::Text("Indices: %d", stats.GetTotalIndexCount());


	ImGui::ColorEdit4("Square Color", glm::value_ptr(m_SquareColor));


	ImGui::End();
}

void Sandbox2D::OnEvent(Hazel::Event& e)
{
	m_CameraController.OnEvent(e);
}

void Sandbox2D::OnAttach()
{
	HZ_PROFILE_FUNCTION();
	m_Texture = (Hazel::Texture2D::Create("asset/textures/awesomeface.png"));
	m_HazelTexture = (Hazel::Texture2D::Create("asset/textures/container.jpg"));
}

void Sandbox2D::OnDetach()
{
}
