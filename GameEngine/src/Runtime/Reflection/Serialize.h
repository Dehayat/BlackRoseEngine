#pragma once
#include "glm/glm.hpp"
#include "ryml/ryml.hpp"
#include "ryml/ryml_std.hpp"
#include "Core/Systems.h"

#include "Reflection/Reflection.h"

template<typename TData>
void WriteField(InfoBase* info, void* obj, const std::string& field, ryml::NodeRef node)
{
	auto val = (TData*)info->GetVar(obj, field);
	node[c4::to_csubstr(field.c_str())] << *val;
}

inline void WriteVector(InfoBase* info, void* obj, const std::string& field, ryml::NodeRef node)
{
	node[c4::to_csubstr(field.c_str())] |= ryml::SEQ;
	auto val = (glm::vec2*)info->GetVar(obj, field);
	node[c4::to_csubstr(field.c_str())].append_child() << val->x;
	node[c4::to_csubstr(field.c_str())].append_child() << val->y;
}

inline void DefaultSerialize(InfoBase* info, void* data, ryml::NodeRef node)
{
	node |= ryml::MAP;
	for(auto& field : info->GetVarNames())
	{
		switch(info->GetType(field))
		{
		case InfoTypes::STRING:
			WriteField<std::string>(info, data, field, node);
			break;
		case InfoTypes::INT:
			WriteField<int>(info, data, field, node);
			break;
		case InfoTypes::BOOL:
			WriteField<bool>(info, data, field, node);
			break;
		case InfoTypes::FLOAT:
			WriteField<float>(info, data, field, node);
			break;
		case InfoTypes::VEC2:
			WriteVector(info, data, field, node);
			break;
		default:
			break;
		}
	}
}

template<typename TData>
void ReadField(InfoBase* info, void* obj, const std::string& field, ryml::NodeRef node)
{
	auto val = (TData*)info->GetVar(obj, field);
	if(node.has_child(c4::to_csubstr(field.c_str())))
	{
		node[c4::to_csubstr(field.c_str())] >> *val;
	}
}

inline void ReadVector(InfoBase* info, void* obj, const std::string& field, ryml::NodeRef node)
{
	auto val = (glm::vec2*)info->GetVar(obj, field);
	if(node.has_child(c4::to_csubstr(field.c_str())))
	{
		node[c4::to_csubstr(field.c_str())][0] >> val->x;
		node[c4::to_csubstr(field.c_str())][1] >> val->y;
	}
}

inline void DefaultDeserialize(InfoBase* info, void* data, ryml::NodeRef node)
{
	node |= ryml::MAP;
	for(auto& field : info->GetVarNames())
	{
		switch(info->GetType(field))
		{
		case InfoTypes::STRING:
			ReadField<std::string>(info, data, field, node);
			break;
		case InfoTypes::INT:
			ReadField<int>(info, data, field, node);
			break;
		case InfoTypes::BOOL:
			ReadField<bool>(info, data, field, node);
			break;
		case InfoTypes::FLOAT:
			ReadField<float>(info, data, field, node);
			break;
		case InfoTypes::VEC2:
			ReadVector(info, data, field, node);
			break;
		default:
			break;
		}
	}
}

#define ROSE_SER(TYPE)	ROSE_INIT_VARS(TYPE);											\
						auto info = ROSE_GETSYSTEM(ReflectionSystem).GetInfo<TYPE>();	\
						DefaultSerialize(info, this, node)

#define ROSE_DESER(TYPE)	ROSE_INIT_VARS(TYPE);											\
							auto info = ROSE_GETSYSTEM(ReflectionSystem).GetInfo<TYPE>();	\
							DefaultDeserialize(info, this, node)