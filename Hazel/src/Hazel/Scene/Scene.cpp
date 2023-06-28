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

	

	void Scene::OnUpdata(Timestep ts)//do rendering stuff
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
		Camera* mainCamera = nullptr;
		glm::mat4* cameraTransform = nullptr;
		{
			auto view = m_Registry.view<TransformComponent, CameraComponent>();//�鿴������component
			for (auto entity : view)
			{
				auto [transform, camera] = view.get<TransformComponent, CameraComponent>(entity);//�󶨵�һ����
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
				auto [transform, sprite] = group.get<TransformComponent, SpriteRendererComponent>(entity);//��ȡ����
				//Renderer::Submit(mesh, transform);//�ύ����Ⱦ���Ǳ�
				Renderer2D::DrawQuad(transform, sprite.Color);//ѭ����Ⱦ
			}
			Renderer2D::EndScene();
		}
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
				cameraComponent.Camera.SetViewPortSize(width, height);
			}
		}
	}

}