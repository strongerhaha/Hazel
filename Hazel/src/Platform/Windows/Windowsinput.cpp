#include"hzpch.h"

#include"Hazel/Core/input.h"
#include<GLFW/glfw3.h>
#include"Hazel/Core/Application.h"
namespace Hazel {
	
	bool Input::IsKeyPressed(int keycode)
	{
		auto window = static_cast<GLFWwindow*> (Application::Get().GetWindow().GetNativeWindow());

		auto state = glfwGetKey(window, keycode);
		return state == GLFW_PRESS || state == GLFW_REPEAT;
	}

	bool Input::IsMouseButtonPressed(int button)
	{
		auto window = static_cast<GLFWwindow*> (Application::Get().GetWindow().GetNativeWindow());
		auto state = glfwGetMouseButton(window, button);

		return state == GLFW_PRESS ;
	}

	glm::vec2 Input::GetMousePosition()
	{

		auto* window = static_cast<GLFWwindow*> (Application::Get().GetWindow().GetNativeWindow());
		double xpos, ypos;

		glfwGetCursorPos(window, &xpos, &ypos);

		return { (float)xpos,(float)ypos };
	}

	float Input::GetMouseX()
	{
		return GetMousePosition().x;
		
	}

	float Input::GetMouseY()
	{
		
		return GetMousePosition().y;
	}





}