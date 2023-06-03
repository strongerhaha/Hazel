#include "Sandbox2D.h"
#include"imgui/imgui.h"
#include<glm/gtc/matrix_transform.hpp>
#include<glm/gtc/type_ptr.hpp>

Sandbox2D::Sandbox2D()
	:Layer("Sandbox2D"), m_CameraController(1280.0f / 720.0f, true)
{

}

void Sandbox2D::OnUpdate(Hazel::Timestep ts)
{
	m_CameraController.OnUpdate(ts);
	Hazel::RenderCommand::SetClearColor({ 0.2f, 0.2f, 0.2f, 1 });
	Hazel::RenderCommand::Clear();


	Hazel::Renderer2D::BeginScene(m_CameraController.GetCamera());
	Hazel::Renderer2D::DrawQuad({ 0.5f,0.0f }, { 0.5f,0.5f }, {0.8f,0.5f,0.7f,1});
	Hazel::Renderer2D::DrawQuad({ -1.0f,0.0f }, { 1.8f,1.8f }, { 0.0f,0.5f,0.7f,1 });
	Hazel::Renderer2D::EndScene();


}

void Sandbox2D::OnImGuiRender()
{
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

	

}

void Sandbox2D::OnDetach()
{
}