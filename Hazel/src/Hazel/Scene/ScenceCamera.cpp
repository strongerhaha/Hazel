#include"hzpch.h"
#include"ScenceCamera.h"

#include<glm/gtc/matrix_transform.hpp>

namespace Hazel {
	SceneCamera::SceneCamera()
	{
	}
	//SceneCamera::~SceneCamera();
	void SceneCamera::SetOrthographic(float size, float nearClip, float farClip)
	{
		m_ProjectionType = ProjectionType::Orthographic;
		m_OrthographicSize = size;
		m_OrthographicNear = nearClip;
		m_OrthographicFar = farClip;
		RecalculateProjection();
	}
	void SceneCamera::SetPerspective(float verticalFOV, float nearClip, float farClip)
	{
		m_ProjectionType = ProjectionType::Perspective;
		m_PerspectiveFOV = verticalFOV;
		m_PerspectiveNear = nearClip;
		m_PerspectiveFar = farClip;
		RecalculateProjection();
	}
	void SceneCamera::SetViewPortSize(uint32_t width, uint32_t height)
	{
		HZ_CORE_ASSERT(width > 0 && height > 0,"1");
		m_AspectRatio = (float)width / (float)height;
		RecalculateProjection();
	}

	void SceneCamera::RecalculateProjection()//为了不让resize的时候被缩小
	{
		if (m_ProjectionType == ProjectionType::Perspective)
		{
			m_Projection = glm::perspective(m_PerspectiveFOV, m_AspectRatio, m_PerspectiveNear, m_PerspectiveFar);//改一下Projection就行 直接改的是camera里面的
		}
		else
		{
		float orthoLeft = -m_OrthographicSize * m_AspectRatio * 0.5f;
		float orthoRight = m_OrthographicSize * m_AspectRatio * 0.5f;
		float orthoBottom = -m_OrthographicSize * 0.5f;
		float orthoTop = m_OrthographicSize * 0.5f;
		m_Projection = glm::ortho(orthoLeft, orthoRight, orthoBottom, orthoTop, m_OrthographicNear, m_OrthographicFar);

		}
	}
}