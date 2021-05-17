#include "myHeader.h"
#include "Class_GameState.h"

const int ScanCodeReturn = 28;
const int ScanCodeV = 47;
const int ScanCodeZ = 44;
const int ScanCodeX = 45;
const int ScanCodeLeft = 75;
const int ScanCodeRight = 77;
const int ScanCodeDown = 80;
//�l�N�X�g�ƃl�N�l�N�̊Ԃ̔w�i�i�^�[���J�ڂ̌��o�p�j
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
    //�摜�̎擾
    CRect targetRect(0, 0, GameWindowSizeX, GameWindowSizeY);
    HDC hWndDC = GetDC(puyoWnd);

    CImage img;
    img.Create(targetRect.Width(), targetRect.Height(), 24);
    CImageDC imgDC(img);

    // �f�X�N�g�b�v�摜��DIBSECTION�֓]��
    BitBlt(imgDC, 0, 0, targetRect.Width(), targetRect.Height(), hWndDC, targetRect.left, targetRect.top, SRCCOPY);

    ReleaseDC(hWnd, hWndDC);
    COLORREF color = img.GetPixel(325, 120);
    TCHAR coldebug[50];
    
    _stprintf_s(coldebug, 50, TEXT("%d"), int(GetHue(color)));
    SetWindowText(hWnd, coldebug);

    return abs(GetHue(color) - HueScreen) >= 4;
}

int search() {//�T���̃��C���֐�
    int PuyoOrder[30];
    //�l�N�l�N�̑g�Ղ�̐F��ǂݍ���
    //PuyoOrder[0] = 
    //PuyoOrder[1] = 
    //PuyoOrder[2] = 
    for (int count = 0; count < 5; count++) {
        //���̎�ȍ~�̃c���������_���ɐ���(�����e�J�����@)
        for (int order = 3; order < 30; order++) {
            PuyoOrder[order] = xor128() % 25;//�c���������_������
        }
        /*
        memo:�c����5+5*4/2=15��ނ�����5*5�ł��������Ă悳�����i�c���̍����������Ȃ��Ă�ł��傤�I�j
        */

        //�����_���ɐ��������c����ɑ΂��āA�r�[���T�[�`�����s���A��Ԃ悢���̎������i��̔ԍ��ƁA�ő�A�����j
        /* 
        memo:�]���֐��ɂ́A�A�����N����ꍇ�ƋN����Ȃ��ꍇ�𕪂��čl����K�v������B
        �A�����N�����ꍇ�A�A�����̕]���Ƀ{�[�i�X������Ƃ悳�����H�i���܂ł����Ă����΂��Ȃ����ƂɂȂ���Ȃ��H�j
        */
        std::vector<GameState> states[30 + 1];
        //���R�A�����s������܂�

    }
    //5�̂����A�����Ƃ��ǂ����I�����AMovePuyo�ő�������s

    return 0;
}

void Update() {
    
    if (!IsTurnTransition()) return;
    if (isDowned) {//�����L�[�𗣂�
        DownPuyo();
    }
    MovePuyo(xor128() % 22 + 1);

    Sleep(120);
}