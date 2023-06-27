#pragma once
#include"glm/glm.hpp"
namespace Hazel {
	class Camera //用于component的camera类
	{
	public:
		Camera() = default;
		Camera(const glm::mat4& projection)
			:m_Projection(projection) {}
		virtual ~Camera() = default;
		const glm::mat4& GetProjection() const { return m_Projection; }

	protected:
		glm::mat4 m_Projection=glm::mat4(1.0f);

	};
}