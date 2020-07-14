#pragma once

#include <stdint.h>
#include <cstddef>

namespace dm
{

std::byte* VPtr_Translate(uint32_t pointer, uint32_t size);

template<typename T>
class VPtr
{
public:
	VPtr(uint32_t ptr)
		: ptr(ptr)
	{}

	bool operator==(std::nullptr_t rhs)
	{
		return ptr == 0;
	}

	template<typename T2>
	bool operator==(const VPtr<T2> rhs) const
	{
		return ptr == rhs.ptr;
	}

	template<typename T2>
	bool operator!=(const VPtr<T2> rhs) const
	{
		return ptr != rhs.ptr;
	}

	T& operator*()
	{
		if (ptr == 0) // https://www.youtube.com/watch?v=bLHL75H_VEM
			throw;

		return *reinterpret_cast<T*>(VPtr_Translate(ptr, sizeof(T)));
	}

	T* operator->()
	{
		return reinterpret_cast<T*>(VPtr_Translate(ptr, sizeof(T)));
	}

	T& operator[](size_t index)
	{
		if (ptr == 0) // https://www.youtube.com/watch?v=bLHL75H_VEM
			throw;

		return *reinterpret_cast<T*>(VPtr_Translate(ptr + index * sizeof(T), sizeof(T)));
	}

	operator bool()
	{
		return ptr != 0;
	}

	// TODO: Should provide a count for when the ptr is expected to be an array
	T* get()
	{
		if (ptr == 0) // https://www.youtube.com/watch?v=bLHL75H_VEM
			return nullptr;

		return reinterpret_cast<T*>(VPtr_Translate(ptr, sizeof(T)));
	}

private:
	uint32_t ptr;
};

static_assert(sizeof(VPtr<uint32_t>) == sizeof(uint32_t));

}