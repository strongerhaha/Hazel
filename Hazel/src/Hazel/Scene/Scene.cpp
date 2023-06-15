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

	Entity Scene::CreateEntity(const std::string& name)
	{
		Entity entity= { m_Registry.create(),this };//用当前这this sence创建一个Entity，m_Registry.create()这个创建了一个entt：：entity
		entity.AddComponent<TransformComponent>();//添加Component 每个创建的entity都会添加
		auto& Tag = entity.AddComponent<TagComponent>();
		Tag.Tag = name.empty() ? "Entity" : name;
		return entity;
	}

	

	void Scene::OnUpdata(Timestep ts)//do rendering stuff
	{
		Camera* mainCamera = nullptr;
		glm::mat4* cameraTransform = nullptr;
		{
			auto group = m_Registry.view<TransformComponent, CameraComponent>();//查看这两个component
			for (auto entity : group)
			{
				auto& [transform, camera] = group.get<TransformComponent, CameraComponent>(entity);//绑定到一起了
				if (camera.Primary)
				{
					mainCamera = &camera.Camera;//找到主要的Camera显示画面
					cameraTransform = &transform.Transform;
					break;
				}
			}
		}
		if (mainCamera)
		{
			Renderer2D::BeginScene(mainCamera->GetProjection(),*cameraTransform);

			auto group = m_Registry.group<TransformComponent>(entt::get<SpriteRendererComponent>);//把他们绑定到一起
			for (auto entity : group)
			{
				auto& [transform, sprite] = group.get<TransformComponent, SpriteRendererComponent>(entity);//提取数据
				//Renderer::Submit(mesh, transform);//提交到渲染器那边
				Renderer2D::DrawQuad(transform, sprite.Color);//循环渲染
			}
			Renderer2D::EndScene();
		}
	}

}