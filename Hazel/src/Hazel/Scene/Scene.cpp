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

	void Scene::DestroyEntity(Entity entity)
	{
		m_Registry.destroy(entity);
	}

	

	

	void Scene::OnUpdataRuntime(Timestep ts)//do rendering stuff
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
		Camera* mainCamera = nullptr;//*是指指针
		glm::mat4 cameraTransform;
		{
			auto view = m_Registry.view<TransformComponent, CameraComponent>();//查看这两个component
			for (auto entity : view)
			{
				auto [transform, camera] = view.get<TransformComponent, CameraComponent>(entity);//绑定到一起了
				if (camera.Primary)
				{
					mainCamera = &camera.Camera;//找到主要的Camera显示画面
					cameraTransform = transform.GetTransform();
					break;
				}
			}
		}
		if (mainCamera)
		{
			Renderer2D::BeginScene(mainCamera->GetProjection(),cameraTransform);

			auto group = m_Registry.group<TransformComponent>(entt::get<SpriteRendererComponent>);//把他们绑定到一起
			for (auto entity : group)
			{
				auto [transform, sprite] = group.get<TransformComponent, SpriteRendererComponent>(entity);//提取数据
				//Renderer::Submit(mesh, transform);//提交到渲染器那边
				//Renderer2D::DrawQuad(transform.GetTransform(), sprite.Color);//循环渲染,里面有的都画出来
				Renderer2D::DrawSprite(transform.GetTransform(), sprite, (int)entity);//新的渲染函数，entity会++从而区分
			}
			Renderer2D::EndScene();
		}
	}

	void Scene::OnUpdataEditor(Timestep ts, EditorCamera& camera)
	{

		Renderer2D::BeginScene(camera);

		auto group = m_Registry.group<TransformComponent>(entt::get<SpriteRendererComponent>);//把他们绑定到一起
		for (auto entity : group)
		{
			auto [transform, sprite] = group.get<TransformComponent, SpriteRendererComponent>(entity);//提取数据
			//Renderer::Submit(mesh, transform);//提交到渲染器那边
			//Renderer2D::DrawQuad(transform.GetTransform(), sprite.Color);//循环渲染,里面有的都画出来
			Renderer2D::DrawSprite(transform.GetTransform(), sprite, (int)entity);//新的渲染函数，entity会++从而区分
		}
		Renderer2D::EndScene();

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
				cameraComponent.Camera.SetViewPortSize(width, height);//重置ViewPortSize，如果FixedAspectRatio=false。 等于true 就不重置，会导致画面变形。
			}
		}
	}
	Entity Scene::GetPrimaryCameraEntity()
	{
		auto view = m_Registry.view<CameraComponent>();
		for (auto entity : view)
		{
			const auto& camera = view.get<CameraComponent>(entity);
			if (camera.Primary)
				return Entity{ entity,this };
		}
		return {};
	}

	template<typename T>
	void Scene::OnComponentAdded(Entity entity, T& component)//在添加component的时候调用一次这里的函数，分别对应
	{
		static_assert(false);
	}
	template<>
	void Scene::OnComponentAdded<TransformComponent>(Entity entity, TransformComponent& component)
	{

	}
	template<>
	void Scene::OnComponentAdded<CameraComponent>(Entity entity, CameraComponent& component)
	{
		component.Camera.SetViewPortSize(m_ViewportWidth, m_ViewportHeight);//在scene这里实现SetViewPortSize函数，达成添加camera的时候调用一次。因为是friend class 所以可以直接调用这里的私有函数
	}
	template<>
	void Scene::OnComponentAdded<SpriteRendererComponent>(Entity entity, SpriteRendererComponent& component)
	{

	}
	template<>
	void Scene::OnComponentAdded<TagComponent>(Entity entity, TagComponent& component)
	{

	}
	template<>
	void Scene::OnComponentAdded<NativeScriptComponent>(Entity entity, NativeScriptComponent& component)
	{

	}

}