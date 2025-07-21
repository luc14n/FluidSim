#pragma once
#include <windows.h>

// Abstract base class for all pages
class Page {
public:
    virtual ~Page() {}

    // Called to create and show all controls for the page
    virtual void Show(HWND hWnd, HINSTANCE hInst) = 0;

    // Called to hide and destroy all controls for the page
    virtual void Hide(HWND hWnd) = 0;

    // Optional: called to custom draw the page (e.g., in WM_PAINT)
    virtual void Draw(HWND hWnd, HDC hdc, RECT* prc) {}

    // Optional: handle WM_COMMAND for page-specific controls
    virtual bool HandleCommand(HWND hWnd, WPARAM wParam, LPARAM lParam) { return false; }
};