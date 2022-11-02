#pragma once

namespace Client
{
	const unsigned int g_iWinSizeX = 1280;
	const unsigned int g_iWinSizeY = 720;

	enum LEVEL { LEVEL_STATIC, LEVEL_LOADING, LEVEL_LOGO, LEVEL_GAMEPLAY, LEVEL_END };
}

#include "../Public/Imgui/imgui.h"
#include "../Public/Imgui/imgui_impl_dx11.h"
#include "../Public/Imgui/imgui_impl_win32.h"

#include <process.h>

// Client_Defines.h

extern HWND			g_hWnd;
extern HINSTANCE	g_hInst;

using namespace Client;