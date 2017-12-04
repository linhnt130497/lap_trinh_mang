#pragma once

//#define _WINSOCK_DEPRECATED_NO_WARNINGS
#include<WinSock2.h>
#include<Windows.h>
#include<windowsx.h>
#include<stdlib.h>
#include<mmsystem.h>
#include<mmreg.h>
#include<list>
#include<queue>

using namespace std;

#pragma comment (lib, "Winmm.lib")

typedef struct {
	DWORD m_dwSeq = 0; // so thu tu cua khoi
	DWORD m_nSize = 0; // kich thuoc
	DWORD m_nSizeP = 0; // kich thuoc khoi thuoc
	BYTE m_abData[65] = { 0 };
	BYTE m_abDataP[65] = { 0 };
} XMITDATA;

#define WM_USER_SOCKET WM_USER + 1

#define BLOCK_ALIGN 65 // 65byte for a block in GSM
#define PORT_NUMBER 8888
#define BLOCK_SIZE BLOCK_ALIGN
#define AVG_BYTES_PER_SEC 1625
#define NUM_BLOCKS ((AVG_BYTES_PER_SEC/ BLOCK_SIZE))//25 block / s
#define PLAYBACK_THRESHOLD 5
#define THRESHOLD  10