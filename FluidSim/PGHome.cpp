#include "Page.h"

class PGHome : public Page {
public:
    PGHome() : hLabelTitle(nullptr), hLabelDesc(nullptr) {}

    void Show(HWND hWnd, HINSTANCE hInst) override {
        // Create a static label for the home page title
        hLabelTitle = CreateWindowW(
            L"STATIC",
            L"Welcome to FluidSim!",
            WS_CHILD | WS_VISIBLE | SS_CENTER,
            20, 60, 400, 32,
            hWnd,
            nullptr,
            hInst,
            nullptr
        );

        // Create a static label for the home page description
        hLabelDesc = CreateWindowW(
            L"STATIC",
            L"Select a menu option to begin.",
            WS_CHILD | WS_VISIBLE | SS_CENTER,
            20, 100, 400, 24,
            hWnd,
            nullptr,
            hInst,
            nullptr
        );
    }

    void Hide(HWND hWnd) override {
        if (hLabelTitle) {
            DestroyWindow(hLabelTitle);
            hLabelTitle = nullptr;
        }
        if (hLabelDesc) {
            DestroyWindow(hLabelDesc);
            hLabelDesc = nullptr;
        }
    }

    void Draw(HWND hWnd, HDC hdc, RECT* prc) override {
        // Optional: custom drawing for the home page
        // (Not needed if using static controls for all content)
    }

    bool HandleCommand(HWND hWnd, WPARAM wParam, LPARAM lParam) override {
        // No commands to handle for the home page
        return false;
    }

private:
    HWND hLabelTitle;
    HWND hLabelDesc;
};

// Factory function for use in main code
Page* CreatePGHome() {
    return new PGHome();
}
