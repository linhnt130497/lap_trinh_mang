#pragma once

#include "stdafx.h"

class CSendBuffer {
public:
	WAVEHDR m_waveHeader;
	XMITDATA m_Data;

public:
	MMRESULT Prepare(HWAVEIN hWaveIn);
	MMRESULT Unprepare(HWAVEIN hWaveIn);
	MMRESULT Add(HWAVEIN hWaveIn);
};