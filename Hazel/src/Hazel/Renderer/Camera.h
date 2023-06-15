#pragma once
#include"glm/glm.hpp"
namespace Hazel {
	class Camera //用于component的camera类
	{
	public:
		Camera(const glm::mat4& projection)
			:m_Projection(projection) {}
		const glm::mat4& GetProjection() const { return m_Projection; }

	private:
		glm::mat4 m_Projection;

	};
}