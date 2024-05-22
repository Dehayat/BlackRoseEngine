#pragma once

#include <entt/locator/locator.hpp>

#define CREATESYSTEM(SYSTEM,...) entt::locator<SYSTEM>::emplace<SYSTEM>(__VA_ARGS__)
#define GETSYSTEM(SYSTEM) entt::locator<SYSTEM>::value()