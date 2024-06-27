#pragma once
#include <map>
#include <vector>
#include <string>
#include <type_traits>
#include <typeinfo>
#include <glm/glm.hpp>

enum class InfoTypes
{
	INT,
	BOOL,
	STRING,
	COLOR,
	FLOAT,
	VEC2,
	GUID,

	UNKNOWN
};


namespace InfoProps
{
	enum
	{
		NONE = 0,
		SER = 1 << 0,
		EDITOR = 1 << 1,
		ALL = SER | EDITOR
	};
}

class InfoBase
{
protected:
	std::vector<std::string> vars;
	std::map<std::string, uint64_t> varOffset;
	std::map<std::string, InfoTypes> varTypes;
	std::map<std::string, int> varProps;

	static InfoTypes GetInfoType(const type_info& type);
	static int GetInfoProps();
	static int GetInfoProps(int props);

public:
	void* GetVar(void* object, const std::string& name) const;
	InfoTypes GetType(const std::string& name) const;
	int GetProps(const std::string& name) const;
	const std::vector<std::string>& GetVarNames() const;
};

class ReflectionSystem
{
	std::map<size_t, InfoBase*> modInfo;
public:
	static ReflectionSystem* instance;
	ReflectionSystem()
	{
		instance = this;
	}
	~ReflectionSystem()
	{
		for(auto& info : modInfo)
		{
			delete info.second;
		}
	}
	template<typename T, typename TInfo>
	void AddInfo()
	{
		static_assert(std::is_base_of<InfoBase, TInfo>::value, "TInfo must inherit from InfoBase");
		if(modInfo.find(typeid(T).hash_code()) != modInfo.end())
		{
			return;
		}
		modInfo[typeid(T).hash_code()] = new TInfo();
	}
	template<typename T>
	InfoBase* GetInfo()
	{
		return modInfo[typeid(T).hash_code()];
	}
};


template<typename T>
class Reflector
{
	InfoBase* info;
public:
	Reflector()
	{
		info = ReflectionSystem::instance->GetInfo<T>();
	}
	void* GetVar(void* object, std::string name) const
	{
		return info->GetVar(object, name);
	}
	InfoTypes GetType(std::string name) const
	{
		return info->GetType(name);
	}
	int GetProps(std::string name) const
	{
		return info->GetProps(name);
	}
	const std::vector<std::string>& GetVarNames() const
	{
		return info->GetVarNames();
	}
};


#define ROSE_INIT_VARS(T) ReflectionSystem::instance->AddInfo<T, T::T##Info>()



#define ROSE_REFLECT_LOOP(seq) ROSE_REFLECT_END(ROSE_REFLECT_A seq)
#define ROSE_REFLECT_BODY(x,...)	vars.push_back(#x);										\
									varOffset[#x] = ((uint8_t*)&data.x) - ((uint8_t*)&data);\
									varTypes[#x] = GetInfoType(typeid(data.x));				\
									varProps[#x] = GetInfoProps(__VA_ARGS__);
#define ROSE_REFLECT_A(x,...) ROSE_REFLECT_BODY(x,__VA_ARGS__) ROSE_REFLECT_B
#define ROSE_REFLECT_B(x,...) ROSE_REFLECT_BODY(x,__VA_ARGS__) ROSE_REFLECT_A
#define ROSE_REFLECT_A_ROSE_REFLECT_END
#define ROSE_REFLECT_B_ROSE_REFLECT_END
#define ROSE_REFLECT_END(...) ROSE_REFLECT_END_(__VA_ARGS__)
#define ROSE_REFLECT_END_(...) __VA_ARGS__##_ROSE_REFLECT_END

#define ROSE_EXPOSE_VARS(TName,...)	public:						   \
							class TName ## Info : public InfoBase{ \
							public:								   \
								TName ## Info(){				   \
									TName data;					   \
									ROSE_REFLECT_LOOP(__VA_ARGS__) \
								}								   \
							};

//Macro expansion
//public:
//	class TestComponentInfo :public InfoBase {
//	public:
//		TestComponentInfo() {
//			TestComponent data;
//			vars.push_back("isStatic");
//			varOffset["isStatic"] = ((uint8_t*)&data.isStatic) - ((uint8_t*)&data);
//			varTypes["isStatic"] = GetInfoType(typeid(data.isStatic));
//		}
//	};
