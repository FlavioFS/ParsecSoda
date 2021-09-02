#include "ThumbnailList.h"

vector<Thumbnail>& ThumbnailList::getThumbnails()
{
    return _thumbnails;
}

bool ThumbnailList::add(Thumbnail thumb)
{
    if (thumb.isValid())
    {
        vector<Thumbnail>::iterator it;
        for (it = _thumbnails.begin(); it != _thumbnails.end(); ++it)
        {
            if ((*it).gameId.compare(thumb.gameId) == 0)
            {
                return false;
            }

            if ((*it).name.compare(thumb.name) > 0)
            {
                _thumbnails.insert(it, thumb);
                return true;
            }
        }

        _thumbnails.push_back(thumb);
        return true;
    }

    return false;
}

bool ThumbnailList::find(string gameId, function<void(Thumbnail&)> callback)
{
    vector<Thumbnail>::iterator it;
    for (it = _thumbnails.begin(); it != _thumbnails.end(); ++it)
    {
        if ((*it).gameId.compare(gameId) == 0)
        {
            if (callback != nullptr)
            {
                callback((*it));
            }
            return true;
        }
    }

    return false;
}