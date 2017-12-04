#include"CSoundWindow.h"

static CSoundWindow *cSoundWindow = new CSoundWindow();
BOOL static CALLBACK SoundWinProcVer2(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);

int main() {

	char nameClass[20] = "VoiceChatVer2";
	WSADATA wsa;
	WSAStartup(MAKEWORD(2, 2), &wsa);

	WNDCLASS wnd;
	wnd.cbClsExtra = 0;
	wnd.cbWndExtra = 0;
	wnd.hbrBackground = (HBRUSH)GetStockObject(WHITE_BRUSH);
	wnd.hCursor = LoadCursor(NULL, IDC_ARROW);
	wnd.hIcon = LoadIcon(NULL, IDI_APPLICATION);
	wnd.hInstance = NULL;
	wnd.lpfnWndProc = (WNDPROC)SoundWinProcVer2;
	wnd.lpszClassName = (LPCWSTR)nameClass;
	wnd.lpszMenuName = NULL;
	wnd.style = 0;

	if (RegisterClass(&wnd) == 0) {
		printf(" Registry class is error : line : %d\n", __LINE__);
		return 0;
	}

	HWND windows = CreateWindow((LPCWSTR)nameClass, L"This is VoiceChat Application", WS_OVERLAPPEDWINDOW, 0, 0, 200, 200, NULL, NULL, NULL, NULL);

	if (windows == NULL) {
		printf("Cannot create windows : line : %d\n", __LINE__);
		return 0;
	}

	
	MSG msg;

	while (GetMessage(&msg, NULL, NULL, NULL)) {
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}

	return 0;
}

BOOL static CALLBACK SoundWinProcVer2(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {

	switch (uMsg) {

	case MM_WIM_DATA:
		cSoundWindow->OnWimData((WAVEHDR*)lParam);
		break;

	case MM_WOM_DONE:
		cSoundWindow->OnWomDone((WAVEHDR*)lParam);
		break;

	case WM_USER_SOCKET:

		if (WSAGETSELECTEVENT(lParam) == FD_READ) {
			printf("Socket co du lieu am thanh den: line : %d\n", __LINE__);
			cSoundWindow->OnsocketRead();
		}
		if (WSAGETSELECTEVENT(lParam) == FD_WRITE) {
			printf("Socket co du lieu chuan bi gui di: line : %d\n", __LINE__);
			cSoundWindow->OnSocketWrite();
		}
		break;

	case MM_WIM_OPEN:
		cSoundWindow->OnWimOpen();
		break;

	case MM_WIM_CLOSE:
		cSoundWindow->OnWimClose();
		break;

	case MM_WOM_OPEN:
		cSoundWindow->OnWomOpen();
		break;

	case MM_WOM_CLOSE:
		cSoundWindow->OnWomClose();
		break;

	case WM_CREATE:
		cSoundWindow->OnInit(hWnd);
		cSoundWindow->OnConnect();
		break;
	}
	return true;
}

