#pragma once

#include "GuestData.h"
#include "Stringer.h"
#include <functional>
#include <vector>

using namespace std;

class GuestDataList
{
public:
	GuestDataList();
	GuestDataList(const std::vector<GuestData> guests);
	bool add(GuestData guest);
	bool find(uint32_t userID, function<void(GuestData& guest)> callback = nullptr);
	bool find(string guestName, function<void(GuestData& guest)> callback = nullptr);
	bool pop(uint32_t userID, function<void(GuestData& guest)> callback = nullptr);
	bool pop(string guestName, function<void(GuestData& guest)> callback = nullptr);
	vector<GuestData>& getGuests();

protected:
	bool findIterator(uint32_t userID, function<void(vector<GuestData>::iterator guest)> callback = nullptr);
	bool findIterator(string guestName, function<void(vector<GuestData>::iterator guest)> callback = nullptr);
	vector<GuestData> _guests;
};

