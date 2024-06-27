#include "Reflection.h"

#include "../Core/Guid.h"

ReflectionSystem* ReflectionSystem::instance;

InfoTypes InfoBase::GetInfoType(const type_info& type)
{
	if(type.hash_code() == typeid(std::string).hash_code())
	{
		return InfoTypes::STRING;
	}
	if(type.hash_code() == typeid(int).hash_code())
	{
		return InfoTypes::INT;
	}
	if(type.hash_code() == typeid(bool).hash_code())
	{
		return InfoTypes::BOOL;
	}
	if(type.hash_code() == typeid(float).hash_code())
	{
		return InfoTypes::FLOAT;
	}
	if(type.hash_code() == typeid(glm::vec4).hash_code())
	{
		return InfoTypes::COLOR;
	}
	if(type.hash_code() == typeid(glm::vec2).hash_code())
	{
		return InfoTypes::VEC2;
	}
	if(type.hash_code() == typeid(Guid).hash_code())
	{
		return InfoTypes::GUID;
	}
	return InfoTypes::UNKNOWN;
}

int InfoBase::GetInfoProps()
{
	return InfoProps::ALL;
}
int InfoBase::GetInfoProps(int props)
{
	return props;
}

void* InfoBase::GetVar(void* object, const std::string& name) const
{
	uint8_t* res = (uint8_t*)object;
	if(varOffset.find(name) != varOffset.end())
	{
		return res + varOffset.at(name);
	}
	return nullptr;
}

InfoTypes InfoBase::GetType(const std::string& name) const
{
	if(varTypes.find(name) != varTypes.end())
	{
		return varTypes.at(name);
	}
	return InfoTypes::UNKNOWN;
}

int InfoBase::GetProps(const std::string& name) const
{
	if(varProps.find(name) != varProps.end())
	{
		return varProps.at(name);
	}
	return InfoProps::NONE;
}

const std::vector<std::string>& InfoBase::GetVarNames() const
{
	return vars;
}

