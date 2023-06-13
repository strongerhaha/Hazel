#include"hzpch.h"
#include "Scene.h"
#include"Components.h"
#include"Hazel/Renderer/Renderer2D.h"
#include<glm/glm.hpp>
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

	entt::entity Scene::CreateEntity()
	{
		return m_Registry.create();
	}

	void Scene::OnUpdata(Timestep ts)//do rendering stuff
	{
		auto group = m_Registry.group<TransformComponent>(entt::get<SpriteRendererComponent>);//�����ǰ󶨵�һ��
		for (auto entity : group)
		{
			auto& [transform, sprite] = group.get<TransformComponent, SpriteRendererComponent>(entity);//��ȡ����
			//Renderer::Submit(mesh, transform);//�ύ����Ⱦ���Ǳ�
			Renderer2D::DrawQuad(transform, sprite.Color);
		}
	}

}