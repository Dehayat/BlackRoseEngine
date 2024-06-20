#pragma once

#include <entt/locator/locator.hpp>

#define ROSE_CREATESYSTEM(SYSTEM,...)	if(!entt::locator<SYSTEM>::has_value()) {				\
										entt::locator<SYSTEM>::emplace<SYSTEM>(__VA_ARGS__);	\
									}															\
									static_assert(true, "")
#define ROSE_DESTROYSYSTEM(SYSTEM,...)	entt::locator<SYSTEM>::reset()
#define ROSE_GETSYSTEM(SYSTEM) entt::locator<SYSTEM>::value()