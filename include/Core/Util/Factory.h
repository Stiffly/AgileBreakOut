/*
	This file is part of Daydream Engine.
	Copyright 2014 Adam Byléhn, Tobias Dahl, Simon Holmberg, Viktor Ljung
	
	Daydream Engine is free software: you can redistribute it and/or modify
	it under the terms of the GNU Lesser General Public License as published by
	the Free Software Foundation, either version 3 of the License, or
	(at your option) any later version.
	
	Daydream Engine is distributed in the hope that it will be useful,
	but WITHOUT ANY WARRANTY; without even the implied warranty of
	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
	GNU Lesser General Public License for more details.
	
	You should have received a copy of the GNU Lesser General Public License
	along with Daydream Engine.  If not, see <http://www.gnu.org/licenses/>.
*/

#ifndef ComponentFactory_h__
#define ComponentFactory_h__

#include <string>
#include <memory>
#include <functional>
#include <map>

namespace dd
{

/** Generic object factory. 
	
	Creates dynamically allocated objects of base type T.
	@tparam T Object type.
*/
template <typename T>
class Factory
{
public:
	/*void Register(std::string name, std::function<T(void)> factoryFunction)
	{
		m_FactoryFunctions[name] = factoryFunction;
	}*/

	/** Register a custom factory function for a type with the factory.

		@tparam T2 Object type of base type T.
		@param factoryFunction The lambda factory function.
	*/
	template <typename T2>
	void Register(std::function<T*(void)> factoryFunction)
	{
		m_FactoryFunctions[typeid(T2).name()] = factoryFunction;
	}

	/** Register a type with the factory.

		@tparam T2 Object type of base type T.
	*/
	template <typename T2>
	void Register()
	{
		m_FactoryFunctions[typeid(T2).name()] = []() { return new T2(); };
		m_CopyFunctions[typeid(T2).name()] = [](const T* t) { return new T2(*static_cast<const T2*>(t)); };
	}

	/** Create an instance of a specific typename.

		@param name Typename of type of base type T, as a string.
		@returns Pointer to the new object of type T.
	*/
	T* Create(std::string name)
	{
		auto it = m_FactoryFunctions.find(name);
		if (it != m_FactoryFunctions.end())
		{
			return it->second();
		}
		else
		{
			return nullptr;
		}
	}

	/** Create an instance of a specific type.

		@tparam T2 Type of base type T.
		@returns Pointer to the new object of type T.
	*/
	template <typename T2>
	T* Create()
	{
		auto it = m_FactoryFunctions.find(typeid(T2).name());
		if (it != m_FactoryFunctions.end())
		{
			return it->second();
		}
		else
		{
			return nullptr;
		}
	}

	/** Create a copy of an existing object of a specific _type_ registered with the factory.

		@param name Typename of type of base type T, as a string.
		@param toCopy The existing object to copy.
		@returns Pointer to the new object of type T2.
	*/
	T* Copy(std::string name, const T* toCopy)
	{
		auto it = m_CopyFunctions.find(name);
		if (it != m_CopyFunctions.end())
		{
			return it->second(toCopy);
		} 
		else
		{
			return nullptr;
		}
	}

	/** Create a copy of an existing object of a specific _typename_ registered with the factory.

		@tparam T2 Type of base type T.
		@param toCopy The existing object to copy.
		@returns Pointer to the new object of type T2.
	*/
	template <typename T2>
	T* Copy(const T* toCopy)
	{
		auto it = m_CopyFunctions.find(typeid(T2).name());
		if (it != m_CopyFunctions.end())
		{
			return it->second(toCopy);
		} else
		{
			return nullptr;
		}
	}

private:
	std::map<std::string, std::function<T*(void)>> m_FactoryFunctions;
	std::map<std::string, std::function<T*(const T*)>> m_CopyFunctions;
};

}

#endif // ComponentFactory_h__