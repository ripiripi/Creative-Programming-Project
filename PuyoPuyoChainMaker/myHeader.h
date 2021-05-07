#pragma once
#include <windows.h>
#include <stdlib.h>
#include <string.h>
#include <tchar.h>
#include <atlimage.h>

#define GameWindowSizeX  816
#define GameWindowSizeY  489

extern HWND hWnd;
extern HWND puyoWnd;

void OperationPuyo(int KeyCode, int ScanCode);
void MovePuyo(int pos, int dir);
void Update();
