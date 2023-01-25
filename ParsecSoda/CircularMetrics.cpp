#include "CircularMetrics.h"

CircularMetrics::CircularMetrics(size_t maxSize) { clear(maxSize); }

const size_t CircularMetrics::size() const { return m_lantencies.size(); }

const size_t CircularMetrics::capacity() const { return m_capacity; }

const size_t CircularMetrics::offset() const { return m_offset; }

const bool CircularMetrics::isEmpty() const { return size() <= 0; }

void CircularMetrics::push(GuestMetrics item)
{
	if (size() < capacity())
	{
		pushP(item);
	}
	else
	{
		setP(offset(), item);
		m_offset = (offset() + 1) % capacity();
	}
}

void CircularMetrics::clear()
{
	clearP();
	m_offset = 0;
}

void CircularMetrics::clear(size_t size) {
	resizeP(0);
	m_capacity = size;
	reserveP(capacity());
	clear();
}

void CircularMetrics::getLatencies(CircularMetrics::FloatListCallback callback) const
{
	apllyCallbackToFloatListP(m_lantencies, callback);
}

void CircularMetrics::getBitrates(CircularMetrics::FloatListCallback callback) const
{
	apllyCallbackToFloatListP(m_bitrates, callback);
}

void CircularMetrics::getEncodes(CircularMetrics::FloatListCallback callback) const
{
	apllyCallbackToFloatListP(m_encodes, callback);
}

void CircularMetrics::getDecodes(CircularMetrics::FloatListCallback callback) const
{
	apllyCallbackToFloatListP(m_decodes, callback);
}

void CircularMetrics::getFastRTs(CircularMetrics::IntListCallback callback) const
{
	applyCallbackToIntListP(m_fastRTs, callback);
}

void CircularMetrics::getSlowRTs(CircularMetrics::IntListCallback callback) const
{
	applyCallbackToIntListP(m_slowRTs, callback);
}

void CircularMetrics::getQueuedFrames(CircularMetrics::IntListCallback callback) const
{
	applyCallbackToIntListP(m_queuedFrames, callback);
}

const size_t CircularMetrics::oldestIndex()
{
	return offset() <= 0 ? 0 : offset();
}

const size_t CircularMetrics::newestIndex()
{
	return offset() <= 0 ? size() - 1 : (size() + offset() - 1) % capacity();
}

void CircularMetrics::getMutexLockedContext(CircularMetrics::Action callback)
{
	if (callback)
	{
		std::lock_guard<std::mutex> lock(m_mutex);
		callback();
	}
}



// ========================================================
// PRIVATE
// ========================================================
void CircularMetrics::pushP(GuestMetrics item)
{
	const ParsecMetrics metrics = item.getMetrics();

	m_lantencies.push_back((float)metrics.networkLatency);
	m_bitrates.push_back((float)metrics.bitrate);
	m_encodes.push_back((float)metrics.encodeLatency);
	m_decodes.push_back((float)metrics.decodeLatency);
	m_fastRTs.push_back((uint32_t)metrics.fastRTs);
	m_slowRTs.push_back((uint32_t)metrics.slowRTs);
	m_queuedFrames.push_back((uint32_t)metrics.queuedFrames);
}

void CircularMetrics::setP(size_t index, GuestMetrics item)
{
	const ParsecMetrics metrics = item.getMetrics();

	if (index >= 0 && index < size())
	{
		m_lantencies[index] = metrics.networkLatency;
		m_bitrates[index] = metrics.bitrate;
		m_encodes[index] = metrics.encodeLatency;
		m_decodes[index] = metrics.decodeLatency;
		m_fastRTs[index] = metrics.fastRTs;
		m_slowRTs[index] = metrics.slowRTs;
		m_queuedFrames[index] = metrics.queuedFrames;
	}
}

void CircularMetrics::clearP()
{
	m_lantencies.clear();
	m_bitrates.clear();
	m_encodes.clear();
	m_decodes.clear();
	m_fastRTs.clear();
	m_slowRTs.clear();
	m_queuedFrames.clear();
}

void CircularMetrics::resizeP(size_t capacity)
{
	m_lantencies.resize(capacity);
	m_bitrates.resize(capacity);
	m_encodes.resize(capacity);
	m_decodes.resize(capacity);
	m_fastRTs.resize(capacity);
	m_slowRTs.resize(capacity);
	m_queuedFrames.resize(capacity);
}

void CircularMetrics::reserveP(size_t capacity)
{
	m_lantencies.reserve(capacity);
	m_bitrates.reserve(capacity);
	m_encodes.reserve(capacity);
	m_decodes.reserve(capacity);
	m_fastRTs.reserve(capacity);
	m_slowRTs.reserve(capacity);
	m_queuedFrames.reserve(capacity);
}

void CircularMetrics::apllyCallbackToFloatListP(const vector<float>& list, CircularMetrics::FloatListCallback callback) const
{
	if (callback)
	{
		callback(list);
	}
}

void CircularMetrics::applyCallbackToIntListP(const vector<uint32_t>& list, CircularMetrics::IntListCallback callback) const
{
	if (callback)
	{
		callback(list);
	}
}

