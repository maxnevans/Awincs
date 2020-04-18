#pragma once

#include "EventDispatcher.h"
#include "DefaultEventHandler.h"
#include "../Events.h"

namespace Awincs
{
	namespace Event
	{
		struct DefaultEventDispatcherContext
		{

		};

		template<typename GHandler>
		class DefaultEventDispatcher
			:
			public EventDispatcher<DefaultEventHandler<GHandler>>
		{
		public:
			void setupContext(DefaultEventDispatcherContext ctx)
			{
			}
		protected:
			virtual bool checkEventType(const CoreEvent& e)
			{
				return true;
			}
			virtual std::shared_ptr<DefaultEventHandler<GHandler>> getNextEventHandler(const CoreEvent& e) override
			{
				// TODO: create iterator that begin from tree crown
				// Reason: not efficient to use vector

				if (current != potentialHandlers.end())
				{
					auto handler = *current;
					current++;

					return handler;
				}

				return nullptr;
			}
			virtual void beforeHandling(const CoreEvent& e) override
			{
				// TODO: create iterator that begin from tree crown
				// Reason: not efficient to use vector

				if (!this->root)
				{
					potentialHandlers = {};
					current = potentialHandlers.end();
					return;
				}

				potentialHandlers = determinePotentialHandlers(this->root, e);
				current = potentialHandlers.begin();
			}
			virtual void afterHandling(const CoreEvent& e) override
			{
				// TODO: create iterator that begin from tree crown
				// Reason: not efficient to use vector

				potentialHandlers.erase(potentialHandlers.begin());
				current = potentialHandlers.end();
			}

		private:
			std::vector<std::shared_ptr<DefaultEventHandler<GHandler>>> determinePotentialHandlers(
				const std::shared_ptr<DefaultEventHandler<GHandler>>& handler, const Event::CoreEvent& e)
			{
				// TODO: create iterator that begin from tree crown
				// Reason: not efficient to use vector

				std::vector<std::shared_ptr<DefaultEventHandler<GHandler>>> potentialHandlers = {};

				for (const auto& child : handler->getChildren())
				{
					auto childHandlers = determinePotentialHandlers(child, e);

					/* Copy by inserting back with optimization */
					if (potentialHandlers.size() > childHandlers.size())
					{
						potentialHandlers.reserve(std::max(potentialHandlers.capacity(), potentialHandlers.size() + childHandlers.size()));
						potentialHandlers.insert(potentialHandlers.end(), childHandlers.begin(), childHandlers.end());
					}
					else
					{
						childHandlers.reserve(std::max(childHandlers.capacity(), potentialHandlers.size() + childHandlers.size()));
						childHandlers.insert(childHandlers.end(), potentialHandlers.begin(), potentialHandlers.end());
						potentialHandlers = std::move(childHandlers);
					}
				}

				return potentialHandlers;
			}

		public:
			static inline typename EventDispatcher<GHandler>::DispatcherTypeHash typeId = 0;

		private:
			std::vector<std::shared_ptr<DefaultEventHandler<GHandler>>> potentialHandlers = {};
			typename std::vector<std::shared_ptr<DefaultEventHandler<GHandler>>>::iterator current = potentialHandlers.end();
		};
	}
}