#include "Guid.h"

std::random_device GuidGenerator::rd;
std::mt19937_64 GuidGenerator::gen;
std::uniform_int_distribution<Guid> GuidGenerator::dis;

Guid GuidGenerator::New() {
	gen = std::mt19937_64(rd());
	return dis(gen);
}
