#include "HotseatManager.h"
#include "GamepadClient.h"

bool HotseatManager::enqueue(const HotseatGuest guest)
{
	bool found = findGuestIterator(guest.guest.userID);

	if (!found)
	{
		m_waitingGuests.push_back(guest);
	}

	return found;
}

void HotseatManager::spectateIndex(const size_t guestIndex)
{
	if (!isGuestIndexInRange(guestIndex)) return;

	m_waitingGuests.erase(m_waitingGuests.begin() + guestIndex);
}

void HotseatManager::next(const size_t seatIndex)
{
	if (!m_isEnabled) return;

	if (!isSeatIndexInRange(seatIndex)) return;

	next(m_seats[seatIndex]);
}

void HotseatManager::next()
{
	if (!m_isEnabled) return;

	findSeatIteratorWithShortestCooldown([&](Hotseat& seat) {
		next(seat);
	});
}

void HotseatManager::refresh(const uint32_t seatIndex)
{
	if (!isSeatIndexInRange(seatIndex)) return;

	m_seats[seatIndex].timer.reset();
}

void HotseatManager::reverse(const uint32_t seatIndex)
{
	if (!isSeatIndexInRange(seatIndex)) return;

	m_waitingGuests.insert(m_waitingGuests.begin(), m_seats[seatIndex].guest);
}

void HotseatManager::setTimer(const uint32_t duration)
{
	m_timerDuration = duration;

	vector<Hotseat>::iterator it = m_seats.begin();
	for (; it != m_seats.end(); ++it)
	{
		it->timer.setDurationSec(m_timerDuration);
	}
}

void HotseatManager::updateAndRotate(GuestList& guestList, GamepadClient& gamepadClient, HotseatAction callback)
{
	updateSeats(gamepadClient.gamepads);
	updateWaitingGuests(guestList, gamepadClient);
	rotate(callback);
}

bool HotseatManager::swapGuests(const size_t i, const size_t j)
{
	if (isGuestIndexInRange(i) && isGuestIndexInRange(j))
	{
		iter_swap(m_waitingGuests.begin() + i, m_waitingGuests.begin() + j);
		return true;
	}
	return false;
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
void HotseatManager::next(Hotseat& seat)
{
	if (!m_isEnabled) return;

	if (m_waitingGuests.size() > 0)
	{
		if (!seat.isEmpty)
		{
			m_waitingGuests.push_back(seat.guest);
		}
		seat.guest = *m_waitingGuests.begin();
		m_waitingGuests.erase(m_waitingGuests.begin());
		seat.timer.start();
		seat.timer.reset();
	}
	else if (seat.isEmpty)
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

void HotseatManager::rotate(HotseatAction callback)
{
	if (!m_isEnabled) return;

	vector<Hotseat>::iterator iSeat = m_seats.begin();

	for (; iSeat != m_seats.end(); ++iSeat)
	{
		if (iSeat->isEmpty || iSeat->timer.isRunComplete())
		{
			next(*iSeat);
			if (callback) callback(*iSeat);
		}
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
				newSeat.timer.reset(m_timerDuration);
				newSeat.isEmpty = true;
				m_seats.push_back(newSeat);
			}
		}
	}
}

void HotseatManager::updateWaitingGuests(GuestList& guestList, GamepadClient& gamepadClient)
{
	vector<HotseatGuest>::iterator iGuest = m_waitingGuests.begin();

	for (; iGuest != m_waitingGuests.end(); ++iGuest)
	{
		iGuest->isOnline = guestList.findUnsafe(iGuest->guest.userID);

		gamepadClient.findPreferences(iGuest->guest.userID, [&](GamepadClient::GuestPreferences prefs) {
			iGuest->isMultitap = !prefs.ignoreDeviceID;
		});
	}
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

void HotseatManager::findSeatIteratorWithShortestCooldown(HotseatAction callback)
{
	vector<Hotseat>::iterator iSeat = m_seats.begin();
	vector<Hotseat>::iterator iShortest = iSeat;
	uint32_t minimumCooldown = iShortest->timer.getRemainingTime();
	uint32_t cooldown;

	for (; iSeat != m_seats.end(); ++iSeat)
	{
		cooldown = iSeat->timer.getRemainingTime();

		if (iSeat->isEmpty)
		{
			iShortest = iSeat;
			break;
		}

		if (cooldown < minimumCooldown)
		{
			iShortest = iSeat;
			minimumCooldown = cooldown;
		}
	}

	if (iShortest != m_seats.end())
	{
		callback(*iShortest);
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