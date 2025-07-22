#include "Page.h"
#include "FluidDatabase.h"
#include <string>
#include <map>
#include <comutil.h>
#include <windows.h>

// Control IDs for the database page
#define IDC_BTN_LIQUIDS     2010
#define IDC_BTN_CONFIGS     2011
#define IDC_BTN_SIMS        2012
#define IDC_BTN_SAVE        2020
#define IDC_BTN_LOAD        2021
#define IDC_BTN_UPDATE      2022
#define MAX_COLUMNS 16

/**
 * Database page implementation for FluidSim.
 * Handles UI for table selection, CRUD actions, and dynamic field display.
 */
class PGDatabase : public Page {
public:
    // Column names for each table, reflecting schema.sql, with * for NOT NULL and [TYPE]
    const wchar_t* LIQUIDS_COLUMNS[7] = {
        L"LiquidID* [INTEGER PRIMARY KEY]",
        L"Name* [TEXT]",
        L"Density [REAL]",
        L"Viscosity [REAL]",
        L"Color [TEXT]",
        L"Description [TEXT]",
        L"OtherPhysicalPropertiesJSON [JSON]"
    };
    const wchar_t* CONFIGS_COLUMNS[12] = {
        L"ConfigID* [INTEGER PRIMARY KEY]",
        L"Name* [TEXT]",
        L"GridSize [TEXT]",
        L"ParticleCount [INTEGER]",
        L"InflowParamsJSON [JSON]",
        L"OutflowParamsJSON [JSON]",
        L"Timestep [REAL]",
        L"MethodOfComputation [TEXT]",
        L"FluidID [INTEGER]",
        L"Description [TEXT]",
        L"IsStandard [INTEGER DEFAULT 0]",
        L"OtherParamsJSON [JSON]"
    };
    const wchar_t* SIMS_COLUMNS[10] = {
        L"SimulationID* [INTEGER PRIMARY KEY]",
        L"ConfigID [INTEGER]",
        L"DateTime [TEXT]",
        L"ResultFilePath [TEXT]",
        L"Duration [REAL]",
        L"Notes [TEXT]",
        L"User [TEXT]",
        L"Seed [INTEGER]",
        L"Version [TEXT]",
        L"OtherMetadataJSON [JSON]"
    };

    HWND hEditControls[MAX_COLUMNS] = { nullptr };
    HWND hLabelControls[MAX_COLUMNS] = { nullptr };
    HWND hDisplayControls[MAX_COLUMNS] = { nullptr };
    int currentColumnCount = 0;

    FluidDatabase db{ "FluidSim.db" };

    PGDatabase()
        : hBtnLiquids(nullptr), hBtnConfigs(nullptr), hBtnSims(nullptr),
        hBtnSave(nullptr), hBtnLoad(nullptr), hBtnUpdate(nullptr),
        hLabelTable(nullptr), selectedTable(0) {
    }

    // Create and show all controls for the database page
    void Show(HWND hWnd, HINSTANCE hInst) override {
        // Initialize DB
        try {
            if (!db.open()) {
                throw std::runtime_error("No DB found to open...");
            }
        }
        catch (const std::bad_exception& ex) {
            MessageBoxA(hWnd, ex.what(), "DB Error", MB_OK | MB_ICONERROR);
        }
        
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

        // Show Liquids table fields by default
        selectedTable = 0;
        ShowTableFields(hWnd, hInst, 0);
    }

    // Hide and destroy all controls for the database page
    void Hide(HWND hWnd) override {
        DestroyIfExists(hBtnLiquids);
        DestroyIfExists(hBtnConfigs);
        DestroyIfExists(hBtnSims);
        DestroyIfExists(hBtnSave);
        DestroyIfExists(hBtnLoad);
        DestroyIfExists(hBtnUpdate);
        DestroyIfExists(hLabelTable);

        // Destroy all dynamic label and edit controls
        for (int i = 0; i < currentColumnCount; ++i) {
            DestroyIfExists(hEditControls[i]);
            DestroyIfExists(hLabelControls[i]);
            DestroyIfExists(hDisplayControls[i]);
        }
        currentColumnCount = 0;
    }

    // Draw the database page title and description
    void Draw(HWND hWnd, HDC hdc, RECT* prc) override {
        const wchar_t* dbTitle = L"Database Management";
        const wchar_t* dbDesc = L"Manage your fluid simulation data here.";
        SetBkMode(hdc, TRANSPARENT);
        DrawTextW(hdc, dbTitle, -1, prc, DT_CENTER | DT_TOP | DT_SINGLELINE);
        RECT descRect = *prc;
        descRect.top += 40;
        DrawTextW(hdc, dbDesc, -1, &descRect, DT_CENTER | DT_TOP | DT_SINGLELINE);
    }

    /**
     * @brief Show fields for the selected table (Liquids, Configs, or Simulations).
     *
     * Destroys any previous field controls and creates new static labels and edit boxes
     * for each column in the selected table. The controls are positioned below the action buttons.
     *
     * @param hWnd Parent window handle.
     * @param hInst Application instance handle.
     * @param tableIndex 0 = Liquids, 1 = Configs, 2 = Simulations
     */
    void ShowTableFields(HWND hWnd, HINSTANCE hInst, int tableIndex) {
        // Destroy previous controls
        for (int i = 0; i < currentColumnCount; ++i) {
            DestroyIfExists(hEditControls[i]);
            DestroyIfExists(hLabelControls[i]);
        }
        currentColumnCount = 0;

        // Select columns for the chosen table
        const wchar_t** columns = nullptr;
        int colCount = 0;
        switch (tableIndex) {
        case 0: columns = LIQUIDS_COLUMNS; colCount = _countof(LIQUIDS_COLUMNS); break;
        case 1: columns = CONFIGS_COLUMNS; colCount = _countof(CONFIGS_COLUMNS); break;
        case 2: columns = SIMS_COLUMNS; colCount = _countof(SIMS_COLUMNS); break;
        }
        currentColumnCount = colCount;

        // Create label and edit controls for each column
        for (int i = 0; i < colCount; ++i) {
            int y = 150 + i * 32;
            // Expanded label width to fit longer text
            hLabelControls[i] = CreateWindowW(L"STATIC", columns[i], WS_CHILD | WS_VISIBLE,
                20, y, 260, 28, hWnd, nullptr, hInst, nullptr);
            hEditControls[i] = CreateWindowW(L"EDIT", L"", WS_CHILD | WS_VISIBLE | WS_BORDER | ES_AUTOHSCROLL,
                290, y, 220, 28, hWnd, nullptr, hInst, nullptr);
            hDisplayControls[i] = CreateWindowW(L"STATIC", L"", WS_CHILD | WS_VISIBLE | WS_BORDER | ES_AUTOHSCROLL | ES_READONLY,
                520, y, 220, 28, hWnd, nullptr, hInst, nullptr);
        }
    }

    /**
     * @brief Extracts the base column name from a label string.
     *
     * This function takes a label string (such as "Name* [TEXT]" or "ConfigID* [INTEGER PRIMARY KEY]")
     * and returns only the column name portion (e.g., "Name" or "ConfigID").
     * It stops at the first occurrence of '*', ' ', or '[' to avoid including type or constraint annotations.
     *
     * @param label The label string containing the column name and optional annotations.
     * @return The extracted column name as a std::string.
     */
    std::string ExtractColumnName(const std::wstring& label) {
        size_t end = label.find_first_of(L"* [");
        return std::string(label.begin(), label.begin() + (end == std::wstring::npos ? label.size() : end));
    }

    // Handle WM_COMMAND for page-specific controls (table selection, save, load, update)
    bool HandleCommand(HWND hWnd, WPARAM wParam, LPARAM lParam) override {
        int wmId = LOWORD(wParam);
        switch (wmId) {
        case IDC_BTN_LIQUIDS:
            selectedTable = 0;
            SetWindowTextW(hLabelTable, L"Liquids");
            ShowTableFields(hWnd, GetModuleHandle(nullptr), 0);
            return true;
        case IDC_BTN_CONFIGS:
            selectedTable = 1;
            SetWindowTextW(hLabelTable, L"Configs");
            ShowTableFields(hWnd, GetModuleHandle(nullptr), 1);
            return true;
        case IDC_BTN_SIMS:
            selectedTable = 2;
            SetWindowTextW(hLabelTable, L"Simulations");
            ShowTableFields(hWnd, GetModuleHandle(nullptr), 2);
            return true;
        case IDC_BTN_SAVE:
        {
            bool success = false;
            wchar_t buffer[256];

            if (selectedTable == 0) { // Liquids
                // Get PK (optional) and fields
                GetWindowTextW(hEditControls[0], buffer, 255); std::wstring wLiquidID = buffer;
                GetWindowTextW(hEditControls[1], buffer, 255); std::wstring wName = buffer;
                if (wName.empty()) {
                    MessageBoxW(hWnd, L"Name is required.", L"Validation", MB_OK | MB_ICONWARNING);
                    return true;
                }
                GetWindowTextW(hEditControls[2], buffer, 255); std::wstring wDensity = buffer;
                GetWindowTextW(hEditControls[3], buffer, 255); std::wstring wViscosity = buffer;
                GetWindowTextW(hEditControls[4], buffer, 255); std::wstring wColor = buffer;
                GetWindowTextW(hEditControls[5], buffer, 255); std::wstring wDescription = buffer;
                GetWindowTextW(hEditControls[6], buffer, 255); std::wstring wOther = buffer;

                std::string liquidID(wLiquidID.begin(), wLiquidID.end());
                std::string name(wName.begin(), wName.end());
                double density = wDensity.empty() ? 0.0 : _wtof(wDensity.c_str());
                double viscosity = wViscosity.empty() ? 0.0 : _wtof(wViscosity.c_str());
                std::string color(wColor.begin(), wColor.end());
                std::string description(wDescription.begin(), wDescription.end());
                std::string other(wOther.begin(), wOther.end());

                success = db.saveLiquidType(liquidID, name, density, viscosity, color, description, other);
            }
            else if (selectedTable == 1) { // Configs
                // Get PK (optional) and fields
                GetWindowTextW(hEditControls[0], buffer, 255); std::wstring wConfigID = buffer;
                GetWindowTextW(hEditControls[1], buffer, 255); std::wstring wName = buffer;
                if (wName.empty()) {
                    MessageBoxW(hWnd, L"Name is required.", L"Validation", MB_OK | MB_ICONWARNING);
                    return true;
                }
                std::map<std::string, std::string> params;
                for (int i = 0; i < _countof(CONFIGS_COLUMNS); ++i) {
                    GetWindowTextW(hEditControls[i], buffer, 255);
                    std::wstring wval = buffer;
                    std::string sval(wval.begin(), wval.end());
                    std::wstring wcol(CONFIGS_COLUMNS[i]);
                    std::string scol(wcol.begin(), wcol.end());
                    params[scol] = sval;
                }
                // If ConfigID is empty, erase it so the DB will auto-generate
                if (params["ConfigID"].empty()) params.erase("ConfigID");
                success = db.saveSimulationParameters(params);
            }
            else if (selectedTable == 2) { // Simulations
                // Get PK (optional) and fields
                GetWindowTextW(hEditControls[0], buffer, 255); std::wstring wSimID = buffer;
                GetWindowTextW(hEditControls[1], buffer, 255); std::wstring wConfigID = buffer;
                GetWindowTextW(hEditControls[2], buffer, 255); std::wstring wDateTime = buffer;
                GetWindowTextW(hEditControls[3], buffer, 255); std::wstring wResultFilePath = buffer;
                GetWindowTextW(hEditControls[4], buffer, 255); std::wstring wDuration = buffer;
                GetWindowTextW(hEditControls[5], buffer, 255); std::wstring wNotes = buffer;
                GetWindowTextW(hEditControls[6], buffer, 255); std::wstring wUser = buffer;
                GetWindowTextW(hEditControls[7], buffer, 255); std::wstring wSeed = buffer;
                GetWindowTextW(hEditControls[8], buffer, 255); std::wstring wVersion = buffer;
                GetWindowTextW(hEditControls[9], buffer, 255); std::wstring wOther = buffer;

                int simID = wSimID.empty() ? 0 : _wtoi(wSimID.c_str());
                int configID = wConfigID.empty() ? 0 : _wtoi(wConfigID.c_str());
                std::string dateTime(wDateTime.begin(), wDateTime.end());
                std::string resultFilePath(wResultFilePath.begin(), wResultFilePath.end());
                double duration = wDuration.empty() ? 0.0 : _wtof(wDuration.c_str());
                std::string notes(wNotes.begin(), wNotes.end());
                std::string user(wUser.begin(), wUser.end());
                int seed = wSeed.empty() ? 0 : _wtoi(wSeed.c_str());
                std::string version(wVersion.begin(), wVersion.end());
                std::string other(wOther.begin(), wOther.end());

                success = db.saveSimulation(simID, configID, dateTime, resultFilePath, duration, notes, user, seed, version, other);
            }

            if (success) {
                MessageBoxW(hWnd, L"Save successful.", L"Save", MB_OK | MB_ICONINFORMATION);
            }
            else {
                MessageBoxW(hWnd, L"Save failed.", L"Save", MB_OK | MB_ICONERROR);
            }
            return true;
        }
        case IDC_BTN_LOAD:
        {
            // Load a record by ID from the first edit box
            std::map<std::string, std::string> data;
            wchar_t idBuffer[64] = { 0 };
            GetWindowTextW(hEditControls[0], idBuffer, 63);

            bool loaded = false;
            if (selectedTable == 0) {
                if (wcslen(idBuffer) > 0) {
                    int liquidID = _wtoi(idBuffer);
                    loaded = db.loadLiquidType(liquidID, data);
                }
            }
            else if (selectedTable == 1) {
                if (wcslen(idBuffer) > 0) {
                    int configID = _wtoi(idBuffer);
                    loaded = db.loadSimulationParameters(configID, data);
                }
            }
            else if (selectedTable == 2) {
                if (wcslen(idBuffer) > 0) {
                    int simID = _wtoi(idBuffer);
                    loaded = db.loadSimulation(simID, data);
                }
            }

            if (loaded) {
                // Fill edit controls with loaded data
                const wchar_t** columns = nullptr;
                int colCount = 0;
                switch (selectedTable) {
                case 0: columns = LIQUIDS_COLUMNS; colCount = _countof(LIQUIDS_COLUMNS); break;
                case 1: columns = CONFIGS_COLUMNS; colCount = _countof(CONFIGS_COLUMNS); break;
                case 2: columns = SIMS_COLUMNS; colCount = _countof(SIMS_COLUMNS); break;
                }
                for (int i = 0; i < colCount; ++i) {
                    std::string scol = ExtractColumnName(columns[i]);
                    auto it = data.find(scol);
                    if (it != data.end()) {
                        std::wstring value(it->second.begin(), it->second.end());
                        SetWindowTextW(hDisplayControls[i], value.c_str());
                    }
                    else {
                        SetWindowTextW(hDisplayControls[i], L"");
                    }
                }
            }
            else {
                MessageBoxW(hWnd, L"Record not found.", L"Load", MB_OK | MB_ICONINFORMATION);
            }
            return true;
        }
        case IDC_BTN_UPDATE:
        {
            bool success = false;
            wchar_t buffer[256];

            if (selectedTable == 0) { // Liquids
                // Require PK (LiquidID) to update
                GetWindowTextW(hEditControls[0], buffer, 255); std::wstring wLiquidID = buffer;
                if (wLiquidID.empty()) {
                    MessageBoxW(hWnd, L"LiquidID is required for update.", L"Validation", MB_OK | MB_ICONWARNING);
                    return true;
                }
                std::map<std::string, std::string> liquidData;
                for (int i = 0; i < _countof(LIQUIDS_COLUMNS); ++i) {
                    GetWindowTextW(hEditControls[i], buffer, 255);
                    std::wstring wval = buffer;
                    std::string sval(wval.begin(), wval.end());
                    std::wstring wcol(LIQUIDS_COLUMNS[i]);
                    std::string scol = ExtractColumnName(wcol);
                    liquidData[scol] = sval;
                }
                int liquidID = _wtoi(wLiquidID.c_str());
                success = db.updateLiquidType(liquidID, liquidData);
            }
            else if (selectedTable == 1) { // Configs
                // Require PK (ConfigID) to update
                GetWindowTextW(hEditControls[0], buffer, 255); std::wstring wConfigID = buffer;
                if (wConfigID.empty()) {
                    MessageBoxW(hWnd, L"ConfigID is required for update.", L"Validation", MB_OK | MB_ICONWARNING);
                    return true;
                }
                std::map<std::string, std::string> params;
                for (int i = 0; i < _countof(CONFIGS_COLUMNS); ++i) {
                    GetWindowTextW(hEditControls[i], buffer, 255);
                    std::wstring wval = buffer;
                    std::string sval(wval.begin(), wval.end());
                    std::wstring wcol(CONFIGS_COLUMNS[i]);
                    std::string scol = ExtractColumnName(wcol);
                    params[scol] = sval;
                }
                int configID = _wtoi(wConfigID.c_str());
                success = db.updateSimulationParameters(configID, params);
            }
            else if (selectedTable == 2) { // Simulations
                // Require PK (SimulationID) to update
                GetWindowTextW(hEditControls[0], buffer, 255); std::wstring wSimID = buffer;
                if (wSimID.empty()) {
                    MessageBoxW(hWnd, L"SimulationID is required for update.", L"Validation", MB_OK | MB_ICONWARNING);
                    return true;
                }
                std::map<std::string, std::string> simData;
                for (int i = 0; i < _countof(SIMS_COLUMNS); ++i) {
                    GetWindowTextW(hEditControls[i], buffer, 255);
                    std::wstring wval = buffer;
                    std::string sval(wval.begin(), wval.end());
                    std::wstring wcol(SIMS_COLUMNS[i]);
                    std::string scol = ExtractColumnName(wcol);
                    simData[scol] = sval;
                }
                int simID = _wtoi(wSimID.c_str());
                success = db.updateSimulation(simID, simData);
            }

            if (success) {
                MessageBoxW(hWnd, L"Update successful.", L"Update", MB_OK | MB_ICONINFORMATION);
            }
            else {
                MessageBoxW(hWnd, L"Update failed.", L"Update", MB_OK | MB_ICONERROR);
            }
            return true;
        }
        default:
            return false;
        }
    }

private:
    // Utility to destroy a control if it exists
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