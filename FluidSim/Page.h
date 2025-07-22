#pragma once
#include <windows.h>

/**
 * @brief Abstract base class for all pages in the FluidSim application.
 *
 * This interface defines the contract for all pages (screens) in the application.
 * Each page is responsible for creating, displaying, hiding, and optionally drawing
 * its own controls, as well as handling page-specific commands.
 */
class Page {
public:
    /**
     * @brief Virtual destructor for safe cleanup in derived classes.
     */
    virtual ~Page() {}

    /**
     * @brief Creates and shows all controls for the page.
     * @param hWnd Parent window handle.
     * @param hInst Application instance handle.
     */
    virtual void Show(HWND hWnd, HINSTANCE hInst) = 0;

    /**
     * @brief Hides and destroys all controls for the page.
     * @param hWnd Parent window handle.
     */
    virtual void Hide(HWND hWnd) = 0;

    /**
     * @brief Optional: Custom drawing for the page (e.g., in WM_PAINT).
     * @param hWnd Parent window handle.
     * @param hdc Device context handle.
     * @param prc Drawing rectangle.
     */
    virtual void Draw(HWND hWnd, HDC hdc, RECT* prc) {}

    /**
     * @brief Optional: Handles WM_COMMAND for page-specific controls.
     * @param hWnd Parent window handle.
     * @param wParam WPARAM from the message.
     * @param lParam LPARAM from the message.
     * @return True if the command was handled, false otherwise.
     */
    virtual bool HandleCommand(HWND hWnd, WPARAM wParam, LPARAM lParam) { return false; }
};