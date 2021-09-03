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
        bool found = find(thumb.gameId);
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

void ThumbnailList::load()
{
    vector<Thumbnail> thumbs = MetadataCache::loadThumbnails();
    vector<Thumbnail>::iterator it;

    for (it = thumbs.begin(); it != thumbs.end(); ++it)
    {
        add(*it);
    }
}

void ThumbnailList::save()
{
    MetadataCache::saveThumbnails(_thumbnails);
}
