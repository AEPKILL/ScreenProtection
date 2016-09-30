#include <windows.h>
#include <gdiplus.h>
#include <SDKDDKVer.h>
#include "ScreenProtection.h"

#define ID_TIMER    1 
#define SECTION_NAME "config"
#define BACKGROUND_KEY "background"
#define TITLE_KEY "title"
#define DESC_KEY "description"
#define FONT_KEY "font"

// 去除启动或退出程序时黑框
#pragma comment ( linker, "/subsystem:windows /entry:mainCRTStartup" ) 
// 引入 GdiPlus
#pragma comment(lib,"gdiplus.lib")

LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);
int main(HINSTANCE hInstance, HINSTANCE hPrevInstance, PSTR szCmdLine, int iCmdShow)
{
	static TCHAR szAppName[] = TEXT("AEPKILL");
	HWND            hwnd;
	MSG            msg;
	WNDCLASS    wndclass;


	wndclass.style = CS_HREDRAW | CS_VREDRAW;
	wndclass.lpfnWndProc = WndProc;
	wndclass.cbClsExtra = 0;
	wndclass.cbWndExtra = 0;
	wndclass.hInstance = hInstance;

	wndclass.hIcon = LoadIcon(NULL, IDI_APPLICATION);
	wndclass.hCursor = LoadCursor(NULL, IDC_ARROW);
	wndclass.hbrBackground = (HBRUSH)GetStockObject(BLACK_BRUSH);
	wndclass.lpszMenuName = NULL;
	wndclass.lpszClassName = szAppName;

	if (!RegisterClass(&wndclass))
	{
		MessageBox(NULL, TEXT("此程序必须运行在NT下!"), szAppName, MB_ICONERROR);
		return 0;
	}

	hwnd = CreateWindow(szAppName, NULL,
		WS_CLIPCHILDREN | WS_CLIPSIBLINGS | WS_POPUP,
		0, 0,
		GetSystemMetrics(SM_CXSCREEN), GetSystemMetrics(SM_CYSCREEN),
		NULL, NULL, hInstance,
		NULL);

	//最大化显示 
	ShowWindow(hwnd, SW_SHOWMAXIMIZED); 
	UpdateWindow(hwnd);
	//隐藏鼠标光标 
	ShowCursor(FALSE); 
	while (GetMessage(&msg, NULL, 0, 0))
	{
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}
	//显示鼠标光标 
	ShowCursor(TRUE); 
	return msg.wParam;
}

LRESULT CALLBACK WndProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	using namespace Gdiplus;
	static HDC          hdc;
	//屏幕的宽度 高度. 
	static  int cxScreen, cyScreen; 
	static GdiplusStartupInput gdiplusStartupInput;
	static ULONG_PTR pGdiToken;
	static Graphics* graphics = NULL;
	static Graphics* memGraphics = NULL;
	static Bitmap* bitmap = NULL;
	static Bitmap* background = NULL;
	static ScreenProtection* screenProtection;
	static HWND taskBar = FindWindow(TEXT("Shell_TrayWnd"), NULL);

	switch (message)
	{
	case WM_CREATE:
		// 隐藏任务栏
		ShowWindow(taskBar, SW_HIDE);
		// 获取屏幕宽高
		cxScreen = GetSystemMetrics(SM_CXSCREEN); 
		cyScreen = GetSystemMetrics(SM_CYSCREEN);
		// 获取hdc
		hdc = GetDC(hwnd);
		// 初始化 GdiPlus
		GdiplusStartup(&pGdiToken, &gdiplusStartupInput, NULL);
		// 初始化画板
		graphics = new Graphics(hdc);
		// 初始化内存位图
		bitmap = new Bitmap(cxScreen , cyScreen);
		// 初始化内存画板
		memGraphics = new Graphics(bitmap);
		// 初始化屏保绘图类
		screenProtection = new ScreenProtection(cxScreen, cyScreen);
		screenProtection->Init(TEXT("D:/ScreenProtection.ini"));
		// 创建定时器
		SetTimer(hwnd, ID_TIMER, 100, NULL);
		return 0;

	case WM_TIMER:
		screenProtection->DrawScreenProtection(memGraphics);
		graphics->DrawImage(bitmap,0,0);
		return 0;

	//处理善后工作 
	case WM_KEYDOWN:
	case WM_LBUTTONDOWN:
	case WM_DESTROY:
		delete graphics;
		delete memGraphics;
		delete bitmap;
		delete screenProtection;
		ShowWindow(taskBar, SW_SHOW);
		GdiplusShutdown(pGdiToken);
		KillTimer(hwnd, ID_TIMER);
		ReleaseDC(hwnd, hdc);
		PostQuitMessage(0);
		return 0;
	}
	return DefWindowProc(hwnd, message, wParam, lParam);
}