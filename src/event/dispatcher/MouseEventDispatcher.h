#pragma once

#include "EventDispatcher.h"
#include "../Events.h"
#include "MouseEventHandler.h"

namespace Awincs
{
	namespace Event
	{
		struct MouseEventDispatcherContext
		{

		};
		template<typename GHandler>
		class MouseEventDispatcher
			:
			public EventDispatcher<MouseEventHandler<GHandler>>
		{
		public:
			void setupContext(MouseEventDispatcherContext ctx)
			{

			}
		protected:
			virtual bool checkEventType(const CoreEvent& e) override
			{
				if (auto me = dynamic_cast<const Mouse::Event*>(&e))
				{
					mouseEvent = *me;
					return true;
				}

				return false;
			}
			virtual std::shared_ptr<MouseEventHandler<GHandler>> getNextEventHandler(const CoreEvent& e) override
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


				if (!this->root->checkMousePoint(mouseEvent->point))
				{
					potentialHandlers = {};
					current = potentialHandlers.end();
					return;
				}

				potentialHandlers = determinePotentialHandlers(this->root, *mouseEvent);
				current = potentialHandlers.begin();
			}
			virtual void afterHandling(const CoreEvent& e) override
			{
				// TODO: create iterator that begin from tree crown
				// Reason: not efficient to use vector

				mouseEvent = std::nullopt;
				potentialHandlers.erase(potentialHandlers.begin());
				current = potentialHandlers.end();
			}

		private:
			std::vector<std::shared_ptr<MouseEventHandler<GHandler>>> determinePotentialHandlers(
				const std::shared_ptr<MouseEventHandler<GHandler>>& handler, const Mouse::Event& e)
			{
				// TODO: create iterator that begin from tree crown
				// Reason: not efficient to use vector

				std::vector<std::shared_ptr<MouseEventHandler<GHandler>>> potentialHandlers = {};

				for (const auto& child : handler->getChildren())
				{
					if (child->checkMousePoint(e.point))
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

						break;
					}
				}

				return potentialHandlers;
			}

		public:
			static inline typename EventDispatcher<GHandler>::DispatcherTypeHash typeId = 0;

		private:
			std::optional<Mouse::Event> mouseEvent = std::nullopt;
			std::vector<std::shared_ptr<MouseEventHandler<GHandler>>> potentialHandlers = {};
			typename std::vector<std::shared_ptr<MouseEventHandler<GHandler>>>::iterator current = potentialHandlers.end();
		};
	}
}