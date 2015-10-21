#ifndef DD_STATICSYSTEM_H__
#define DD_STATICSYSTEM_H__

#include <unordered_map>

namespace dd
{

class StaticSystem
{
private:
	StaticSystem();

public:
	template <typename T>
	static T* Get() 
	{
		return static_cast<T*>(Instances.at(typeid(T).hash_code()));
	}

	template <typename T>
	static void Set(T* ptr)
	{
		Instances[typeid(T).hash_code()] = static_cast<void*>(ptr);
	}


private:
	static std::unordered_map<size_t, void*> Instances;
};

}

#endif