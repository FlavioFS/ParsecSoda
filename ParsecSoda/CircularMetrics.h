#pragma once

#include <vector>
#include <assert.h>
#include <functional>
#include <mutex>
#include "Guest.h"

using namespace std;

class CircularMetrics
{
	typedef function<void(const vector<float>&)> FloatListCallback;
	typedef function<void(const vector<uint32_t>&)> IntListCallback;
	typedef function<void(void)> Action;

public:
	CircularMetrics(size_t maxSize = 120);
	
	const size_t size() const;
	const size_t capacity() const;
	const size_t offset() const;
	const bool isEmpty() const;

	void push(GuestMetrics item);

	void clear();
	void clear(size_t size);

	void getLatencies(CircularMetrics::FloatListCallback callback) const;
	void getBitrates(CircularMetrics::FloatListCallback callback) const;
	void getEncodes(CircularMetrics::FloatListCallback callback) const;
	void getDecodes(CircularMetrics::FloatListCallback callback) const;
	void getFastRTs(CircularMetrics::IntListCallback callback) const;
	void getSlowRTs(CircularMetrics::IntListCallback callback) const;
	void getQueuedFrames(CircularMetrics::IntListCallback callback) const;

	const size_t oldestIndex();
	const size_t newestIndex();

	void getMutexLockedContext(CircularMetrics::Action callback);

private:
	void pushP(GuestMetrics item);
	void setP(size_t index, GuestMetrics item);
	void clearP();
	void resizeP(size_t capacity);
	void reserveP(size_t capacity);

	void apllyCallbackToFloatListP(const vector<float>& list, CircularMetrics::FloatListCallback callback) const;
	void applyCallbackToIntListP(const vector<uint32_t>& list, CircularMetrics::IntListCallback callback) const;

	size_t m_capacity = 100;
	size_t m_offset = 0;
	vector<float> m_lantencies;
	vector<float> m_bitrates;
	vector<float> m_encodes;
	vector<float> m_decodes;
	vector<uint32_t> m_fastRTs;
	vector<uint32_t> m_slowRTs;
	vector<uint32_t> m_queuedFrames;

	mutex m_mutex;
};