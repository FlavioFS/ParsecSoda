#include "HotseatManager.h"

HotseatManager::HotseatManager() : m_timerDurationSec(MetadataCache::preferences.hotseatDurationSeconds)
{
}

bool HotseatManager::enqueue(const HotseatGuest guest)
{
	bool found = findGuestIterator(guest.guest.userID);

	if (!found)
	{
		m_waitingGuests.push_back(guest);
	}

	return found;
}

void HotseatManager::incrementDesiredSeatByUserID(uint32_t userID)
{
	findGuestIndex(userID, [&](size_t index) {
		incrementDesiredSeat(index);
	});
}

void HotseatManager::incrementDesiredSeat(size_t guestIndex)
{
	int seatIndex = m_waitingGuests[guestIndex].desiredSeatIndex;
	seatIndex = isSeatIndexInRange(seatIndex + 1) ? seatIndex + 1 : HotseatGuest::ANY_SEAT;
	m_waitingGuests[guestIndex].desiredSeatIndex = seatIndex;
}

void HotseatManager::decrementDesiredSeatByUserID(uint32_t userID)
{
	findGuestIndex(userID, [&](size_t index) {
		decrementDesiredSeat(index);
	});
}

void HotseatManager::decrementDesiredSeat(size_t guestIndex)
{
	int seatIndex = m_waitingGuests[guestIndex].desiredSeatIndex;

	if (seatIndex == HotseatGuest::ANY_SEAT) seatIndex = m_seats.size() - 1;
	else if (isSeatIndexInRange(seatIndex)) seatIndex = seatIndex - 1;
	else seatIndex = HotseatGuest::ANY_SEAT;

	m_waitingGuests[guestIndex].desiredSeatIndex = seatIndex;
}

int HotseatManager::setDesiredSeat(uint32_t userID, int seatIndex)
{
	int result = -1;

	findGuestIndex(userID, [&](size_t index) {
		if (isSeatIndexInRange(seatIndex) || seatIndex == HotseatGuest::ANY_SEAT)
		{
			result = seatIndex;
		}
		else
		{
			result = HotseatGuest::ANY_SEAT;
		}

		m_waitingGuests[index].desiredSeatIndex = result;
	});

	return result;
}

void HotseatManager::spectateGuest(const size_t guestIndex)
{
	if (!isGuestIndexInRange(guestIndex)) return;

	m_waitingGuests.erase(m_waitingGuests.begin() + guestIndex);
}

bool HotseatManager::spectateGuestID(const uint32_t userID)
{
	bool found = false;

	found = findSeatByGuest(userID, [&](Hotseat& seat, int seatIndex) {
		spectateSeat(seat);
	});

	found = found || findGuestIndex(userID, [&](size_t guestIndex) {
		spectateGuest(guestIndex);
	});

	return found;
}

void HotseatManager::spectateSeat(const size_t seatIndex)
{
	if (!isSeatIndexInRange(seatIndex)) return;

	spectateSeat(m_seats[seatIndex]);
}

void HotseatManager::spectateSeat(Hotseat& seat)
{
	seat.guest = HotseatGuest();
	seat.isEmpty = true;
	seat.timer.stop();
	seat.gamepad->clearOwner();
	seat.gamepad->isPuppet = false;
}

void HotseatManager::next(const size_t seatIndex)
{
	if (!m_isEnabled) return;

	if (!isSeatIndexInRange(seatIndex)) return;

	next(m_seats[seatIndex], seatIndex);
}

void HotseatManager::next()
{
	if (!m_isEnabled) return;

	findSeatIteratorWithShortestCooldown([&](Hotseat& seat, int index) {
		next(seat, index);
	});
}

void HotseatManager::next(Hotseat& seat, int index)
{
	if (!m_isEnabled) return;

	bool found = popNextGuest(index, [&](HotseatGuest& nextGuest) {
		if (!seat.isEmpty)
		{
			m_waitingGuests.push_back(seat.guest);
		}
		seat.guest = nextGuest;
		seat.timer.start();
		seat.timer.reset();
		seat.isEmpty = false;
	});

	if (!found)
	{
		if (seat.isEmpty)
		{
			seat.timer.stop();
			seat.timer.reset();
		}
		else if (seat.timer.isRunComplete())
		{
			seat.timer.start();
			seat.timer.reset();
		}
	}
}

void HotseatManager::refresh(const size_t seatIndex)
{
	if (!isSeatIndexInRange(seatIndex)) return;

	refresh(m_seats[seatIndex]);
}

void HotseatManager::refresh(Hotseat& seat)
{
	seat.timer.reset();
}

void HotseatManager::refreshAll()
{
	auto iSeat = m_seats.begin();
	for (; iSeat != m_seats.end(); ++iSeat)
	{
		refresh(*iSeat);
	}
}

void HotseatManager::reverse(const uint32_t seatIndex)
{
	if (!isSeatIndexInRange(seatIndex)) return;

	m_waitingGuests.insert(m_waitingGuests.begin(), m_seats[seatIndex].guest);
}

void HotseatManager::setTimer(const uint32_t duration)
{
	m_timerDurationSec = duration;

	vector<Hotseat>::iterator it = m_seats.begin();
	for (; it != m_seats.end(); ++it)
	{
		it->timer.setDurationSec(m_timerDurationSec);
	}
}

void HotseatManager::updateAndRotate(
	const vector<AGamepad*>& gamepads,
	GuestToBoolAction isGuestOnlineCallback,
	GuestToBoolAction isMultitapGuestCallback
)
{
	updateSeats(gamepads);
	updateGuests(isGuestOnlineCallback, isMultitapGuestCallback);
	rotate();
}

void HotseatManager::moveGuestTo(const size_t sourceIndex, const size_t targetIndex)
{
	if (targetIndex == sourceIndex || targetIndex == sourceIndex + 1) return;

	if (isGuestIndexInRange(sourceIndex) && targetIndex <= m_waitingGuests.size())
	{
		HotseatGuest sourceGuest = m_waitingGuests[sourceIndex];
		m_waitingGuests.erase(m_waitingGuests.begin() + sourceIndex);
		
		const size_t finaltargetIndex = (sourceIndex < targetIndex) ? targetIndex - 1: targetIndex;

		m_waitingGuests.insert(m_waitingGuests.begin() + finaltargetIndex, sourceGuest);
	}
}

void HotseatManager::cutLine(const size_t guestIndex)
{
	if (!isGuestIndexInRange(guestIndex)) return;

	HotseatGuest cutter = m_waitingGuests[guestIndex];

	m_waitingGuests.erase(m_waitingGuests.begin() + guestIndex);
	m_waitingGuests.insert(m_waitingGuests.begin(), cutter);
	next();
}


const vector<Hotseat>& HotseatManager::getSeats() const
{
	return m_seats;
}

const vector<HotseatGuest>& HotseatManager::getWaitingGuests() const
{
	return m_waitingGuests;
}

const bool HotseatManager::isQueueEmpty() const
{
	return m_waitingGuests.size() <= 0;
}

const bool HotseatManager::isEnabled() const
{
	return m_isEnabled;
}

void HotseatManager::setEnabled(const bool isRunning)
{
	m_isEnabled = isRunning;
}

void HotseatManager::runThreadSafe(HotseatManager::Action callback)
{
	if (callback)
	{
		lock_guard<mutex> lock(m_mutex);
		callback();
	}
}


// ========================================================
// Private
// ========================================================
void HotseatManager::rotate()
{
	if (!m_isEnabled) return;

	vector<Hotseat>::iterator iSeat = m_seats.begin();
	int index = 0;

	for (; iSeat != m_seats.end(); ++iSeat)
	{
		if (iSeat->isEmpty || iSeat->timer.isRunComplete())
		{
			next(*iSeat, index);
		}
		else if (!iSeat->guest.isOnline)
		{
			spectateSeat((size_t)index);
		}

		++index;
	}
}

void HotseatManager::updateSeats(const vector<AGamepad*>& gamepads)
{
	size_t connectedCount = 0;
	size_t lastSeatIndex = 0;

	vector<AGamepad*>::const_iterator iPad = gamepads.begin();

	for (; iPad != gamepads.end(); ++iPad)
	{
		if ((*iPad)->isConnected())
		{
			connectedCount++;
			lastSeatIndex = connectedCount - 1;

			if (connectedCount <= m_seats.size())
			{
				m_seats[lastSeatIndex].gamepad = *iPad;
			}
			else
			{
				Hotseat newSeat(*iPad);
				newSeat.timer.stop();
				newSeat.timer.setDurationSec(m_timerDurationSec);
				newSeat.timer.reset();
				newSeat.isEmpty = true;
				m_seats.push_back(newSeat);
			}

			if (!m_seats[lastSeatIndex].isEmpty && !m_seats[lastSeatIndex].guest.isOnline)
			{
				m_seats[lastSeatIndex].isEmpty = true;
			}
		}
	}

	if (m_seats.size() > connectedCount)
	{
		auto iSeat = m_seats.begin() + connectedCount;
		if (!iSeat->isEmpty)
		{
			m_waitingGuests.insert(m_waitingGuests.begin(), iSeat->guest);
		}
		m_seats.erase(iSeat, m_seats.end());
	}
}

void HotseatManager::updateGuests(GuestToBoolAction isGuestOnlineCallback, GuestToBoolAction isMultitapGuestCallback)
{
	vector<HotseatGuest>::iterator iGuest = m_waitingGuests.begin();

	for (; iGuest != m_waitingGuests.end(); ++iGuest)
	{
		iGuest->isOnline = iGuest->isMaster || (isGuestOnlineCallback ? isGuestOnlineCallback(iGuest->guest) : false);
		iGuest->isMultitap = isMultitapGuestCallback ? isMultitapGuestCallback(iGuest->guest) : false;
	}

	// Seated guests
	HotseatGuest* guest;
	for (size_t i = 0; i < m_seats.size(); i++)
	{
		guest = &m_seats[i].guest;
		guest->isOnline = guest->isMaster || (isGuestOnlineCallback ? isGuestOnlineCallback(guest->guest) : false);
		guest->isMultitap = isMultitapGuestCallback ? isMultitapGuestCallback(guest->guest) : false;
	}
}

bool HotseatManager::findSeatByGuest(uint32_t userID, HotseatAction callback)
{
	vector<Hotseat>::iterator it = m_seats.begin();
	int index = 0;

	for (; it != m_seats.end(); ++it)
	{
		if (it->guest.guest.userID == userID)
		{
			if (callback)
			{
				callback(*it, index);
			}
			return true;
		}

		++index;
	}

	return false;
}

bool HotseatManager::findGuestIterator(uint32_t userID, HotseatGuestAction callback)
{
	vector<HotseatGuest>::iterator it = m_waitingGuests.begin();

	for (; it != m_waitingGuests.end(); ++it)
	{
		if (it->guest.userID == userID)
		{
			if (callback)
			{
				callback(*it);
			}
			return true;
		}
	}

	return false;
}

bool HotseatManager::findGuestIndex(const uint32_t userID, IndexAction callback)
{
	vector<HotseatGuest>::iterator it = m_waitingGuests.begin();
	size_t index = 0;

	for (; it != m_waitingGuests.end(); ++it)
	{
		if (it->guest.userID == userID)
		{
			if (callback)
			{
				callback(index);
			}
			return true;
		}

		index++;
	}

	return false;
}

bool HotseatManager::popNextGuest(const int& desiredSeatIndex, HotseatGuestAction callback)
{
	for (size_t i = 0; i < m_waitingGuests.size(); ++i)
	{
		if (!m_waitingGuests[i].isOnline)
		{
			m_waitingGuests.erase(m_waitingGuests.begin() + i);
			continue;
		}

		if (m_waitingGuests[i].desiredSeatIndex == HotseatGuest::ANY_SEAT || m_waitingGuests[i].desiredSeatIndex == desiredSeatIndex)
		{
			HotseatGuest guest = m_waitingGuests[i];
			m_waitingGuests.erase(m_waitingGuests.begin() + i);

			if (callback)
			{
				callback(guest);
				return true;
			}
		}
	}
	
	return false;
}

void HotseatManager::findSeatIteratorWithShortestCooldown(HotseatAction callback)
{
	vector<Hotseat>::iterator iSeat = m_seats.begin();
	vector<Hotseat>::iterator iShortest = iSeat;

	if (iSeat == m_seats.end()) return;

	uint32_t minimumCooldown = iShortest->timer.getRemainingTime();
	uint32_t cooldown;

	int index = 0, shortestIndex = 0;
	for (; iSeat != m_seats.end(); ++iSeat)
	{
		cooldown = iSeat->timer.getRemainingTime();

		if (iSeat->isEmpty)
		{
			iShortest = iSeat;
			shortestIndex = index;
			break;
		}

		if (cooldown < minimumCooldown)
		{
			iShortest = iSeat;
			minimumCooldown = cooldown;
			shortestIndex = index;
		}

		++index;
	}

	if (iShortest != m_seats.end())
	{
		callback(*iShortest, shortestIndex);
	}
}

bool HotseatManager::isSeatIndexInRange(const size_t index)
{
	return index < m_seats.size();
}

bool HotseatManager::isGuestIndexInRange(const size_t index)
{
	return index < m_waitingGuests.size();
}