#include "myHeader.h"

// The main window class name.
static TCHAR szWindowClass[] = _T("DesktopApp");

// The string that appears in the application's title bar.
static TCHAR szTitle[] = _T("Puyo Puyo Chain Maker");

HINSTANCE hInst;
static TCHAR colorstr[50];

POINT mouse_p;
static TCHAR mousestr[50];

HWND hWnd;
HWND puyoWnd;



// Forward declarations of functions included in this code module:
LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);

int CALLBACK WinMain(
    _In_ HINSTANCE hInstance,
    _In_opt_ HINSTANCE hPrevInstance,
    _In_ LPSTR     lpCmdLine,
    _In_ int       nCmdShow)
{
    WNDCLASSEX wcex;

    wcex.cbSize = sizeof(WNDCLASSEX);
    wcex.style = CS_HREDRAW | CS_VREDRAW;
    wcex.lpfnWndProc = WndProc;
    wcex.cbClsExtra = 0;
    wcex.cbWndExtra = 0;
    wcex.hInstance = hInstance;
    wcex.hIcon = LoadIcon(hInstance, IDI_APPLICATION);
    wcex.hCursor = LoadCursor(NULL, IDC_ARROW);
    wcex.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
    wcex.lpszMenuName = NULL;
    wcex.lpszClassName = szWindowClass;
    wcex.hIconSm = LoadIcon(wcex.hInstance, IDI_APPLICATION);

    if (!RegisterClassEx(&wcex))
    {
        MessageBox(NULL,
            _T("Call to RegisterClassEx failed!"),
            _T("Windows Desktop Guided Tour"),
            NULL);

        return 1;
    }

    // Store instance handle in our global variable
    hInst = hInstance;

    // The parameters to CreateWindow explained:
    // szWindowClass: the name of the application
    // szTitle: the text that appears in the title bar
    // WS_OVERLAPPEDWINDOW: the type of window to create
    // CW_USEDEFAULT, CW_USEDEFAULT: initial position (x, y)
    // 500, 100: initial size (width, length)
    // NULL: the parent of this window
    // NULL: this application does not have a menu bar
    // hInstance: the first parameter from WinMain
    // NULL: not used in this application
    hWnd = CreateWindow(
        szWindowClass,
        szTitle,
        WS_OVERLAPPEDWINDOW,
        CW_USEDEFAULT, CW_USEDEFAULT,
        500, 100,
        NULL,
        NULL,
        hInstance,
        NULL
    );

    if (!hWnd)
    {
        MessageBox(NULL,
            _T("Call to CreateWindow failed!"),
            _T("Puyo Puyo Chain Maker"),
            NULL);

        return 1;
    }

    puyoWnd = FindWindowA("PuyoChampions", NULL);//PuyoChampions
    if (!puyoWnd) {
        MessageBox(NULL,
            _T("ゲームウィンドウが見つかりません。\n「ぷよぷよ eスポーツ」を起動してね"),
            _T("Puyo Puyo Chain Maker"),
            NULL);

        return 1;
    }

    if (!SetForegroundWindow(puyoWnd)) {
        MessageBox(NULL,
            _T("Error"),
            _T("Puyo Puyo Chain Maker"),
            NULL);

        return 1;
    }
    Sleep(300);
    

    SetWindowPos(hWnd, NULL, 0, GameWindowSizeY+1, 0, 0, SWP_NOZORDER | SWP_NOSIZE);
    SetWindowPos(puyoWnd, NULL, 0, 0, GameWindowSizeX, GameWindowSizeY, SWP_NOZORDER);

    Sleep(300);

    CRect targetRect(0, 0,GameWindowSizeX,GameWindowSizeY);
    HDC hWndDC = GetDC(puyoWnd);

    CImage img;
    img.Create(targetRect.Width(), targetRect.Height(), 24);
    CImageDC imgDC(img);

    // デスクトップ画像をDIBSECTIONへ転送
    BitBlt(imgDC, 0, 0, targetRect.Width(), targetRect.Height(), hWndDC, targetRect.left, targetRect.top, SRCCOPY);

    ReleaseDC(hWnd, hWndDC);
    COLORREF color = img.GetPixel(323, 112);
    int r = GetRValue(color);
    int g = GetGValue(color);
    int b = GetBValue(color);
    _stprintf_s(colorstr, 50, TEXT("%d,%d,%d"),r,g,b);

    GetCursorPos(&mouse_p);
    _stprintf_s(mousestr, 50, TEXT("%d,%d"), int(mouse_p.x), int(mouse_p.y));
    // The parameters to ShowWindow explained:
    // hWnd: the value returned from CreateWindow
    // nCmdShow: the fourth parameter from WinMain
    ShowWindow(hWnd, nCmdShow);
    UpdateWindow(hWnd);

    OperationPuyo(VK_RETURN, 28);//プレイスタート
    Sleep(300);
    MovePuyo(2, 3);


    // Main message loop:
    MSG msg;
    while (true)
    {
        if (PeekMessage( &msg, NULL, 0, 0, PM_NOREMOVE ) ){
            if(GetMessage(&msg,NULL,0,0)){
            TranslateMessage( &msg );
            DispatchMessage( &msg );
            }
            else{// ループ終了
            break;
            }
        }
        else{
        // デッドタイム
        // ここにゲーム関連の処理を記述する
        }
    }

    return (int)msg.wParam;
}

//  FUNCTION: WndProc(HWND, UINT, WPARAM, LPARAM)
//  PURPOSE:  Processes messages for the main window.
//  WM_PAINT    - Paint the main window
//  WM_DESTROY  - post a quit message and return
LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    PAINTSTRUCT ps;
    HDC hdc;
    TCHAR greeting[] = _T("Find GameWindow!");
    TCHAR mouse[50];

    switch (message)
    {
    case WM_PAINT:
        hdc = BeginPaint(hWnd, &ps);

        // Here your application is laid out.
        // For this introduction, we just print out "Hello, Windows desktop!"
        // in the top left corner.
        TextOut(hdc,
            5, 5,
            colorstr, _tcslen(colorstr));//greeting, _tcslen(greeting));
        // End application-specific layout section.

        EndPaint(hWnd, &ps);
        break;
    case WM_MOUSEMOVE:
        GetCursorPos(&mouse_p);
        _stprintf_s(mouse, 50, TEXT("%d,%d"), int(mouse_p.x),int(mouse_p.y));
        SetWindowText(hWnd, mouse);
        break;
    case WM_DESTROY:
        PostQuitMessage(0);
        break;
    default:
        return DefWindowProc(hWnd, message, wParam, lParam);
        break;
    }

    return 0;
}