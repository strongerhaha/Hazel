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

	Scene::Scene()//初始化
	{
#if ENTT_EXAMPLE_CODE
		entt::entity entity = m_Registry.create();//创建仓库
		m_Registry.emplace<TransformComponent>(entity,glm::mat4(1.0f));//模板，类似vector

		m_Registry.on_construct<TransformComponent>().connect<&OnTransformConstruct>();//每次创建TransformComponent都会调用OnTransformConstruct

		if (m_Registry.has<TransformComponent>(entity))//判断是不是有这个TransformComponent
			TransformComponent& transform = m_Registry.get<TransformComponent>(entity);

		auto view = m_Registry.view<TransformComponent>();
		for (auto entity : view)
		{
			TransformComponent& transform = view.get< TransformComponent>(entity);
		}
		auto group = m_Registry.group<TransformComponent>(entt::get<MeshComponent>);//把他们绑定到一起
		for (auto entiy : group)
		{
			auto& [transform,mesh] = group.get<TransformComponent,MeshComponent>(entity);//提取数据
			//Renderer::Submit(mesh, transform);//提交到渲染器那边
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
		auto group = m_Registry.group<TransformComponent>(entt::get<SpriteRendererComponent>);//把他们绑定到一起
		for (auto entity : group)
		{
			auto& [transform, sprite] = group.get<TransformComponent, SpriteRendererComponent>(entity);//提取数据
			//Renderer::Submit(mesh, transform);//提交到渲染器那边
			Renderer2D::DrawQuad(transform, sprite.Color);
		}
	}

}