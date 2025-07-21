// FluidSimGUI.cpp : Defines the entry point for the application.
//

#include "framework.h"
#include "FluidSimGUI.h"
#include "Resource.h"

#pragma comment(lib, "comctl32.lib")

#define MAX_LOADSTRING 100

// Global Variables:
HINSTANCE hInst;                                // current instance
WCHAR szTitle[MAX_LOADSTRING];                  // The title bar text
WCHAR szWindowClass[MAX_LOADSTRING];            // the main window class name
INT g_showScreen = 0;                   // Flag to show home screen
HWND g_hToolbar = nullptr;				   // Toolbar handle

// Forward declarations of functions included in this code module:
ATOM                MyRegisterClass(HINSTANCE hInstance);
BOOL                InitInstance(HINSTANCE, int);
LRESULT CALLBACK    WndProc(HWND, UINT, WPARAM, LPARAM);
INT_PTR CALLBACK    About(HWND, UINT, WPARAM, LPARAM);

int APIENTRY wWinMain(_In_ HINSTANCE hInstance,
	_In_opt_ HINSTANCE hPrevInstance,
	_In_ LPWSTR    lpCmdLine,
	_In_ int       nCmdShow)
{
	UNREFERENCED_PARAMETER(hPrevInstance);
	UNREFERENCED_PARAMETER(lpCmdLine);

	SetProcessDPIAware();

	// Initialize SQLite Database
	sqlite3* db = nullptr;
	bool dbExists = (_waccess(L"FluidSim.db", 0) == 0);

	int rc = sqlite3_open("FluidSim.db", &db);
	if (rc) {
		MessageBox(nullptr, L"Can't open database", L"Error", MB_OK | MB_ICONERROR);
		return rc;
	}

	// If the database is new, initialize schema from schema.sql
	if (!dbExists) {
		// Find and load the resource
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

	// Initialize global strings
	LoadStringW(hInstance, IDS_APP_TITLE, szTitle, MAX_LOADSTRING);
	LoadStringW(hInstance, IDC_FLUIDSIMGUI, szWindowClass, MAX_LOADSTRING);
	MyRegisterClass(hInstance);

	// Perform application initialization:
	if (!InitInstance(hInstance, nCmdShow))
	{
		sqlite3_close(db);
		return FALSE;
	}

	HACCEL hAccelTable = LoadAccelerators(hInstance, MAKEINTRESOURCE(IDC_FLUIDSIMGUI));

	MSG msg;

	// Main message loop:
	while (GetMessage(&msg, nullptr, 0, 0))
	{
		if (!TranslateAccelerator(msg.hwnd, hAccelTable, &msg))
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
	}

	sqlite3_close(db);
	return (int)msg.wParam;
}



//
//  FUNCTION: MyRegisterClass()
//
//  PURPOSE: Registers the window class.
//
ATOM MyRegisterClass(HINSTANCE hInstance)
{
	WNDCLASSEXW wcex;

	wcex.cbSize = sizeof(WNDCLASSEX);

	wcex.style = CS_HREDRAW | CS_VREDRAW;
	wcex.lpfnWndProc = WndProc;
	wcex.cbClsExtra = 0;
	wcex.cbWndExtra = 0;
	wcex.hInstance = hInstance;
	wcex.hIcon = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_FLUIDSIMGUI));
	wcex.hCursor = LoadCursor(nullptr, IDC_ARROW);
	wcex.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
	wcex.lpszMenuName = MAKEINTRESOURCEW(IDC_FLUIDSIMGUI);
	wcex.lpszClassName = szWindowClass;
	wcex.hIconSm = LoadIcon(wcex.hInstance, MAKEINTRESOURCE(IDI_SMALL));

	return RegisterClassExW(&wcex);
}

//
//   FUNCTION: InitInstance(HINSTANCE, int)
//
//   PURPOSE: Saves instance handle and creates main window
//
//   COMMENTS:
//
//        In this function, we save the instance handle in a global variable and
//        create and display the main program window.
//
BOOL InitInstance(HINSTANCE hInstance, int nCmdShow)
{
	hInst = hInstance; // Store instance handle in our global variable

	HWND hWnd = CreateWindowW(szWindowClass, szTitle, WS_OVERLAPPEDWINDOW,
		CW_USEDEFAULT, 0, CW_USEDEFAULT, 0, nullptr, nullptr, hInstance, nullptr);

	// Initialize common controls
	INITCOMMONCONTROLSEX icex = { sizeof(INITCOMMONCONTROLSEX), ICC_BAR_CLASSES };
	InitCommonControlsEx(&icex);

	// Create toolbar
	g_hToolbar = CreateWindowEx(0, TOOLBARCLASSNAME, nullptr,
		WS_CHILD | WS_VISIBLE | TBSTYLE_FLAT | TBSTYLE_TOOLTIPS,
		0, 0, 0, 0, hWnd, nullptr, hInst, nullptr);

	if (g_hToolbar)
	{
		// Add image list for toolbar icons
		HIMAGELIST hImageList = ImageList_Create(32, 32, ILC_COLOR32 | ILC_MASK, 1, 1);
		
		HICON hIconHome = (HICON)LoadImage(hInst, MAKEINTRESOURCE(IDI_HOME), IMAGE_ICON, 32, 32, LR_DEFAULTCOLOR);
		ImageList_AddIcon(hImageList, hIconHome);

		HICON hIconDatabase = (HICON)LoadImage(hInst, MAKEINTRESOURCE(IDI_DATABASE), IMAGE_ICON, 32, 32, LR_DEFAULTCOLOR);
		ImageList_AddIcon(hImageList, hIconDatabase);

		SendMessage(g_hToolbar, TB_SETIMAGELIST, 0, (LPARAM)hImageList);

		// Define the Toolbar buttons
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
		ShowWindow(g_hToolbar, SW_SHOW);
	}

	if (!hWnd)
	{
		return FALSE;
	}

	ShowWindow(hWnd, nCmdShow);
	UpdateWindow(hWnd);

	return TRUE;
}

//
//  FUNCTION: WndProc(HWND, UINT, WPARAM, LPARAM)
//
//  PURPOSE: Processes messages for the main window.
//
//  WM_COMMAND  - process the application menu
//  WM_PAINT    - Paint the main window
//  WM_DESTROY  - post a quit message and return
//
//
LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	switch (message)
	{
	case WM_COMMAND:
	{
		int wmId = LOWORD(wParam);
		// Parse the menu selections:
		switch (wmId)
		{
		case IDM_ABOUT:
			DialogBox(hInst, MAKEINTRESOURCE(IDD_ABOUTBOX), hWnd, About);
			break;
		case IDM_EXIT:
			DestroyWindow(hWnd);
			break;
		case IDM_DATABASE:
		case IDM_TOOLBAR_DATABASE:
			g_showScreen = IDM_DATABASE;
			InvalidateRect(hWnd, nullptr, TRUE); // Request redraw
			break;
		case IDM_HOME:
		case IDM_TOOLBAR_HOME:
			g_showScreen = IDM_HOME;
			InvalidateRect(hWnd, nullptr, TRUE); // Request redraw
			break;
		default:
			return DefWindowProc(hWnd, message, wParam, lParam);
		}
	}
	break;
	case WM_PAINT:
	{
		PAINTSTRUCT ps;
		HDC hdc = BeginPaint(hWnd, &ps);

		// Get client area
		RECT clientRect;
		GetClientRect(hWnd, &clientRect);

		// Offset by toolbar height if toolbar exists
		if (g_hToolbar)
		{
			RECT tbRect;
			GetWindowRect(g_hToolbar, &tbRect);
			clientRect.top += (tbRect.bottom - tbRect.top);
		}

		// Fill background
		switch (g_showScreen)
		{
			case IDM_DATABASE:
			case IDM_TOOLBAR_DATABASE:
			{
				// Draw a simple database screen
				const wchar_t* dbTitle = L"Database Management";
				const wchar_t* dbDesc = L"Manage your fluid simulation data here.";
				SetBkMode(hdc, TRANSPARENT);
				DrawTextW(hdc, dbTitle, -1, &clientRect, DT_CENTER | DT_TOP | DT_SINGLELINE);
				RECT descRect = clientRect;
				descRect.top += 40;
				DrawTextW(hdc, dbDesc, -1, &descRect, DT_CENTER | DT_TOP | DT_SINGLELINE);
			}
				break;
			case IDM_HOME:
			case IDM_TOOLBAR_HOME:
			default:
			{
				// Draw a simple home screen
				const wchar_t* homeTitle = L"Welcome to FluidSim!";
				const wchar_t* homeDesc = L"Select a menu option to begin.";

				SetBkMode(hdc, TRANSPARENT);
				DrawTextW(hdc, homeTitle, -1, &clientRect, DT_CENTER | DT_TOP | DT_SINGLELINE);

				RECT descRect = clientRect;
				descRect.top += 40;
				DrawTextW(hdc, homeDesc, -1, &descRect, DT_CENTER | DT_TOP | DT_SINGLELINE);
			}
		}

		EndPaint(hWnd, &ps);
	}
	break;
	case WM_DESTROY:
		PostQuitMessage(0);
		break;
	case WM_SIZE:
		if (g_hToolbar)
			SendMessage(g_hToolbar, TB_AUTOSIZE, 0, 0);
		break;
	default:
		return DefWindowProc(hWnd, message, wParam, lParam);
	}
	return 0;
}

// Message handler for about box.
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
