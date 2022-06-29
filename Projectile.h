#pragma once
#include <iostream>
#include "Engine.h"

class Projectile
{
public:
    float defaultVelocity = 10.0;
    float currentVelocity = defaultVelocity;
    float direction = 0.0;
    //float spawnDelay = 1.0; // delay before next projectile spawn
    int type = 0; //0 - harmless, 1 - dangerous
    uint32_t radius = 20;
    uint32_t x = 0;
    uint32_t y = 0;
    uint32_t colour = (255 << 24) + (255 << 16) + (255 << 8) + 255;

	void spawnProjectile()
	{
		x = 0;
		y = rand() % SCREEN_WIDTH;
		type = rand() % 2;
		if (type) colour = (255 << 24) + (0 << 16) + (0 << 8) + 0; // black
		else colour = (255 << 24) + (255 << 16) + (255 << 8) + 255; // white
	}

	void updateProjectile()
	{
		if (y < SCREEN_WIDTH && x < SCREEN_HEIGHT)
		{
			x += currentVelocity * direction;
			y += currentVelocity * direction;
		}
	}
};