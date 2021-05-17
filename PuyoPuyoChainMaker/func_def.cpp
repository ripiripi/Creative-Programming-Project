#include "myHeader.h"
#include "Class_GameState.h"

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

void MovePuyo(int num){//num(1-22)
    int dir, pos;
    dir = num % 4;
    pos = num / 4 - 2;
	/*
	pos:第二ぷよの配置位置
        範囲:-2-+3
		初期位置:0
		右への移動で+1(左で-1)
	dir:ぷよの回転
		1:  回転無し
		0: 左回転(z)
		3:  右回転(x)
        2: 一回転(入れ替え)
	*/
    
    switch (dir) {
    case 1:
        break;
    case 0:
        OperationPuyo(0x5A,ScanCodeZ);
        break;
    case 3:
        OperationPuyo(0x58, ScanCodeX);
        break;
    case 2:
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

int search() {//探索のメイン関数
    int PuyoOrder[30];
    //ネクネクの組ぷよの色を読み込む
    //PuyoOrder[0] = 
    //PuyoOrder[1] = 
    //PuyoOrder[2] = 
    for (int count = 0; count < 5; count++) {
        //次の手以降のツモをランダムに生成(モンテカルロ法)
        for (int order = 3; order < 30; order++) {
            PuyoOrder[order] = xor128() % 25;//ツモをランダム生成
        }
        /*
        memo:ツモは5+5*4/2=15種類だけど5*5でやっちゃってよさそう（ツモの作り方もこうなってるでしょう！）
        */

        //ランダムに生成したツモ列に対して、ビームサーチを実行し、一番よい次の手を決定（手の番号と、最大連鎖数）
        /* 
        memo:評価関数には、連鎖が起こる場合と起こらない場合を分けて考える必要がある。
        連鎖を起こす場合、連鎖数の評価にボーナスをつけるとよさそう？（いつまでたっても発火しないことにつながらない？）
        */
        std::vector<GameState> states[30 + 1];
        //当然連鎖実行操作も含む

    }
    //5つのうち、もっとも良い手を選択し、MovePuyoで操作を実行

    return 0;
}

void Update() {
    
    if (!IsTurnTransition()) return;
    if (isDowned) {//下矢印キーを離す
        DownPuyo();
    }
    MovePuyo(xor128() % 22 + 1);

    Sleep(120);
}