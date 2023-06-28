#pragma once
#include"Hazel/Core/Core.h"

#include <glm/glm.hpp>
namespace Hazel {

	class Input
	{
	public:
		static bool IsKeyPressed(int keycode);
		static bool IsMouseButtonPressed(int button);
		 static float GetMouseX(); 
		 static float GetMouseY();
		
		 static glm::vec2 GetMousePosition();

	};
}