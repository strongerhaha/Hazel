#include <Hazel.h>
#include "Hazel/Core/EntryPoint.h"

#include"EditorLayer.h"

namespace Hazel {

	class Hazelnut : public Hazel::Application
	{
	public:
		Hazelnut()
			:Application("Hazelnut")
		{
			PushLayer(new EditorLayer());//�����Layer���ȥ
		}

		~Hazelnut()
		{

		}

	};

	Application* CreateApplication()
	{
		return new Hazelnut();
	}
}
