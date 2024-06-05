#pragma once
#include <unordered_map>
#include <random>

#include <entt/entity/entity.hpp>

typedef std::uint64_t Guid;
typedef std::unordered_map <Guid, entt::entity> EntityMap;
typedef std::unordered_map <entt::entity, Guid> GuidMap;

inline static entt::entity NoEntity() {
	return entt::entity(-1);
}

class Entities {
private:
	EntityMap allEntities;
	GuidMap allEntityGuids;
	std::random_device rd;
	std::mt19937_64 gen;
	std::uniform_int_distribution<Guid> dis;

public:
	Entities();
	entt::registry& GetRegistry();
	entt::entity GetEntity(Guid guid);
	Guid GetEntityGuid(entt::entity);
	bool EntityExists(Guid guid);
	bool EntityExists(entt::entity);
	void DestroyAllEntities();
	void AddEntity(Guid guid, entt::entity entity);
	entt::entity CreateEntity();
	entt::entity CreateEntity(Guid guid);
	void DestroyEntity(entt::entity entity);
	Guid GenerateGuid() {
		return dis(gen);
	}
};