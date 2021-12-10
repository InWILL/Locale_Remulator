#include <iostream>
#include <Windows.h>

LRESULT CALLBACK WinSunProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
    return DefWindowProc(hwnd, uMsg, wParam, lParam);
}

int main()
{
	for (int i = 0; i < 5; i++) {
		std::cout << "From simple program " << i << "\n";
		Sleep(500);
	}
	MessageBox(NULL, "桭", "偩偪", NULL);
    WNDCLASS wndclass;
    wndclass.style = CS_HREDRAW | CS_VREDRAW; //设置当水平长度或垂直长度改变或移动窗口时，重画整个窗口
    wndclass.lpfnWndProc = (WNDPROC)WinSunProc; //设置WinSunProc为窗口处理函数
    wndclass.cbClsExtra = 0;
    wndclass.cbWndExtra = 0;
    wndclass.hInstance = NULL;
    wndclass.hIcon = LoadIcon(NULL, IDI_ERROR);
    wndclass.hCursor = LoadCursor(NULL, IDC_ARROW);
    wndclass.hbrBackground = (HBRUSH)GetStockObject(DKGRAY_BRUSH);
    wndclass.lpszMenuName = NULL;
    wndclass.lpszClassName = "WeiXin";
    RegisterClass(&wndclass);
    HWND hwnd;
    hwnd = CreateWindow("WeiXin",
        "桭",
        WS_OVERLAPPEDWINDOW & ~WS_MAXIMIZEBOX,
        100, 100, 640, 480,
        NULL,
        NULL,
        NULL,
        NULL);
    ShowWindow(hwnd, SW_SHOWNORMAL);
    UpdateWindow(hwnd);
    MSG msg;
    while (GetMessage(&msg, NULL, 0, 0))
    {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }
}