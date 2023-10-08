#pragma once
#include <map>
#include <typeindex>
#include <list>
#include <memory>
#include "Event.h"


class IEventCallback {
private:
	virtual void Call(Event& e) = 0;

public:
	void Execute(Event& e) {
		Call(e);
	}
};

template<typename TOwner, typename TEvent>
class EventCallback : public IEventCallback {
private:
	typedef void (TOwner::* CallbackFunction)(TEvent&);

	TOwner* ownerInstance;
	CallbackFunction callbackFunction;

	virtual void Call(Event& e) override {
		std::invoke(callbackFunction, ownerInstance, static_cast<TEvent&>(e));
	}

public:
	EventCallback(TOwner* ownerInstance, CallbackFunction callbackFunction) {
		this->ownerInstance = ownerInstance;
		this->callbackFunction = callbackFunction;
	}

};

typedef std::list<std::unique_ptr<IEventCallback>> HandlerList;

class EventBus {

private:
	std::map<std::type_index, std::unique_ptr<HandlerList>> subscribers;

public:
	EventBus() {}
	~EventBus() {}

	template<typename TEvent, typename TOwner>
	void ListenToEvent(TOwner* ownerInstance, void(TOwner::* callbackFunction)(TEvent&)) {
		if (!subscribers[typeid(TEvent)].get()) {
			subscribers[typeid(TEvent)] = std::make_unique<HandlerList>();
		}
		auto subscriber = std::make_unique<EventCallback<TOwner, TEvent>>(ownerInstance, callbackFunction);
		subscribers[typeid(TEvent)]->push_back(std::move(subscriber));
	}

	template<typename TEvent, typename ...TArgs>
	void EmitEvent(TArgs&& ...args) {
		if (subscribers[typeid(TEvent)].get()) {
			TEvent event(std::forward<TArgs>(args)...);
			for (auto it = subscribers[typeid(TEvent)]->begin(); it != subscribers[typeid(TEvent)]->end(); it++) {
				it->get()->Execute(event);
			}
		}
	}

	void Reset() {
		subscribers.clear();
	}

};