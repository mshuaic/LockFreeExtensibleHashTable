/*
C++ AtomicMarkableReference
Credit to Tsyvarev
https://stackoverflow.com/a/40251680

*/

#pragma once

#include <cstdint>

template<class T>
class MarkableReference
{
private:
	uintptr_t val = 0;
	static const uintptr_t mask = 1;
public:

	MarkableReference(T ref= NULL, bool mark = false)
	{
		val = ((uintptr_t)ref & ~mask) | (mark ? 1 : 0);
	}
	T getRef(void)
	{
		return (T)(val & ~mask);
	}
	bool getMark(void)
	{
		return (val & mask);
	}
	
	T get(bool* marked)
	{
		*marked = val & mask;
		return (T)(val & ~mask);
	}


	T operator->()
	{
		return (T)(val & ~mask);
	}

	// Access contained value
	// Returns the stored value by val.
	// This is a type-cast operator
	operator T() const
	{
		return (T)(val & ~mask);
	}

};