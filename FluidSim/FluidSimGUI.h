#pragma once

#include "resource.h"  
#include "sqlite3.h"

#define FLUIDSIM_VERSION L"0.1.000"

#include <fstream>
#include <string>
#include <iterator>
#include <io.h>
#include <commctrl.h>
#include <dwmapi.h>
#include <uxtheme.h>

// Pages
#include "PGHome.h"
#include "PGDatabase.h"

// Color mode enum for dark/light mode support
enum class ColorMode { Light, Dark };

// Global color mode variable
extern ColorMode g_colorMode;

// Function declarations for FluidSimGUI.cpp
int APIENTRY wWinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPWSTR lpCmdLine, int nCmdShow);
ATOM MyRegisterClass(HINSTANCE hInstance);
BOOL InitInstance(HINSTANCE hInstance, int nCmdShow);
LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);
INT_PTR CALLBACK About(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam);
INT_PTR CALLBACK SettingsDlgProc(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam);
void ApplyColorMode(HWND hWnd);
bool IsDarkModeEnabled();
void SetWindowDarkMode(HWND hwnd, bool dark);
void EnableDarkTitleBar(HWND hwnd, bool dark);