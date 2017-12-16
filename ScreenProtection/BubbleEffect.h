#pragma once
#include <math.h>
#include <time.h>
#include "Effect.h"
#include "RandFloatNumber.h"

using namespace Gdiplus;

struct Bubble
{
	double x;
	double y;
	double alpha;
	double size;
	double velocity;
};

class BubbleEffect: public Effect{
public:
	BubbleEffect()
	{
		this->name = L"bubble";
	}
	
	void Init(int width, int height)
	{
		this->width = width;
		this->height = height;
		this->bubblesNumber = std::ceil(width / 2);
		this->bubbles = new Bubble[this->bubblesNumber];
		srand(time(NULL));
		for (int i = 0; i < this->bubblesNumber; i++)
		{
			this->bubbles[i] = this->MakeRandBubble();
		}
	}

	void DrawEffect(Graphics* graphics)
	{
		SolidBrush sBrush(Color(0, 0, 0, 0));
		for (int i = 0; i < this->bubblesNumber; i++)
		{
			Bubble& bubble = this->bubbles[i];
			sBrush.SetColor(Color(bubble.alpha * 255,255, 255, 255));
			graphics->FillEllipse(&sBrush, bubble.x, bubble.y, (INT)bubble.size, (INT)bubble.size);
			bubble.y -= bubble.velocity;
			bubble.alpha -= 0.0005;
			if (bubble.alpha <= 0)
			{
				this->bubbles[i] = this->MakeRandBubble();
			}
		}
	}

	~BubbleEffect()
	{
		delete[] this->bubbles;
		delete this->transparentBrush;
	}
private:
	int bubblesNumber;
	int width;
	int height;
	
	Bubble * bubbles;
	Brush* transparentBrush = new SolidBrush(Color(0, 0, 0, 0));
	Bubble MakeRandBubble()
	{
		return  Bubble({
			RandFloatNumber()*this->width,
			this->height + RandFloatNumber() * 100,
			0.1 + RandFloatNumber() * 0.3,
			0.1 + RandFloatNumber() * 10,
			RandFloatNumber()
			});
	}
};
