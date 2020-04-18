#pragma once

#include "pch.h"

#include "dispatcher/EventDispatcher.h"

namespace Awincs
{
	namespace Event
	{
		template<typename GHandler>
		class EventManager
		{
		public:
			EventManager(std::shared_ptr<GHandler> root)
				:
				root(root)
			{
			}

			template<typename GEvent>
			void dispatch(GEvent e)
			{
				for (const auto& dispatcher : dispatchers)
					if (!dispatcher.second->dispatch(e))
						break;
			}

			template<typename GContext, typename GEventDispatcher>
			void setupContext(const GContext& ctx)
			{
				auto findable = dispatchers.find(GEventDispatcher::typeId);
				expect(findable != dispatchers.end());
				auto dispatcher = *findable;

				dispatcher.setupContext(ctx);
			}

			template<typename GDispatcher>
			void registerDispatcher()
			{
				GDispatcher::typeId = typeid(GDispatcher).hash_code();
				dispatchers[GDispatcher::typeId] = std::static_pointer_cast<EventDispatcher<GHandler>>(std::make_shared<GDispatcher>(root));
			}

		private:
			std::shared_ptr<GHandler> root = nullptr;
			std::map<typename EventDispatcher<GHandler>::DispatcherTypeHash, std::shared_ptr<EventDispatcher<GHandler>>> dispatchers;
		};
	}
}