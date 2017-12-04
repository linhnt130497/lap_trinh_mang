#pragma once
#include "stdafx.h"
#include "CSendBuffer.h"
#include "CRecvBuffer.h"

typedef std::queue<CSendBuffer*> CSendBufQ;
typedef std::list<CRecvBuffer*>  CRecvBufL;
typedef CRecvBufL::iterator      CBufLIter;

class CSoundWindow {
public:
	// --- OnConect() ---//
	HWND			m_hWnd;
	HWAVEIN			m_hWaveIn;
	CSendBuffer		m_aInBlocks[NUM_BLOCKS]; // 25 // 25 blocks/s
	int				m_iCountIn; // count buffer sent to capture device
	DWORD			m_dwOutSeq;  // number of buffer
	SOCKET			m_Socket;
	SOCKADDR_IN		m_SockAddr;
	bool			m_fOutClosing; // true if playback device is Closing
	bool		    m_fInClosing;

	//--- OnWimData() ---//
	DWORD			m_nPrevSize;
	CSendBufQ		m_SendBufQueue;// m_qpXmitBufs
	BYTE			m_abPrevData[BLOCK_SIZE];


	// --- OnWomOpen() ---//
	HWAVEOUT		m_hWaveOut;
	CRecvBuffer		m_aOutBlocks[NUM_BLOCKS * 2];
	CRecvBufL		m_lpFreeBufs;
	int				m_iCountOut;
	DWORD			m_dwSeqExp;


	// ---- OnSocketRead() ---- //
	CRecvBufL	m_lpPlayBufs;


	// ----  JitterControl() ---- //
	bool m_fDelay;


	// --- Cancle() ---//
	bool m_fExiting;

public:
	void OnInit(HWND hWnd);
	
	bool OnConnect();

	void OnWimOpen();

	// khi du lieu duoc ghi day vao buffer: MM_WOM_DONE
	void OnWimData(WAVEHDR *pHdrWave);

	// chuc nang send data
	void OnSocketWrite();

	void OnWimClose();

	void OnWomOpen();

	void OnsocketRead();
	void JitterControl();
	void RecoverPrevData(CRecvBuffer *pBuffer);

	void OnWomDone(WAVEHDR* pHdrWave);

	void OnCancle();
	void OnDisconnect();

	void OnWomClose();
};

