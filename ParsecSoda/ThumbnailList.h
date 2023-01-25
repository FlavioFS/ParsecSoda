#pragma once

#include <vector>
#include <functional>
#include <mutex>
#include "Thumbnail.h"
#include "Stringer.h"
#include "MetadataCache.h"

using namespace std;

class ThumbnailList
{
public:
	typedef function<void(Thumbnail&)> ItemCallback;
	typedef function<void(vector<Thumbnail>&)> ListCallback;

	void getThumbnails(ThumbnailList::ListCallback callback);
	bool find(string gameId, ThumbnailList::ItemCallback callback = nullptr);
	bool add(Thumbnail room);
	void load();
	void save();

private:
	bool addUnsafe(Thumbnail room);
	bool findUnsafe(string gameId, ThumbnailList::ItemCallback callback = nullptr);
	vector<Thumbnail> _thumbnails;
	mutex _mutex;
};

