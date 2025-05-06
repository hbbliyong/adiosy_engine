#pragma  once

#include "Core/Render/AdTexture.h"
#include "Core/Render/AdSampler.h"
#include "Platform/AdGraphicContext.h"
#include "entt/core/type_info.hpp"
namespace ade
{
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
	protected:
		AdMaterial() = default;
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