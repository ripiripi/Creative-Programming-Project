#pragma once
#include <windows.h>
#include <stdlib.h>
#include <string.h>
#include <tchar.h>
#include <atlimage.h>
#include <chrono>
#include <vector>

#define GameWindowSizeX  816
#define GameWindowSizeY  489

extern HWND hWnd;
extern HWND puyoWnd;


void OperationPuyo(int KeyCode, int ScanCode);
void MovePuyo(int num);
void Update();
unsigned long xor128();

enum class PuyoColor
{
	none = -1,
	red = 0,
	green = 1,
	blue = 2,
	yellow = 3,
	purple = 4,
};