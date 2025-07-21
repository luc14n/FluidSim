#include "Page.h"

// Control IDs for the database page
#define IDC_BTN_LIQUIDS     2010
#define IDC_BTN_CONFIGS     2011
#define IDC_BTN_SIMS        2012
#define IDC_BTN_SAVE        2020
#define IDC_BTN_LOAD        2021
#define IDC_BTN_UPDATE      2022

class PGDatabase : public Page {
public:
    PGDatabase()
        : hBtnLiquids(nullptr), hBtnConfigs(nullptr), hBtnSims(nullptr),
          hBtnSave(nullptr), hBtnLoad(nullptr), hBtnUpdate(nullptr),
          hLabelTable(nullptr), selectedTable(0) {}

    void Show(HWND hWnd, HINSTANCE hInst) override {
        // Table selection buttons
        hBtnLiquids = CreateWindowW(L"BUTTON", L"Liquids", WS_TABSTOP | WS_VISIBLE | WS_CHILD,
            20, 60, 100, 30, hWnd, (HMENU)IDC_BTN_LIQUIDS, hInst, nullptr);
        hBtnConfigs = CreateWindowW(L"BUTTON", L"Configs", WS_TABSTOP | WS_VISIBLE | WS_CHILD,
            130, 60, 100, 30, hWnd, (HMENU)IDC_BTN_CONFIGS, hInst, nullptr);
        hBtnSims = CreateWindowW(L"BUTTON", L"Simulations", WS_TABSTOP | WS_VISIBLE | WS_CHILD,
            240, 60, 120, 30, hWnd, (HMENU)IDC_BTN_SIMS, hInst, nullptr);

        // Label to show which table is selected
        hLabelTable = CreateWindowW(L"STATIC", L"Liquids", WS_VISIBLE | WS_CHILD | SS_CENTER,
            380, 60, 100, 30, hWnd, nullptr, hInst, nullptr);

        // Action buttons
        hBtnSave = CreateWindowW(L"BUTTON", L"Save", WS_TABSTOP | WS_VISIBLE | WS_CHILD,
            20, 100, 80, 30, hWnd, (HMENU)IDC_BTN_SAVE, hInst, nullptr);
        hBtnLoad = CreateWindowW(L"BUTTON", L"Load", WS_TABSTOP | WS_VISIBLE | WS_CHILD,
            110, 100, 80, 30, hWnd, (HMENU)IDC_BTN_LOAD, hInst, nullptr);
        hBtnUpdate = CreateWindowW(L"BUTTON", L"Update", WS_TABSTOP | WS_VISIBLE | WS_CHILD,
            200, 100, 80, 30, hWnd, (HMENU)IDC_BTN_UPDATE, hInst, nullptr);
    }

    void Hide(HWND hWnd) override {
        DestroyIfExists(hBtnLiquids);
        DestroyIfExists(hBtnConfigs);
        DestroyIfExists(hBtnSims);
        DestroyIfExists(hBtnSave);
        DestroyIfExists(hBtnLoad);
        DestroyIfExists(hBtnUpdate);
        DestroyIfExists(hLabelTable);
    }

    void Draw(HWND hWnd, HDC hdc, RECT* prc) override {
        // Draw the database page title and description
        const wchar_t* dbTitle = L"Database Management";
        const wchar_t* dbDesc = L"Manage your fluid simulation data here.";
        SetBkMode(hdc, TRANSPARENT);
        DrawTextW(hdc, dbTitle, -1, prc, DT_CENTER | DT_TOP | DT_SINGLELINE);
        RECT descRect = *prc;
        descRect.top += 40;
        DrawTextW(hdc, dbDesc, -1, &descRect, DT_CENTER | DT_TOP | DT_SINGLELINE);
    }

    bool HandleCommand(HWND hWnd, WPARAM wParam, LPARAM lParam) override {
        int wmId = LOWORD(wParam);
        switch (wmId) {
        case IDC_BTN_LIQUIDS:
            selectedTable = 0;
            SetWindowTextW(hLabelTable, L"Liquids");
            return true;
        case IDC_BTN_CONFIGS:
            selectedTable = 1;
            SetWindowTextW(hLabelTable, L"Configs");
            return true;
        case IDC_BTN_SIMS:
            selectedTable = 2;
            SetWindowTextW(hLabelTable, L"Simulations");
            return true;
        case IDC_BTN_SAVE:
            // TODO: Call FluidDatabase::save* for selectedTable
            return true;
        case IDC_BTN_LOAD:
            // TODO: Call FluidDatabase::load* for selectedTable
            return true;
        case IDC_BTN_UPDATE:
            // TODO: Call FluidDatabase::update* for selectedTable
            return true;
        default:
            return false;
        }
    }

private:
    void DestroyIfExists(HWND& hCtrl) {
        if (hCtrl) {
            DestroyWindow(hCtrl);
            hCtrl = nullptr;
        }
    }

    HWND hBtnLiquids;
    HWND hBtnConfigs;
    HWND hBtnSims;
    HWND hBtnSave;
    HWND hBtnLoad;
    HWND hBtnUpdate;
    HWND hLabelTable;
    int selectedTable;
};

// Factory function for use in main code
Page* CreatePGDatabase() {
    return new PGDatabase();
}
