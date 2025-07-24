#include "Page.h"
#include "PGHome.h"
#include "Resource.h"
#include "FluidSimGUI.h" // for g_hLabelTitleBgBrush

PGHome::PGHome() : hLabelTitle(nullptr), hLabelDesc(nullptr) {}

PGHome::~PGHome() {
    // No brush cleanup here; handled by GUI
}

void PGHome::Show(HWND hWnd, HINSTANCE hInst) {  
    // Create a static label for the home page title
    hLabelTitle = CreateWindowW(
        L"STATIC",
        L"Welcome to FluidSim!",
        WS_CHILD | WS_VISIBLE | SS_CENTER,
        20, 70, 400, 30,
        hWnd,
        (HMENU)IDC_LABEL_TITLE, // Set the control ID
        hInst,
        nullptr
    );

    // Create a static label for the home page description
    hLabelDesc = CreateWindowW(
        L"STATIC",
        L"Select a menu option to begin.",
        WS_CHILD | WS_VISIBLE | SS_CENTER,
        20, 110, 400, 60,
        hWnd,
        (HMENU)IDC_LABEL_NOCLR,
        hInst,
        nullptr
    );
}

void PGHome::Hide(HWND hWnd) {
    if (hLabelTitle) {
        DestroyWindow(hLabelTitle);
        hLabelTitle = nullptr;
    }
    if (hLabelDesc) {
        DestroyWindow(hLabelDesc);
        hLabelDesc = nullptr;
    }
}

void PGHome::Draw(HWND hWnd, HDC hdc, RECT* prc) {
    // Optional: custom drawing for the home page
    // (Not needed if using static controls for all content)
}

bool PGHome::HandleCommand(HWND hWnd, WPARAM wParam, LPARAM lParam) {
    // No commands to handle for the home page
    return false;
}

// Factory function for use in main code
Page* CreatePGHome() {
    return new PGHome();
}
