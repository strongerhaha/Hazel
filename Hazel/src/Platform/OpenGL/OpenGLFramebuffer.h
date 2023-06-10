#pragma once
#include"Hazel/Renderer/Framebuffer.h"
namespace Hazel {
	class OpenGLFramebuffer :public Frambuffer
	{
	public:
		OpenGLFramebuffer(const FramebufferSpecification& spec);
		virtual ~OpenGLFramebuffer();
		void Invalidate();
		virtual const FramebufferSpecification& GetSpecification()const override { return m_Specification; };
		virtual const uint32_t& GetColorAttachment()const override { return m_ColorAttachment; };
		virtual void Bind() override;
		virtual void Unbind() override;

	private:
		uint32_t m_RendererID;
		uint32_t m_ColorAttachment;//��������
		uint32_t m_DepthAttachment;//�������
		FramebufferSpecification m_Specification;
	};
}