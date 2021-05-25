#include "myHeader.h"
#include "Class_GameState.h"
#include "Class_TimeController.h"

const int ScanCodeReturn = 28;
const int ScanCodeV = 47;
const int ScanCodeZ = 44;
const int ScanCodeX = 45;
const int ScanCodeLeft = 75;
const int ScanCodeRight = 77;
const int ScanCodeDown = 80;
//�l�N�X�g�ƃl�N�l�N�̊Ԃ̔w�i�i�^�[���J�ڂ̌��o�p�j
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
	pos:���Ղ�̔z�u�ʒu
        �͈�:-2-+3
		�����ʒu:0
		�E�ւ̈ړ���+1(����-1)
	dir:�Ղ�̉�]
		1:  ��]����
		0: ����](z)
		3:  �E��](x)
        2: ���](����ւ�)
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

bool IsTurnTransition() {
    //�摜�̎擾
    CRect targetRect(0, 0, GameWindowSizeX, GameWindowSizeY);
    HDC hWndDC = GetDC(puyoWnd);

    CImage img;
    img.Create(targetRect.Width(), targetRect.Height(), 24);
    CImageDC imgDC(img);

    // �f�X�N�g�b�v�摜��DIBSECTION�֓]��
    BitBlt(imgDC, 0, 0, targetRect.Width(), targetRect.Height(), hWndDC, targetRect.left, targetRect.top, SRCCOPY);

    ReleaseDC(hWnd, hWndDC);
    COLORREF color = img.GetPixel(325, 120);//325,120
    TCHAR coldebug[50];
    int hue = GetHue(GetRValue(color), GetGValue(color), GetBValue(color));
    
    _stprintf_s(coldebug, 50, TEXT("%d"), hue);
    //SetWindowText(hWnd, coldebug);
    
    if (abs(hue - HueScreen) >= 4) {
        TCHAR coldebug[50];
        _stprintf_s(coldebug, 50, TEXT("SARCHING_NOW"));
        SetWindowText(hWnd, coldebug);
    }
    return abs(hue - HueScreen) >= 4;
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
    case 0://�l�N�X�g�@��
        SearchPuyoPosition = std::make_pair(313,82);
        break;
    case 1://�l�N�X�g�@��
        SearchPuyoPosition = std::make_pair(313, 105);
        break;
    case 2://�l�N�l�N�@��
        SearchPuyoPosition = std::make_pair(333, 134);
        break;
    case 3://�l�N�l�N�@��
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
    //if(PuyoNum == 0)SetWindowText(hWnd, coldebug);
    
    if (hue >= 30 && hue <= 50)//yellow
        return PuyoColor::yellow;
    else if (hue >= 265 && hue <= 285)
        return PuyoColor::purple;//purple
    else if (hue >= 210 && hue <= 225)//blue
        return PuyoColor::blue;
    else if (hue >= 350 || hue <= 10)//red
        return PuyoColor::red;
    else if (hue >= 100 && hue <= 115)//green
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

GameState CurrentState;

int search() {//�T���̃��C���֐�
    int PuyoOrder[30];
    //�l�N�l�N�̑g�Ղ�̐F��ǂݍ���
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
    SetUsePuyoColor(Color[0]);
    SetUsePuyoColor(Color[1]);
    TwoNextPuyo = std::make_pair(UsePuyo[int(Color[0])], UsePuyo[int(Color[1])]);

    PuyoOrder[2] = TwoNextPuyo.first * 4 + TwoNextPuyo.second;

    const int BEAM_WIDTH = 50;
    const int MAX_DEPTH = 3;
    int memoryscore = 0;

    int totaltime = 0;
    std::vector<int> ReserveOperation(5);

    for (int count = 0; count < 1; count++) {
        //���̎�ȍ~�̃c���������_���ɐ���(�����e�J�����@)
        for (int order = 3; order < MAX_DEPTH; ++order) {
            PuyoOrder[order] = xor128() % 16;
        }
        //�����_���ɐ��������c����ɑ΂��āA�r�[���T�[�`�����s���A��Ԃ悢���̎������i��̔ԍ��ƁA�ő�A�����j
        std::vector<GameState> States[MAX_DEPTH + 1];
        GameState Initial_State = CurrentState;
        States[0].emplace_back(Initial_State);
        for (int depth = 0; depth < MAX_DEPTH; ++depth) {
            
            sort(States[depth].begin(), States[depth].end());
            reverse(States[depth].begin(), States[depth].end());
            if (States[depth].size() > BEAM_WIDTH)
                States[depth].erase(States[depth].begin() + BEAM_WIDTH, States[depth].end());
                
            
            for (int OperationNumber = 1; OperationNumber <= 22; OperationNumber++) {
                for (GameState State : States[depth]) {
                
                    TimeController time;
                    time.Reset();
                    State.OperationAndValueState(OperationNumber,
                                                        std::make_pair(signed char(PuyoOrder[depth] / 4),
                                                        signed char(PuyoOrder[depth] % 4)),
                                                        depth == 0);
                    totaltime += int(time.Current());
                    States[depth + 1].emplace_back(State);
                }
            }
        }
        sort(States[MAX_DEPTH].begin(), States[MAX_DEPTH].end());
        reverse(States[MAX_DEPTH].begin(), States[MAX_DEPTH].end());
        ReserveOperation[count] = States[MAX_DEPTH][0].FirstOperation;
        CurrentState.OperationAndValueState(ReserveOperation[count], std::make_pair(signed char(PuyoOrder[0] / 4),signed char(PuyoOrder[0] % 4)), true);
        memoryscore = States[MAX_DEPTH][0].MaxScore;

    }
    TCHAR coldebug[50];

    _stprintf_s(coldebug, 50, TEXT("%d"), memoryscore);
    SetWindowText(hWnd, coldebug);
    //5�̂����A�����Ƃ��ǂ����I�����AMovePuyo�ő�������s
    CurrentState.init();
    CurrentState.debug();
    return CurrentState.FirstOperation;
    
    std::vector<std::pair<int, int>> OperationCount(23, std::make_pair(0, 0));
    for (int count = 0; count < 5; ++count) {
        OperationCount[ReserveOperation[count]].first++;
        OperationCount[ReserveOperation[count]].second = ReserveOperation[count];
    }
    sort(OperationCount.begin(), OperationCount.end());
    if (OperationCount[4].first == 1) {
        return OperationCount[xor128() % 5].second;
    }
    else if (OperationCount[4].first == 2) {
        if (OperationCount[3].first == 2) return OperationCount[3 + xor128() % 2].second;
        else return OperationCount[4].second;
    }
    else {
        return OperationCount[4].second;
    }
    return OperationCount[4].second;
    //return 1;// xor128() % 22 + 1;
    
}

bool StartFlag2 = false;
bool StartFlag3 = false;
bool JudgeStart(){
    PuyoColor Color[4];
    //StartFlag = true;
    for (int i = 0; i < 4; ++i) {
        Color[i] = JudgePuyoColor(i);
        SetUsePuyoColor(Color[i]);
    }
    if (Color[0] != PuyoColor::none && Color[1] != PuyoColor::none && Color[2] != PuyoColor::none && Color[3] != PuyoColor::none) {
       
            StartFlag = true;

            NextPuyo = std::make_pair(UsePuyo[int(Color[0])], UsePuyo[int(Color[1])]);
            TwoNextPuyo = std::make_pair(UsePuyo[int(Color[2])], UsePuyo[int(Color[3])]);
            
            TCHAR coldebug[50];
            _stprintf_s(coldebug, 50, TEXT("START"));
            SetWindowText(hWnd, coldebug);
            
            return true;
        }
    
}
//error �ԁX����

void Update() {

    //JudgeStart��true�ɂȂ�܂Ń��[�v
    if (!StartFlag && JudgeStart())return;
    //JudgeStart �ȍ~�� IsTurnTransition�@���݂�
    if (!IsTurnTransition()) return;

    TimeController time;
    time.Reset();
    if (isDowned) {//�����L�[�𗣂�
        DownPuyo();
    }
    int ope = search();
    MovePuyo(ope);

    int jikan =int(time.Current());
    TCHAR coldebug[50];

    _stprintf_s(coldebug, 50, TEXT("%d"),ope);
    //SetWindowText(hWnd, coldebug);
}