#pragma once
#include <windows.h>
#include <gdiplus.h>
#include <string>

#define DEFAULT_STRING TEXT("AEPKILL_102654123.0")

class ScreenProtection 
{
public:
	void DrawScreenProtection(Gdiplus::Graphics* graphics);
	bool Init(PTCHAR configFilePath);
	ScreenProtection(int width, int height);
	~ScreenProtection();
private:
	Gdiplus::Font* titleFont = NULL;
	Gdiplus::Font* descriptorFont = NULL;
	Gdiplus::Font* timeFont = NULL;
	Gdiplus::Image* background = NULL;

	Gdiplus::Brush* brush = NULL;
	int screenHeight, screenWidth;
	TCHAR title[MAX_PATH];
	TCHAR description[MAX_PATH];
	void InitBackground();
	void DrawTextContent(Gdiplus::Graphics* graphics);
	void DrawLine(Gdiplus::Graphics* graphics, int x , int y , int length, int width);
};