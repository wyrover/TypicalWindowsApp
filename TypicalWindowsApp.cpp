#include "stdafx.h"

// maximum length of resource string plus one
static const INT    s_nMaxLoadString = 512;

// the class name of the main window
static const TCHAR  s_szClassName[] = TEXT("TypicalWindowsApp");

// the application
struct WinApp {
    HINSTANCE   m_hInst;
    HWND        m_hWnd;
    HICON       m_hIcon;
    HACCEL      m_hAccel;

    // constructor
    WinApp(HINSTANCE hInst) {
        ZeroMemory(this, sizeof(*this));
        m_hInst = hInst;
    }

    // not thread-safe!
    LPTSTR loadString(UINT nID) {
        static TCHAR s_sz[s_nMaxLoadString];
        s_sz[0] = 0;
        ::LoadString(m_hInst, nID, s_sz, s_nMaxLoadString);
        return s_sz;
    }

    // not thread-safe!
    LPTSTR loadString2(UINT nID) {
        static TCHAR s_sz[s_nMaxLoadString];
        s_sz[0] = 0;
        ::LoadString(m_hInst, nID, s_sz, s_nMaxLoadString);
        return s_sz;
    }

    // register window classes
    bool registerClasses();

    // start up
    bool startup(INT nCmdShow) {
        // load accessories
        m_hIcon = ::LoadIcon(m_hInst, MAKEINTRESOURCE(1));
        m_hAccel = ::LoadAccelerators(m_hInst, MAKEINTRESOURCE(1));

        // create the main window
        ::CreateWindow(s_szClassName, loadString(1), WS_OVERLAPPEDWINDOW,
            CW_USEDEFAULT, 0, 
            CW_USEDEFAULT, 0, 
            NULL, NULL, m_hInst, this);
        if (m_hWnd == NULL) {
            MessageBoxA(NULL, "ERROR: CreateWindow", NULL, MB_ICONERROR);
            return false;
        }

        // show the window
        ::ShowWindow(m_hWnd, nCmdShow);
        ::UpdateWindow(m_hWnd);

        // process command line
        for (INT i = 1; i < __argc; ++i) {
            ::MessageBox(m_hWnd, __targv[i], TEXT("File"), MB_ICONINFORMATION);
        }
        return true;
    } // startup

    INT run() {
        MSG msg;
        // message loop
        while (::GetMessage(&msg, NULL, 0, 0)) {
            if (!::TranslateAccelerator(m_hWnd, m_hAccel, &msg)) {
                ::TranslateMessage(&msg);
                ::DispatchMessage(&msg);
            }
        }
        return INT(msg.wParam);
    } // run

    // WM_CREATE
    bool onCreate() {
        ::DragAcceptFiles(m_hWnd, TRUE);
        return true;
    } // onCreate

    // WM_DROPFILES
    void onDropFiles(HDROP hDrop) {
        TCHAR szPath[MAX_PATH];
        UINT cFiles = ::DragQueryFile(hDrop, 0xFFFFFFFF, NULL, 0);
        for (UINT i = 0; i < cFiles; ++i) {
            ::DragQueryFile(hDrop, i, szPath, MAX_PATH);
            ::MessageBox(m_hWnd, szPath, TEXT("File"), MB_ICONINFORMATION);
        }
        ::DragFinish(hDrop);
    } // onDropFiles

    // WM_DESTROY
    void onDestroy() {
        ::PostQuitMessage(0);
    } // onDestroy

    // WM_PAINT
    void onPaint() {
        RECT rc;
        ::GetClientRect(m_hWnd, &rc);

        PAINTSTRUCT ps;
        HDC hdc = ::BeginPaint(m_hWnd, &ps);
        if (hdc != NULL) {
            // draw something...
            SelectObject(hdc, ::GetStockObject(BLACK_PEN));
            ::MoveToEx(hdc, rc.left, rc.top, NULL);
            ::LineTo(hdc, rc.right, rc.bottom);
            ::MoveToEx(hdc, rc.right, rc.top, NULL);
            ::LineTo(hdc, rc.left, rc.bottom);

            static const char s_text[] = "TypicalWindowsApp";
            ::SetBkMode(hdc, TRANSPARENT);
            ::TextOutA(hdc, 0, 0, s_text, ::lstrlenA(s_text));

            ::EndPaint(m_hWnd, &ps);
        }
    } // onPaint

    // WM_SIZE
    void onSize(UINT type, INT width, INT height) {
        // NOTE: m_hWnd can be NULL at here. Needs check.
        if (m_hWnd == NULL) {
            return;
        }
        ::InvalidateRect(m_hWnd, NULL, TRUE);
    } // onSize

    // IDM_EXIT
    void onExit() {
        ::DestroyWindow(m_hWnd);
    } // onExit

    // IDM_ABOUT
    void onAbout();
}; // WinApp

// the dialog procedure of IDD_ABOUTBOX
INT_PTR CALLBACK
AboutDialogProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {
    switch (uMsg) {
    case WM_INITDIALOG:
        return TRUE;
    case WM_COMMAND:
        switch (LOWORD(wParam)) {
        case IDOK:
            ::EndDialog(hWnd, IDOK);
            break;
        case IDCANCEL:
            ::EndDialog(hWnd, IDCANCEL);
            break;
        default:
            break;
        }
        break;
    default:
        break;
    }
    return 0;
} // AboutDialogProc

void WinApp::onAbout() {
    ::DialogBox(m_hInst, MAKEINTRESOURCE(IDD_ABOUTBOX), m_hWnd,
        AboutDialogProc);
} // WinApp::onAbout

// the main window procedure
LRESULT CALLBACK
WindowProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {
    static WinApp *papp = NULL;
    LPCREATESTRUCT pcs;

    switch (uMsg) {
    case WM_CREATE:
        pcs = reinterpret_cast<LPCREATESTRUCT>(lParam);
        assert(pcs != NULL);
        papp = reinterpret_cast<WinApp *>(pcs->lpCreateParams);
        assert(papp != NULL);
        papp->m_hWnd = hWnd;
        if (!papp->onCreate()) {
            return -1;
        }
        break;
    case WM_COMMAND:
        switch (LOWORD(wParam)) {
        case IDM_EXIT:
            papp->onExit();
            break;
        case IDM_ABOUT:
            papp->onAbout();
            break;
        }
        break;
    case WM_SIZE:
        papp->onSize(UINT(wParam), LOWORD(lParam), HIWORD(lParam));
        break;
    case WM_PAINT:
        papp->onPaint();
        break;
    case WM_DROPFILES:
        papp->onDropFiles(reinterpret_cast<HDROP>(wParam));
        break;
    case WM_DESTROY:
        papp->onDestroy();
        break;
    default:
        return DefWindowProc(hWnd, uMsg, wParam, lParam);
    }

    return 0;
} // WindowProc

bool WinApp::registerClasses() {
    WNDCLASSEX wcx;

    ZeroMemory(&wcx, sizeof(wcx));
    wcx.cbSize = sizeof(wcx);
    wcx.style = CS_DBLCLKS;
    wcx.lpfnWndProc = WindowProc;
    wcx.hInstance = m_hInst;
    wcx.hIcon = ::LoadIcon(m_hInst, MAKEINTRESOURCE(1));
    wcx.hCursor = ::LoadCursor(NULL, IDC_ARROW);
    wcx.hbrBackground = reinterpret_cast<HBRUSH>(COLOR_3DFACE + 1);
    wcx.lpszMenuName = MAKEINTRESOURCE(1);
    wcx.lpszClassName = s_szClassName;
    wcx.hIconSm = ::LoadIcon(m_hInst, MAKEINTRESOURCE(1));
    if (!::RegisterClassEx(&wcx)) {
        return false;
    }
    return true;
} // WinApp::registerClasses

// Win32 App main function
extern "C"
INT APIENTRY _tWinMain(
    HINSTANCE   hInstance,
    HINSTANCE   hPrevInstance,
    LPTSTR      lpCmdLine,
    INT         nCmdShow)
{
    WinApp app(hInstance);

    if (!app.registerClasses()) {
        MessageBoxA(NULL, "ERROR: RegisterClass failed", NULL,
            MB_ICONERROR);
        return 1;
    }

    int ret;

    try {
        if (!app.startup(nCmdShow)) {
            ret = 2;
        } else {
            ret = app.run();
        }
    } catch (const std::bad_alloc&) {
        MessageBoxA(NULL, "ERROR: Out of memory", NULL,
            MB_ICONERROR);
        ret = -1;
    }

    return ret;
} // _tWinMain
