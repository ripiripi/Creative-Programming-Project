#include "myHeader.h"

const int ScanCodeReturn = 28;
const int ScanCodeV = 47;
const int ScanCodeZ = 44;
const int ScanCodeX = 45;
const int ScanCodeLeft = 75;
const int ScanCodeRight = 77;
const int ScanCodeDown = 80;
//ネクストとネクネクの間の背景（ターン遷移の検出用）
const int HueScreen = 195;
bool isDowned = false;

unsigned long xor128() {
    static unsigned long x = 123456789, y = 362436069, z = 521288629, w = 88675123;
    unsigned long t = (x ^ (x << 11));
    x = y;
    y = z;
    z = w;
    return (w = (w ^ (w >> 19)) ^ (t ^ (t >> 8)));
}

void OperationPuyo(int KeyCode,int ScanCode){//,bool WaitTime = true) {
    INPUT Press[1] = {};
    INPUT Release[1] = {};
    ZeroMemory(Press, sizeof(Press));
    ZeroMemory(Release, sizeof(Release));
    Press[0].type = INPUT_KEYBOARD;
    Press[0].ki.wVk = KeyCode;
    Press[0].ki.wScan = ScanCode;
    Press[0].ki.dwFlags = KEYEVENTF_SCANCODE;

    Release[0].type = INPUT_KEYBOARD;
    Release[0].ki.wVk = KeyCode;
    Release[0].ki.wScan = ScanCode;
    Release[0].ki.dwFlags = KEYEVENTF_KEYUP | KEYEVENTF_SCANCODE;

    SendInput(ARRAYSIZE(Press), Press, sizeof(INPUT));
    Sleep(100);
    SendInput(ARRAYSIZE(Release), Release, sizeof(INPUT));

    if (true) {
        Sleep(200);
    }
}

void DownPuyo() {
    INPUT Press[1] = {};
    INPUT Release[1] = {};
    ZeroMemory(Press, sizeof(Press));
    ZeroMemory(Release, sizeof(Release));
    Press[0].type = INPUT_KEYBOARD;
    Press[0].ki.wVk = 0x56;//VK_DOWN;
    Press[0].ki.wScan = ScanCodeV;
    Press[0].ki.dwFlags = KEYEVENTF_SCANCODE;

    Release[0].type = INPUT_KEYBOARD;
    Release[0].ki.wVk = 0x56;// VK_DOWN;
    Release[0].ki.wScan = ScanCodeV;
    Release[0].ki.dwFlags = KEYEVENTF_KEYUP | KEYEVENTF_SCANCODE;
    if (isDowned) {
        SendInput(ARRAYSIZE(Release), Release, sizeof(INPUT));
    }else
    SendInput(ARRAYSIZE(Press), Press, sizeof(INPUT));
    isDowned = !isDowned;
}

void MovePuyo(int pos,int dir){
	/*
	pos:第二ぷよの配置位置
        範囲:-2-+3
		初期位置:0
		右への移動で+1(左で-1)
	dir:ぷよの回転
		0:  回転無し
		1: 左回転(z)
		2:  右回転(x)
        3: 一回転(入れ替え)
	*/
    switch (dir) {
    case 0:
        break;
    case 1:
        OperationPuyo(0x5A,ScanCodeZ);
        break;
    case 2:
        OperationPuyo(0x58, ScanCodeX);
        break;
    case 3:
        OperationPuyo(0x5A, ScanCodeZ);
        OperationPuyo(0x5A, ScanCodeZ);
        break;
    }
    while (pos != 0) {
        if (pos > 0) {
            OperationPuyo(VK_RIGHT, ScanCodeRight);
            pos--;
        }
        if (pos < 0) {
            OperationPuyo(VK_LEFT, ScanCodeLeft);
            pos++;
        }
    }
    DownPuyo();

}
int GetHue(COLORREF color) {
    int hue = 0;
    int Max = 0;
    int Min = 0;
    int r = GetRValue(color);
    int g = GetGValue(color);
    int b = GetBValue(color);
    if (r == b == g)return 0;
    Max = max(r,max(b,g));
    Min = min(r,min(b, g));
    if (r >= g && r >= b) {
        hue = int(60.0 * (float(g - b) / float(Max - Min)));
    }
    else if (g >= r && g >= b) {
        hue = int(60.0 * (float(b-r) / float(Max - Min)))+120;
    }
    else {
        hue = int(60.0 * (float(r - g) / float(Max - Min)))+240;
    }
    if (hue < 0)hue += 360;
    return hue;
}
bool IsTurnTransition() {
    //画像の取得
    CRect targetRect(0, 0, GameWindowSizeX, GameWindowSizeY);
    HDC hWndDC = GetDC(puyoWnd);

    CImage img;
    img.Create(targetRect.Width(), targetRect.Height(), 24);
    CImageDC imgDC(img);

    // デスクトップ画像をDIBSECTIONへ転送
    BitBlt(imgDC, 0, 0, targetRect.Width(), targetRect.Height(), hWndDC, targetRect.left, targetRect.top, SRCCOPY);

    ReleaseDC(hWnd, hWndDC);
    COLORREF color = img.GetPixel(325, 120);
    TCHAR coldebug[50];
    
    _stprintf_s(coldebug, 50, TEXT("%d"), int(GetHue(color)));
    SetWindowText(hWnd, coldebug);

    return abs(GetHue(color) - HueScreen) >= 4;
}

void Update() {
    
    if (!IsTurnTransition()) return;
    if (isDowned) {
        DownPuyo();
    }
    xor128();
    MovePuyo(xor128() % 6 - 2, xor128() % 4);

    Sleep(120);
}