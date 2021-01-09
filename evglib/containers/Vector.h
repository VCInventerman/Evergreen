#pragma once

#include "../common/types.h"
#include "../common/ContiguousBuf.h"

//todo: interfaces: ex. Vector implements Array
//todo: template by whether or not vector owns memory instead of appending View



template <typename T>
class Vector
{
public:
	ContiguousBufPtrEnd<T> data;

	Vector() = default;
	Vector(const ContiguousBufPtrEnd<T>& _data) : data(_data) {}
};

template <typename T>
class VectorView
{
public:
	ContiguousBufPtrEnd<T> data;

	VectorView() = default;
	VectorView(const ContiguousBufPtrEnd<T>& _data) : data(_data) {}
};