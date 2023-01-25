#include "GuestMetricsHistory.h"

bool GuestMetricsHistory::findSafe(uint32_t userID, GuestMetricsHistory::ItemCallback callback)
{
	lock_guard<mutex> lock(m_mutex);
	return findUnsafe(userID, callback);
}
bool GuestMetricsHistory::findUnsafe(uint32_t userID, GuestMetricsHistory::ItemCallback callback)
{
	return findIteratorUnsafe(userID, [&](Iterator it) {
		if (callback)
		{
			callback(it->second);
		}
	});
}

void GuestMetricsHistory::pushSafe(Guest guest)
{
	lock_guard<mutex> lock(m_mutex);
	pushUnsafe(guest);
}
void GuestMetricsHistory::pushUnsafe(Guest guest)
{
	pushUnsafe(guest.userID, guest.metrics);
}

void GuestMetricsHistory::pushSafe(uint32_t userID, GuestMetrics metrics)
{
	lock_guard<mutex> lock(m_mutex);
	pushUnsafe(userID, metrics);
}
void GuestMetricsHistory::pushUnsafe(uint32_t userID, GuestMetrics metrics)
{
	m_map[userID].push(metrics);
}

void GuestMetricsHistory::eraseSafe(uint32_t userID)
{
	lock_guard<mutex> lock(m_mutex);
	eraseUnsafe(userID);
}
void GuestMetricsHistory::eraseUnsafe(uint32_t userID)
{
	findUnsafe(userID, [&](CircularMetrics& history) {
		history.clear();
		m_map.erase(userID);
	});
}

void GuestMetricsHistory::getMutexLockContext(GuestMetricsHistory::Action callback)
{
	if (callback)
	{
		lock_guard<mutex> lock(m_mutex);
		callback();
	}
}

bool GuestMetricsHistory::findIteratorUnsafe(uint32_t userID, IteratorCallback callback)
{
	GuestMetricsHistory::Iterator it = m_map.find(userID);
	bool found = (it != m_map.end());

	if (found && callback)
	{
		callback(it);
	}

	return found;
}