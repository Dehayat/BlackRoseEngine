#include "Components/TransformComponent.h"
#include "Reflection/Serialize.h"

#include "Core/Systems.h"

#include "Core/Log.h"

using namespace glm;

TransformComponent::TransformComponent(vec2 position, vec2 scale, float rotation)
{
	this->position = position;
	this->scale = scale;
	this->rotation = rotation;

	this->parent = NoEntity();
	this->scaleSign = vec2(1,1);
	this->matrixL2W = mat3(0);
	globalPosition = vec2();
	globalScale = vec2();
	globalRotation = 0;
	level = 0;
}

TransformComponent::TransformComponent(ryml::NodeRef& node)
{
	this->position = vec2(0, 0);
	this->scale = vec2(1, 1);
	this->rotation = 0;

	this->scaleSign = vec2(1, 1);
	this->parent = NoEntity();
	this->matrixL2W = mat3(0);
	globalPosition = vec2();
	globalScale = vec2();
	globalRotation = 0;
	level = 0;

	ROSE_DESER(TransformComponent);
}

void TransformComponent::Serialize(ryml::NodeRef node)
{
	ROSE_SER(TransformComponent);
}

void TransformComponent::CalcMatrix()
{
	auto matT = mat3(
		1, 0, position.x,
		0, 1, position.y,
		0, 0, 1
	);
	rotation = glm::mod(rotation + 360, 360.0f);
	auto cosT = cos(glm::radians(rotation));
	auto sinT = sin(glm::radians(rotation));
	auto matR = mat3(
		cosT, -sinT, 0,
		sinT, cosT, 0,
		0, 0, 1
	);
	auto matS = mat3(
		scale.x, 0, 0,
		0, scale.y, 0,
		0, 0, 1
	);
	matrixL2W = matS * matR * matT;
	if(parent!=NoEntity())
	{
		entt::registry& registry = ROSE_GETSYSTEM(EntitySystem).GetRegistry();
		auto matrixP2W = registry.get<TransformComponent>(parent).matrixL2W;
		matrixL2W = matrixL2W * matrixP2W;
	}
}

void TransformComponent::UpdateGlobals()
{
	CalcMatrix();
	globalPosition = GetPosition(matrixL2W);
	globalScale = GetScale(matrixL2W);
	globalRotation = GetRotation(matrixL2W);
	globalRotation = glm::mod(globalRotation + 360, 360.0f);
	scaleSign = vec2(sign(scale.x), sign(scale.y));
	if(parent!=NoEntity())
	{
		auto& pTrx = ROSE_GETSYSTEM(EntitySystem).GetRegistry().get<TransformComponent>(parent);
		scaleSign = pTrx.scaleSign * scaleSign;
	}
}

void TransformComponent::UpdateLocals()
{
	globalRotation = glm::mod(globalRotation + 360, 360.0f);
	if(parent!=NoEntity())
	{
		auto& pTrx = ROSE_GETSYSTEM(EntitySystem).GetRegistry().get<TransformComponent>(parent);
		auto matW2P = inverse(pTrx.matrixL2W);

		position = GetPosition(matW2P, globalPosition);

		auto matGR = MakeRotMatrix(globalRotation);
		auto matLR = matGR * matW2P * MakeScaleMatrix(scale);
		rotation = GetRotation(matLR);
		rotation = glm::mod(rotation + 360, 360.0f);

		auto oldGlobalScale = GetScale(matrixL2W);
		auto scaleChange = globalScale / oldGlobalScale;
		scale = scale * scaleChange;
	} else
	{
		position = globalPosition;

		auto matGR = MakeRotMatrix(globalRotation);
		auto matLR = matGR * MakeScaleMatrix(scale);
		rotation = GetRotation(matLR);
		rotation = glm::mod(rotation + 360, 360.0f);

		globalScale = scale;
	}
	CalcMatrix();
}

float TransformComponent::GetRotation(mat3 matrix, float angle)
{
	auto cosT = cos(glm::radians(angle));
	auto sinT = sin(glm::radians(angle));
	auto matR = mat3(
		cosT, -sinT, 0,
		sinT, cosT, 0,
		0, 0, 1
	);
	matrix = matR * matrix;
	auto dirVec = vec2(matrix[1][0], matrix[0][0]);
	dirVec = normalize(dirVec);
	auto rotation = glm::degrees(std::atan2f(dirVec.x, dirVec.y));
	rotation = mod(rotation + 360, 360.0f);
	return rotation;
}
float TransformComponent::GetRotation(mat3 matrix)
{
	auto dirVec = vec2(matrix[1][0], matrix[0][0]);
	dirVec = normalize(dirVec);
	auto rotation = glm::degrees(std::atan2f(dirVec.x, dirVec.y));
	rotation = mod(rotation + 360, 360.0f);
	return rotation;
}
vec2 TransformComponent::GetPosition(mat3 matrix, vec2 localPos)
{
	auto pos = vec3(localPos, 1);
	return pos * matrix;
}
vec2 TransformComponent::GetDir(mat3 matrix, vec2 localDir)
{
	auto dir = vec3(localDir, 0);
	return dir * matrix;
}
mat3 TransformComponent::MakeRotMatrix(float angle)
{
	auto cosT = cos(glm::radians(angle));
	auto sinT = sin(glm::radians(angle));
	return mat3(
		cosT, -sinT, 0,
		sinT, cosT, 0,
		0, 0, 1
	);
}
mat3 TransformComponent::MakeScaleMatrix(vec2 scale)
{
	return mat3(
		scale.x, 0, 0,
		0, scale.y, 0,
		0, 0, 1
	);
}
vec2 TransformComponent::GetScale(mat3 matrix)
{
	auto scale = glm::vec2(glm::sqrt(matrix[0][0] * matrix[0][0] + matrix[1][0] * matrix[1][0]), glm::sqrt(matrix[0][1] * matrix[0][1] + matrix[1][1] * matrix[1][1]));
	return scale;
}