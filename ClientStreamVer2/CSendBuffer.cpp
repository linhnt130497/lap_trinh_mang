#include "CSendBuffer.h"

MMRESULT CSendBuffer::Prepare(HWAVEIN hWaveIn) {
	ZeroMemory(&m_waveHeader, sizeof(WAVEHDR));
	m_waveHeader.dwBufferLength = BLOCK_SIZE;// 25
	m_waveHeader.lpData = (char*)(m_Data.m_abData);
	m_waveHeader.dwUser = (DWORD) this; // a ponter to CSendBuffer

	return waveInPrepareHeader(hWaveIn, &m_waveHeader, sizeof(m_waveHeader));
}

MMRESULT CSendBuffer::Add(HWAVEIN hWaveIn) {
	return waveInAddBuffer(hWaveIn, &m_waveHeader, sizeof(m_waveHeader));
}

MMRESULT CSendBuffer::Unprepare(HWAVEIN hWaveIn) {
	return waveInUnprepareHeader(hWaveIn, &m_waveHeader, sizeof(m_waveHeader));
}