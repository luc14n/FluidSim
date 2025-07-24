// FluidSimGUI.cpp : Defines the entry point for the application.
// Main GUI logic for FluidSim, including window creation, toolbar (ribbon), and page management.

#include "framework.h"
#include "FluidSimGUI.h"
#include "Resource.h"

#pragma comment(lib, "comctl32.lib")
#pragma comment(lib, "uxtheme.lib")
#pragma comment(lib, "dwmapi.lib")

#define MAX_LOADSTRING 100

// Global Variables
HINSTANCE hInst;                                // Current instance
WCHAR szTitle[MAX_LOADSTRING];                  // The title bar text
WCHAR szWindowClass[MAX_LOADSTRING];            // The main window class name
HWND g_hToolbar = nullptr;                      // Toolbar (ribbon) handle
Page* g_currentPage = nullptr;                  // Pointer to the current page
map<string, HBRUSH> g_brushes;

ColorMode g_colorMode = ColorMode::Light;

// Forward declarations
ATOM                MyRegisterClass(HINSTANCE hInstance);
BOOL                InitInstance(HINSTANCE, int);
LRESULT CALLBACK    WndProc(HWND, UINT, WPARAM, LPARAM);
INT_PTR CALLBACK    About(HWND, UINT, WPARAM, LPARAM);

/**
 * @brief Application entry point. Initializes the database, registers the window class,
 *        creates the main window, and runs the message loop.
 */
int APIENTRY wWinMain(_In_ HINSTANCE hInstance,
    _In_opt_ HINSTANCE hPrevInstance,
    _In_ LPWSTR    lpCmdLine,
    _In_ int       nCmdShow)
{
    UNREFERENCED_PARAMETER(hPrevInstance);
    UNREFERENCED_PARAMETER(lpCmdLine);

    SetProcessDPIAware();

    // --- Database Initialization ---
    sqlite3* db = nullptr;
    bool dbExists = (_waccess(L"FluidSim.db", 0) == 0);
    int rc = sqlite3_open("FluidSim.db", &db);
    if (rc) {
        MessageBox(nullptr, L"Can't open database", L"Error", MB_OK | MB_ICONERROR);
        return rc;
    }

    // If the database is new, initialize schema from schema.sql
    if (!dbExists) {
        HRSRC hRes = FindResource(nullptr, MAKEINTRESOURCE(SCHEMA_SQL), RT_RCDATA);
        if (!hRes) {
            MessageBox(nullptr, L"Missing schema resource", L"Error", MB_OK | MB_ICONERROR);
            sqlite3_close(db);
            return FALSE;
        }
        HGLOBAL hData = LoadResource(nullptr, hRes);
        if (!hData) {
            MessageBox(nullptr, L"Failed to load schema resource", L"Error", MB_OK | MB_ICONERROR);
            sqlite3_close(db);
            return FALSE;
        }
        DWORD dataSize = SizeofResource(nullptr, hRes);
        const char* schemaSQL = static_cast<const char*>(LockResource(hData));
        if (!schemaSQL) {
            MessageBox(nullptr, L"Failed to lock schema resource", L"Error", MB_OK | MB_ICONERROR);
            sqlite3_close(db);
            return FALSE;
        }
        char* errMsg = nullptr;
        rc = sqlite3_exec(db, schemaSQL, nullptr, nullptr, &errMsg);
        if (rc != SQLITE_OK) {
            MessageBoxA(nullptr, errMsg, "SQL Error", MB_OK | MB_ICONERROR);
            sqlite3_free(errMsg);
            sqlite3_close(db);
            return FALSE;
        }
    }

    // --- Window Class and Main Window Creation ---
    LoadStringW(hInstance, IDS_APP_TITLE, szTitle, MAX_LOADSTRING);
    LoadStringW(hInstance, IDC_FLUIDSIMGUI, szWindowClass, MAX_LOADSTRING);
    MyRegisterClass(hInstance);

    if (!InitInstance(hInstance, nCmdShow)) {
        sqlite3_close(db);
        return FALSE;
    }

    HACCEL hAccelTable = LoadAccelerators(hInstance, MAKEINTRESOURCE(IDC_FLUIDSIMGUI));

    // --- Main Message Loop ---
    MSG msg;
    while (GetMessage(&msg, nullptr, 0, 0)) {
        if (!TranslateAccelerator(msg.hwnd, hAccelTable, &msg)) {
            TranslateMessage(&msg);
            DispatchMessage(&msg);
        }
    }

    sqlite3_close(db);
    return (int)msg.wParam;
}

/**
 * @brief Registers the main window class.
 */
ATOM MyRegisterClass(HINSTANCE hInstance)
{
    WNDCLASSEXW wcex = {};
    wcex.cbSize = sizeof(WNDCLASSEX);
    wcex.style = CS_HREDRAW | CS_VREDRAW;
    wcex.lpfnWndProc = WndProc;
    wcex.cbClsExtra = 0;
    wcex.cbWndExtra = 0;
    wcex.hInstance = hInstance;
    wcex.hIcon = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_FLUIDSIMGUI_STD));
    wcex.hCursor = LoadCursor(nullptr, IDC_ARROW);
    wcex.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
    wcex.lpszMenuName = MAKEINTRESOURCEW(IDC_FLUIDSIMGUI);
    wcex.lpszClassName = szWindowClass;
    wcex.hIconSm = LoadIcon(wcex.hInstance, MAKEINTRESOURCE(IDI_FLUIDSIMGUI_STD));
    return RegisterClassExW(&wcex);
}

/**
 * @brief Initializes the main application window, toolbar (ribbon), and sets the default page.
 */
BOOL InitInstance(HINSTANCE hInstance, int nCmdShow)
{
    hInst = hInstance;

    HWND hWnd = CreateWindowW(szWindowClass, szTitle, WS_OVERLAPPEDWINDOW,
        CW_USEDEFAULT, 0, CW_USEDEFAULT, 0, nullptr, nullptr, hInstance, nullptr);

	// Initialize global brushes for color modes
    InitBrushes();

	// ApplyColorMode is a function that applies the current color mode to the window.
    ApplyColorMode(hWnd);

    // Set the default page to Home
    g_currentPage = CreatePGHome();
    if (g_currentPage) g_currentPage->Show(hWnd, hInst);

    // Initialize common controls (required for toolbar/ribbon)
    INITCOMMONCONTROLSEX icex = { sizeof(INITCOMMONCONTROLSEX), ICC_BAR_CLASSES };
    InitCommonControlsEx(&icex);

    // --- Create Toolbar (Ribbon) ---
    g_hToolbar = CreateWindowEx(0, TOOLBARCLASSNAME, nullptr,
        WS_CHILD | WS_VISIBLE | TBSTYLE_FLAT | TBSTYLE_TOOLTIPS,
        0, 0, 0, 0, hWnd, nullptr, hInst, nullptr);

    SetWindowTheme(g_hToolbar, L"", L"");

    if (g_hToolbar) {
        UpdateToolbarIcons();

        // Define toolbar buttons
        TBBUTTON tbb[2] = {};
        tbb[0].iBitmap = 0;
        tbb[0].idCommand = IDM_TOOLBAR_HOME;
        tbb[0].fsState = TBSTATE_ENABLED;
        tbb[0].fsStyle = TBSTYLE_BUTTON;
        tbb[0].iString = (INT_PTR)L"Home";
        tbb[1].iBitmap = 1;
        tbb[1].idCommand = IDM_TOOLBAR_DATABASE;
        tbb[1].fsState = TBSTATE_ENABLED;
        tbb[1].fsStyle = TBSTYLE_BUTTON;
        tbb[1].iString = (INT_PTR)L"Database";

        SendMessage(g_hToolbar, TB_BUTTONSTRUCTSIZE, (WPARAM)sizeof(TBBUTTON), 0);
        SendMessage(g_hToolbar, TB_ADDBUTTONS, 2, (LPARAM)&tbb);
        SendMessage(g_hToolbar, TB_AUTOSIZE, 0, 0);
        

		// Add padding to the top of the toolbar
        RECT rc;
        GetWindowRect(g_hToolbar, &rc);
        int toolbarHeight = (rc.bottom - rc.top) + 64; // 8px top + 8px bottom
        SetWindowPos(g_hToolbar, nullptr, 0, 8, rc.right - rc.left, toolbarHeight, SWP_NOZORDER | SWP_NOACTIVATE);
        ShowWindow(g_hToolbar, SW_SHOW);
    }

    if (!hWnd) {
        return FALSE;
    }

    ShowWindow(hWnd, nCmdShow);
    UpdateWindow(hWnd);
    return TRUE;
}

/**
 * @brief Main window procedure. Handles all messages for the main window, including
 *        command routing, painting, and page switching.
 */
LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    switch (message)
    {
    case WM_COMMAND:
    {
        int wmId = LOWORD(wParam);
        // Handle menu and toolbar commands for page switching
        switch (wmId)
        {
        case IDM_ABOUT:
            DialogBox(hInst, MAKEINTRESOURCE(IDD_ABOUTBOX), hWnd, About);
            break;
        case IDM_EXIT:
            DestroyWindow(hWnd);
            break;
        case IDM_SETTINGS:
            if (DialogBox(hInst, MAKEINTRESOURCE(IDD_SETTINGSBOX), hWnd, SettingsDlgProc) == IDOK) {
                ApplyColorMode(hWnd);
            }
            break;
        case IDM_HOME:
        case IDM_TOOLBAR_HOME:
            // Switch to Home page
            if (g_currentPage) {
                g_currentPage->Hide(hWnd);
                delete g_currentPage;
            }
            g_currentPage = CreatePGHome();
            g_currentPage->Show(hWnd, hInst);
            InvalidateRect(hWnd, nullptr, TRUE);
            break;
        case IDM_DATABASE:
        case IDM_TOOLBAR_DATABASE:
            // Switch to Database page
            if (g_currentPage) {
                g_currentPage->Hide(hWnd);
                delete g_currentPage;
            }
            g_currentPage = CreatePGDatabase();
            g_currentPage->Show(hWnd, hInst);
            InvalidateRect(hWnd, nullptr, TRUE);
            break;
        default:
            // Delegate page-specific commands to the current page
            if (g_currentPage && g_currentPage->HandleCommand(hWnd, wParam, lParam))
                break;
            return DefWindowProc(hWnd, message, wParam, lParam);
        }
    }
    break;
    case WM_ERASEBKGND:
    {
        switch (g_colorMode)
        {
            case ColorMode::Dark:
            {
            HDC hdc = (HDC)wParam;
            RECT rc;
            GetClientRect(hWnd, &rc);
            // Use a dark gray color (e.g., RGB(32,32,32))
            HBRUSH hBrush = CreateSolidBrush(DARKMODE_DARK_GREY);
            FillRect(hdc, &rc, hBrush);
            DeleteObject(hBrush);
            return 1; // Indicate we handled background erase
            break;
            }
			case ColorMode::Light:
                // For light mode, let the default background be drawn
                return DefWindowProc(hWnd, message, wParam, lParam);
                break;
            default:
				return DefWindowProc(hWnd, message, wParam, lParam);
        }
        break;
    }
    case WM_PAINT:
    {
        PAINTSTRUCT ps;
        HDC hdc = BeginPaint(hWnd, &ps);

        // Offset drawing area by toolbar height
        RECT clientRect;
        GetClientRect(hWnd, &clientRect);
        if (g_hToolbar) {
            RECT tbRect;
            GetWindowRect(g_hToolbar, &tbRect);
            clientRect.top += (tbRect.bottom - tbRect.top);
        }

        // Fill background for dark mode (in case WM_ERASEBKGND missed any area)
        if (g_colorMode == ColorMode::Dark) {
            HBRUSH hBrush = CreateSolidBrush(RGB(32, 32, 32));
            FillRect(hdc, &clientRect, hBrush);
            DeleteObject(hBrush);
        }

        // Draw the current page below the ribbon
        if (g_currentPage)
            g_currentPage->Draw(hWnd, hdc, &clientRect);

        EndPaint(hWnd, &ps);
    }
    break;
    case WM_DESTROY:
        // Cleanup current page and exit
        if (g_currentPage) {
            g_currentPage->Hide(hWnd);
            delete g_currentPage;
            g_currentPage = nullptr;
        }
        for (auto& pair : g_brushes) {
            if (pair.second) DeleteObject(pair.second);
        }
        g_brushes.clear();
        PostQuitMessage(0);
        break;
    case WM_SIZE:
        // Resize toolbar to fit new window size
        if (g_hToolbar)
            SendMessage(g_hToolbar, TB_AUTOSIZE, 0, 0);
        break;
    case WM_NOTIFY:
    {
        LPNMHDR lpnmh = (LPNMHDR)lParam;
        if (lpnmh->hwndFrom == g_hToolbar && lpnmh->code == NM_CUSTOMDRAW)
        {
            LPNMTBCUSTOMDRAW pcd = (LPNMTBCUSTOMDRAW)lParam;
            switch (pcd->nmcd.dwDrawStage)
            {
            case CDDS_PREPAINT:
            {
                // Paint the toolbar background
                HDC hdc = pcd->nmcd.hdc;
                RECT rc;
                GetClientRect(g_hToolbar, &rc);
                HBRUSH hBrush = CreateSolidBrush(
                    (g_colorMode == ColorMode::Dark) ? DARKMODE_PURPLE : LIGHTMODE_PURPLE
                );
                FillRect(hdc, &rc, hBrush);
                DeleteObject(hBrush);
                // Allow default drawing of buttons, etc.
                return CDRF_NOTIFYITEMDRAW;
            }
            case CDDS_ITEMPREPAINT:
                // Set text color for toolbar buttons
                if (g_colorMode == ColorMode::Dark)
                    pcd->clrText = DARKMODE_LIGHT_TEXT;
                if (g_colorMode == ColorMode::Light)
					pcd->clrText = LIGHTMODE_DARK_TEXT;
                // else: let system default
                return CDRF_DODEFAULT;
            }
        }
        break;
    }
    case WM_CTLCOLORSTATIC:
    {
        HDC hdcStatic = (HDC)wParam;
        HWND hStatic = (HWND)lParam;
        int ctrlId = GetDlgCtrlID(hStatic);

        // Example: Label title
        if (ctrlId > IDC_LABEL_TITLE && ctrlId <= IDC_LABEL_TITLE_MAX) {
            SetBkColor(hdcStatic, g_colorMode == ColorMode::Dark ? DARKMODE_PURPLE : LIGHTMODE_PURPLE);
            SetTextColor(hdcStatic, g_colorMode == ColorMode::Dark ? DARKMODE_LIGHT_TEXT : LIGHTMODE_DARK_TEXT);
            return (INT_PTR)g_brushes[g_colorMode == ColorMode::Dark ? "LabelTitleBg_Dark" : "LabelTitleBg_Light"];
        }
        // Example: Label Buttons
        if (ctrlId > IDC_LABEL_BTTN && ctrlId <= IDC_LABEL_BTTN_MAX) {
            SetBkColor(hdcStatic, g_colorMode == ColorMode::Dark ? DARKMODE_BLUE : LIGHTMODE_BLUE);
            SetTextColor(hdcStatic, g_colorMode == ColorMode::Dark ? DARKMODE_LIGHT_TEXT : LIGHTMODE_DARK_TEXT);
            return (INT_PTR)g_brushes[g_colorMode == ColorMode::Dark ? "Button_Dark" : "Button_Light"];
        }
        // Example: Label Display
        if (ctrlId > IDC_LABEL_DISP && ctrlId <= IDC_LABEL_DISP_MAX) {
            SetBkColor(hdcStatic, g_colorMode == ColorMode::Dark ? DARKMODE_LIGHT_GREY : LIGHTMODE_LIGHT_GREY);
            SetTextColor(hdcStatic, g_colorMode == ColorMode::Dark ? DARKMODE_LIGHT_TEXT : LIGHTMODE_DARK_TEXT);
            return (INT_PTR)g_brushes[g_colorMode == ColorMode::Dark ? "Display_Dark" : "Display_Light"];
        }
        
        // --- Fallback: Use a default brush if no specific brush is found ---
        string fallbackKey = g_colorMode == ColorMode::Dark ? "DARKMODE_LIGHT_GREY" : "LIGHTMODE_DARK_GREY";
        auto it = g_brushes.find(fallbackKey);
        if (it != g_brushes.end() && it->second) {
            SetBkColor(hdcStatic, g_colorMode == ColorMode::Dark ? DARKMODE_LIGHT_GREY : LIGHTMODE_DARK_GREY);
            SetTextColor(hdcStatic, g_colorMode == ColorMode::Dark ? DARKMODE_LIGHT_TEXT : LIGHTMODE_DARK_TEXT);
            return (INT_PTR)it->second;
        }
        else {
            // As a last resort, use a stock brush
            return (INT_PTR)GetStockObject(WHITE_BRUSH);
        }
        break;
    }
    case WM_CTLCOLORBTN:
    {
        HDC hdcBtn = (HDC)wParam;
        HWND hBtn = (HWND)lParam;
        SetBkColor(hdcBtn, g_colorMode == ColorMode::Dark ? DARKMODE_MID_GREY : LIGHTMODE_MID_GREY);
        SetTextColor(hdcBtn, g_colorMode == ColorMode::Dark ? DARKMODE_LIGHT_TEXT : LIGHTMODE_DARK_TEXT);
        return (INT_PTR)g_brushes[g_colorMode == ColorMode::Dark ? "Button_Dark" : "Button_Light"];
    }
    case WM_CTLCOLOREDIT:
    {
        HDC hdcEdit = (HDC)wParam;
        HWND hEdit = (HWND)lParam;
        SetBkColor(hdcEdit, g_colorMode == ColorMode::Dark ? DARKMODE_MID_GREY : LIGHTMODE_LIGHT_GREY);
        SetTextColor(hdcEdit, g_colorMode == ColorMode::Dark ? DARKMODE_LIGHT_TEXT : LIGHTMODE_DARK_TEXT);
        return (INT_PTR)g_brushes[g_colorMode == ColorMode::Dark ? "Edit_Dark" : "Edit_Light"];
    }
    default:
        return DefWindowProc(hWnd, message, wParam, lParam);
    }
    return 0;
}

/**
 * @brief Dialog procedure for the About box.
 */
INT_PTR CALLBACK About(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
    UNREFERENCED_PARAMETER(lParam);
    switch (message)
    {
    case WM_INITDIALOG:
    {
        wchar_t versionText[128];
        swprintf(versionText, 128, L"FluidSim, Version %s", FLUIDSIM_VERSION);
        SetDlgItemText(hDlg, IDC_VERSION_TEXT, versionText);
        return (INT_PTR)TRUE;
    }
    case WM_COMMAND:
        if (LOWORD(wParam) == IDOK || LOWORD(wParam) == IDCANCEL)
        {
            EndDialog(hDlg, LOWORD(wParam));
            return (INT_PTR)TRUE;
        }
        break;
    }
    return (INT_PTR)FALSE;
}

/**
 * @brief Dialog procedure for the Settings dialog.
 *        Allows the user to select between Light and Dark color modes.
 */
INT_PTR CALLBACK SettingsDlgProc(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
    switch (message)
    {
    case WM_INITDIALOG:
        // Set radio based on current mode
        CheckRadioButton(hDlg, IDC_RADIO_LIGHT, IDC_RADIO_DARK,
            g_colorMode == ColorMode::Dark ? IDC_RADIO_DARK : IDC_RADIO_LIGHT);
        return (INT_PTR)TRUE;
    case WM_COMMAND:
        switch (LOWORD(wParam))
        {
        case IDOK:
            g_colorMode = IsDlgButtonChecked(hDlg, IDC_RADIO_DARK) ? ColorMode::Dark : ColorMode::Light;
            EndDialog(hDlg, IDOK);
            return (INT_PTR)TRUE;
        case IDCANCEL:
            EndDialog(hDlg, IDCANCEL);
            return (INT_PTR)TRUE;
        }
        break;
    }
    return (INT_PTR)FALSE;
}

/**
 * @brief Applies the selected color mode to the main window and its controls.
 *        Sets dark mode attributes, changes icons, and updates the theme.
 */
void ApplyColorMode(HWND hWnd)
{
    BOOL dark = (g_colorMode == ColorMode::Dark);
    // Set dark title bar (Windows 10 1809+)
    const DWORD DWMWA_USE_IMMERSIVE_DARK_MODE = 20;
    DwmSetWindowAttribute(hWnd, DWMWA_USE_IMMERSIVE_DARK_MODE, &dark, sizeof(dark));
    // Set dark theme for controls (optional, for child controls)
    SetWindowTheme(hWnd, dark ? L"DarkMode_Explorer" : L"", nullptr);

    // Change icon if you have different icons for each mode
    int iconId = dark ? IDI_FLUIDSIMGUI_INV : IDI_FLUIDSIMGUI_STD;
    HICON hIcon = LoadIcon(hInst, MAKEINTRESOURCE(iconId));
    SendMessage(hWnd, WM_SETICON, ICON_BIG, (LPARAM)hIcon);
    SendMessage(hWnd, WM_SETICON, ICON_SMALL, (LPARAM)hIcon);

	// Enable dark mode for the toolbar
    UpdateToolbarIcons();

	// Update the background brush for the title label
    if (g_brushes["LabelTitleBg"]) {
        DeleteObject(g_brushes["LabelTitleBg"]);
        g_brushes["LabelTitleBg"] = nullptr;
    }
    COLORREF color = (g_colorMode == ColorMode::Dark) ? DARKMODE_PURPLE : LIGHTMODE_PURPLE;
    g_brushes["LabelTitleBg"] = CreateSolidBrush(color);

    // Force a redraw
    InvalidateRect(hWnd, nullptr, TRUE);
    InvalidateRect(g_hToolbar, nullptr, TRUE);
}

/**
 * @brief Updates the toolbar icons based on the current color mode.
 *        This function replaces the image list with new icons for dark/light modes.
 */
void UpdateToolbarIcons()
{
    if (!g_hToolbar) return;

    // Remove any existing image list
    HIMAGELIST hOld = (HIMAGELIST)SendMessage(g_hToolbar, TB_GETIMAGELIST, 0, 0);
    if (hOld) ImageList_Destroy(hOld);

    // Choose icon resources based on color mode
    int iconHome = (g_colorMode == ColorMode::Dark) ? IDI_HOME_INV : IDI_HOME_STD;
    int iconDatabase = (g_colorMode == ColorMode::Dark) ? IDI_DATABASE_INV : IDI_DATABASE_STD;

    HIMAGELIST hImageList = ImageList_Create(32, 32, ILC_COLOR32 | ILC_MASK, 2, 2);
    HICON hIconHome = (HICON)LoadImage(hInst, MAKEINTRESOURCE(iconHome), IMAGE_ICON, 32, 32, LR_DEFAULTCOLOR | LR_CREATEDIBSECTION);
    ImageList_AddIcon(hImageList, hIconHome);
    HICON hIconDatabase = (HICON)LoadImage(hInst, MAKEINTRESOURCE(iconDatabase), IMAGE_ICON, 32, 32, LR_DEFAULTCOLOR | LR_CREATEDIBSECTION);
    ImageList_AddIcon(hImageList, hIconDatabase);

    SendMessage(g_hToolbar, TB_SETIMAGELIST, 0, (LPARAM)hImageList);

    // Optionally, force a redraw
    InvalidateRect(g_hToolbar, nullptr, TRUE);
}

/**
 * @brief Initializes global brushes for the application.
 *        Creates brushes for light and dark modes, used for various UI elements.
 */
void InitBrushes() {
    if (g_brushes.empty()) {
        // Label title background
        g_brushes["LabelTitleBg_Light"] = CreateSolidBrush(LIGHTMODE_PURPLE);
        g_brushes["LabelTitleBg_Dark"] = CreateSolidBrush(DARKMODE_PURPLE);

        // Button background
        g_brushes["Button_Light"] = CreateSolidBrush(LIGHTMODE_MID_GREY);
        g_brushes["Button_Dark"] = CreateSolidBrush(DARKMODE_MID_GREY);

        // Display area background
        g_brushes["Display_Light"] = CreateSolidBrush(LIGHTMODE_LIGHT_GREY);
        g_brushes["Display_Dark"] = CreateSolidBrush(DARKMODE_LIGHT_GREY);

        // Add more as needed for other control types
        // Example: Edit control background
        g_brushes["Edit_Light"] = CreateSolidBrush(LIGHTMODE_LIGHT_GREY);
        g_brushes["Edit_Dark"] = CreateSolidBrush(DARKMODE_MID_GREY);

        // Fallback: create a brush for any colorref constant not already covered
        g_brushes["DARKMODE_DARK_GREY"] = CreateSolidBrush(DARKMODE_DARK_GREY);
        g_brushes["DARKMODE_MID_GREY"] = CreateSolidBrush(DARKMODE_MID_GREY);
        g_brushes["DARKMODE_LIGHT_GREY"] = CreateSolidBrush(DARKMODE_LIGHT_GREY);
        g_brushes["DARKMODE_PURPLE"] = CreateSolidBrush(DARKMODE_PURPLE);
        g_brushes["DARKMODE_BLUE"] = CreateSolidBrush(DARKMODE_BLUE);
        g_brushes["DARKMODE_LIGHT_TEXT"] = CreateSolidBrush(DARKMODE_LIGHT_TEXT);

        g_brushes["LIGHTMODE_DARK_GREY"] = CreateSolidBrush(LIGHTMODE_DARK_GREY);
        g_brushes["LIGHTMODE_MID_GREY"] = CreateSolidBrush(LIGHTMODE_MID_GREY);
        g_brushes["LIGHTMODE_LIGHT_GREY"] = CreateSolidBrush(LIGHTMODE_LIGHT_GREY);
        g_brushes["LIGHTMODE_PURPLE"] = CreateSolidBrush(LIGHTMODE_PURPLE);
        g_brushes["LIGHTMODE_BLUE"] = CreateSolidBrush(LIGHTMODE_BLUE);
        g_brushes["LIGHTMODE_DARK_TEXT"] = CreateSolidBrush(LIGHTMODE_DARK_TEXT);
	}
}