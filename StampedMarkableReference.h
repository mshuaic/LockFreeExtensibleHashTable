#ifndef STAMPEDREFERENCE_H
#define STAMPEDREFERENCE_H

#include <cstdint>
#if SIZE_MAX == UINT32_MAX
#error "USE 64BIT SYSTEM"
#elif SIZE_MAX == UINT64_MAX
#define SIZE 48
#define STAMP_MASK 0xFFFF000000000000
#define MARK_MASK 0x1
#endif

#include <cstddef>
//#define size_t size_t//unsigned short int

template<class T>
class StampedMarkableReference
{
private:
	uintptr_t val = 0;
	static const uintptr_t stampMask = STAMP_MASK;
	static const uintptr_t markMask = MARK_MASK;
	static const unsigned int shift = SIZE;
public:
	StampedMarkableReference(T ref, bool mark)
	{
		val = ((uintptr_t)ref & ~stampMask & ~markMask)  | (mark ? 1 : 0);
	}

	StampedMarkableReference(T ref = NULL, size_t stamp = 0, bool mark = false)
	{
		val = ((uintptr_t)ref & ~stampMask & ~markMask) | (stamp << shift) | (mark ? 1 : 0);
	}
	T getRef(void)
	{
		return (T)(val & ~stampMask & ~markMask);
	}

	T getStamp(size_t* stamp)
	{
		*stamp = (val & stampMask) >> shift;
		return (T)(val & ~stampMask & ~markMask);
	}

	T getMark(bool* mark)
	{
		*mark = val & markMask == 1 ? true : false;
		return (T)(val & ~stampMask & ~markMask);
	}

	T get(size_t* stamp, bool* mark)
	{
		*stamp = (val & stampMask) >> shift;
		*mark = val & markMask == 1 ? true:false;
		return (T)(val & ~stampMask & ~markMask);
	}

	void setVal(T ref)
	{
		this->val = ((uintptr_t)ref & ~stampMask & ~markMask);
	}

	T operator->()
	{
		return (T)(val & ~stampMask & ~markMask);
	}

	// Access contained value
	// Returns the stored value by val.
	// This is a type-cast operator
	operator T() const
	{
		return (T)(val & ~stampMask & ~markMask);
	}

};

#endif