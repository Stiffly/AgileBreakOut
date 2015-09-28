#ifndef NAMESPACE_FILENAME_H__
#define NAMESPACE_FILENAME_H__

#include <standard_library>

#include <external_dependency>

#include "Internal/Header.h"

namespace dd
{

template <typename T>
class BaseClassName
{
public:
	BaseClassName(T* child)
		: m_Child(child) { }

	virtual bool PublicMemberFunction(bool value) = 0;

private:
	T* m_Child = nullptr;
};

class ClassName : public BaseClassName<ClassName>
{
public:
	// Constructor
	ClassName(int arg1)
		: BaseClassName(this)
		, m_PrivateMember1(arg1) { }

	// Destructor
	~ClassName()
	{
		if (m_PrivateMember2 != nullptr) {
			delete m_PrivateMember2;
			m_PrivateMember2 = nullptr;
		}
	}

	// Getter
	int& PrivateMember1() const { return m_PrivateMember1; }
	// Setter
	void SetPrivateMember1(const int& privateMember1) { m_PrivateMember1 = privateMember1; }

	// Public member function
	template <typename T>
	bool PublicMemberFunction(bool value) override
	{
		if (!value) {
			return false;
		}

		if (m_PrivateMember2 != nullptr) {
			return true;
		} else {
			return false;
		}
	}

private:
	// Private members with default values
	int m_PrivateMember1 = 1;
	int* m_PrivateMember2 = nullptr;

	// Private member function
	bool privateMemberFunction()
	{
		switch (m_PrivateMember1) {
		case 1:
			m_PrivateMember2 = new ClassName(2);
			break;
		default:
			return false;
		}

		return false;
	}
};

}

#endif