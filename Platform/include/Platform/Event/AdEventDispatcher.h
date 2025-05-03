#pragma once

#include "Platform/Event/AdMouseEvent.h"
#include "Platform/Event/AdWindowEvent.h"
namespace ade
{
	class AdEventObserver;
	using Func_EventHandle = std::function<void(AdEvent& e)>;

	struct EventHandler
	{
		AdEventObserver* observer;
		Func_EventHandle func;
	};

	class AdEventDispatcher
	{
	public:
		AdEventDispatcher(const AdEventDispatcher&) = delete;
		AdEventDispatcher& operator= (const AdEventDispatcher&) = delete;
		~AdEventDispatcher();

		static AdEventDispatcher* GetInstance() { return &s_Instance; }
		template<typename T>
		void AddObserverHandler(AdEventObserver* observer, const std::function<void(const T&)>& func)
		{
			if (!observer || !func)
			{
				return;
			}

			auto eventFunc = [func](const AdEvent& e)
			{
				const T& event = static_cast<const T&>(e);
				return func(event);
			};

			EventHandler handler{
				.observer = observer,
				.func = eventFunc
			};
			mObserverHandlerMap[T::GetStaticType()].emplace_back(handler);
		}

		void DestroyObserver(AdEventObserver* observer)
		{
			for (auto& mapIt : mObserverHandlerMap)
			{
				mapIt.second.erase(std::remove_if(mapIt.second.begin(), mapIt.second.end(), [observer](const EventHandler& handler)
					{
						return (handler.observer && handler.observer == observer);
					}), mapIt.second.end());
			}
		}

		void Dispatch(AdEvent& event);
	private:
		AdEventDispatcher() = default;


	private:
		std::unordered_map<AdEventType, std::vector< EventHandler>> mObserverHandlerMap;
		static AdEventDispatcher s_Instance;
	};
}