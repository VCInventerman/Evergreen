#pragma once

template <class T>
class Pointer
{
public:
	T* 
};

template <class T>
using Ptr = Pointer<T>;

template <class T>
using CPtr = const Pointer<T>;

