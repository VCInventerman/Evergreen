#pragma once

template <class T>
class Ptr
{
public:
	T* 
};

template <class T>
using CPtr = const Ptr<T>;

template <class T>
using Ref = T&;

template <class T>
using CRef = const T&;