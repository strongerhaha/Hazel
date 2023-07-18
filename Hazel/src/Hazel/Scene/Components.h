#pragma once
#include"glm/glm.hpp"
#include"ScenceCamera.h"//�̳���Camera
#include <glm/gtc/matrix_transform.hpp>
#include <string>
#include "ScriptableEntity.h"
#define GLM_ENABLE_EXPERIMENTAL
#include<glm/gtx/quaternion.hpp>
#include"Hazel/Renderer/Texture.h"
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
		glm::vec3 Translation = { 0.0f,0.0f,0.0f };
		glm::vec3 Rotation = { 0.0f,0.0f,0.0f };
		glm::vec3 Scale = { 1.0f,1.0f,1.0f };
		TransformComponent() = default;
		TransformComponent(const TransformComponent&) = default;
		TransformComponent(const glm::vec3& translation)//&���ã��������µĿռ�ֱ��������ָ���������ݡ�
			:Translation(translation) {}

		glm::mat4 GetTransform()const
		{
			glm::mat4 rotation = glm::toMat4(glm::quat(Rotation));
			return glm::translate(glm::mat4(1.0f), Translation) *
				rotation *
				glm:: scale(glm::mat4(1.0f), Scale);
		}
	};

	struct SpriteRendererComponent
	{
		glm::vec4 Color{ 1.0f,1.0f ,1.0f ,1.0f };
		Ref<Texture2D> Texture;
		float TilingFactor = 1.0f;
		SpriteRendererComponent() = default;
		SpriteRendererComponent(const SpriteRendererComponent&) = default;
		SpriteRendererComponent(const glm::vec4 color)//&���ã��������µĿռ�ֱ��������ָ���������ݡ�
			:Color(color) {}
	};
	
	struct CubeRendererComponent
	{
		glm::vec4 Color{ 1.0f,1.0f ,1.0f ,1.0f };
		bool Isq = true;
		CubeRendererComponent() = default;
		CubeRendererComponent(const CubeRendererComponent&) = default;
		CubeRendererComponent(const glm::vec4 color, bool isq = false)//&���ã��������µĿռ�ֱ��������ָ���������ݡ�
			:Color(color), Isq(isq) {}
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