#pragma once

class SemVer
{
public:
	Int major = 1; // Max 1023
	Int minor = 0; // Max 2047
	Int patch = 0; // Max 2047

	SemVer() = default; // 1.0.0
	SemVer(const Int _major, const Int _minor, const Int _patch) : major(_major), minor(_minor), patch(_patch) {}

	// Emit a compressed version of this. 
	UInt32 emit()
	{
		
	}

	template <typename T>
	bool isCompat(T&& rhs)
	{
		return major == rhs.major;
	}
};