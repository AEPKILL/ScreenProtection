#pragma once

#include "Effect.h"
#include "BubbleEffect.h"

static  Effect* effectList[] = {
	new BubbleEffect()
};

static const int effectListLength = sizeof(effectList) / sizeof(Effect*);
