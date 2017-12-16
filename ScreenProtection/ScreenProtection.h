#pragma once
#include <windows.h>
#include <gdiplus.h>
#include <string>

#define DEFAULT_STRING TEXT("AEPKILL_102654123.0")

using namespace Gdiplus;

class ScreenProtection 
{
public:
	void DrawScreenProtection(Graphics* graphics)
	{
		graphics->Clear(Color(255, 0, 0));
		this->DrawTextContent(graphics);
	}
	bool Init(PTCHAR configFilePath) 
	{
		static TCHAR strBuffer[MAX_PATH];
		LCID local = GetThreadLocale();

		// 初始化背景图片
		GetPrivateProfileString(TEXT("config"), TEXT("background"), DEFAULT_STRING, strBuffer, MAX_PATH, configFilePath);
		this->background = Image::FromFile(strBuffer);
		// 获取 title
		GetPrivateProfileString(TEXT("config"), TEXT("title"), TEXT("AEPKILL"), this->title, MAX_PATH, configFilePath);

		// 获取 description
		GetPrivateProfileString(TEXT("config"), TEXT("description"), TEXT("你怀疑生活洗刷你的灵感，可你又拖到明晚！"), this->description, MAX_PATH, configFilePath);

		// 创建字体
		GetPrivateProfileString(TEXT("config"), TEXT("font"), TEXT("Microsoft Yahei"), strBuffer, MAX_PATH, configFilePath);
		this->titleFont = new Font(strBuffer, 40);
		this->descriptorFont = new Font(strBuffer, 16);
		this->timeFont = new Font(strBuffer, 24);

		// 初始化画刷
		this->brush = new SolidBrush(Color(255, 255, 255));

		// Init Backgorund
		this->InitBackground();

		return true;
	}
	ScreenProtection(int width, int height)
	{
		this->screenWidth = width;
		this->screenHeight = height;
	}
	~ScreenProtection()
	{
		delete this->timeFont;
		delete this->descriptorFont;
		delete this->titleFont;
		delete this->background;
		delete this->brush;
	}

private:
	Gdiplus::Font* titleFont = NULL;
	Gdiplus::Font* descriptorFont = NULL;
	Gdiplus::Font* timeFont = NULL;
	Gdiplus::Image* background = NULL;
	Gdiplus::Brush* brush = NULL;
	
	int screenHeight, screenWidth;
	TCHAR title[MAX_PATH];
	TCHAR description[MAX_PATH];

	void InitBackground()
	{
		Image* mask = NULL;
		Image* mask2 = NULL;
		Image* newBackground = NULL;
		Graphics* graphics = NULL;
		LinearGradientBrush* brush = new LinearGradientBrush(Rect(0, 0, this->screenWidth, this->screenHeight), Color(255 * 0.6, 68, 68, 68), Color(255 * 0.9, 0, 0, 0), 140);

		newBackground = new Bitmap(this->screenWidth, this->screenHeight);

		mask = new Bitmap(this->screenWidth, this->screenHeight);
		graphics = new Graphics(mask);
		graphics->FillRectangle(brush, Rect(0, 0, this->screenWidth, this->screenHeight));
		delete graphics;

		mask2 = new Bitmap(this->screenWidth, this->screenHeight);
		graphics = new Graphics(mask2);
		graphics->Clear(Color((byte)(255 * 0.2), 68, 68, 68));
		delete graphics;

		graphics = new Graphics(newBackground);
		graphics->DrawImage(this->background, Rect(0, 0, this->screenWidth, this->screenHeight), 0, 0, this->background->GetWidth(), this->background->GetHeight(), UnitPixel);
		graphics->DrawImage(mask, Rect(0, 0, this->screenWidth, this->screenHeight), 0, 0, mask->GetWidth(), mask->GetHeight(), UnitPixel);
		graphics->DrawImage(mask2, Rect(0, 0, this->screenWidth, this->screenHeight), 0, 0, mask2->GetWidth(), mask2->GetHeight(), UnitPixel);
		delete graphics;

		delete this->background;
		this->background = newBackground;

		delete mask;
		delete mask2;
		delete brush;
	}

	void DrawTextContent(Gdiplus::Graphics* graphics)
	{
		static TCHAR timeStr[MAX_PATH];
		SYSTEMTIME time;

		RectF* rectTitle = new RectF(0, 0, 0, 0);
		RectF* rectDescription = new RectF(0, 0, 0, 0);
		RectF* rectTime = new RectF(0, 0, 0, 0);

		int titleY = 0, descriptionY = 0, lineWidth = 1, lineMargin = 20, lineLength = 0;


		// 获取时间
		GetLocalTime(&time);
		wsprintf(timeStr, TEXT("%02d:%02d:%02d"), time.wHour, time.wMinute, time.wSecond);

		// 绘制背景
		graphics->DrawImage(this->background, Rect(0, 0, this->screenWidth, this->screenHeight), 0, 0, this->background->GetWidth(), this->background->GetHeight(), UnitPixel);

		// 测量文本宽高
		graphics->MeasureString(this->title, lstrlen(this->title), this->titleFont, PointF(0, 50), rectTitle);
		graphics->MeasureString(this->description, lstrlen(this->description), this->descriptorFont, PointF(0, 50), rectDescription);
		graphics->MeasureString(timeStr, lstrlen(timeStr), this->timeFont, PointF(0, 50), rectTime);

		// 计算文本位置
		titleY = (this->screenHeight - rectTitle->Height - rectDescription->Height - (lineMargin + lineWidth) * 2) / 2;
		descriptionY = titleY + rectTitle->Height + lineWidth + lineMargin;

		// 绘制文本
		graphics->DrawString(this->title, lstrlen(this->title), this->titleFont, PointF((this->screenWidth - rectTitle->Width) / 2, titleY), this->brush);
		graphics->DrawString(this->description, lstrlen(this->description), this->descriptorFont, PointF((this->screenWidth - rectDescription->Width) / 2, descriptionY), this->brush);

		// 绘制分割线
		lineLength = rectTitle->Width + 120;
		this->DrawLine(graphics, (this->screenWidth - lineLength) / 2, titleY + rectTitle->Height, lineLength, lineWidth);

		lineLength = (int)lineLength*0.3;
		this->DrawLine(graphics, (this->screenWidth - lineLength) / 2, descriptionY + rectDescription->Height + lineMargin, lineLength, lineWidth);

		// 绘制时间
		graphics->DrawString(timeStr, lstrlen(timeStr), this->timeFont, PointF((this->screenWidth - rectTime->Width) / 2, descriptionY + rectDescription->Height + lineMargin + lineMargin), this->brush);

		delete rectTime;
		delete rectTitle;
		delete rectDescription;
	}
	void DrawLine(Gdiplus::Graphics* graphics, int x, int y, int length, int width)
	{
		Pen pen(Color((int)255 * 0.25, 255, 255, 255), width);
		graphics->DrawLine(&pen, x, y, x + length, y);
	}
};