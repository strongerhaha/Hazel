#pragma once

#include "Core.h"

#include "Window.h"
#include "Hazel/Core/LayerStack.h"
#include "Hazel/Events/Event.h"
#include "Hazel/Events/ApplicationEvent.h"
#include"Hazel/ImGui/ImGuiLayer.h"
#include"Hazel/Renderer/OrthographicCamera.h"
#include"Hazel/Renderer/Buffer.h"
#include"Hazel/Renderer/Shader.h"
#include"Hazel/Renderer/VertexArray.h"
#include"Hazel/Core/Timestep.h"
namespace Hazel {

	class HAZEL_API Application
	{
	public:
		Application(const std::string& name="Hazel APP");
		virtual ~Application();

		void OnEvent(Event& e);

		void PushLayer(Layer* layer);
		void PushOverlay(Layer* layer);
		void Run();
		inline Window& GetWindow() { return *m_Window; }
		void Close();
		inline static Application& Get() { return *s_Instance; }
	private:
		
		bool OnWindowClose(WindowCloseEvent& e);
		bool OnWindowResize(WindowResizeEvent& e);
	private:

		std::unique_ptr<Window> m_Window;

		ImGuiLayer* m_ImGuiLayer;
		bool m_Running = true,m_Minimized=false;
		LayerStack m_LayerStack;
		Timestep m_Timestep;
		float m_LastFrameTime = 0.0f;

	private:
		static Application* s_Instance;
	};

	// To be defined in CLIENT
	Application* CreateApplication();

}