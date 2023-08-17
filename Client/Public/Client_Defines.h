#pragma once

namespace Client
{
	const unsigned int g_iWinSizeX = 1280;
	const unsigned int g_iWinSizeY = 720;

	enum LEVEL { LEVEL_STATIC, LEVEL_LOADING, LEVEL_LOGO, LEVEL_GAMEPLAY_PLATFORM, LEVEL_GAMEPLAY, LEVEL_END };

	enum SOUND_ID { SOUND_BGM, SOUND_HATKID, SOUND_HATKID_UPPER, SOUND_HATKID_LOWER, SOUND_SKILL, SOUND_ENEMY, SOUND_ENEMYSKILL, SOUND_EFFECT, SOUND_UI, SOUND_END };
}

#include <process.h>

// Client_Defines.h

extern HWND			g_hWnd;
extern HINSTANCE	g_hInst;

using namespace Client;