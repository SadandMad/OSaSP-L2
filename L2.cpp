#include <windows.h>

#define COLS 5
#define ROWS 4
LPCSTR arr[ROWS][COLS] = { {"Hello\0", "World\0", "I\0", "am\0", "fine\0"},
						{"Get\0", "Your\0", "Ass\0", "Back\0", "Here\0"},
						{"Fisting\0", "is\0", "three\0", "hundrer\0", "bucks\0"},
						{"One\0", "Two\nTwo\0", "Three Three Three\0", "Four Four\nFour Four\0", "Five\nFive\nFive\nFive\nFive\0"} };

int width;
int charHeight;
int wndwHeight;
int height = 0;
int amount = 0;

RECT Rect;

/* This is where all the input to the window goes to */
LRESULT CALLBACK WndProc(HWND hwnd, UINT Message, WPARAM wParam, LPARAM lParam) {
	HDC hdc;
	switch (Message) {
		/* Upon destruction, tell the main thread to stop */
	case WM_DESTROY: {
		PostQuitMessage(0);
		break;
	}
	case WM_PAINT: {
		int newHeight, curHeight = 0;
		height = 0;
		RECT rect;
		PAINTSTRUCT ps;
		hdc = BeginPaint(hwnd, &ps);
		FillRect(hdc, &Rect, (HBRUSH)(COLOR_WINDOW + 1));
		HPEN hPen = CreatePen(PS_SOLID, 3, RGB(0, 0, 0));
		SelectObject(hdc, hPen);
		HFONT hFont = CreateFont(charHeight * 0.8, width * 0.75 / amount, 0, 0, 0, 0, 0, 0,
			OEM_CHARSET,
			0, 0, 0, VARIABLE_PITCH,
			L"Times New Roman");
		SelectObject(hdc, hFont);
		for (int j = 0; j < ROWS; j++) {
			for (int i = 0; i < COLS; i++) {
				rect.left = width * (i + 0.1);
				rect.top = height + charHeight * 0.1;
				rect.right = rect.left + width * 0.8;
				rect.bottom = wndwHeight;
				newHeight = DrawTextA(hdc, arr[j][i], strlen(arr[j][i]), &rect, DT_NOCLIP or DT_CALCRECT);
				if (newHeight > curHeight) {
					curHeight = newHeight;
				}
			}
			height += curHeight + charHeight * 1 / 5;
			MoveToEx(hdc, 0, height, NULL);
			LineTo(hdc, width * COLS, height);
			curHeight = 0;
		}
		for (int j = 0; j <= ROWS; j++) {
			for (int i = 0; i <= COLS; i++) {
				MoveToEx(hdc, width * i, 0, NULL);
				LineTo(hdc, width * i, height);
			}
		}
		DeleteObject(hFont);
		EndPaint(hwnd, &ps);
		return 0;
	}

	case WM_SIZE: {
		GetClientRect(hwnd, &Rect);
		width = Rect.right / COLS;
		charHeight = 50;
		wndwHeight = Rect.bottom;
		RECT Wnd;
		GetWindowRect(hwnd, &Wnd);
		if (wndwHeight > height)
		{
			MoveWindow(hwnd, Wnd.left, Wnd.top, Wnd.right - Wnd.left, height + 45, TRUE);
		}
		InvalidateRect(hwnd, NULL, FALSE);
		break;
	}
	
	/* All other messages (a lot of them) are processed using default procedures */
	default:
		return DefWindowProc(hwnd, Message, wParam, lParam);
	}
	return 0;
}

/* The 'main' function of Win32 GUI programs: this is where execution starts */
int CALLBACK WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow) {
	WNDCLASSEX wc; /* A properties struct of our window */
	HWND hwnd; /* A 'HANDLE', hence the H, or a pointer to our window */
	MSG Msg; /* A temporary location for all messages */

	/* zero out the struct and set the stuff we want to modify */
	memset(&wc, 0, sizeof(wc));
	wc.cbSize = sizeof(WNDCLASSEX);
	wc.lpfnWndProc = WndProc; /* This is where we will send messages to */
	wc.hInstance = hInstance;
	wc.hCursor = LoadCursor(NULL, IDC_CROSS);

	/* White, COLOR_WINDOW is just a #define for a system color, try Ctrl+Clicking it */
	wc.hbrBackground = (HBRUSH)(COLOR_WINDOW);
	wc.lpszClassName = L"MyCoolWinow";
	wc.hIcon = LoadIcon(NULL, IDI_APPLICATION); /* Load a standard icon */
	wc.hIconSm = LoadIcon(NULL, IDI_APPLICATION); /* use the name "A" to use the project icon */

	if (!RegisterClassEx(&wc)) {
		MessageBox(NULL, L"Ошибка при регистрации окна!", L"Проблемка!", MB_ICONEXCLAMATION | MB_OK);
		return 0;
	}

	hwnd = CreateWindowEx(WS_EX_CLIENTEDGE, L"MyCoolWinow", L"L1", WS_VISIBLE | WS_OVERLAPPEDWINDOW,
		CW_USEDEFAULT, /* x */
		CW_USEDEFAULT, /* y */
		800, /* width */
		600, /* height */
		NULL, NULL, hInstance, NULL);

	if (hwnd == NULL) {
		MessageBox(NULL, L"Ошибка при создании окна!", L"Проблемка!", MB_ICONEXCLAMATION | MB_OK);
		return 0;
	}

	for (int j = 0; j < ROWS; j++) {
		for (int i = 0; i < COLS; i++) {
			int len = strcspn(arr[j][i], "\n");
			if (len > amount)
				amount = len;
			/*const char* pos;
			do {
				pos = strchr(arr[j][i], '\n');
				if (pos) {
					if (pos - arr[j][i] > amount)
						amount = strlen(arr[j][i]);
				}
			} while (pos);*/
		}
	}

	/*
		This is the heart of our program where all input is processed and
		sent to WndProc. Note that GetMessage blocks code flow until it receives something, so
		this loop will not produce unreasonably high CPU usage
	*/
	while (GetMessage(&Msg, NULL, 0, 0) > 0) { /* If no error is received... */
		TranslateMessage(&Msg); /* Translate key codes to chars if present */
		DispatchMessage(&Msg); /* Send it to WndProc */
	}
	return Msg.wParam;
}