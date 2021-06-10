#pragma once

#include "Texture.h"

class AppIcons
{
public:
	AppIcons(ID3D11Device* pd3dDevice);

	const Texture play() { return _iconPlay; }
	const Texture stop() { return _iconStop; }
	const Texture yes() { return _iconYes; }
	const Texture no() { return _iconNo; }
	const Texture lock() { return _iconLock; }
	const Texture unlock() { return _iconUnlock; }
	const Texture send() { return _iconSend; }
	const Texture refresh() { return _iconRefresh; }
	const Texture mic() { return _iconMic; }
	const Texture speakers() { return _iconSpeakers; }
	const Texture empty() { return _iconEmpty; }

private:
	Texture _iconPlay;
	Texture _iconStop;
	Texture _iconYes;
	Texture _iconNo;
	Texture _iconLock;
	Texture _iconUnlock;
	Texture _iconSend;
	Texture _iconRefresh;
	Texture _iconMic;
	Texture _iconSpeakers;
	Texture _iconEmpty;
};

