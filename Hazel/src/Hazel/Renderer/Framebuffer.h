#pragma once
#include"Hazel/Core/Core.h"
namespace Hazel {
	enum class FramebufferTextureFormat
	{
		None=0,

		RGBA8,
		RED_INTEGER,
		DEPTH24STENCIL8,

		Depth= DEPTH24STENCIL8
	};
	struct FramebufferTextureSpecification
	{
		FramebufferTextureSpecification() = default;
		FramebufferTextureSpecification(FramebufferTextureFormat format)
			:TextureFormat(format) {}

		FramebufferTextureFormat TextureFormat=FramebufferTextureFormat::None;
	};
	struct FramebufferAttachmentSpecification
	{
		FramebufferAttachmentSpecification() = default;
		FramebufferAttachmentSpecification(std::initializer_list<FramebufferTextureSpecification> attachments)//?
			:Attachments(attachments) {}

		std::vector<FramebufferTextureSpecification> Attachments;
	};
	struct FramebufferSpecification
	{
		uint32_t Width, Height;
		uint32_t Samples = 1;
		FramebufferAttachmentSpecification Attachments;//这里新加的，可以初始化
		bool SwapChainTarget = false;//是不是直接画出来，不用frame，
	};

	class Framebuffer
	{
	public:
		virtual ~Framebuffer() = default;
		virtual void Bind()=0;
		virtual void Unbind() = 0;
		virtual void Resize(uint32_t Width, uint32_t Height) = 0;
		virtual int ReadPixel(uint32_t attachementIndex, int x, int y) = 0;//用于鼠标选择
		virtual const FramebufferSpecification& GetSpecification()const = 0;
		static Ref<Framebuffer> Create(const FramebufferSpecification& spec);
		virtual  uint32_t GetColorAttachmentRendererID(uint32_t index = 0)const=0;
	};
}