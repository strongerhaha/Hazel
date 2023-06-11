#pragma once
#include"Hazel/Renderer/Framebuffer.h"
namespace Hazel {
	class OpenGLFramebuffer :public Framebuffer
	{
	public:
		OpenGLFramebuffer(const FramebufferSpecification& spec);
		virtual ~OpenGLFramebuffer();
		void Invalidate();
		virtual const FramebufferSpecification& GetSpecification()const override { return m_Specification; };
		virtual const uint32_t& GetColorAttachment()const override { return m_ColorAttachment; };
		virtual void Bind() override;
		virtual void Unbind() override;
		virtual void Resize(uint32_t Width, uint32_t Height) override;
	private:
		uint32_t m_RendererID=0;
		uint32_t m_ColorAttachment=0;//返回纹理
		uint32_t m_DepthAttachment=0;//返回深度
		FramebufferSpecification m_Specification;
	};
}