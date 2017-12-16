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

// ȥ���������˳�����ʱ�ڿ�
#pragma comment ( linker, "/subsystem:windows /entry:mainCRTStartup" ) 
// ���� GdiPlus
#pragma comment(lib,"gdiplus.lib")


LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);

// �����ļ�·��
TCHAR iniFilePath[MAX_PATH];

// �Ƿ�����Ч
bool enableEffect = true;

int main(HINSTANCE hInstance, HINSTANCE hPrevInstance, PSTR szCmdLine, int iCmdShow)
{
	static TCHAR szAppName[] = TEXT("AEPKILL");
	HWND            hwnd;
	MSG            msg;
	WNDCLASS    wndclass;
	LPTSTR commandLine = GetCommandLine();
	int argc = 0;
	int iniFileLength = 0;
	LPWSTR* argv = CommandLineToArgvW(commandLine, &argc);
	static HWND taskBar = FindWindow(TEXT("Shell_TrayWnd"), NULL);
	
	// �ϳ������ļ�·��
	lstrcat(iniFilePath,argv[0]);
	iniFileLength = lstrlen(iniFilePath);
	iniFileLength = iniFileLength - 4;
	iniFilePath[iniFileLength] = '\0';
	lstrcat(iniFilePath, TEXT(".ini"));

	// �ͷ���Դ
	LocalFree(commandLine);
	LocalFree(argv);

	// ������Ϣ
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
		MessageBox(NULL, TEXT("�˳������������NT��!"), szAppName, MB_ICONERROR);
		return 0;
	}

	// ������������
	hwnd = CreateWindow(szAppName, NULL,
		WS_CLIPCHILDREN | WS_CLIPSIBLINGS | WS_POPUP,
		0, 0,
		GetSystemMetrics(SM_CXSCREEN), GetSystemMetrics(SM_CYSCREEN),
		NULL, NULL, hInstance,
		NULL);

	// �����ʾ 
	ShowWindow(hwnd, SW_SHOWMAXIMIZED); 
	UpdateWindow(hwnd);

	// ���������������� 
	ShowCursor(FALSE); 
	ShowWindow(taskBar, SW_HIDE);
	
	// ��Ϣѭ��
	while (GetMessage(&msg, NULL, 0, 0))
	{
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}
	
	// ��ʾ������������
	ShowCursor(TRUE); 
	ShowWindow(taskBar, SW_SHOW);

	return msg.wParam;
}

LRESULT CALLBACK WndProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	using namespace Gdiplus;
	static HDC          hdc;
	// ��Ļ�Ŀ�� �߶�
	static  int cxScreen, cyScreen; 
	static GdiplusStartupInput gdiplusStartupInput;
	static ULONG_PTR pGdiToken;
	static Graphics* graphics = NULL;
	static Graphics* memGraphics = NULL;
	static Bitmap* bitmap = NULL;
	static Bitmap* background = NULL;
	static ScreenProtection* screenProtection;
	
	switch (message)
	{
	case WM_CREATE:
		// ��ȡ��Ļ���
		cxScreen = GetSystemMetrics(SM_CXSCREEN); 
		cyScreen = GetSystemMetrics(SM_CYSCREEN);
		// ��ȡhdc
		hdc = GetDC(hwnd);
		// ��ʼ�� GdiPlus
		GdiplusStartup(&pGdiToken, &gdiplusStartupInput, NULL);
		// ��ʼ������
		graphics = new Graphics(hdc);
		// ��ʼ���ڴ�λͼ
		bitmap = new Bitmap(cxScreen , cyScreen);
		// ��ʼ���ڴ滭��
		memGraphics = new Graphics(bitmap);
		// �����
		memGraphics->SetSmoothingMode(Gdiplus::SmoothingModeHighQuality);
		// ��ʼ��������ͼ��
		screenProtection = new ScreenProtection(cxScreen, cyScreen);
		screenProtection->Init(iniFilePath);

		// ������ʱ�� ��Чģʽ����Ҫÿ��ˢ��60֡
		if (enableEffect) 
		{
			SetTimer(hwnd, ID_TIMER, 1000/55, NULL);
		}
		else
		{
			SetTimer(hwnd, ID_TIMER, 1000, NULL);
		}

		return 0;

	case WM_TIMER:
		screenProtection->DrawScreenProtection(memGraphics);
		graphics->DrawImage(bitmap,0,0);
		return 0;

	//�����ƺ��� 
	case WM_KEYDOWN:
	case WM_LBUTTONDOWN:
	case WM_DESTROY:
		delete graphics;
		delete memGraphics;
		delete bitmap;
		delete screenProtection;
		GdiplusShutdown(pGdiToken);
		KillTimer(hwnd, ID_TIMER);
		ReleaseDC(hwnd, hdc);
		PostQuitMessage(0);
		return 0;
	}
	return DefWindowProc(hwnd, message, wParam, lParam);
}