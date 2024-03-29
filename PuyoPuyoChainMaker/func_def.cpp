#include "myHeader.h"
#include "Class_GameState.h"
#include "Class_TimeController.h"

const int ScanCodeReturn = 28;
const int ScanCodeEscape = 1;
const int ScanCodeV = 47;
const int ScanCodeZ = 44;
const int ScanCodeX = 45;
const int ScanCodeLeft = 75;
const int ScanCodeRight = 77;
const int ScanCodeDown = 80;
//ネクストとネクネクの間の背景（ターン遷移の検出用）
const int HueScreen = 195;
bool isStarted = false;
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
    int MPos = pos;
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
        if (MPos >= 0) {
            OperationPuyo(0x5A, ScanCodeZ);
            OperationPuyo(0x5A, ScanCodeZ);
        }
        else {
            OperationPuyo(0x58, ScanCodeX);
            OperationPuyo(0x58, ScanCodeX);
        }
        break;
    }
    
    DownPuyo();

}
int GetHue(int r,int g,int b) {
    int hue = 0;
    int Max = 0;
    int Min = 0;
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
int GetValue() {
    //画像の取得
    CRect targetRect(0, 0, GameWindowSizeX, GameWindowSizeY);
    HDC hWndDC = GetDC(puyoWnd);

    CImage img;
    img.Create(targetRect.Width(), targetRect.Height(), 24);
    CImageDC imgDC(img);

    // デスクトップ画像をDIBSECTIONへ転送
    BitBlt(imgDC, 0, 0, targetRect.Width(), targetRect.Height(), hWndDC, targetRect.left, targetRect.top, SRCCOPY);

    ReleaseDC(hWnd, hWndDC);
    COLORREF color = img.GetPixel(325, 120);//325,120

    return max(GetRValue(color), max(GetGValue(color), GetBValue(color)))* 100 / 255;
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
    COLORREF color = img.GetPixel(325, 120);//325,120
    TCHAR coldebug[50];
    int hue = GetHue(GetRValue(color), GetGValue(color), GetBValue(color));
    
    _stprintf_s(coldebug, 50, TEXT("%d"), hue);
    //SetWindowText(hWnd, coldebug);
    int Value = max(GetRValue(color), max(GetGValue(color), GetBValue(color)))*100 /255;
    
    if (abs(hue - HueScreen) >= 4 && Value >= 80) {
        TCHAR coldebug[50];
        _stprintf_s(coldebug, 50, TEXT("SARCHING_NOW"));
        SetWindowText(hWnd, coldebug);
    }
    return abs(hue - HueScreen) >= 4 && Value >= 80;
}

PuyoColor JudgePuyoColor(int PuyoNum) {
    CRect targetRect(0, 0, GameWindowSizeX, GameWindowSizeY);
    HDC hWndDC = GetDC(puyoWnd);
    CImage img;
    img.Create(targetRect.Width(), targetRect.Height(), 24);
    CImageDC imgDC(img);
    BitBlt(imgDC, 0, 0, targetRect.Width(), targetRect.Height(), hWndDC, targetRect.left, targetRect.top, SRCCOPY);
    ReleaseDC(hWnd, hWndDC);

    std::pair<int, int> SearchPuyoPosition;
    switch (PuyoNum) {
    case 0://ネクスト　上
        SearchPuyoPosition = std::make_pair(313,82);
        break;
    case 1://ネクスト　下
        SearchPuyoPosition = std::make_pair(313, 105);
        break;
    case 2://ネクネク　上
        SearchPuyoPosition = std::make_pair(333, 132);
        break;
    case 3://ネクネク　下
        SearchPuyoPosition = std::make_pair(333, 153);
        break;
    }
    int AveR = 0;
    int AveG = 0;
    int AveB = 0;
    int dX[5] = { -3,0,3,0,0 };
    int dY[5] = { 0,0,0,-3,3 };
    for (int num = 0; num < 5; num++) {
        
        COLORREF color = img.GetPixel(SearchPuyoPosition.first + dX[num], SearchPuyoPosition.second + dY[num]);
        AveR += GetRValue(color);
        AveG += GetGValue(color);
        AveB += GetBValue(color);
    }
    int hue = GetHue(AveR / 5 , AveG / 5, AveB / 5);
    TCHAR coldebug[50];
    _stprintf_s(coldebug, 50, TEXT("%d"), hue);
    //if(PuyoNum == 2)SetWindowText(hWnd, coldebug);
    
    if (hue >= 25 && hue <= 55)//yellow
        return PuyoColor::yellow;
    else if (hue >= 265 && hue <= 285)
        return PuyoColor::purple;//purple
    else if (hue >= 215 && hue <= 235)//blue
        return PuyoColor::blue;
    else if (hue >= 348 || hue <= 10)//red
        return PuyoColor::red;
    else if (hue >= 85 && hue <= 125)//green
        return PuyoColor::green;

    return PuyoColor::none;
}

std::pair<int,int> NextPuyo;
std::pair<int,int> TwoNextPuyo;
bool StartFlag = false;
bool UsePuyoAlreadySetFour = false;
std::vector<int> UsePuyo(5, -1);


void SetUsePuyoColor(const PuyoColor& Puyo) {

    if (UsePuyoAlreadySetFour)return;
    int UseColorCount = 0;
    for (int i = 0; i < 5; i++) {
        if (UsePuyo[i] != -1) {
            UseColorCount++;
        }
    }

    for (int i = 0; i < 5; i++) {
        if (Puyo == PuyoColor(i) && UsePuyo[i] == -1) {
            UsePuyo[i] = UseColorCount;
            UseColorCount++;
        }
    }

    if (UseColorCount == 4)
        UsePuyoAlreadySetFour = true;
}

int ccnt = 0;
GameState CurrentState;

int search() {//探索のメイン関数
    int PuyoOrder[49];
    //ネクネクの組ぷよの色を読み込む
    PuyoOrder[0] = NextPuyo.first * 4 + NextPuyo.second;
    PuyoOrder[1] = TwoNextPuyo.first * 4 + TwoNextPuyo.second;
    NextPuyo = TwoNextPuyo;

    PuyoColor Color[2];
    Color[0] = JudgePuyoColor(2);
    Color[1] = JudgePuyoColor(3);
    while (int(Color[0]) == -1 || int(Color[1]) == -1) {
        Sleep(5);
        Color[0] = JudgePuyoColor(2);
        Color[1] = JudgePuyoColor(3);
    }
    ccnt++;

    SetUsePuyoColor(Color[0]);
    SetUsePuyoColor(Color[1]);
    TwoNextPuyo = std::make_pair(UsePuyo[int(Color[0])], UsePuyo[int(Color[1])]);

    PuyoOrder[2] = TwoNextPuyo.first * 4 + TwoNextPuyo.second;

    //debug
    OperationPuyo(VK_ESCAPE, ScanCodeEscape);

    const int BEAM_WIDTH = 50;
    const int MAX_DEPTH = 28;
    const int TRY_NUM = 5;
    
    int totaltime = 0;
    std::vector<std::pair<int,int>> OpeAndRensaNum(TRY_NUM);//操作、連鎖数
    
    CurrentState.debug(true);
    
    for (int count = 0; count < TRY_NUM; count++) {

        //次の手以降のツモをランダムに生成(モンテカルロ法)
        for (int order = 3; order < MAX_DEPTH; ++order) {
            PuyoOrder[order] = xor128() % 16;
        }
        //ランダムに生成したツモ列に対して、ビームサーチを実行し、一番よい次の手を決定（手の番号と、最大連鎖数）
        std::vector<GameState> States[49 + 1];
        GameState Initial_State = CurrentState;
        States[0].emplace_back(Initial_State);

        std::vector<int> ope(23, 0);

        for (int depth = 0; depth < MAX_DEPTH; ++depth) {
            
            sort(States[depth].begin(), States[depth].end());

            if (States[depth].size() > BEAM_WIDTH)
                States[depth].erase(States[depth].begin() + BEAM_WIDTH, States[depth].end());
                
            for (int OperationNumber = 1; OperationNumber <= 22; ++OperationNumber) {
                if (PuyoOrder[depth] / 4 == PuyoOrder[depth] % 4 && OperationNumber % 2 == 0)continue;
                //if (xor128() % 2 == 0)continue;
                for (GameState State : States[depth]) {
                    
                    int rensa = State.OperationAndValueState(OperationNumber,
                                                            std::make_pair(signed char(PuyoOrder[depth] / 4),
                                                            signed char(PuyoOrder[depth] % 4)),
                                                            depth == 0);
                    States[depth + 1].emplace_back(State);
                    ope[State.FirstOperation] = max(ope[State.FirstOperation],rensa);
                }
            }
        }
        int MaxRensa =  0;
        int MaxOpe = 0;
        for (int OperationNumber = 1; OperationNumber <= 22; ++OperationNumber) {
            if (ope[OperationNumber] > MaxRensa) {
                MaxRensa = ope[OperationNumber];
                MaxOpe = OperationNumber;
            }
            else if (ope[OperationNumber] == MaxRensa && xor128() % 2 == 0) {
                MaxOpe = OperationNumber;
            }
        }
        OpeAndRensaNum[count] = std::make_pair(MaxOpe, MaxRensa);
    }
    
    int max_ren = 0;
    std::vector<int> ope(23, 0);
    for (int i = 0; i < TRY_NUM; ++i) {
        ope[OpeAndRensaNum[i].first] += OpeAndRensaNum[i].second;
        max_ren = max(max_ren, OpeAndRensaNum[i].second);
    }
    int ReturnOpe = 0;
    int curMax = -100000;
    for (int i = 1; i <= 22; ++i) {
        if (ope[i] > curMax) {
            curMax = ope[i];
            ReturnOpe = i;
        }
    }

    CurrentState.debug(false);
    

    CurrentState.OperationAndValueState(ReturnOpe, std::make_pair(signed char(PuyoOrder[0] / 4), signed char(PuyoOrder[0] % 4)), true);
    CurrentState.init();

    TCHAR coldebug[50];
    _stprintf_s(coldebug, 50, TEXT("%d"), signed char(max_ren));
    //SetWindowText(hWnd, coldebug);

    //ポーズ解除
    OperationPuyo(VK_RETURN, 28);
    Sleep(550);
    
    return ReturnOpe;
}


bool JudgeStart(){
    PuyoColor Color[4];
    //StartFlag = true;
    
    for (int i = 0; i < 4; ++i) {
        Color[i] = JudgePuyoColor(i);
    }

    int Value = GetValue();
    TCHAR coldebug[50];

    _stprintf_s(coldebug, 50, TEXT("%d"), Value);
    //SetWindowText(hWnd, coldebug);
    if (Value < 85 && Color[0] != PuyoColor::none && Color[1] != PuyoColor::none && Color[2] != PuyoColor::none && Color[3] != PuyoColor::none) {
            
            for (int i = 0; i < 4; ++i) {
                SetUsePuyoColor(Color[i]);
            }

            StartFlag = true;

            NextPuyo = std::make_pair(UsePuyo[int(Color[0])], UsePuyo[int(Color[1])]);
            TwoNextPuyo = std::make_pair(UsePuyo[int(Color[2])], UsePuyo[int(Color[3])]);
            
            TCHAR coldebug[50];
            _stprintf_s(coldebug, 50, TEXT("START"));
            //SetWindowText(hWnd, coldebug);
            
            return true;
    }
    return false;
}
//error 赤々黄紫

void Update() {

    //JudgeStartがtrueになるまでループ
    if (!StartFlag && JudgeStart())return;
    //JudgeStart 以降は IsTurnTransition　をみる
    if (!IsTurnTransition()) return;

    if (isDowned) {//下矢印キーを離す
        DownPuyo();
    }
    Sleep(50);
    int ope = search();
    MovePuyo(ope);
}