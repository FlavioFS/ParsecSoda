#include "ThumbnailList.h"

void ThumbnailList::getThumbnails(ThumbnailList::ListCallback callback)
{
    if (callback)
    {
        lock_guard<mutex> lock(_mutex);
        callback(_thumbnails);
    }
}

bool ThumbnailList::add(Thumbnail thumb)
{
    lock_guard<mutex> lock(_mutex);
    return addUnsafe(thumb);
}

bool ThumbnailList::find(string gameId, ThumbnailList::ItemCallback callback)
{
    lock_guard<mutex> lock(_mutex);
    return findUnsafe(gameId, callback);
}

void ThumbnailList::load()
{
    lock_guard<mutex> lock(_mutex);

    vector<Thumbnail> thumbs = MetadataCache::loadThumbnails();
    vector<Thumbnail>::iterator it;

    for (it = thumbs.begin(); it != thumbs.end(); ++it)
    {
        addUnsafe(*it);
    }
}

void ThumbnailList::save()
{
    lock_guard<mutex> lock(_mutex);

    MetadataCache::saveThumbnails(_thumbnails);
}

bool ThumbnailList::addUnsafe(Thumbnail thumb)
{
    if (thumb.isValid())
    {
        vector<Thumbnail>::iterator it;
        bool found = findUnsafe(thumb.gameId);
        if (found)
        {
            return false;
        }

        // Insertion sort
        for (it = _thumbnails.begin(); it != _thumbnails.end(); ++it)
        {
            if (Stringer::compareNoCase((*it).name, thumb.name) > 0)
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

bool ThumbnailList::findUnsafe(string gameId, ThumbnailList::ItemCallback callback)
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
