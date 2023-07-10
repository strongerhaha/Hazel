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
		virtual uint32_t GetColorAttachmentRendererID(uint32_t index = 0)const override { HZ_CORE_ASSERT(index < m_ColorAttachments.size(), ""); return m_ColorAttachments[index]; };
		virtual void Bind() override;
		virtual void Unbind() override;
		virtual void Resize(uint32_t Width, uint32_t Height) override;
		virtual int ReadPixel(uint32_t attachementIndex, int x, int y)override;//根据鼠标所指的地方显示对应画出来的texture  有什么东西（fragment shader里面的）
		virtual void ClearAttachment(uint32_t attachmentIndex, int value)override;

	private:
		uint32_t m_RendererID=0;
		//uint32_t m_ColorAttachment=0;//返回纹理
		uint32_t m_DepthAttachment=0;//返回深度
		FramebufferSpecification m_Specification;

		std::vector<FramebufferTextureSpecification>m_ColorAttachmentSpecifications;
		FramebufferTextureSpecification m_DepthAttachmentSpecifications = FramebufferTextureFormat::None;
	
		std::vector<uint32_t>m_ColorAttachments;

	};
}