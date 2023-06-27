#pragma once
#include"glm/glm.hpp"
#include"ScenceCamera.h"//继承了Camera
#include <string>
#include "ScriptableEntity.h"
namespace Hazel {


	struct TagComponent
	{
		std::string Tag;
		TagComponent() = default;
		TagComponent(const TagComponent&) = default;
		TagComponent(const std::string& tag)//&引用，不创建新的空间直接用它的指针会更改内容。
			:Tag(tag) {}
		
	};
	struct TransformComponent
	{
		glm::mat4 Transform = glm::mat4{ 1.0f };
		TransformComponent() = default;
		TransformComponent(const TransformComponent&) = default;
		TransformComponent(const glm::mat4& transform)//&引用，不创建新的空间直接用它的指针会更改内容。
			:Transform(transform) {}
		operator glm::mat4& () { return Transform; }
		operator const glm::mat4& () const { return Transform; }//重载运算符(glm::mat4)实现DoMath(transform);
	};

	struct SpriteRendererComponent
	{
		glm::vec4 Color{ 1.0f,1.0f ,1.0f ,1.0f };
		SpriteRendererComponent() = default;
		SpriteRendererComponent(const SpriteRendererComponent&) = default;
		SpriteRendererComponent(const glm::vec4 color)//&引用，不创建新的空间直接用它的指针会更改内容。
			:Color(color) {}
	};

	struct CameraComponent//改这里就是改Component system里面的东西。
	{
		SceneCamera Camera;
		bool Primary = true;//是不是初始的摄像头
		bool FixedAspectRatio = false;
		CameraComponent() = default;
		CameraComponent(const CameraComponent&) = default;
		//CameraComponent(const glm::mat4 projection)//&引用，不创建新的空间直接用它的指针会更改内容。
			//:Camera(projection) {}//把这个删了会导致m_SecondCamera.AddComponent<CameraComponent>(glm::ortho(-1.0f, 1.0f, -1.0f, 1.0f, -1.0f, 1.0f));不能添加参数
	};
	struct NativeScriptComponent
	{
		ScriptableEntity* Instance = nullptr;
		std::function<void()> InstantiateFunction;
		std::function<void()> DestroyInstanceFunction;
		std::function<void(ScriptableEntity*)> OnCreateFunction;
		std::function<void(ScriptableEntity*)> OnDestroyFunction;
		std::function<void(ScriptableEntity* ,Timestep)> OnUpdateFunction;
		template<typename T>//bind的模板，T
		void Bind()
		{
			InstantiateFunction = [&]() {Instance = new T(); };
			DestroyInstanceFunction = [&]() {delete (T*)Instance; Instance = nullptr; };
			OnCreateFunction = [](ScriptableEntity* instace) {((T*)instace)->OnCreate(); };
			OnDestroyFunction = [](ScriptableEntity* instace) {((T*)instace)->OnDestroy(); };
			OnUpdateFunction = [](ScriptableEntity* instace,Timestep ts) {((T*)instace)->OnUpdate(ts); };
		}

	};
}