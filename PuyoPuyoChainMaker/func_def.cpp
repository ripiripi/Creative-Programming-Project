#include "myHeader.h"


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
const int ScanCodeReturn = 28;
const int ScanCodeZ = 44;
const int ScanCodeX = 45;
const int ScanCodeLeft = 75;
const int ScanCodeRight = 77;
const int ScanCodeDown = 80;

void MovePuyo(int pos,int dir){
	/*
	pos:���Ղ�̔z�u�ʒu
        �͈�:-2-+3
		�����ʒu:0
		�E�ւ̈ړ���+1(����-1)
	dir:�Ղ�̉�]
		0:  ��]����
		1: ����](z)
		2:  �E��](x)
        3: ���](����ւ�)
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

}


