#pragma once

#include "GuestData.h"
#include "Tier.h"

class GuestTier
{
public:
	GuestTier(uint32_t userID, Tier tier) {
		this->userID = userID;
		this->tier = tier;
	}
	uint32_t userID;
	Tier tier;
};

