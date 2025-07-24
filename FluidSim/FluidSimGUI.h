#pragma once

#include "resource.h"  
#include "sqlite3.h"

#define FLUIDSIM_VERSION L"0.1.000"

#include <fstream>
#include <string>
#include <map>
#include <iterator>
#include <io.h>
#include <commctrl.h>
#include <dwmapi.h>
#include <uxtheme.h>

// Pages
#include "PGHome.h"
#include "PGDatabase.h"

using namespace std;

// Color mode enum for dark/light mode support
enum class ColorMode { Light, Dark };

// Global color mode variable
extern ColorMode g_colorMode;
extern std::map<std::string, HBRUSH> g_brushes;

// Color definitions
constexpr COLORREF DARKMODE_DARK_GREY = RGB(32, 40, 60);
constexpr COLORREF DARKMODE_MID_GREY = RGB(72, 76, 90);
constexpr COLORREF DARKMODE_LIGHT_GREY = RGB(129, 144, 152);
constexpr COLORREF DARKMODE_PURPLE = RGB(90, 70, 120);
constexpr COLORREF DARKMODE_BLUE = RGB(103, 193, 201);
constexpr COLORREF DARKMODE_LIGHT_TEXT = RGB(240, 240, 240);

constexpr COLORREF LIGHTMODE_DARK_GREY = RGB(130, 150, 180);
constexpr COLORREF LIGHTMODE_MID_GREY = RGB(176, 179, 191);
constexpr COLORREF LIGHTMODE_LIGHT_GREY = RGB(244, 245, 246);
constexpr COLORREF LIGHTMODE_PURPLE = RGB(180, 180, 240);
constexpr COLORREF LIGHTMODE_BLUE = RGB(180, 240, 240);
constexpr COLORREF LIGHTMODE_DARK_TEXT = RGB(15, 15, 15);

// Function declarations for FluidSimGUI.cpp
int APIENTRY wWinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPWSTR lpCmdLine, int nCmdShow);
ATOM MyRegisterClass(HINSTANCE hInstance);
BOOL InitInstance(HINSTANCE hInstance, int nCmdShow);
LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);
INT_PTR CALLBACK About(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam);
INT_PTR CALLBACK SettingsDlgProc(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam);
void ApplyColorMode(HWND hWnd);
void UpdateToolbarIcons();
void InitBrushes();
bool IsDarkModeEnabled();
void SetWindowDarkMode(HWND hwnd, bool dark);
void EnableDarkTitleBar(HWND hwnd, bool dark);