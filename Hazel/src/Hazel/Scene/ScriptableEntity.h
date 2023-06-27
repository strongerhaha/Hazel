#pragma once
#include"Entity.h"
namespace Hazel {
	class ScriptableEntity 
	{
	public:
		template<typename T>
		T& GetComponent()
		{
			return m_Entity.GetComponent<T>();//直接调用Entity里面的函数
		}	
	private:
		Entity m_Entity;
		friend class Scene;
	};

}