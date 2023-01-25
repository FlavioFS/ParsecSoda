#pragma once

#include "Guest.h"
#include "CircularMetrics.h"
#include <map>
#include <functional>
#include <mutex>


class GuestMetricsHistory
{
public:
	typedef map<uint32_t, CircularMetrics>::iterator Iterator;
	typedef function<void(Iterator)> IteratorCallback;
	typedef function<void(CircularMetrics&)> ItemCallback;
	typedef function<void(void)> Action;

	bool findSafe(uint32_t userID, GuestMetricsHistory::ItemCallback callback = nullptr);
	bool findUnsafe(uint32_t userID, GuestMetricsHistory::ItemCallback callback = nullptr);
	void pushSafe(Guest guest);
	void pushUnsafe(Guest guest);
	void pushSafe(uint32_t userID, GuestMetrics metrics);
	void pushUnsafe(uint32_t userID, GuestMetrics metrics);
	void eraseSafe(uint32_t userID);
	void eraseUnsafe(uint32_t userID);
	void getMutexLockContext(GuestMetricsHistory::Action callback);

private:
	bool findIteratorUnsafe(uint32_t userID, IteratorCallback callback);

	map<uint32_t, CircularMetrics> m_map;
	mutex m_mutex;
};

