#include"hzpch.h"
#include "Scene.h"
#include"Components.h"
#include"Hazel/Renderer/Renderer2D.h"
#include<glm/glm.hpp>
#include"Entity.h"
namespace Hazel {

	Scene::Scene()//初始化
	{

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
		//update scripts
		{
			m_Registry.view<NativeScriptComponent>().each([=](auto entity, auto& nsc)//奇怪的使用方式
				{
					if (!nsc.Instance)
					{
						nsc.Instance=nsc.InstantiateScript();//调用反回函数
						nsc.Instance->m_Entity = Entity{ entity,this };//设置Instance里面的entity
						nsc.Instance->OnCreate();//调用virtual
					}
					nsc.Instance->OnUpdate(ts);
				});
		}//通过Component控制
		Camera* mainCamera = nullptr;
		glm::mat4* cameraTransform = nullptr;
		{
			auto view = m_Registry.view<TransformComponent, CameraComponent>();//查看这两个component
			for (auto entity : view)
			{
				auto [transform, camera] = view.get<TransformComponent, CameraComponent>(entity);//绑定到一起了
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
				auto [transform, sprite] = group.get<TransformComponent, SpriteRendererComponent>(entity);//提取数据
				//Renderer::Submit(mesh, transform);//提交到渲染器那边
				Renderer2D::DrawQuad(transform, sprite.Color);//循环渲染
			}
			Renderer2D::EndScene();
		}
	}

	void Scene::OnViewportResize(uint32_t width, uint32_t height)
	{
		m_ViewportWidth = width;
		m_ViewportHeight = height;
		auto view = m_Registry.view<CameraComponent>();//查看这component
		for (auto entity : view)
		{
			auto& cameraComponent = view.get<CameraComponent>(entity);
			if (!cameraComponent.FixedAspectRatio)
			{
				cameraComponent.Camera.SetViewPortSize(width, height);
			}
		}
	}

}