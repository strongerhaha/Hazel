#pragma once
#include"Scene.h"
#include"entt.hpp"
namespace Hazel {
	class Entity //提供方便的函数给entt
	{
	public:
		Entity() = default;
		Entity(entt::entity handle,Scene* scene);
		Entity(const Entity& other) = default;

		template<typename T,typename... Args>
		T& AddComponent(Args&&... args)
		{
			HZ_CORE_ASSERT(!HasComponet<T>(), "Entity already has componet!");
			return m_Scene->m_Registry.emplace<T>(m_EntityHandle, std::forward<Args>(args)...);
		}

		template<typename T>
		T& GetComponent()
		{
			HZ_CORE_ASSERT(HasComponet<T>(), "Entity does not have componet!");
			return m_Scene->m_Registry.get<T>(m_EntityHandle);
		}

		template<typename T>
		bool HasComponet()
		{
			return m_Scene->m_Registry.has<T>(m_EntityHandle);//用模板T，判断有没有一个函数，T可以是任何，m_EntityHandle是registry里面的一个entity
		}

		template<typename T>
		void RemoveComponet()
		{
			HZ_CORE_ASSERT(!HasComponet<T>(), "Entity does not have componet!");
			return m_Scene->m_Registry.remove<T>(m_EntityHandle);//用模板T，判断有没有一个函数，T可以是任何，m_EntityHandle是registry里面的一个entity
		}
		operator bool() const { return m_EntityHandle != entt::null; }

	private:
		entt::entity m_EntityHandle{ entt::null };
		Scene* m_Scene=nullptr;//为啥不用weak ref，或者SharePoint，而用scene role pointer  12bit  只是个接口没有内容可以用这个？可能/Scene不想被删掉在内存中，仅仅只想指向一个地址，不用weak因为之后要转变为intrusive reference countingsystem

	};


}