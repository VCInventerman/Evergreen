#pragma once

#include "evergreen/common/SemVer.h"

// An asset is a binary resource. It is identified by a unique UUID that stays the same regardless of version. Versions are associated with a specific hash of the data.
//TODO: File checking
class Asset
{
public:
	SemVer version;
	Hash hash;
};