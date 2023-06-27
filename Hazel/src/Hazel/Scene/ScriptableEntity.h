#pragma once
#include"Entity.h"
namespace Hazel {
	class ScriptableEntity 
	{
	public:
		template<typename T>
		T& GetComponent()
		{
			return m_Entity.GetComponent<T>();//ֱ�ӵ���Entity����ĺ���
		}	
	private:
		Entity m_Entity;
		friend class Scene;
	};

}