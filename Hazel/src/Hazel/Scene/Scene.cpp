#include"hzpch.h"
#include "Scene.h"
#include"Components.h"
#include"Hazel/Renderer/Renderer2D.h"
#include<glm/glm.hpp>
#include"Entity.h"
namespace Hazel {

	Scene::Scene()//��ʼ��
	{

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

	void Scene::DestroyEntity(Entity entity)
	{
		m_Registry.destroy(entity);
	}

	

	

	void Scene::OnUpdataRuntime(Timestep ts)//do rendering stuff
	{
		//update scripts
		{
			m_Registry.view<NativeScriptComponent>().each([=](auto entity, auto& nsc)//��ֵ�ʹ�÷�ʽ
				{
					if (!nsc.Instance)
					{
						nsc.Instance=nsc.InstantiateScript();//���÷��غ���
						nsc.Instance->m_Entity = Entity{ entity,this };//����Instance�����entity
						nsc.Instance->OnCreate();//����virtual
					}
					nsc.Instance->OnUpdate(ts);
				});
		}//ͨ��Component����
		Camera* mainCamera = nullptr;//*��ָָ��
		glm::mat4 cameraTransform;
		{
			auto view = m_Registry.view<TransformComponent, CameraComponent>();//�鿴������component
			for (auto entity : view)
			{
				auto [transform, camera] = view.get<TransformComponent, CameraComponent>(entity);//�󶨵�һ����
				if (camera.Primary)
				{
					mainCamera = &camera.Camera;//�ҵ���Ҫ��Camera��ʾ����
					cameraTransform = transform.GetTransform();
					break;
				}
			}
		}
		if (mainCamera)
		{
			Renderer2D::BeginScene(mainCamera->GetProjection(),cameraTransform);

			auto group = m_Registry.group<TransformComponent>(entt::get<SpriteRendererComponent>);//�����ǰ󶨵�һ��
			for (auto entity : group)
			{
				auto [transform, sprite] = group.get<TransformComponent, SpriteRendererComponent>(entity);//��ȡ����
				//Renderer::Submit(mesh, transform);//�ύ����Ⱦ���Ǳ�
				//Renderer2D::DrawQuad(transform.GetTransform(), sprite.Color);//ѭ����Ⱦ,�����еĶ�������
				Renderer2D::DrawSprite(transform.GetTransform(), sprite, (int)entity);//�µ���Ⱦ������entity��++�Ӷ�����
			}
			Renderer2D::EndScene();
		}
	}

	void Scene::OnUpdataEditor(Timestep ts, EditorCamera& camera)
	{

		Renderer2D::BeginScene(camera);

		auto group = m_Registry.group<TransformComponent>(entt::get<SpriteRendererComponent>);//�����ǰ󶨵�һ��
		for (auto entity : group)
		{
			auto [transform, sprite] = group.get<TransformComponent, SpriteRendererComponent>(entity);//��ȡ����
			//Renderer::Submit(mesh, transform);//�ύ����Ⱦ���Ǳ�
			//Renderer2D::DrawQuad(transform.GetTransform(), sprite.Color);//ѭ����Ⱦ,�����еĶ�������
			Renderer2D::DrawSprite(transform.GetTransform(), sprite, (int)entity);//�µ���Ⱦ������entity��++�Ӷ�����
		}
		Renderer2D::EndScene();

	}

	void Scene::OnViewportResize(uint32_t width, uint32_t height)
	{
		m_ViewportWidth = width;
		m_ViewportHeight = height;
		auto view = m_Registry.view<CameraComponent>();//�鿴��component
		for (auto entity : view)
		{
			auto& cameraComponent = view.get<CameraComponent>(entity);
			if (!cameraComponent.FixedAspectRatio)
			{
				cameraComponent.Camera.SetViewPortSize(width, height);//����ViewPortSize�����FixedAspectRatio=false�� ����true �Ͳ����ã��ᵼ�»�����Ρ�
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
	void Scene::OnComponentAdded(Entity entity, T& component)//�����component��ʱ�����һ������ĺ������ֱ��Ӧ
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
		component.Camera.SetViewPortSize(m_ViewportWidth, m_ViewportHeight);//��scene����ʵ��SetViewPortSize������������camera��ʱ�����һ�Ρ���Ϊ��friend class ���Կ���ֱ�ӵ��������˽�к���
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