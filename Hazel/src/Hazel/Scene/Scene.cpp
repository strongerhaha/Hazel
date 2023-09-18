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

	Scene::Scene()//初始化
	{

	}
		
	Scene::~Scene()
	{
	}

	template<typename Component>
	static void CopyComponent(entt::registry& dst, entt::registry& src, const std::unordered_map<UUID, entt::entity>& enttMap)//引用会修改内容
	{
		auto view = src.view<Component>();
		for (auto e : view)
		{
			UUID uuid = src.get<IDComponent>(e).ID;
			HZ_CORE_ASSERT(enttMap.find(uuid) != enttMap.end(),"");
			entt::entity dstEnttID = enttMap.at(uuid);//使用enttMap对应查找dstEnttID

			auto& component = src.get<Component>(e);
			dst.emplace_or_replace<Component>(dstEnttID, component);//获得Component并对应创建
		}
	}

	template<typename Component>
	static void CopyComponentIfExists(Entity dst, Entity src)
	{
		if (src.HasComponent<Component>())
			dst.AddOrReplaceComponent<Component>(src.GetComponent<Component>());//用于复制entity，emplace_or_replace
	}

	Ref<Scene> Scene::Copy(Ref<Scene> other)//复制Scene
	{
		Ref<Scene> newScene = CreateRef<Scene>();

		newScene->m_ViewportWidth = other->m_ViewportWidth;
		newScene->m_ViewportHeight = other->m_ViewportHeight;

		auto& srcSceneRegistry = other->m_Registry;
		auto& dstSceneRegistry = newScene->m_Registry;
		std::unordered_map<UUID, entt::entity> enttMap;//用Map一一对应每个uuid

		// Create entities in new scene
		auto idView = srcSceneRegistry.view<IDComponent>();//查看所有idComponent
		for (auto e : idView)
		{
			UUID uuid = srcSceneRegistry.get<IDComponent>(e).ID;
			const auto& name = srcSceneRegistry.get<TagComponent>(e).Tag;
			Entity newEntity = newScene->CreateEntityWithUUID(uuid, name);
			enttMap[uuid] = (entt::entity)newEntity;//每个uuid对应一个Entity
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
		Entity entity = { m_Registry.create(),this };//用当前这this sence创建一个Entity，m_Registry.create()这个创建了一个entt：：entity
		entity.AddComponent<IDComponent>(uuid);//添加IDComponent 每个创建的entity都会添加//用uuid添加
		entity.AddComponent<TransformComponent>();//添加TransformComponent 每个创建的entity都会添加

		auto& Tag = entity.AddComponent<TagComponent>();
		Tag.Tag = name.empty() ? "Entity" : name;
		return entity;
	}

	Entity Scene::CreateLightEntity(const std::string& name)
	{
		Entity entity = { m_Registry.create(),this };//用当前这this sence创建一个Entity，m_Registry.create()这个创建了一个entt：：entity
		entity.AddComponent<IDComponent>(UUID());//添加IDComponent 每个创建的entity都会添加//用uuid添加
		entity.AddComponent<TransformComponent>();//添加TransformComponent 每个创建的entity都会添加
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


			Renderer2D::BeginScene(mainCamera->GetProjection(),cameraTransform);//这里设置了相机的projection和view
			
			auto group = m_Registry.group<TransformComponent>(entt::get<SpriteRendererComponent>);//把他们绑定到一起
			for (auto entity : group)
			{
				auto [transform, sprite] = group.get<TransformComponent, SpriteRendererComponent>(entity);//提取数据
				//Renderer::Submit(mesh, transform);//提交到渲染器那边
				//Renderer2D::DrawQuad(transform.GetTransform(), sprite.Color);//循环渲染,里面有的都画出来
				Renderer2D::DrawSprite(transform.GetTransform(), sprite, (int)entity);//新的渲染函数，entity会++从而区分
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
		//auto group = m_Registry.group<TransformComponent>(entt::get<SpriteRendererComponent>);//把他们绑定到一起
		auto group = m_Registry.view<TransformComponent, SpriteRendererComponent>();
		for (auto entity : group)
		{
			auto [transform, sprite] = group.get<TransformComponent, SpriteRendererComponent>(entity);//提取数据
			//Renderer2D::DrawSprite(transform.GetTransform(), sprite, (int)entity);//新的渲染函数，entity会++从而区分 
			if (sprite.LightSwitch && HasLight)
				Renderer2D::DrawLightSprite(transform.GetTransform(), sprite, LightPos, LightColor, (int)entity);
			else
				Renderer2D::DrawSprite(transform.GetTransform(), sprite, (int)entity);//新的渲染函数，entity会++从而区分 
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
			auto [transform, cube] = group2.get<TransformComponent, CubeRendererComponent>(entity);//提取数据
			//Renderer2D::DrawSprite(transform.GetTransform(), sprite, (int)entity);//新的渲染函数，entity会++从而区分 
			if (cube.LightSwitch && HasLight)
				Renderer2D::DrawLightCube(transform.GetTransform(), cube, LightPos, LightColor, (int)entity);
			else
				Renderer2D::DrawCube(transform.GetTransform(), cube, (int)entity);//新的渲染函数，entity会++从而区分 
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
	void Scene::DuplicateEntity(Entity entity)//复制entity
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
		if (m_ViewportWidth > 0 && m_ViewportHeight > 0)
			component.Camera.SetViewPortSize(m_ViewportWidth, m_ViewportHeight);
		//component.Camera.SetViewPortSize(m_ViewportWidth, m_ViewportHeight);//在scene这里实现SetViewPortSize函数，达成添加camera的时候调用一次。因为是friend class 所以可以直接调用这里的私有函数
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