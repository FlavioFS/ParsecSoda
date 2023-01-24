#pragma once

#include <vector>
#include <assert.h>
#include <functional>
#include <mutex>

using namespace std;

template <class T>
class CircularList
{
	typedef function<void(vector<T>&)> ListCallback;
	typedef function<void(void)> Action;

public:
	inline CircularList(size_t maxSize = 300);
	
	inline const size_t size() const;
	inline const size_t capacity() const;
	inline const size_t offset() const;
	inline const bool isEmpty() const;

	inline void push(T item);

	inline void clear();
	inline void clear(size_t size);

	inline void getList(CircularList::ListCallback callback);

	inline const size_t firstIndex();
	inline const size_t lastIndex();

	inline void getMutexLockedContext(CircularList::Action callback);

private:
	size_t m_capacity = 100;
	size_t m_offset = 0;
	vector<T> m_items;

	mutex m_mutex;
};


template<class T>
inline CircularList<T>::CircularList(size_t maxSize) { clear(maxSize); }

template<class T>
inline const size_t CircularList<T>::size() const { return m_items.size(); }

template<class T>
inline const size_t CircularList<T>::capacity() const { return m_capacity; }

template<class T>
inline const size_t CircularList<T>::offset() const { return m_offset; }

template<class T>
inline const bool CircularList<T>::isEmpty() const { return size() <= 0; }

template<class T>
inline void CircularList<T>::push(T item)
{
	if (size() < capacity())
	{
		m_items.push_back(item);
	}
	else
	{
		m_items[offset()] = item;
		m_offset = (offset() + 1) % capacity();
	}
}

template<class T>
inline void CircularList<T>::clear()
{
	m_items.clear();
	m_offset = 0;
}

template<class T>
inline void CircularList<T>::clear(size_t size) {
	m_items.resize(0);
	m_capacity = size;
	m_items.reserve(capacity());
	m_items.clear();
	m_offset = 0;
}

template<class T>
inline void CircularList<T>::getList(CircularList::ListCallback callback)
{
	if (callback)
	{
		callback(m_items);
	}
}

template<class T>
inline const size_t CircularList<T>::firstIndex()
{
	return offset() <= 0 ? 0 : offset();
}

template<class T>
inline const size_t CircularList<T>::lastIndex()
{
	return offset() <= 0 ? size() - 1 : (size() + offset() - 1) % capacity();
}

template<class T>
inline void CircularList<T>::getMutexLockedContext(CircularList::Action callback)
{
	if (callback)
	{
		std::lock_guard<std::mutex> lock(m_mutex);
		callback();
	}
}