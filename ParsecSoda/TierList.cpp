#include "TierList.h"

void TierList::setTier(uint32_t userID, Tier tier)
{
    vector<GuestTier>::iterator it;
    for (it = _guestTiers.begin(); it != _guestTiers.end(); ++it)
    {
        if (userID == (*it).userID)
        {
            if (tier != Tier::PLEB)
            {
                (*it).tier = tier;
            }
            else
            {
                _guestTiers.erase(it);
            }

            saveTiers();

            return;
        }
    }

    if (tier != Tier::PLEB)
    {
        _guestTiers.push_back(GuestTier(userID, tier));
        saveTiers();
    }
}

Tier TierList::getTier(uint32_t userID)
{
    vector<GuestTier>::iterator it;
    for (it = _guestTiers.begin(); it != _guestTiers.end(); ++it)
    {
        if ((*it).userID == userID)
        {
            return (*it).tier;
        }
    }

    return Tier::PLEB;
}

void TierList::loadTiers()
{
    _guestTiers = MetadataCache::loadGuestTiers();
}

bool TierList::saveTiers()
{
    return MetadataCache::saveGuestTiers(_guestTiers);
}
