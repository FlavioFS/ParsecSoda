#pragma once

#include "AGamepad.h"
#include "GuestData.h"
#include "Stopwatch.h"
#include "MetadataCache.h"
#include <vector>
#include <mutex>
#include <functional>

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

	static const int ANY_SEAT { -1 };

	GuestData guest;
	uint32_t deviceID = 0;
	bool isKeyboard = false;
	bool isMaster = false;
	bool isOnline = true;
	bool isMultitap = false;
	int desiredSeatIndex = ANY_SEAT;
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
	typedef function<void(size_t)> IndexAction;
	typedef function<void(Hotseat&, int index)> HotseatAction;
	typedef function<void(HotseatGuest&)> HotseatGuestAction;
	typedef function<const bool(const GuestData&)> GuestToBoolAction;

	HotseatManager();

	/** 
	 * Try to enqueue a new guest (if not in line already).
	 * @param guest The new guest to add.
	 * @return True if guest was added to list. False otherwise (e.g.: guest already in list).
	 */
	bool enqueue(const HotseatGuest guest);

	void incrementDesiredSeat(uint32_t userID);
	void decrementDesiredSeat(uint32_t userID);
	int setDesiredSeat(uint32_t userID, int seatIndex);
	
	/** 
	 * Turn target into spectator (remove guest from queue).
	 * @param guestIndex Waiting queue index.
	 */
	void spectateGuest(const size_t guestIndex);

	/**
	 * Turn target into spectator (remove guest from queue).
	 * @param userID Guest's userID.
	 */
	bool spectateGuestID(const uint32_t userID);

	/**
	 * Turn seating guest into spectator (remove guest from seat and waiting line).
	 * @param seatIndex Seat index.
	 */
	void spectateSeat(const size_t seatIndex);

	/**
	 * Turn seating guest into spectator (remove guest from seat and waiting line).
	 * @param seat Seat reference.
	 */
	void spectateSeat(Hotseat& seat);
	
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
	 * Skip a seating guest to next in line - pick specific seat.
	 * @param seat Seat to rotate.
	 */
	void next(Hotseat& seat, int index);

	/**
	 * Restart cooldown of a specific seat.
	 * @param seatIndex Seat index.
	 */
	void refresh(const size_t seatIndex);

	/**
	 * Restart cooldown of a specific seat.
	 * @param seat Seat reference.
	 */
	void refresh(Hotseat& seat);

	/**
	 * Restart cooldown of all seats.
	 */
	void refreshAll();

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
	 * @param gamepads Gamepad list to match.
	 * @param isGuestOnlineCallback Callback to check if a guest is still online and update its HotseatGuest.isOnline status.
	 * @param isMultitapGuestCallback Callback to check if a guest preferences and update its HotseatGuest.isMultitap status.
	 * @param hotSeatUpdatecallback Procedure applied to each rotated seat. Invoked with the values of next guest.
	 */
	void updateAndRotate(
		const vector<AGamepad*>& gamepads,
		GuestToBoolAction isGuestOnlineCallback,
		GuestToBoolAction isMultitapGuestCallback
	);

	/** 
	 * Try to reorder guests. Remove SOURCE guest and insert before TARGET guest.
	 * @param sourceIndex Index of guest to be moved.
	 * @param j targetIndex of position to insert in front.
	 * @return True if swap was successfull. False otherwise (e.g.: invalid index range).
	 */
	void moveGuestTo(const size_t sourceIndex, const size_t targetIndex);

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
	 * Find the Hotseat in which a specific guest is seating.
	 * @param userID The guest to seatch for.
	 * @param callback Functional procedure to run on retrieved Hotseat.
	 */
	bool findSeatByGuest(uint32_t userID, HotseatAction callback);
	
	/**
	 * Check if waiting line contains any guests.
	 * @return True if queue is empty. False otherwise.
	 */
	const bool isQueueEmpty() const;
	
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
	 * Cycle waiting guests, dequeue waiting line and fill empty and timed out seats.
	 */
	void rotate();

	/**
	 * Automatically match the amount of seats with the amount of connected gamepads in a list.
	 * @param gamepads Gamepad list to scan and match.
	 */
	void updateSeats(const vector<AGamepad*>& gamepads);

	/** 
	 * Fetch state of guests in waiting queue.
	 * @param isGuestOnlineCallback Callback to check if a guest is still online and update its HotseatGuest.isOnline status.
	 * @param isMultitapGuestCallback Callback to check if a guest preferences and update its HotseatGuest.isMultitap status.
	 */
	void updateGuests(GuestToBoolAction isGuestOnlineCallback, GuestToBoolAction isMultitapGuestCallback);

	/**
	 * Find a guest in waiting line by it's userID and run a callback if found.
	 * @param userID The guest ID to search for.
	 * @param callback Functional procedure to run on found guest.
	 * @return True if guest was found (callback invoked). False otherwise (callback ignored).
	 */
	bool findGuestIterator(const uint32_t userID, HotseatGuestAction callback = nullptr);

	/**
	 * Find a guest index from it's userID and run a callback if found.
	 * @param userID The guest ID to search for.
	 * @param callback Functional procedure to run on found guest.
	 * @return True if guest was found (callback invoked). False otherwise (callback ignored).
	 */
	bool findGuestIndex(const uint32_t userID, IndexAction callback = nullptr);

	/**
	 * Search in waiting queue for the next online guest and removes it from queue.
	 * @param desiredSeatIndex Seat index the guest is waiting for.
	 * @param callback Invoked when an online guest was found and poped from queue.
	 * @return True if an online guest was found (callback invoked). False otherwise (callback ignored).
	 */
	bool popNextGuest(const int& desiredSeatIndex, HotseatGuestAction callback);

	/**
	 * Find the Hotseat closest to timeout. Empty Hotseats take priority.
	 * @param callback Functional procedure to run on retrieved Hotseat.
	 */
	void findSeatIteratorWithShortestCooldown(HotseatAction callback);

	bool isSeatIndexInRange(const size_t index);
	bool isGuestIndexInRange(const size_t index);


	bool m_isEnabled = false;
	uint32_t m_timerDurationSec;

	vector<Hotseat> m_seats;
	vector<HotseatGuest> m_waitingGuests;

	mutex m_mutex;
};