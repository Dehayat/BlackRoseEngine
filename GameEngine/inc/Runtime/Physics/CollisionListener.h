#pragma once
#include <box2d/b2_world_callbacks.h>

class ContactListener :public b2ContactListener {
	virtual void BeginContact(b2Contact* contact) override;
	virtual void EndContact(b2Contact* contact) override;
};