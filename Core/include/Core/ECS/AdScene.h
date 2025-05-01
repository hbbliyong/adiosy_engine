#pragma once
#include "Core/ECS/AdUUID.h"
#include "entt/src/entt/entity/registry.hpp"
namespace ade
{
	class AdNode;
	class AdEntity;

	class AdScene
	{
	public:
	private:
		std::string mName;
		entt::registry mEcsRegistry;

		std::unordered_map<entt::entity, std::shared_ptr<AdEntity>> mEntities;
		std::shared_ptr<AdNode> mRootNode;

		friend class AdEntity;
	};
}