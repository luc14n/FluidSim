#pragma once
#include "Page.h"

/**
 * @brief Database page for the FluidSim application.
 *
 * Displays controls for managing the database tables (Liquids, Configs, Simulations).
 * Inherits from the Page interface.
 */
class PGDatabase : public Page {
public:
    /**
     * @brief Constructor. Initializes control handles to nullptr and sets the default table.
     */
    PGDatabase();

    /**
     * @brief Creates and shows all controls for the database page.
     * @param hWnd Parent window handle.
     * @param hInst Application instance handle.
     */
    void Show(HWND hWnd, HINSTANCE hInst) override;

    /**
     * @brief Hides and destroys all controls for the database page.
     * @param hWnd Parent window handle.
     */
    void Hide(HWND hWnd) override;

    /**
     * @brief Custom drawing for the database page (title and description).
     * @param hWnd Parent window handle.
     * @param hdc Device context handle.
     * @param prc Drawing rectangle.
     */
    void Draw(HWND hWnd, HDC hdc, RECT* prc) override;

    /**
     * @brief Handles WM_COMMAND for page-specific controls (table selection, save, load, update).
     * @param hWnd Parent window handle.
     * @param wParam WPARAM from message.
     * @param lParam LPARAM from message.
     * @return True if the command was handled, false otherwise.
     */
    bool HandleCommand(HWND hWnd, WPARAM wParam, LPARAM lParam) override;

private:
    HWND hBtnLiquids;   ///< Handle to the "Liquids" button.
    HWND hBtnConfigs;   ///< Handle to the "Configs" button.
    HWND hBtnSims;      ///< Handle to the "Simulations" button.
    HWND hBtnSave;      ///< Handle to the "Save" button.
    HWND hBtnLoad;      ///< Handle to the "Load" button.
    HWND hBtnUpdate;    ///< Handle to the "Update" button.
    HWND hBtnQuery;    ///< Handle to the "Query" button.
    HWND hLabelTable;   ///< Handle to the label showing the selected table.
    int selectedTable;  ///< Index of the currently selected table (0: Liquids, 1: Configs, 2: Sims).
};

/**
 * @brief Factory function to create a new PGDatabase page.
 * @return Pointer to a new PGDatabase instance as a Page*.
 */
Page* CreatePGDatabase();