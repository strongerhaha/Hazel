#include"hzpch.h"
#include "Renderer2D.h"
#include"VertexArray.h"
#include"Shader.h"
#include"RenderCommand.h"
#include<glm/gtc/matrix_transform.hpp>
namespace Hazel {
	
	//Ref<IndexBuffer>cubeIB;
	struct QuadVertex
	{
		glm::vec3 Position;
		glm::vec4 Color;
		glm::vec2 TexCoord;//坐标之类
		float TexIndex;//对应的TexIndex0是空白，最大32
		float TilingFactor;//大小

		//editor
		int EntityID;
	};

	struct Renderer2DData
	{
		static const uint32_t MaxQuads = 10080;//每次的最大个数。
		static const uint32_t MaxVertices = MaxQuads * 4;
		static const uint32_t MaxIndices = MaxQuads * 6;
		static const uint32_t MaxTextureSlots = 32;//最大可加载的纹理个数

		Ref<VertexArray> QuadVertexArray;
		Ref<Shader> TextureShader;
		Ref<Texture2D> WhiteTexture;
		Ref<VertexBuffer>QuadVertexBuffer;

		uint32_t QuadIndexCount = 0;
		uint32_t CubeIndexCount = 0;


		QuadVertex* QuadVertexBufferBase = nullptr;
		QuadVertex* QuadVertexBufferPtr = nullptr;

		std::array<Ref<Texture2D>, MaxTextureSlots> TextureSlots;//0对应空白，纹理数组
		uint32_t TextureSlotIndex = 1;
		glm::vec4 QuadVertexPositions[8];//基础坐标，用来与transform相乘

		Renderer2D::Statistics Stats;
	};
	static Renderer2DData s_Data;



	void Renderer2D::Init()//初始化需要的东西
	{
		HZ_PROFILE_FUNCTION();

		s_Data.QuadVertexArray = VertexArray::Create();

		s_Data.QuadVertexBuffer.reset(VertexBuffer::Create(s_Data.MaxVertices *sizeof(QuadVertex)));

		s_Data.QuadVertexBuffer->SetLayout({//对应shader里面的layout
			{ShaderDataType::Float3,"a_Position"},
			{ShaderDataType::Float4,"a_Color"},
			{ShaderDataType::Float2,"a_TexCoord"},
			{ShaderDataType::Float,"a_TexIndex"},
			{ShaderDataType::Float,"a_TilingFactor"},
			{ShaderDataType::Int, "a_EntityID" }
			});
		s_Data.QuadVertexArray->AddVertexBuffer(s_Data.QuadVertexBuffer);

		s_Data.QuadVertexBufferBase = new QuadVertex[s_Data.MaxVertices];

		uint32_t* quadIndices = new uint32_t[s_Data.MaxIndices];
		uint32_t offset = 0;

		for (uint32_t i = 0; i < s_Data.MaxIndices/6; i += 6)
		{
			quadIndices[i + 0] = offset + 0;
			quadIndices[i + 1] = offset + 1;
			quadIndices[i + 2] = offset + 2;

			quadIndices[i + 3] = offset + 2;
			quadIndices[i + 4] = offset + 3;
			quadIndices[i + 5] = offset + 0;

			offset += 4;
		}//关于index，批处理每个index坐标


		uint32_t* quadIndices1 = new uint32_t[s_Data.MaxIndices];
		offset = 0;
		for (uint32_t i = 0; i < s_Data.MaxIndices/6; i += 36)//这上传就会删掉。
		{
			quadIndices1[i + 0] = offset + 0;
			quadIndices1[i + 1] = offset + 1;
			quadIndices1[i + 2] = offset + 2;
					   
			quadIndices1[i + 3] = offset + 2;
			quadIndices1[i + 4] = offset + 3;
			quadIndices1[i + 5] = offset + 0;//后面
					   
			quadIndices1[i + 6] = offset + 6;
			quadIndices1[i + 7] = offset + 1;
			quadIndices1[i + 8] = offset + 2;
					   
			quadIndices1[i + 9] = offset + 1;
			quadIndices1[i + 10] = offset + 5;
			quadIndices1[i + 11] = offset + 6;//右边
					   
			quadIndices1[i + 12] = offset + 3;
			quadIndices1[i + 13] = offset + 6;
			quadIndices1[i + 14] = offset + 2;
					   
			quadIndices1[i + 15] = offset + 7;
			quadIndices1[i + 16] = offset + 3;
			quadIndices1[i + 17] = offset + 6;//上
					   
			quadIndices1[i + 18] = offset + 0;
			quadIndices1[i + 19] = offset + 1;
			quadIndices1[i + 20] = offset + 5;
					   
			quadIndices1[i + 21] = offset + 5;
			quadIndices1[i + 22] = offset + 4;
			quadIndices1[i + 23] = offset + 0;//下
					   
			quadIndices1[i + 24] = offset + 7;
			quadIndices1[i + 25] = offset + 0;
			quadIndices1[i + 26] = offset + 3;
					   
			quadIndices1[i + 27] = offset + 7;
			quadIndices1[i + 28] = offset + 4;
			quadIndices1[i + 29] = offset + 0;//左
					   
			quadIndices1[i + 30] = offset + 4;
			quadIndices1[i + 31] = offset + 6;
			quadIndices1[i + 32] = offset + 5;
					   
			quadIndices1[i + 33] = offset + 4;
			quadIndices1[i + 34] = offset + 6;
			quadIndices1[i + 35] = offset + 7;//前

			offset += 8;
		}//关于index，批处理每个index坐标
		Ref<IndexBuffer>quadIB;
		quadIB.reset(IndexBuffer::Create(quadIndices, s_Data.MaxIndices));

		
		//cubeIB.reset(IndexBuffer::Create(quadIndices1, s_Data.MaxIndices));
		s_Data.QuadVertexArray->SetIndexBuffer(quadIB);
		//s_Data.QuadVertexArray->SetIndexBuffer(cubeIB);


		delete[] quadIndices;
		delete[] quadIndices1;
		s_Data.WhiteTexture = Texture2D::Create(1, 1);
		uint32_t whiteTextureData = 0xffffffff;
		s_Data.WhiteTexture->SetData(&whiteTextureData, sizeof(uint32_t));

		int32_t samplers[s_Data.MaxTextureSlots];
		for (uint32_t i = 0; i < s_Data.MaxTextureSlots; i++)
			samplers[i] = i;

		s_Data.TextureShader = Shader::Create("asset/shaders/Texture.glsl");
		s_Data.TextureShader->Bind();
		s_Data.TextureShader->SetIntArray("u_Textures", samplers, s_Data.MaxTextureSlots);

		s_Data.TextureSlots[0] = s_Data.WhiteTexture;//空白纹理

		s_Data.QuadVertexPositions[0] = { -0.5f,-0.5f,-0.5f,1.0f };
		s_Data.QuadVertexPositions[1] = { 0.5f,-0.5f,-0.5f,1.0f };
		s_Data.QuadVertexPositions[2] = { 0.5f,0.5f,-0.5f,1.0f };
		s_Data.QuadVertexPositions[3] = { -0.5f,0.5f,-0.5f,1.0f };//基础坐标，用来与transform相乘

		s_Data.QuadVertexPositions[4] = { -0.5f,-0.5f,0.5f,1.0f };
		s_Data.QuadVertexPositions[5] = { 0.5f,-0.5f,0.5f,1.0f };
		s_Data.QuadVertexPositions[6] = { 0.5f,0.5f,0.5f,1.0f };
		s_Data.QuadVertexPositions[7] = { -0.5f,0.5f,0.5f,1.0f };
	}
	void Renderer2D::Shutdown()
	{
		delete[] s_Data.QuadVertexBufferBase;
		HZ_PROFILE_FUNCTION();
	}
	void Renderer2D::Reset()
	{
	}
	void Renderer2D::BeginScene(const Camera& camera, const glm::mat4& transform)
	{
		HZ_PROFILE_FUNCTION();
		glm::mat4 viewProj = camera.GetProjection() * glm::inverse(transform);
		s_Data.TextureShader->Bind();
		s_Data.TextureShader->SetMat4("u_ViewProjection", viewProj);

		StartBatch();
	}
	void Renderer2D::BeginScene(const OrthographicCamera& camera)
	{
		HZ_PROFILE_FUNCTION();
		s_Data.TextureShader->Bind();
		s_Data.TextureShader->SetMat4("u_ViewProjection", camera.GetViewProjectionMatrix());
		StartBatch();
		
	}

	void Renderer2D::BeginScene(const EditorCamera& camera)
	{
		HZ_PROFILE_FUNCTION();
		glm::mat4 viewProj = camera.GetViewProjection();
		s_Data.TextureShader->Bind();
		s_Data.TextureShader->SetMat4("u_ViewProjection", viewProj);

		StartBatch();
	}

	void Renderer2D::EndScene()
	{
		HZ_PROFILE_FUNCTION();
		uint32_t dataSize = (uint32_t)((uint8_t*)s_Data.QuadVertexBufferPtr - (uint8_t*)s_Data.QuadVertexBufferBase);
		s_Data.QuadVertexBuffer->SetData(s_Data.QuadVertexBufferBase, dataSize);
		Flush();
	}

	void Renderer2D::Flush()
	{
		if (s_Data.QuadIndexCount == 0&&s_Data.CubeIndexCount==0)
			return; // Nothing to draw
		uint32_t dataSize = (uint32_t)((uint8_t*)s_Data.QuadVertexBufferPtr - (uint8_t*)s_Data.QuadVertexBufferBase);
		s_Data.QuadVertexBuffer->SetData(s_Data.QuadVertexBufferBase, dataSize);
		for (uint32_t i = 0; i < s_Data.TextureSlotIndex; i++)
			s_Data.TextureSlots[i]->Bind(i);
		RenderCommand::DrawIndexed(s_Data.QuadVertexArray, s_Data.QuadIndexCount+ s_Data.CubeIndexCount);
		s_Data.Stats.DrawCalls++;//每次调用Flush函数DrawCall+1
		
	}
	void Renderer2D::NextBatch()
	{
		Flush();
		StartBatch();
	}
	void Renderer2D::DrawQuad(const glm::vec2& position, const glm::vec2& size, const glm::vec4& color)
	{
		DrawQuad({ position.x,position.y,0.0f }, size, color);
	}

	void Renderer2D::DrawQuad(const glm::vec3& position, const glm::vec2& size, const glm::vec4& color)
	{
		HZ_PROFILE_FUNCTION();
	
		glm::mat4 transform = glm::translate(glm::mat4(1.0f), position)
			* glm::scale(glm::mat4(1.0f), { size.x,size.y,1.0f });

		DrawQuad(transform, color);
	}

	void Renderer2D::DrawQuad(const glm::vec2& position, const glm::vec2& size, const Ref<Texture2D>& texture, float tilingFactor, const glm::vec4& tintColor)
	{
		DrawQuad({ position.x,position.y,0.0f }, size, texture, tilingFactor, tintColor);
	}

	void Renderer2D::DrawQuad(const glm::vec3& position, const glm::vec2& size, const Ref<Texture2D>& texture, float tilingFactor, const glm::vec4& tintColor)
	{
		HZ_PROFILE_FUNCTION();
		
		glm::mat4 transform = glm::translate(glm::mat4(1.0f), position)
			* glm::scale(glm::mat4(1.0f), { size.x,size.y,1.0f });

		DrawQuad(transform, texture, tilingFactor);
	}

	void Renderer2D::DrawQuad(const glm::vec2& position, const glm::vec2& size, const Ref<SubTexture2D>& subtexture, float tilingFactor, const glm::vec4& tintColor)
	{
		DrawQuad({ position.x,position.y,0.0f }, size, subtexture, tilingFactor, tintColor);
	}

	void Renderer2D::DrawQuad(const glm::vec3& position, const glm::vec2& size, const Ref<SubTexture2D>& subtexture, float tilingFactor, const glm::vec4& tintColor)
	{
		HZ_PROFILE_FUNCTION();
		constexpr size_t quadVertexCount = 4;//点的个数
		constexpr glm::vec4 color = { 1.0f, 1.0f, 1.0f, 1.0f };
		const glm::vec2* textureCoords = subtexture->GetTexCoords();
		const Ref<Texture2D> texture = subtexture->GetTexture();


		if (s_Data.QuadIndexCount >= Renderer2DData::MaxIndices)
			NextBatch();

		float textureIndex = 0.0f;

		for (uint32_t i = 1; i < s_Data.TextureSlotIndex; i++)
		{
			if (*s_Data.TextureSlots[i].get() == *texture.get())//遍历TextureSlots，判断是否已经加载texture
			{
				textureIndex = (float)i;
				break;
			}
		}
		if (textureIndex == 0.0f)
		{
			textureIndex = (float)s_Data.TextureSlotIndex;
			s_Data.TextureSlots[s_Data.TextureSlotIndex] = texture;
			s_Data.TextureSlotIndex++;
		}
		glm::mat4 transform = glm::translate(glm::mat4(1.0f), position)
			* glm::scale(glm::mat4(1.0f), { size.x,size.y,1.0f });

		for (size_t i = 0; i < quadVertexCount; i++)
		{
			s_Data.QuadVertexBufferPtr->Position = transform * s_Data.QuadVertexPositions[i];
			s_Data.QuadVertexBufferPtr->Color = color;
			s_Data.QuadVertexBufferPtr->TexCoord = textureCoords[i];
			s_Data.QuadVertexBufferPtr->TexIndex = textureIndex;
			s_Data.QuadVertexBufferPtr->TilingFactor = tilingFactor;
			s_Data.QuadVertexBufferPtr++;
		}
		s_Data.QuadIndexCount += 6;
		s_Data.Stats.QuadCount++;
	}

	void Renderer2D::DrawQuad(const glm::mat4& transform, const glm::vec4& color, int entityID)
	{
		HZ_PROFILE_FUNCTION();
		constexpr size_t quadVertexCount = 4;
		const float textureIndex = 0.0f;
		constexpr glm::vec2 textureCoords[] = { { 0.0f, 0.0f }, { 1.0f, 0.0f }, { 1.0f, 1.0f }, { 0.0f, 1.0f } };
		const float tilingFactor = 1.0f;
		if (s_Data.QuadIndexCount >= Renderer2DData::MaxIndices)
			NextBatch();

		for (size_t i = 0; i < quadVertexCount; i++)
		{
			s_Data.QuadVertexBufferPtr->Position = transform * s_Data.QuadVertexPositions[i];
			s_Data.QuadVertexBufferPtr->Color = color;
			s_Data.QuadVertexBufferPtr->TexCoord = textureCoords[i];
			s_Data.QuadVertexBufferPtr->TexIndex = textureIndex;
			s_Data.QuadVertexBufferPtr->TilingFactor = tilingFactor;
			s_Data.QuadVertexBufferPtr->EntityID = entityID;
			s_Data.QuadVertexBufferPtr++;
		}

		s_Data.QuadIndexCount += 6;
		s_Data.Stats.QuadCount++;//要改   这个是正方形的个数，还有正方体的个数
	}

	void Renderer2D::DrawQuad(const glm::mat4& transform, const Ref<Texture2D>& texture, float tilingFactor, const glm::vec4& tintColor, int entityID)
	{
		HZ_PROFILE_FUNCTION();
		constexpr size_t quadVertexCount = 4;
		constexpr glm::vec4 color = { 1.0f, 1.0f, 1.0f, 1.0f };
		constexpr glm::vec2 textureCoords[] = { { 0.0f, 0.0f }, { 1.0f, 0.0f }, { 1.0f, 1.0f }, { 0.0f, 1.0f } };

		if (s_Data.QuadIndexCount >= Renderer2DData::MaxIndices)
			NextBatch();

		float textureIndex = 0.0f;

		for (uint32_t i = 1; i < s_Data.TextureSlotIndex; i++)
		{
			if (*s_Data.TextureSlots[i].get() == *texture.get())//遍历TextureSlots，判断是否已经加载texture
			{
				textureIndex = (float)i;
				break;
			}
		}
		if (textureIndex == 0.0f)
		{
			textureIndex = (float)s_Data.TextureSlotIndex;
			s_Data.TextureSlots[s_Data.TextureSlotIndex] = texture;
			s_Data.TextureSlotIndex++;
		}

		for (size_t i = 0; i < quadVertexCount; i++)
		{
			s_Data.QuadVertexBufferPtr->Position = transform * s_Data.QuadVertexPositions[i];
			s_Data.QuadVertexBufferPtr->Color = color;
			s_Data.QuadVertexBufferPtr->TexCoord = textureCoords[i];
			s_Data.QuadVertexBufferPtr->TexIndex = textureIndex;
			s_Data.QuadVertexBufferPtr->TilingFactor = tilingFactor;
			s_Data.QuadVertexBufferPtr->EntityID = entityID;//新增加的int
			s_Data.QuadVertexBufferPtr++;
		}
		s_Data.QuadIndexCount += 6;
		s_Data.Stats.QuadCount++;
	}

	


	void Renderer2D::DrawRotationQuad(const glm::vec3& position, const glm::vec2& size, float rotation, const glm::vec4& color)
	{
		HZ_PROFILE_FUNCTION();
		if (s_Data.QuadIndexCount >= Renderer2DData::MaxIndices)
			NextBatch();

		const float textureIndex = 0.0f;
		const float tilingFactor = 1.0f;
		constexpr size_t quadVertexCount = 4;
		
		constexpr glm::vec2 textureCoords[] = { { 0.0f, 0.0f }, 
			{ 1.0f, 0.0f }, 
			{ 1.0f, 1.0f }, 
			{ 0.0f, 1.0f } };
		glm::mat4 transform = glm::translate(glm::mat4(1.0f), position)
			* glm::rotate(glm::mat4(1.0f), rotation, { 0.0f,0.0f,1.0f })
			* glm::scale(glm::mat4(1.0f), { size.x,size.y,1.0f });

		for (size_t i = 0; i < quadVertexCount; i++)
		{
			s_Data.QuadVertexBufferPtr->Position = transform * s_Data.QuadVertexPositions[i];
			s_Data.QuadVertexBufferPtr->Color = color;
			s_Data.QuadVertexBufferPtr->TexCoord = textureCoords[i];
			s_Data.QuadVertexBufferPtr->TexIndex = textureIndex;
			s_Data.QuadVertexBufferPtr->TilingFactor = tilingFactor;
			s_Data.QuadVertexBufferPtr++;
		}
		s_Data.QuadIndexCount += 6;

		s_Data.Stats.QuadCount++;
	}

	void Renderer2D::DrawRotationQuad(const glm::vec2& position, const glm::vec2& size, float rotation, const glm::vec4& color)
	{
		DrawRotationQuad({ position.x,position.y,0.0f }, size, rotation, color);
	}

	void Renderer2D::DrawRotationQuad(const glm::vec2& position, const glm::vec2& size, float rotation, const Ref<Texture2D>& texture, float tilingFactor, const glm::vec4& tintColor)
	{
		DrawRotationQuad({ position.x,position.y,0.0f }, size, rotation, texture, tilingFactor, tintColor);
	}
	void Renderer2D::DrawRotationQuad(const glm::vec3& position, const glm::vec2& size, float rotation, const Ref<Texture2D>& texture, float tilingFactor, const glm::vec4& tintColor)
	{
		HZ_PROFILE_FUNCTION();
	

		if (s_Data.QuadIndexCount >= Renderer2DData::MaxIndices)
			NextBatch();

		constexpr size_t quadVertexCount = 4;
		constexpr glm::vec4 color = { 1.0f, 1.0f, 1.0f, 1.0f };
		constexpr glm::vec2 textureCoords[] = { { 0.0f, 0.0f }, { 1.0f, 0.0f }, { 1.0f, 1.0f }, { 0.0f, 1.0f } };
		float textureIndex = 0.0f;

		for (uint32_t i = 1; i < s_Data.TextureSlotIndex; i++)
		{
			if (*s_Data.TextureSlots[i].get() == *texture.get())//遍历TextureSlots，判断是否已经加载texture
			{
				textureIndex = (float)i;
				break;
			}
		}

		if (textureIndex == 0.0f)
		{
			textureIndex = (float)s_Data.TextureSlotIndex;
			s_Data.TextureSlots[s_Data.TextureSlotIndex] = texture;
			s_Data.TextureSlotIndex++;
		}
		glm::mat4 transform = glm::translate(glm::mat4(1.0f), position)
			* glm::rotate(glm::mat4(1.0f),rotation, { 0.0f,0.0f,1.0f })
			* glm::scale(glm::mat4(1.0f), { size.x,size.y,1.0f });


		for (size_t i = 0; i < quadVertexCount; i++)
		{
			s_Data.QuadVertexBufferPtr->Position = transform * s_Data.QuadVertexPositions[i];
			s_Data.QuadVertexBufferPtr->Color = color;
			s_Data.QuadVertexBufferPtr->TexCoord = textureCoords[i];
			s_Data.QuadVertexBufferPtr->TexIndex = textureIndex;
			s_Data.QuadVertexBufferPtr->TilingFactor = tilingFactor;
			s_Data.QuadVertexBufferPtr++;
		}
		s_Data.QuadIndexCount += 6;

		s_Data.Stats.QuadCount++;
	}

	void Renderer2D::DrawRotationQuad(const glm::vec2& position, const glm::vec2& size, float rotation, const Ref<SubTexture2D>& subtexture, float tilingFactor, const glm::vec4& tintColor)
	{
		
		DrawRotationQuad({ position.x,position.y,0.0f }, size, rotation, subtexture, tilingFactor, tintColor);
	}
	void Renderer2D::DrawSprite(const glm::mat4& transform, SpriteRendererComponent& src, int entityID)
	{
		if (src.Texture)
			DrawQuad(transform, src.Texture, src.TilingFactor, src.Color, entityID);
		else
			DrawQuad(transform, src.Color, entityID);//SpriteRendererComponent通过这个调用
	}
	
	void Renderer2D::DrawRotationQuad(const glm::vec3& position, const glm::vec2& size, float rotation, const Ref<SubTexture2D>& subtexture, float tilingFactor, const glm::vec4& tintColor)
	{
		HZ_PROFILE_FUNCTION();


		if (s_Data.QuadIndexCount >= Renderer2DData::MaxIndices)
			NextBatch();

		constexpr size_t quadVertexCount = 4;
		constexpr glm::vec4 color = { 1.0f, 1.0f, 1.0f, 1.0f };
		const glm::vec2* textureCoords = subtexture->GetTexCoords();
		const Ref<Texture2D> texture = subtexture->GetTexture();
		float textureIndex = 0.0f;

		for (uint32_t i = 1; i < s_Data.TextureSlotIndex; i++)
		{
			if (*s_Data.TextureSlots[i].get() == *texture.get())//遍历TextureSlots，判断是否已经加载texture
			{
				textureIndex = (float)i;
				break;
			}
		}

		if (textureIndex == 0.0f)
		{
			textureIndex = (float)s_Data.TextureSlotIndex;
			s_Data.TextureSlots[s_Data.TextureSlotIndex] = texture;
			s_Data.TextureSlotIndex++;
		}
		glm::mat4 transform = glm::translate(glm::mat4(1.0f), position)
			* glm::rotate(glm::mat4(1.0f), rotation, { 0.0f,0.0f,1.0f })
			* glm::scale(glm::mat4(1.0f), { size.x,size.y,1.0f });


		for (size_t i = 0; i < quadVertexCount; i++)
		{
			s_Data.QuadVertexBufferPtr->Position = transform * s_Data.QuadVertexPositions[i];
			s_Data.QuadVertexBufferPtr->Color = color;
			s_Data.QuadVertexBufferPtr->TexCoord = textureCoords[i];
			s_Data.QuadVertexBufferPtr->TexIndex = textureIndex;
			s_Data.QuadVertexBufferPtr->TilingFactor = tilingFactor;
			s_Data.QuadVertexBufferPtr++;
		}

		s_Data.QuadIndexCount += 6;

		s_Data.Stats.QuadCount++;
	}

	Renderer2D::Statistics Renderer2D::GetStats()
	{

		return s_Data.Stats;
	}

	void Renderer2D::ResetStats()
	{
		memset(&s_Data.Stats, 0, sizeof(Statistics));
	}

	void Renderer2D::StartBatch()
	{
		s_Data.QuadIndexCount = 0;
		s_Data.CubeIndexCount = 0;
		s_Data.QuadVertexBufferPtr = s_Data.QuadVertexBufferBase;
		s_Data.TextureSlotIndex = 1;//纹理绑定，0是空白纹理
	}
	
}
