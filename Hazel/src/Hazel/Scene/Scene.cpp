#include"hzpch.h"
#include "Scene.h"
#include"Components.h"
#include"Hazel/Renderer/Renderer2D.h"
#include<glm/glm.hpp>
#include"Entity.h"
namespace Hazel {
	static void DoMath(const glm::mat4& transform)
	{

	}
	static void OnTransformConstruct(entt::registry& registry, entt::entity entity)
	{

	}

	Scene::Scene()//��ʼ��
	{
#if ENTT_EXAMPLE_CODE
		entt::entity entity = m_Registry.create();//�����ֿ�
		m_Registry.emplace<TransformComponent>(entity,glm::mat4(1.0f));//ģ�壬����vector

		m_Registry.on_construct<TransformComponent>().connect<&OnTransformConstruct>();//ÿ�δ���TransformComponent�������OnTransformConstruct

		if (m_Registry.has<TransformComponent>(entity))//�ж��ǲ��������TransformComponent
			TransformComponent& transform = m_Registry.get<TransformComponent>(entity);

		auto view = m_Registry.view<TransformComponent>();
		for (auto entity : view)
		{
			TransformComponent& transform = view.get< TransformComponent>(entity);
		}
		auto group = m_Registry.group<TransformComponent>(entt::get<MeshComponent>);//�����ǰ󶨵�һ��
		for (auto entiy : group)
		{
			auto& [transform,mesh] = group.get<TransformComponent,MeshComponent>(entity);//��ȡ����
			//Renderer::Submit(mesh, transform);//�ύ����Ⱦ���Ǳ�
		}
#endif
	}
		
	Scene::~Scene()
	{
	}

	Entity Scene::CreateEntity(const std::string& name)
	{
		Entity entity= { m_Registry.create(),this };//�õ�ǰ��this sence����һ��Entity��m_Registry.create()���������һ��entt����entity
		entity.AddComponent<TransformComponent>();//���Component ÿ��������entity�������
		auto& Tag = entity.AddComponent<TagComponent>();
		Tag.Tag = name.empty() ? "Entity" : name;
		return entity;
	}

	

	void Scene::OnUpdata(Timestep ts)//do rendering stuff
	{
		Camera* mainCamera = nullptr;
		glm::mat4* cameraTransform = nullptr;
		{
			auto group = m_Registry.view<TransformComponent, CameraComponent>();//�鿴������component
			for (auto entity : group)
			{
				auto& [transform, camera] = group.get<TransformComponent, CameraComponent>(entity);//�󶨵�һ����
				if (camera.Primary)
				{
					mainCamera = &camera.Camera;//�ҵ���Ҫ��Camera��ʾ����
					cameraTransform = &transform.Transform;
					break;
				}
			}
		}
		if (mainCamera)
		{
			Renderer2D::BeginScene(mainCamera->GetProjection(),*cameraTransform);

			auto group = m_Registry.group<TransformComponent>(entt::get<SpriteRendererComponent>);//�����ǰ󶨵�һ��
			for (auto entity : group)
			{
				auto& [transform, sprite] = group.get<TransformComponent, SpriteRendererComponent>(entity);//��ȡ����
				//Renderer::Submit(mesh, transform);//�ύ����Ⱦ���Ǳ�
				Renderer2D::DrawQuad(transform, sprite.Color);//ѭ����Ⱦ
			}
			Renderer2D::EndScene();
		}
	}

}