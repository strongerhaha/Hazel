#pragma once
#include"glm/glm.hpp"
#include"ScenceCamera.h"//�̳���Camera
#include <string>
#include "ScriptableEntity.h"
namespace Hazel {


	struct TagComponent
	{
		std::string Tag;
		TagComponent() = default;
		TagComponent(const TagComponent&) = default;
		TagComponent(const std::string& tag)//&���ã��������µĿռ�ֱ��������ָ���������ݡ�
			:Tag(tag) {}
		
	};
	struct TransformComponent
	{
		glm::mat4 Transform = glm::mat4{ 1.0f };
		TransformComponent() = default;
		TransformComponent(const TransformComponent&) = default;
		TransformComponent(const glm::mat4& transform)//&���ã��������µĿռ�ֱ��������ָ���������ݡ�
			:Transform(transform) {}
		operator glm::mat4& () { return Transform; }
		operator const glm::mat4& () const { return Transform; }//���������(glm::mat4)ʵ��DoMath(transform);
	};

	struct SpriteRendererComponent
	{
		glm::vec4 Color{ 1.0f,1.0f ,1.0f ,1.0f };
		SpriteRendererComponent() = default;
		SpriteRendererComponent(const SpriteRendererComponent&) = default;
		SpriteRendererComponent(const glm::vec4 color)//&���ã��������µĿռ�ֱ��������ָ���������ݡ�
			:Color(color) {}
	};

	struct CameraComponent//��������Ǹ�Component system����Ķ�����
	{
		SceneCamera Camera;
		bool Primary = true;//�ǲ��ǳ�ʼ������ͷ
		bool FixedAspectRatio = false;
		CameraComponent() = default;
		CameraComponent(const CameraComponent&) = default;
		//CameraComponent(const glm::mat4 projection)//&���ã��������µĿռ�ֱ��������ָ���������ݡ�
			//:Camera(projection) {}//�����ɾ�˻ᵼ��m_SecondCamera.AddComponent<CameraComponent>(glm::ortho(-1.0f, 1.0f, -1.0f, 1.0f, -1.0f, 1.0f));������Ӳ���
	};
	struct NativeScriptComponent
	{
		ScriptableEntity* Instance = nullptr;

		ScriptableEntity* (*InstantiateScript)();
		void(*DestroyScript)(NativeScriptComponent*);
		//std::function<void(ScriptableEntity*)> OnCreateFunction;//��virtual�����ˡ�Scriptentity
		template<typename T>//bind��ģ�壬T
		void Bind()
		{
			InstantiateScript = []() {return static_cast<ScriptableEntity * >( new T()); };//CameraControllerǿ��תΪScriptableEntity  �������Ǽ̳й�ϵ
			DestroyScript = [](NativeScriptComponent* nsc) {delete nsc->Instance; nsc->Instance = nullptr; };
		}

	};
}