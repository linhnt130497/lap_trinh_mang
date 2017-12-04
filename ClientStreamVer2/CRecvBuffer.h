#pragma once

#include "stdafx.h"

class CRecvBuffer {
public:
	WAVEHDR m_WaveHeader; // header for data
	XMITDATA m_Data;
public:
	MMRESULT Prepare(HWAVEOUT hWaveOut);
	MMRESULT Unprepare(HWAVEOUT hWaveOut);
	MMRESULT Add(HWAVEOUT hWaveOut);
};
