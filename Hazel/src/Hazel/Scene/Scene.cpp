#include"hzpch.h"
#include "Scene.h"
#include"Components.h"
#include"Hazel/Renderer/Renderer2D.h"
#include<glm/glm.hpp>
#include"Entity.h"
#include "ScriptableEntity.h"

#include "box2d/b2_world.h"
#include "box2d/b2_body.h"
#include "box2d/b2_fixture.h"
#include "box2d/b2_polygon_shape.h"

namespace Hazel {

	static b2BodyType Rigidbody2DTypeToBox2DBody(Rigidbody2DComponent::BodyType bodyType)
	{
		switch (bodyType)
		{
		case Rigidbody2DComponent::BodyType::Static:    return b2_staticBody;
		case Rigidbody2DComponent::BodyType::Dynamic:   return b2_dynamicBody;
		case Rigidbody2DComponent::BodyType::Kinematic: return b2_kinematicBody;
		}

		HZ_CORE_ASSERT(false, "Unknown body type");
		return b2_staticBody;
	}

	Scene::Scene()//��ʼ��
	{

	}
		
	Scene::~Scene()
	{
	}

	template<typename Component>
	static void CopyComponent(entt::registry& dst, entt::registry& src, const std::unordered_map<UUID, entt::entity>& enttMap)//���û��޸�����
	{
		auto view = src.view<Component>();
		for (auto e : view)
		{
			UUID uuid = src.get<IDComponent>(e).ID;
			HZ_CORE_ASSERT(enttMap.find(uuid) != enttMap.end(),"");
			entt::entity dstEnttID = enttMap.at(uuid);//ʹ��enttMap��Ӧ����dstEnttID

			auto& component = src.get<Component>(e);
			dst.emplace_or_replace<Component>(dstEnttID, component);//���Component����Ӧ����
		}
	}

	template<typename Component>
	static void CopyComponentIfExists(Entity dst, Entity src)
	{
		if (src.HasComponent<Component>())
			dst.AddOrReplaceComponent<Component>(src.GetComponent<Component>());//���ڸ���entity��emplace_or_replace
	}

	Ref<Scene> Scene::Copy(Ref<Scene> other)//����Scene
	{
		Ref<Scene> newScene = CreateRef<Scene>();

		newScene->m_ViewportWidth = other->m_ViewportWidth;
		newScene->m_ViewportHeight = other->m_ViewportHeight;

		auto& srcSceneRegistry = other->m_Registry;
		auto& dstSceneRegistry = newScene->m_Registry;
		std::unordered_map<UUID, entt::entity> enttMap;//��Mapһһ��Ӧÿ��uuid

		// Create entities in new scene
		auto idView = srcSceneRegistry.view<IDComponent>();//�鿴����idComponent
		for (auto e : idView)
		{
			UUID uuid = srcSceneRegistry.get<IDComponent>(e).ID;
			const auto& name = srcSceneRegistry.get<TagComponent>(e).Tag;
			Entity newEntity = newScene->CreateEntityWithUUID(uuid, name);
			enttMap[uuid] = (entt::entity)newEntity;//ÿ��uuid��Ӧһ��Entity
		}

		// Copy components (except IDComponent and TagComponent)
		CopyComponent<TransformComponent>(dstSceneRegistry, srcSceneRegistry, enttMap);
		CopyComponent<SpriteRendererComponent>(dstSceneRegistry, srcSceneRegistry, enttMap);
		CopyComponent<CircleRendererComponent>(dstSceneRegistry, srcSceneRegistry, enttMap);
		CopyComponent<CameraComponent>(dstSceneRegistry, srcSceneRegistry, enttMap);
		CopyComponent<NativeScriptComponent>(dstSceneRegistry, srcSceneRegistry, enttMap);
		CopyComponent<Rigidbody2DComponent>(dstSceneRegistry, srcSceneRegistry, enttMap);
		CopyComponent<BoxCollider2DComponent>(dstSceneRegistry, srcSceneRegistry, enttMap);
		CopyComponent<CubeRendererComponent>(dstSceneRegistry, srcSceneRegistry, enttMap);
		CopyComponent<LightSystemComponent>(dstSceneRegistry, srcSceneRegistry, enttMap);
		return newScene;
	}
	Entity Scene::CreateEntity(const std::string& name)
	{
		return CreateEntityWithUUID(UUID(), name);
	}
	Entity Scene::CreateEntityWithUUID(UUID uuid, const std::string& name)
	{
		Entity entity = { m_Registry.create(),this };//�õ�ǰ��this sence����һ��Entity��m_Registry.create()���������һ��entt����entity
		entity.AddComponent<IDComponent>(uuid);//���IDComponent ÿ��������entity�������//��uuid���
		entity.AddComponent<TransformComponent>();//���TransformComponent ÿ��������entity�������

		auto& Tag = entity.AddComponent<TagComponent>();
		Tag.Tag = name.empty() ? "Entity" : name;
		return entity;
	}

	Entity Scene::CreateLightEntity(const std::string& name)
	{
		Entity entity = { m_Registry.create(),this };//�õ�ǰ��this sence����һ��Entity��m_Registry.create()���������һ��entt����entity
		entity.AddComponent<IDComponent>(UUID());//���IDComponent ÿ��������entity�������//��uuid���
		entity.AddComponent<TransformComponent>();//���TransformComponent ÿ��������entity�������
		entity.AddComponent<LightSystemComponent>();
		auto& Tag = entity.AddComponent<TagComponent>();
		Tag.Tag = name.empty() ? "Light System Entity" : name;
		return entity;
	}

	void Scene::DestroyEntity(Entity entity)
	{
		m_Registry.destroy(entity);
	}

	void Scene::OnRuntimeStart()
	{
		m_PhysicsWorld = new b2World({ 0.0f, -9.8f });

		auto view = m_Registry.view<Rigidbody2DComponent>();
		for (auto e : view)
		{
			Entity entity = { e, this };
			auto& transform = entity.GetComponent<TransformComponent>();
			auto& rb2d = entity.GetComponent<Rigidbody2DComponent>();

			b2BodyDef bodyDef;
			bodyDef.type = Rigidbody2DTypeToBox2DBody(rb2d.Type);
			bodyDef.position.Set(transform.Translation.x, transform.Translation.y);
			bodyDef.angle = transform.Rotation.z;

			b2Body* body = m_PhysicsWorld->CreateBody(&bodyDef);
			body->SetFixedRotation(rb2d.FixedRotation);
			rb2d.RuntimeBody = body;

			if (entity.HasComponent<BoxCollider2DComponent>())
			{
				auto& bc2d = entity.GetComponent<BoxCollider2DComponent>();

				b2PolygonShape boxShape;
				boxShape.SetAsBox(bc2d.Size.x * transform.Scale.x, bc2d.Size.y * transform.Scale.y);

				b2FixtureDef fixtureDef;
				fixtureDef.shape = &boxShape;
				fixtureDef.density = bc2d.Density;
				fixtureDef.friction = bc2d.Friction;
				fixtureDef.restitution = bc2d.Restitution;
				fixtureDef.restitutionThreshold = bc2d.RestitutionThreshold;
				body->CreateFixture(&fixtureDef);
			}
		}
	}

	void Scene::OnRuntimeStop()
	{
		delete m_PhysicsWorld;
		m_PhysicsWorld = nullptr;
	}
	

	

	void Scene::OnUpdateRuntime(Timestep ts)//do rendering stuff
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

	// Physics
		{
			const int32_t velocityIterations = 6;
			const int32_t positionIterations = 2;
			m_PhysicsWorld->Step(ts, velocityIterations, positionIterations);

			// Retrieve transform from Box2D
			auto view = m_Registry.view<Rigidbody2DComponent>();
			for (auto e : view)
			{
				Entity entity = { e, this };
				auto& transform = entity.GetComponent<TransformComponent>();
				auto& rb2d = entity.GetComponent<Rigidbody2DComponent>();
				
				b2Body* body = (b2Body*)rb2d.RuntimeBody;
				const auto& position = body->GetPosition();
				transform.Translation.x = position.x;
				transform.Translation.y = position.y;
				transform.Rotation.z = body->GetAngle();
			}
		}
		// Render 2D
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


			Renderer2D::BeginScene(mainCamera->GetProjection(),cameraTransform);//���������������projection��view
			
			auto group = m_Registry.group<TransformComponent>(entt::get<SpriteRendererComponent>);//�����ǰ󶨵�һ��
			for (auto entity : group)
			{
				auto [transform, sprite] = group.get<TransformComponent, SpriteRendererComponent>(entity);//��ȡ����
				//Renderer::Submit(mesh, transform);//�ύ����Ⱦ���Ǳ�
				//Renderer2D::DrawQuad(transform.GetTransform(), sprite.Color);//ѭ����Ⱦ,�����еĶ�������
				Renderer2D::DrawSprite(transform.GetTransform(), sprite, (int)entity);//�µ���Ⱦ������entity��++�Ӷ�����
			}

			
			auto view = m_Registry.view<TransformComponent, CircleRendererComponent>();	
			for (auto entity : view)
			{
				auto [transform, circle] = view.get<TransformComponent, CircleRendererComponent>(entity);

				Renderer2D::DrawCircle(transform.GetTransform(), circle.Color, circle.Thickness, circle.Fade, (int)entity);
			}
			
			auto view1 = m_Registry.view<TransformComponent, CubeRendererComponent>();
			for (auto entity : view1)
			{
				auto [transform, cube] = view1.get<TransformComponent, CubeRendererComponent>(entity);

				Renderer2D::DrawCube(transform.GetTransform(), cube, (int)entity);
			}
			Renderer2D::EndScene();
		}
	}

	void Scene::OnUpdateEditor(Timestep ts, EditorCamera& camera)
	{
		int index = 0;
		Renderer2D::BeginScene(camera);
	
		bool HasLight = false;
		glm::vec3 LightPos;	glm::vec3 LightColor;
		
		auto view2 = m_Registry.view<TransformComponent, LightSystemComponent>();
		for (auto entity : view2)
		{
			auto [transform, LightSystem] = view2.get<TransformComponent, LightSystemComponent>(entity);
			LightPos = LightSystem.LightPos;
			transform.Translation= LightSystem.LightPos;
			LightColor = LightSystem.LightColor;
			HasLight = true;
		}
		//auto group = m_Registry.group<TransformComponent>(entt::get<SpriteRendererComponent>);//�����ǰ󶨵�һ��
		auto group = m_Registry.view<TransformComponent, SpriteRendererComponent>();
		for (auto entity : group)
		{
			auto [transform, sprite] = group.get<TransformComponent, SpriteRendererComponent>(entity);//��ȡ����
			//Renderer2D::DrawSprite(transform.GetTransform(), sprite, (int)entity);//�µ���Ⱦ������entity��++�Ӷ����� 
			if (sprite.LightSwitch && HasLight)
				Renderer2D::DrawLightSprite(transform.GetTransform(), sprite, LightPos, LightColor, (int)entity);
			else
				Renderer2D::DrawSprite(transform.GetTransform(), sprite, (int)entity);//�µ���Ⱦ������entity��++�Ӷ����� 
		}
		// Draw circles
		
		auto view = m_Registry.view<TransformComponent, CircleRendererComponent>();
		for (auto entity : view)
		{
			auto [transform, circle] = view.get<TransformComponent, CircleRendererComponent>(entity);

			Renderer2D::DrawCircle(transform.GetTransform(), circle.Color, circle.Thickness, circle.Fade, (int)entity);
		}

		/*auto view1 = m_Registry.view<TransformComponent, CubeRendererComponent>();
		for (auto entity : view1)
		{
			auto [transform, cube] = view1.get<TransformComponent, CubeRendererComponent>(entity);

			Renderer2D::DrawCube(transform.GetTransform(), cube, (int)entity);
		}*/

		auto group2 = m_Registry.view<TransformComponent, CubeRendererComponent>();
		for (auto entity : group2)
		{
			auto [transform, cube] = group2.get<TransformComponent, CubeRendererComponent>(entity);//��ȡ����
			//Renderer2D::DrawSprite(transform.GetTransform(), sprite, (int)entity);//�µ���Ⱦ������entity��++�Ӷ����� 
			if (cube.LightSwitch && HasLight)
				Renderer2D::DrawLightCube(transform.GetTransform(), cube, LightPos, LightColor, (int)entity);
			else
				Renderer2D::DrawCube(transform.GetTransform(), cube, (int)entity);//�µ���Ⱦ������entity��++�Ӷ����� 
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
	void Scene::DuplicateEntity(Entity entity)//����entity
	{
		std::string name = entity.GetName();
		Entity newEntity = CreateEntity(name);

		CopyComponentIfExists<TransformComponent>(newEntity, entity);
		CopyComponentIfExists<SpriteRendererComponent>(newEntity, entity);
		CopyComponentIfExists<CameraComponent>(newEntity, entity);
		CopyComponentIfExists<CircleRendererComponent>(newEntity, entity);
		CopyComponentIfExists<NativeScriptComponent>(newEntity, entity);
		CopyComponentIfExists<Rigidbody2DComponent>(newEntity, entity);
		CopyComponentIfExists<BoxCollider2DComponent>(newEntity, entity);
		CopyComponentIfExists<CubeRendererComponent>(newEntity, entity);
		CopyComponentIfExists<LightSystemComponent>(newEntity, entity);
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
		if (m_ViewportWidth > 0 && m_ViewportHeight > 0)
			component.Camera.SetViewPortSize(m_ViewportWidth, m_ViewportHeight);
		//component.Camera.SetViewPortSize(m_ViewportWidth, m_ViewportHeight);//��scene����ʵ��SetViewPortSize������������camera��ʱ�����һ�Ρ���Ϊ��friend class ���Կ���ֱ�ӵ��������˽�к���
	}
	template<>
	void Scene::OnComponentAdded<SpriteRendererComponent>(Entity entity, SpriteRendererComponent& component)
	{

	}
	template<>
	void Scene::OnComponentAdded<CubeRendererComponent>(Entity entity, CubeRendererComponent& component)
	{

	}
	template<>
	void Scene::OnComponentAdded<IDComponent>(Entity entity, IDComponent& component)
	{

	}
	template<>
	void Scene::OnComponentAdded<TagComponent>(Entity entity, TagComponent& component)
	{

	}

	template<>
	void Scene::OnComponentAdded<Rigidbody2DComponent>(Entity entity, Rigidbody2DComponent& component)
	{
	}
	template<>
	void Scene::OnComponentAdded<CircleRendererComponent>(Entity entity, CircleRendererComponent& component)
	{
	}
	template<>
	void Scene::OnComponentAdded<BoxCollider2DComponent>(Entity entity, BoxCollider2DComponent& component)
	{
	}
	template<>
	void Scene::OnComponentAdded<NativeScriptComponent>(Entity entity, NativeScriptComponent& component)
	{

	}
	template<>
	void Scene::OnComponentAdded<LightSystemComponent>(Entity entity, LightSystemComponent& component)
	{

	}
}