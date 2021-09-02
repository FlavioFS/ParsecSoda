#pragma once

#include <vector>
#include <functional>
#include "Thumbnail.h"

using namespace std;

class ThumbnailList
{
public:
	vector<Thumbnail>& getThumbnails();
	bool add(Thumbnail room);
	bool find(string gameId, function<void(Thumbnail&)> callback = nullptr);

private:

	vector<Thumbnail> _thumbnails;
};

