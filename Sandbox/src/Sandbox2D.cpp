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

	
	{
	HZ_PROFILE_SCOPE("Renderer Prep");
	Hazel::RenderCommand::SetClearColor({ 0.2f, 0.2f, 0.2f, 1 });
	Hazel::RenderCommand::Clear();

	}

	{
		
		HZ_PROFILE_SCOPE("Renderer Draw");
		Hazel::Renderer2D::BeginScene(m_CameraController.GetCamera());
		Hazel::Renderer2D::DrawQuad({ -1.0f,0.0f }, { m_SquareColor .x,m_SquareColor .y}, { 0.0f,0.5f,0.9f,1.0f });
		Hazel::Renderer2D::DrawQuad({ 1.0f,-1.0f }, { 1.8f,1.8f }, m_HazelTexture,1.0f, { 0.0f,0.5f,0.9f,1.0f });
		Hazel::Renderer2D::EndScene();
	}

}

void Sandbox2D::OnImGuiRender()
{
	HZ_PROFILE_FUNCTION();
	ImGui::Begin("Settings");
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
