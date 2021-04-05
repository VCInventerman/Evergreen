#pragma once

class Object
{
public:
	using This = Object;

public: // Access is discouraged
	virtual String name() { return "Unnamed Object"; }

public:
	// virtual type_info* type() { return typeid(This); }
};