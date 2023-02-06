#pragma once

#include "GuestData.h"
#include "GuestList.h"
#include "Stopwatch.h"
#include "AGamepad.h"
#include "MetadataCache.h"
#include <vector>
#include <mutex>
#include <algorithm>
#include <functional>

// Forward declarations
class GamepadClient;

using namespace std;


// ========================================================
// HotseatGuest
// ========================================================
class HotseatGuest
{
public:
	HotseatGuest() {}
	HotseatGuest(GuestData guest, const uint32_t deviceID, const bool isKeyboard = false, const bool isMaster = false, const bool isOnline = true)
		: guest(guest), deviceID(deviceID), isKeyboard(isKeyboard), isMaster(isMaster), isOnline(isOnline) {}
	
	HotseatGuest(Guest guest, const uint32_t deviceID, const bool isKeyboard = false, const bool isMaster = false, const bool isOnline = true)
		: HotseatGuest(GuestData(guest.name, guest.userID), deviceID, isKeyboard, isMaster, isOnline) {}

	GuestData guest;
	uint32_t deviceID = 0;
	bool isKeyboard = false;
	bool isMaster = false;
	bool isOnline = true;
	bool isMultitap = false;
};



// ========================================================
// Hotseat
// ========================================================
class Hotseat
{
public:
	Hotseat(AGamepad* gamepad) : gamepad(gamepad) {};

	HotseatGuest guest;
	Stopwatch timer;
	AGamepad* gamepad;

	bool isEmpty = true;
};



// ========================================================
// HotseatManager
// ========================================================
class HotseatManager
{
public:
	typedef function<void(void)> Action;
	typedef function<void(Hotseat&)> HotseatAction;
	typedef function<void(HotseatGuest&)> HotseatGuestAction;

	/** 
	 * Try to enqueue a new guest (if not in line already).
	 * @param guest The new guest to add.
	 * @return True if guest was added to list. False otherwise (e.g.: guest already in list).
	 */
	bool enqueue(const HotseatGuest guest);
	
	/** 
	 * Turn target into spectator (remove guest from queue).
	 * @param guestIndex Waiting queue index.
	 */
	void spectateIndex(const size_t guestIndex);
	
	/** 
	 * Skip a seating guest to next in line - pick specific seat.
	 * @param seatIndex Seat index.
	 */
	void next(const size_t seatIndex);

	/**
	 * Skip a seating guest to next in line - pick seat with shortest timer.
	 */
	void next();

	/**
	 * Restart cooldown of a specific seat.
	 * @param seatIndex Seat index.
	 */
	void refresh(const uint32_t seatIndex);

	/**
	 * Make an already seating guest go back to the front of line (useful for seat count changes).
	 * @param seatIndex Seat index.
	 */
	void reverse(const uint32_t seatIndex);

	/**
	 * Set the allowed time the seats may have with gamepads (in seconds).
	 * @param index Timer duration in seconds.
	 */
	void setTimer(const uint32_t duration);
	
	/**
	 * Handle manager state (run this every thread loop).
	 * @param callback Procedure applied to each rotated seat. Invoked with the values of next guest.
	 */
	void updateAndRotate(GuestList& guestList, GamepadClient& gamepadClient, HotseatAction callback);

	/** 
	 * Try to swap the positions of two waiting guests.
	 * @param i Index of first guest.
	 * @param j Index of second guest.
	 * @return True if swap was successfull. False otherwise (e.g.: invalid index range).
	 */
	bool swapGuests(const size_t i, const size_t j);

	/**
	 * Make waiting guest cut line and go instantly to a seat.
	 * @param guestIndex Guest index.
	 */
	void cutLine(const size_t guestIndex);

	/**
	 * Get the list of seats.
	 * @return Vector of Hotseats.
	 */
	const vector<Hotseat>& getSeats() const;

	/**
	 * Get the list of waiting guests.
	 * @return Vector of HotseatGuests.
	 */
	const vector<HotseatGuest>& getWaitingGuests() const;

	/**
	 * Is Hotseats module running?
	 * @return True if HotseatManager is active. False otherwise.
	 */
	const bool isEnabled() const;

	/**
	 * Is Hotseats module running?
	 * @return True if HotseatManager is active. False otherwise.
	 */
	void setEnabled(const bool isRunning);

	/**
	 * Run a procedure in a thread-safe context (mutex-locked).
	 * @param callback Procedure to run.
	 */
	void runThreadSafe(HotseatManager::Action callback);



private:
	/**
	 * Skip a seating guest to next in line - pick specific seat.
	 * @param seat Seat to rotate.
	 */
	void next(Hotseat& seat);

	/**
	 * Cycle waiting guests, dequeue waiting line and fill empty and timed out seats.
	 * @param callback Procedure applied to each rotated seat. Invoked with the values of next guest.
	 */
	void rotate(HotseatAction callback);

	/**
	 * Automatically match the amount of seats with the amount of connected gamepads in a list.
	 * @param gamepads Gamepad list to scan and match.
	 */
	void updateSeats(const vector<AGamepad*>& gamepads);

	/** 
	 * Fetch state of guests in waiting queue.
	 * @param guests Guest list to scan and match.
	 */
	void updateWaitingGuests(GuestList& guestList, GamepadClient& gamepadClient);

	/**
	 * Find a guest's vector::iterator from it's userID and run a callback if found.
	 * @param userID The guest ID to serach for.
	 * @param callback Functional procedure to run on found guest.
	 * @return True if guest was found (callback invoked). False otherwise (callback ignored).
	 */
	bool findGuestIterator(const uint32_t userID, HotseatGuestAction callback = nullptr);

	/**
	 * Find the Hotseat closest to timeout. Empty Hotseats take priority.
	 * @param callback Functional procedure to run on retrieved Hotseat.
	 */
	void findSeatIteratorWithShortestCooldown(HotseatAction callback);

	bool isSeatIndexInRange(const size_t index);
	bool isGuestIndexInRange(const size_t index);


	bool m_isEnabled = false;
	uint32_t m_timerDuration;

	vector<Hotseat> m_seats;
	vector<HotseatGuest> m_waitingGuests;

	mutex m_mutex;
};