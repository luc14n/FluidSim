#pragma once
#include "Page.h"

/**
 * @brief Home page for the FluidSim application.
 *
 * Displays a welcome message and description. Inherits from the Page interface.
 */
class PGHome : public Page {
public:
    /**
     * @brief Constructor. Initializes control handles to nullptr.
     */
    PGHome();

    /**
     * @brief Destructor.
     */
    ~PGHome() override;

    /**
     * @brief Creates and shows all controls for the home page.
     * @param hWnd Parent window handle.
     * @param hInst Application instance handle.
     */
    void Show(HWND hWnd, HINSTANCE hInst) override;

    /**
     * @brief Hides and destroys all controls for the home page.
     * @param hWnd Parent window handle.
     */
    void Hide(HWND hWnd) override;

    /**
     * @brief Optional: Custom drawing for the home page (not used here).
     * @param hWnd Parent window handle.
     * @param hdc Device context handle.
     * @param prc Drawing rectangle.
     */
    void Draw(HWND hWnd, HDC hdc, RECT* prc) override;

    /**
     * @brief Handles WM_COMMAND for page-specific controls (not used here).
     * @param hWnd Parent window handle.
     * @param wParam WPARAM from message.
     * @param lParam LPARAM from message.
     * @return Always false for the home page.
     */
    bool HandleCommand(HWND hWnd, WPARAM wParam, LPARAM lParam) override;

private:
    HWND hLabelTitle; ///< Handle to the title label control.
    HWND hLabelDesc;  ///< Handle to the description label control.
};

/**
 * @brief Factory function to create a new PGHome page.
 * @return Pointer to a new PGHome instance as a Page*.
 */
Page* CreatePGHome();