#pragma once

#include <windows.h>
#include <gdiplus.h>
#include <string>
class Effect
{
public:
	std::string name;
	virtual void DrawEffect(Gdiplus::Graphics* graphics) = 0;
};