#include "myHeader.h"

static TCHAR szWindowClass[] = _T("DesktopApp");

static TCHAR szTitle[] = _T("Puyo Puyo Chain Maker");

HINSTANCE hInst;
static TCHAR colorstr[50];

POINT mouse_p;
static TCHAR mousestr[50];

HWND hWnd;
HWND puyoWnd;

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

    hInst = hInstance;

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

    puyoWnd = FindWindowA("PuyoChampions", NULL);
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

    OperationPuyo(VK_RETURN, 28);//プレイスタート
    Sleep(2500);
    //MovePuyo(2, 3);

    
    GetCursorPos(&mouse_p);
    _stprintf_s(mousestr, 50, TEXT("%d,%d"), int(mouse_p.x), int(mouse_p.y));
    
    ShowWindow(hWnd, nCmdShow);
    UpdateWindow(hWnd);

    MSG msg;
    while (true)
    {
        if (PeekMessage( &msg, NULL, 0, 0, PM_NOREMOVE ) ){
            if(GetMessage(&msg,NULL,0,0)){
            TranslateMessage( &msg );
            DispatchMessage( &msg );
            }
            else{
            break;
            }
        }
        else{
        // ここにゲーム関連の処理を記述する
            Update();
        }
    }

    return (int)msg.wParam;
}

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

        TextOut(hdc,
            5, 5,
            greeting, _tcslen(greeting));

        EndPaint(hWnd, &ps);
        
        break;
    case WM_MOUSEMOVE:
        GetCursorPos(&mouse_p);
        _stprintf_s(mouse, 50, TEXT("%d,%d"), int(mouse_p.x),int(mouse_p.y));
        //SetWindowText(hWnd, mouse);
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