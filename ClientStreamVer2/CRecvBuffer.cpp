#include "CRecvBuffer.h"

MMRESULT CRecvBuffer::Prepare(HWAVEOUT hWaveOut) {
	ZeroMemory(&m_WaveHeader, sizeof(WAVEHDR));
	m_WaveHeader.dwBufferLength = BLOCK_SIZE;//25
	m_WaveHeader.lpData = (char*)m_Data.m_abData;
	m_WaveHeader.dwUser = (DWORD)this; // pointer to CRecvBuffer

	return waveOutPrepareHeader(hWaveOut, &m_WaveHeader, sizeof(m_WaveHeader));
}

MMRESULT CRecvBuffer::Add(HWAVEOUT hWaveOut) {
	return waveOutWrite(hWaveOut, &m_WaveHeader, sizeof(m_WaveHeader));
}

MMRESULT CRecvBuffer::Unprepare(HWAVEOUT hWaveOut) {
	return waveOutUnprepareHeader(hWaveOut, &m_WaveHeader, sizeof(m_WaveHeader));
}
