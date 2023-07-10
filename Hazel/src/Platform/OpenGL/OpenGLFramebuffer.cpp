#include"hzpch.h"
#include"OpenGLFramebuffer.h"
#include<glad/glad.h>
namespace Hazel {
	static const uint32_t s_MaxFramebufferSize = 8192;
	namespace Utils {//把framebuffer的创建整合
		static GLenum TextureTarget(bool multisampled)//判断
		{
			return multisampled ? GL_TEXTURE_2D_MULTISAMPLE : GL_TEXTURE_2D;
		}
		static void CreateTextures(bool mutisampled, uint32_t* outID, uint32_t	count)
		{
			glCreateTextures(TextureTarget(mutisampled), count, outID);
		}
		static void BindTexture(bool multisampled, uint32_t id)
		{
			glBindTexture(TextureTarget(multisampled), id);
		}
		static bool IsDepthFormat(FramebufferTextureFormat format)
		{
			switch (format)
			{
				case Hazel::FramebufferTextureFormat::DEPTH24STENCIL8:return true;
			}
			return false;
		}
		static GLenum HazelFBTextureFormattoGL(FramebufferTextureFormat fomat)
		{
			switch (fomat)
			{
			case Hazel::FramebufferTextureFormat::RGBA8:       return GL_RGBA8;
			case Hazel::FramebufferTextureFormat::RED_INTEGER: return GL_RED_INTEGER;
			}
			HZ_CORE_ASSERT(false,"");
			return 0;
		}
		static void AttachColorTexture(uint32_t id, int samples, GLenum internalFormat, GLenum format, uint32_t width, uint32_t height, int index)//format=GL_RBGA
		{
			bool multisampled = samples > 1;
			if (multisampled)
			{
				glTexImage2DMultisample(GL_TEXTURE_2D_MULTISAMPLE, samples, internalFormat, width, height, GL_FALSE);
			}
			else
			{
				glTexImage2D(GL_TEXTURE_2D, 0, internalFormat, width, height, 0, format, GL_UNSIGNED_BYTE, nullptr);

				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
				glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

			}
			glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0 + index, TextureTarget(multisampled), id, 0);
		}

		static void AttachDepthTexture(uint32_t id, int samples, GLenum format,GLenum AttachmentType, uint32_t width, uint32_t height)
		{
			bool multisampled = samples > 1;
			if (multisampled)
			{
				glTexImage2DMultisample(GL_TEXTURE_2D_MULTISAMPLE, samples, format, width, height, GL_FALSE);
			}
			else
			{
				glTexStorage2D(GL_TEXTURE_2D, 1, format, width, height);

				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

			}
			glFramebufferTexture2D(GL_FRAMEBUFFER, AttachmentType, TextureTarget(multisampled), id, 0);
		}
	}
	
	OpenGLFramebuffer::OpenGLFramebuffer(const FramebufferSpecification& spec)
		:m_Specification(spec)
	{
		for (auto spec : m_Specification.Attachments.Attachments)//遍历每一个FramebufferSpecification的spec
		{
			if (!Utils::IsDepthFormat(spec.TextureFormat))
				m_ColorAttachmentSpecifications.emplace_back(spec);
			else
				m_DepthAttachmentSpecifications = spec.TextureFormat;
		}
		Invalidate();
	}
	OpenGLFramebuffer::~OpenGLFramebuffer()
	{
		glDeleteFramebuffers(1, &m_RendererID);
		glDeleteTextures(m_ColorAttachments.size(), m_ColorAttachments.data());
		glDeleteTextures(1, &m_DepthAttachment);
	}
	void OpenGLFramebuffer::Invalidate()
	{//帧缓冲

		if (m_RendererID)
		{
			glDeleteFramebuffers(1, &m_RendererID);
			glDeleteTextures(m_ColorAttachments.size(), m_ColorAttachments.data());
			glDeleteTextures(1, &m_DepthAttachment);
			m_ColorAttachments.clear();
			m_DepthAttachment = 0;
		}

		glCreateFramebuffers(1, &m_RendererID);//新建一个类似画布吧,附加至少一个缓冲（颜色、深度或模板缓冲）。至少有一个颜色附件(Attachment)。所有的附件都必须是完整的（保留了内存）。每个缓冲都应该有相同的样本数。
		glBindFramebuffer(GL_FRAMEBUFFER, m_RendererID);//绑定ID，

		//Attachements
		bool multisample = m_Specification.Samples > 1;
		if (m_ColorAttachmentSpecifications.size())//初始化之类的
		{
			m_ColorAttachments.resize(m_ColorAttachmentSpecifications.size());
			Utils::CreateTextures(multisample, m_ColorAttachments.data(), m_ColorAttachments.size());
			for (size_t i=0;i< m_ColorAttachments.size();i++)
			{
				Utils::BindTexture(multisample, m_ColorAttachments[i]);//m_ColorAttachments==rendererID
				switch (m_ColorAttachmentSpecifications[i].TextureFormat)
				{
				case FramebufferTextureFormat::RGBA8:
					Utils::AttachColorTexture(m_ColorAttachments[i], m_Specification.Samples, GL_RGBA8,GL_RGBA, m_Specification.Width, m_Specification.Height, i);
					break;
				case FramebufferTextureFormat::RED_INTEGER:
					Utils::AttachColorTexture(m_ColorAttachments[i], m_Specification.Samples, GL_R32I, GL_RED_INTEGER, m_Specification.Width, m_Specification.Height, i);
					break;
				}
			}
		}

		if (m_DepthAttachmentSpecifications.TextureFormat != FramebufferTextureFormat::None)
		{
			Utils::CreateTextures(multisample, &m_DepthAttachment, 1);
			Utils::BindTexture(multisample, m_DepthAttachment);
			switch (m_DepthAttachmentSpecifications.TextureFormat)
			{
				case FramebufferTextureFormat::DEPTH24STENCIL8:
					Utils::AttachDepthTexture(m_DepthAttachment, m_Specification.Samples, GL_DEPTH24_STENCIL8, GL_DEPTH_STENCIL_ATTACHMENT, m_Specification.Width, m_Specification.Height);
					break;
			}
		}

		if (m_ColorAttachments.size() > 1)
		{
			HZ_CORE_ASSERT(m_ColorAttachments.size() <= 4,"");
			GLenum buffers[4] = { GL_COLOR_ATTACHMENT0,GL_COLOR_ATTACHMENT1,GL_COLOR_ATTACHMENT2,GL_COLOR_ATTACHMENT3 };
			glDrawBuffers(m_ColorAttachments.size(), buffers);
		}
		else if (m_ColorAttachments.empty())
		{
			glDrawBuffer(GL_NONE);
		}
#if 0//把他整合到上面的函数了
		glCreateTextures(GL_TEXTURE_2D, 1, &m_ColorAttachment);//纹理附件创建
		glBindTexture(GL_TEXTURE_2D, m_ColorAttachment);//绑定纹理
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, m_Specification.Width, m_Specification.Height, 0, GL_RGBA, GL_UNSIGNED_BYTE, nullptr);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, m_ColorAttachment, 0);//将它附加到帧缓冲

		glCreateTextures(GL_TEXTURE_2D, 1, &m_DepthAttachment);//深度和模板缓冲纹理
		glBindTexture(GL_TEXTURE_2D, m_DepthAttachment);
		//glTexStorage2D(GL_TEXTURE_2D, 0, GL_DEPTH24_STENCIL8, m_Specification.Width, m_Specification.Height);
		//将深度缓冲和模板缓冲附加为一个单独的纹理。纹理的每32位数值将包含24位的深度信息和8位的模板信息。
		glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH24_STENCIL8, m_Specification.Width, m_Specification.Height, 0,
			GL_DEPTH_STENCIL, GL_UNSIGNED_INT_24_8, NULL);

		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_TEXTURE_2D, m_DepthAttachment, 0);//附加一个深度和模板缓冲纹理到帧缓冲对象
#endif
		HZ_CORE_ASSERT(glCheckFramebufferStatus(GL_FRAMEBUFFER) == GL_FRAMEBUFFER_COMPLETE, "Framebuffer is incomplete!");

		glBindFramebuffer(GL_FRAMEBUFFER, 0);
	}
	void OpenGLFramebuffer::Bind()
	{
		glBindFramebuffer(GL_FRAMEBUFFER, m_RendererID);
		glViewport(0, 0, m_Specification.Width, m_Specification.Height);

		
	}
	void OpenGLFramebuffer::Unbind()
	{
		glBindFramebuffer(GL_FRAMEBUFFER, 0);
	}
	void OpenGLFramebuffer::Resize(uint32_t Width, uint32_t Height)
	{
		if (Width == 0 || Height == 0 || Width >= s_MaxFramebufferSize || Height >= s_MaxFramebufferSize)
		{
			HZ_CORE_WARN("Attempt to resize framebuffer to {0},{1}", Width, Height);
			return;
		}
		m_Specification.Width = Width;
		m_Specification.Height = Height;

		Invalidate();

	}
	int OpenGLFramebuffer::ReadPixel(uint32_t attachmentIndex, int x, int y)
	{
		HZ_CORE_ASSERT(attachmentIndex < m_ColorAttachments.size(),"");

		glReadBuffer(GL_COLOR_ATTACHMENT0+ attachmentIndex);
		int pixelData;
		glReadPixels(x,y,1,1,GL_RED_INTEGER,GL_INT,&pixelData);//用这读取shader里面的东西，或者说GPU的
		return pixelData;
	}
	void OpenGLFramebuffer::ClearAttachment(uint32_t attachmentIndex, int value)
	{
		HZ_CORE_ASSERT(attachmentIndex < m_ColorAttachments.size(), "");

		auto& spec = m_ColorAttachmentSpecifications[attachmentIndex];
		spec.TextureFormat;

		glClearTexImage(m_ColorAttachments[attachmentIndex], 0, 
			Utils::HazelFBTextureFormattoGL(spec.TextureFormat), GL_INT, &value);//把value搞进去重置m_ColorAttachments[1]的

	}
}