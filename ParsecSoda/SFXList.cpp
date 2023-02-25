#include "SFXList.h"

void SFXList::init(const char* jsonPath)
{
	_lastUseTimestamp = steady_clock::now();
	_lastCooldown = 0;
	stringstream tags;

	_sfxList.clear();

    if (MTY_FileExists(jsonPath))
    {
		try
		{
			MTY_JSON* json = MTY_JSONReadFile(jsonPath);
			const MTY_JSON* sfxArray = MTY_JSONObjGetItem(json, "sfx");

			uint32_t size = MTY_JSONGetLength(sfxArray);

			for (size_t i = 0; i < size; ++i)
			{
				SFX sfx;
				const MTY_JSON* ji = MTY_JSONArrayGetItem(sfxArray, i);
				
				char path[1024];
				bool pathSuccess = MTY_JSONObjGetString(ji, "path", path, 1024);
				if (pathSuccess && path != nullptr) sfx.path = path;

				char tag[1024];
				bool tagSuccess = MTY_JSONObjGetString(ji, "tag", tag, 1024);
				if (tagSuccess && tag != nullptr) sfx.tag = tag;

				uint32_t cooldown = 0;
				bool cooldownSuccess = MTY_JSONObjGetUInt(ji, "cooldown", &cooldown);
				if (cooldownSuccess) sfx.cooldown = cooldown;

				if (pathSuccess && tagSuccess && cooldownSuccess)
				{
					_sfxList.push_back(sfx);
				}
			}

			sort(_sfxList.begin(), _sfxList.end(), [](const SFX a, const SFX b) {
				return a.tag.compare(b.tag) < 0;
			});

			for (size_t i = 0; i < _sfxList.size(); ++i)
			{
				if (i > 0)
				{
					tags << ", ";
				}
				tags << _sfxList[i].tag;
			}

			_loadedTags = tags.str();
			tags.clear();
			MTY_JSONDestroy(&json);
		}
		catch (const std::exception&)
		{}
    }
}

int64_t SFXList::getRemainingCooldown()
{
	const sec delta = steady_clock::now() - _lastUseTimestamp;
	if (_lastCooldown - delta.count() > 0)
	{
		return _lastCooldown - delta.count();
	}
	return 0;
}

SFXList::SFXPlayResult SFXList::play(const string tag)
{
	vector<SFX>::iterator it = _sfxList.begin();
	for (; it != _sfxList.end(); ++it)
	{
		if ((*it).tag.compare(tag) == 0)
		{
			if (getRemainingCooldown() > 0)
			{
				return SFXPlayResult::COOLDOWN;
			}

			string path = string("./sfx/custom/") + string((*it).path);
			wstring wide (path.begin(), path.end());
			PlaySound(wide.c_str(), NULL, SND_FILENAME | SND_NODEFAULT | SND_ASYNC);
			_lastCooldown = (*it).cooldown;
			_lastUseTimestamp = steady_clock::now();
			return SFXPlayResult::OK;
		}
	}

    return SFXPlayResult::NOT_FOUND;
}

const string SFXList::loadedTags()
{
	return _loadedTags;
}

const size_t SFXList::size()
{
	return _sfxList.size();
}
