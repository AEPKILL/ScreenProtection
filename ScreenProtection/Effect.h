#pragma once
#include <windows.h>
#include <gdiplus.h>
#include <string>
class Effect
{
public:
	std::wstring name;
	virtual void DrawEffect(Gdiplus::Graphics* graphics) = 0;
	virtual void Init(int width ,int height) 
	{

	}
};