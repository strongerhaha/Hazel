#pragma once
#include"Core.h"
#include"Events/Event.h"
namespace Hazel {
	class HAZEL_API Application
	{
	public:
		Application();
		~Application();
		void Run();
		
	};
	Application* CreateApplication();
}


