#include "CSoundWindow.h"


bool CSoundWindow :: OnConnect() {
	GSM610WAVEFORMAT WaveFormatGSM;
	MMRESULT mmRC;

	// setup socket
	ZeroMemory(&m_SockAddr, sizeof(m_SockAddr));
	m_Socket = socket(AF_INET, SOCK_DGRAM, IPPROTO_IP);
	m_SockAddr.sin_family = AF_INET;
	m_SockAddr.sin_port = htons(PORT_NUMBER);
	m_SockAddr.sin_addr.s_addr = inet_addr("127.0.0.1");

	bind(m_Socket, (SOCKADDR*) &m_SockAddr, sizeof(m_SockAddr));
	connect(m_Socket, (SOCKADDR*) &m_SockAddr, sizeof(m_SockAddr));

	// inittialize WaveFormatGSM610
	WaveFormatGSM.wfx.wFormatTag = WAVE_FORMAT_GSM610;
	WaveFormatGSM.wfx.nChannels = 1; 
	WaveFormatGSM.wfx.nSamplesPerSec = 8000; 
	WaveFormatGSM.wfx.nAvgBytesPerSec = 1625; // 1625 bytes / s
	WaveFormatGSM.wfx.nBlockAlign = 65;
	WaveFormatGSM.wfx.wBitsPerSample = 0;
	WaveFormatGSM.wfx.cbSize = 2;
	WaveFormatGSM.wSamplesPerBlock = 320;

	// Open playback device
	mmRC = waveOutOpen(&m_hWaveOut, (UINT)WAVE_MAPPER, (LPWAVEFORMATEX)&(WaveFormatGSM.wfx), (DWORD)m_hWnd, (DWORD) NULL, CALLBACK_WINDOW);

	if (mmRC != MMSYSERR_NOERROR) {
		printf(" CSoundWindow.cpp: Error open playback device at line : %d , error code: %d\n", __LINE__, GetLastError());
	}
	else {
		m_fOutClosing = false;
		printf(" CSoundWindow.cpp: Open playback device success !! line: %d\n", __LINE__);
	}

	// open capture device
	mmRC = waveInOpen(&m_hWaveIn, (UINT)WAVE_MAPPER,(LPWAVEFORMATEX)&(WaveFormatGSM.wfx),(DWORD)m_hWnd, NULL, CALLBACK_WINDOW);
	if (mmRC != MMSYSERR_NOERROR)
		printf("CSoundWindow.cpp: Error open capture device at line : %d, error code : %d\n", __LINE__, GetLastError());
	else {
		m_fInClosing = false;
		waveInStart(m_hWaveIn);
		printf("CSoundWindow.cpp: open capture device is sucess !! line: %d \n", __LINE__);
	}

	if (!(m_fInClosing && m_fOutClosing)) {
		printf(" CSoundWindow.cpp: have a lost of device are error ! line: %d \n", __LINE__);
		return false;
	}

	return true;
	
}



// the message is sent to  my window when a waveform - audio input device is open by waveInOpen() runtine
void CSoundWindow::OnWimOpen() {
	// Prepare 25 buffers and send to waveIn device
	m_dwOutSeq = 0; // setup number of outbuffer now is 0
	m_iCountIn = 0; // no have buffer in capture device
	for (int i = 0; i < NUM_BLOCKS; i++) {
		// prepare 25 buffer 
		m_aInBlocks[i].Prepare(m_hWaveIn);
		//send to waveIn device
		m_aInBlocks[i].Add(m_hWaveIn);
		m_iCountIn++; // 
	}
}



// the waveIn device will return data block when write full buffer and send a message to my windows
void CSoundWindow::OnWimData(WAVEHDR *pHdrWave) {
	CSendBuffer *pAudioBuffer; // a pointer to buffer
	XMITDATA *pXmitData; // a pointer to data block will send

	m_iCountIn--; 
	pAudioBuffer = (CSendBuffer*)(pHdrWave->dwUser);
	pAudioBuffer->Unprepare(m_hWaveIn);

	if (!m_fInClosing) {
		pXmitData = &(pAudioBuffer->m_Data);

		// define data to send and chane them
		pXmitData->m_nSize = (DWORD)(pHdrWave->dwBytesRecorded);
		pXmitData->m_dwSeq = m_dwOutSeq++;
		pXmitData->m_nSizeP = m_nPrevSize;

		// copy data block, we can recovery when have a data block is lost
		memcpy(pXmitData->m_abDataP, m_abPrevData, m_nPrevSize);

		// prepare this data willbe ...
		m_nPrevSize = pXmitData->m_nSize;
		memset(m_abPrevData, 0, sizeof(m_abPrevData));
		memcpy(m_abPrevData, pXmitData->m_abData, m_nPrevSize);

		// insert to Queue
		m_SendBufQueue.push(pAudioBuffer);
		OnSocketWrite(); //  we will send data in this function
	}
	else {
		if (m_iCountIn == 0) {
			waveInClose(m_hWaveIn);
		}
	}
} // end OnWimData function




void CSoundWindow::OnSocketWrite() {
	CSendBuffer *pBuffer;
	if (m_fInClosing)
		return;
	
	while (!m_SendBufQueue.empty()) {// have data block in Queue
		pBuffer = m_SendBufQueue.front();
		if (send(m_Socket, (char*) &(pBuffer->m_Data), sizeof(XMITDATA), 0) == SOCKET_ERROR) {
			printf("CSoundWindow.cpp: send data occur error !! line: %d \n", __LINE__);
			break;
		}
		m_SendBufQueue.pop(); // remove
		memset(pBuffer, 0, sizeof(CSendBuffer));
		pBuffer->Prepare(m_hWaveIn);
		pBuffer->Add(m_hWaveIn);
		m_iCountIn++;
	}
}



// MM_WIM_CLOSE message is sent to my window when capture device is closed
void CSoundWindow::OnWimClose() {// MM_WIM_CLOSE 
	m_hWaveIn = 0;
	printf("CSoundWindow.cpp: capture device is closed !! line: %d \n", __LINE__);
}



//MM_WOM_OPEN message is sent to my window when call weveOutOpen(...) routine
void CSoundWindow::OnWomOpen() {
	m_iCountOut = 0;
	m_dwSeqExp = 0;// so luong block nhan duoc

	for (int i = 0; i < NUM_BLOCKS * 2; i++ ) {
		m_aOutBlocks[i].Prepare(m_hWaveOut);
		m_lpFreeBufs.push_back(&(m_aOutBlocks[i]));
	}
}// prepare to send toplayback device



//  FD_READ
void CSoundWindow::OnsocketRead() {
	CRecvBuffer *pBuffer;
	XMITDATA *pData;

	if (m_fOutClosing)
		return;

	if (m_lpFreeBufs.empty()) {// neu bo nho dem trong, su dung bo nho tam de luu
		XMITDATA Data;
		recv(m_Socket, (char*)&Data, sizeof(XMITDATA), 0);
		printf("CSoundWindow.cpp: No free buffer !! line: %d \n", __LINE__);
		return;
	}

	pBuffer = (CRecvBuffer*)(m_lpFreeBufs.front());
	pData = &(pBuffer->m_Data);
	if (recv(m_Socket, (char*) pData, sizeof(*pData), 0) == SOCKET_ERROR) {
		printf("CSoundWindow.cpp: Socket error !! line: %d \n", __LINE__);
	}
	else {
		if (pData->m_dwSeq == 0)
			m_dwSeqExp = 0;

		if (pData->m_dwSeq >= m_dwSeqExp) {
			CBufLIter Iter;

			for (Iter = m_lpPlayBufs.begin(); Iter != m_lpPlayBufs.end(); Iter ++) {
				if ((*Iter)->m_Data.m_dwSeq == pData->m_dwSeq)
					return;// don't insert

				if ((*Iter)->m_Data.m_dwSeq > pData->m_dwSeq)
					break;
			}

			m_lpFreeBufs.pop_front(); // remove first Item
			m_lpPlayBufs.insert(Iter, pBuffer);

			JitterControl();
		}
	}
}

void CSoundWindow::JitterControl() {
	CRecvBuffer *pBuffer;
	if (m_fDelay) { // playback is delaying
		if (m_lpPlayBufs.size() >= THRESHOLD) {// buffers > 10
			// start play
			m_fDelay = false;

			for (int i = 0; i < THRESHOLD; i++) {
				pBuffer = m_lpPlayBufs.front();
				m_lpPlayBufs.pop_front();

				// check dwSeq
				if (pBuffer->m_Data.m_dwSeq == (m_dwSeqExp + 1)) {
					RecoverPrevData(pBuffer);
					i++;
					pBuffer->Prepare(m_hWaveOut);
					pBuffer->Add(m_hWaveOut);
				}
				else {// play
					pBuffer->Prepare(m_hWaveOut);
					pBuffer->Add(m_hWaveOut);
				}

				m_iCountOut++;
				m_dwSeqExp = pBuffer->m_Data.m_dwSeq + 1;

			}// end for
		}// end if buffer > 10
		return;
	}

	// if delay = false

	if (m_iCountOut == 0) {
		// delay = true vi day la buffer dau tien nhan duoc
		m_fDelay = true;
		printf("CSoundWindow.cpp: m_iCountOut = 0 !! line: %d \n", __LINE__);
		return;
	}

	for (;;) {
		if (m_lpPlayBufs.empty())
			return;

		pBuffer = m_lpPlayBufs.front();

		// recover if can
		if (pBuffer->m_Data.m_dwSeq == (m_dwSeqExp + 1)) {
			RecoverPrevData(pBuffer);
			m_dwSeqExp++;
		}

		if (pBuffer->m_Data.m_dwSeq == m_dwSeqExp) {
			pBuffer->Prepare(m_hWaveOut);
			pBuffer->Add(m_hWaveOut);
			m_iCountOut ++ ;
			m_dwSeqExp = pBuffer->m_Data.m_dwSeq + 1;
			m_lpPlayBufs.pop_front();
			continue;
		}

		if (m_iCountOut < PLAYBACK_THRESHOLD) {
			m_dwSeqExp = pBuffer->m_Data.m_dwSeq + 1;
			printf("CSoundWindow.cpp: data is skipping ( tich luy )  !! line: %d \n", __LINE__);
			continue;
		}
		break;
	} // end for
}


void CSoundWindow::RecoverPrevData(CRecvBuffer *pBuffer) {
	CRecvBuffer *pBufferP;

	if (m_lpFreeBufs.empty()) {// error if no have free buffer
		printf("CSoundWindow.cpp: no freeBuffer  !! line: %d \n", __LINE__);
		return;
	}

	pBufferP = m_lpFreeBufs.front();
	m_lpFreeBufs.pop_front();

	pBufferP->m_Data.m_nSize = pBuffer->m_Data.m_nSizeP;
	memcpy(pBufferP->m_Data.m_abData, pBuffer->m_Data.m_abDataP, pBufferP->m_Data.m_nSize);

	pBufferP->Prepare(m_hWaveOut);
	pBufferP->Add(m_hWaveOut);
	m_iCountOut++;

}


// The MM_WOM_DONE message is sent to a window 
//when the given output buffer is being returned to the application
void CSoundWindow::OnWomDone(WAVEHDR *pHdrWave) {
	CRecvBuffer *pBuffer;
	// playback done -- Unprepare buffer and add to free list
	pBuffer = (CRecvBuffer*)(pHdrWave->dwUser);
	pBuffer->Unprepare(m_hWaveOut);
	m_iCountOut--;
	m_lpFreeBufs.push_back(pBuffer);// nghi cho nay co van de vi chua prepare
	
	if(!m_fOutClosing){
		JitterControl();
	}
	else {
		if (m_iCountOut == 0)
			waveOutClose(m_hWaveOut);
	}
}// end OnWomDone



void CSoundWindow::OnCancle() {
	if ((m_hWaveOut != 0) || (m_hWaveIn != 0)) {
		OnDisconnect();
		m_fExiting = true;
	}
	else {
		printf("CSoundWindow.cpp: the call is closed  !! line: %d \n", __LINE__);
	}
}



void CSoundWindow::OnDisconnect() {
	if (m_hWaveOut != 0) { 
		m_fOutClosing = true; 
		closesocket(m_Socket);  
		waveOutReset(m_hWaveOut);

		if (m_iCountOut == 0) 
			waveOutClose(m_hWaveOut); 

	} if (m_hWaveIn != 0) { 
		m_fInClosing = true; 
							 
		waveInReset(m_hWaveIn);   
		if (m_iCountIn == 0) 
			waveInClose(m_hWaveIn);  
	} return; 
} 


void CSoundWindow::OnWomClose() {
	m_hWaveOut = 0;
	printf("CSoundWindow.cpp: the palyback device is closing  !! line: %d \n", __LINE__);
}

void CSoundWindow::OnInit(HWND hWnd) {
	m_hWnd = hWnd;
	m_fExiting = false;
	m_fInClosing = m_fOutClosing = true;
	m_hWaveIn = 0;
	m_hWaveOut = 0;
	m_fDelay = false;

	WSAAsyncSelect(m_Socket, m_hWnd, WM_USER_SOCKET, FD_READ | FD_WRITE);
}

