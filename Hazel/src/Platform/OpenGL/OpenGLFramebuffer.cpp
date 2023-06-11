#include"hzpch.h"
#include"OpenGLFramebuffer.h"
#include<glad/glad.h>
namespace Hazel {
	OpenGLFramebuffer::OpenGLFramebuffer(const FramebufferSpecification& spec)
		:m_Specification(spec)
	{
		Invalidate();
	}
	OpenGLFramebuffer::~OpenGLFramebuffer()
	{
		glDeleteFramebuffers(1, &m_RendererID);
	}
	void OpenGLFramebuffer::Invalidate()
	{//֡����
		glCreateFramebuffers(1, &m_RendererID);//�½�һ�����ƻ�����,��������һ�����壨��ɫ����Ȼ�ģ�建�壩��������һ����ɫ����(Attachment)�����еĸ����������������ģ��������ڴ棩��ÿ�����嶼Ӧ������ͬ����������
		glBindFramebuffer(GL_FRAMEBUFFER, m_RendererID);//��ID��
		glCreateTextures(GL_TEXTURE_2D, 1, &m_ColorAttachment);//������������
		glBindTexture(GL_TEXTURE_2D, m_ColorAttachment);//������
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, m_Specification.Width, m_Specification.Height, 0, GL_RGBA, GL_UNSIGNED_BYTE, nullptr);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, m_ColorAttachment, 0);//�������ӵ�֡����

		glCreateTextures(GL_TEXTURE_2D, 1, &m_DepthAttachment);//��Ⱥ�ģ�建������
		glBindTexture(GL_TEXTURE_2D, m_DepthAttachment);
		//glTexStorage2D(GL_TEXTURE_2D, 0, GL_DEPTH24_STENCIL8, m_Specification.Width, m_Specification.Height);
		//����Ȼ����ģ�建�帽��Ϊһ��������������������ÿ32λ��ֵ������24λ�������Ϣ��8λ��ģ����Ϣ��
		glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH24_STENCIL8, m_Specification.Width, m_Specification.Height, 0,
			GL_DEPTH_STENCIL, GL_UNSIGNED_INT_24_8, NULL);

		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_TEXTURE_2D, m_DepthAttachment, 0);//����һ����Ⱥ�ģ�建��������֡�������
		HZ_CORE_ASSERT(glCheckFramebufferStatus(GL_FRAMEBUFFER) == GL_FRAMEBUFFER_COMPLETE, "Framebuffer is incomplete!");

		glBindFramebuffer(GL_FRAMEBUFFER, 0);
	}
	void OpenGLFramebuffer::Bind()
	{
		glBindFramebuffer(GL_FRAMEBUFFER, m_RendererID);
	}
	void OpenGLFramebuffer::Unbind()
	{
		glBindFramebuffer(GL_FRAMEBUFFER, 0);
	}
}