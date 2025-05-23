#pragma  once

#include "Core/Render/AdTexture.h"
#include "Core/Render/AdSampler.h"
#include "Platform/AdGraphicContext.h"
#include "entt/core/type_info.hpp"
namespace ade
{
	//Shaader Params
	struct TextureParam
	{
		bool enable;
		alignas(4) float uvRotation{ 0.0f };//对齐是为了和shader的std140保持一致
		alignas(16) glm::vec4 uvTransform{1.0f, 1.0f, 0.0f, 0.0f};//x,y-->scale,z,w-->translation
	};

	struct ModelPC
	{
		alignas(16) glm::mat4 modelMat;
		alignas(16) glm::mat3 normalMat;
	};

	struct TextureView
	{
		AdTexture* texture = nullptr;
		AdSampler* sampler = nullptr;
		bool bEnable = true;
		glm::vec2 uvTranslation{0.f, 0.f};
		float uvRotation{ 0.f };
		glm::vec2 uvScale{1.0f, 1.0f };

		bool IsValid() const
		{
			return bEnable && texture != nullptr && sampler != nullptr;
		}
	};

	class AdMaterial
	{
	public:
		AdMaterial(const AdMaterial&) = delete;
		AdMaterial& operator=(const AdMaterial&) = delete;

		static void UpdateTextureParams(const TextureView* textureView, TextureParam* param)
		{
			param->enable = textureView->IsValid() && textureView->bEnable;
			param->uvRotation = textureView->uvRotation;
			param->uvTransform = { textureView->uvScale.x,textureView->uvScale.y,textureView->uvTranslation.x,textureView->uvTranslation.y };
		}

		int32_t GetIndex() const
		{
			return mIndex;
		}
		bool ShouldFlushParams() const { return bShouldFlushParams; }
		bool ShouldFlushResource() const { return bShouldFlushResource; }
		void FinishFlushParams() { bShouldFlushParams = false; }
		void FinishFlushResource() { bShouldFlushResource = false; }

		bool HasTexture(uint32_t id) const;
		const TextureView* GetTextureView(uint32_t id) const;
		void SetTextureView(uint32_t id, AdTexture* texture, AdSampler* sampler);
		void UpdateTextureViewEnable(uint32_t id, bool enable);
		void UpdateTextureViewUVTranslation(uint32_t id, const glm::vec2& uvTranslation);
		void UpdateTextureViewUVRotation(uint32_t id, float uvRotation);
		void UpdateTextureViewUVScale(uint32_t id, const glm::vec2& uvScale);
	protected:
		AdMaterial() = default;

		bool bShouldFlushParams=false;
		bool bShouldFlushResource=false;//texture 是否发生替换，material 内部设置
	private:
		int32_t mIndex = -1;
		std::unordered_map<uint32_t, TextureView> mTextures;
		friend class AdMaterialFactory;
	};

	class AdMaterialFactory
	{
	public:
		AdMaterialFactory(const AdMaterialFactory&) = delete;
		AdMaterialFactory& operator=(const AdMaterialFactory&) = delete;

		static AdMaterialFactory* GetInstance()
		{
			return &s_MaterialFactory;
		}

		~AdMaterialFactory()
		{
			mMaterials.clear();
		}

		template<typename T>
		size_t GetMaterialSize()
		{
			uint32_t typeId = entt::type_id<T>().hash();
			if (mMaterials.find(typeId)==mMaterials.end())
			{
				return 0;
			}
			return mMaterials[typeId].size();
		}

		template<typename T>
		T* CreateMaterial()
		{
			auto mat = std::make_shared<T>();
			uint32_t typeId = entt::type_id<T>().hash();

			uint32_t index = 0;
			if (mMaterials.find(typeId)==mMaterials.end())
			{
				mMaterials.insert({ typeId,{mat} });
			}
			else
			{
				index = mMaterials[typeId].size();
				mMaterials[typeId].push_back(mat);
			}
			mat->mIndex = index;
			return mat.get();
		}

	private:
		AdMaterialFactory() = default;
		static AdMaterialFactory s_MaterialFactory;
		std::unordered_map<uint32_t, std::vector<std::shared_ptr<AdMaterial>>> mMaterials;
	};
}