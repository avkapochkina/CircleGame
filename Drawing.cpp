#include <stdlib.h>
#include <stdio.h>
#include <memory.h>
#include <iostream>
/*
const uint32_t black = (255 << 24) + (0 << 16) + (0 << 8) + 0;
const uint32_t white = (255 << 24) + (255 << 16) + (255 << 8) + 255;
const uint32_t red = (255 << 24) + (255 << 16);
const uint32_t green = (255 << 24) + (255 << 8);
const uint32_t blue = (255 << 24) + 255;
const int pixelSize = sizeof(uint32_t);


// Bresenham's algorithm
void drawCircle(int x0, int y0, int radius, uint32_t colour)
{
	int x = 0;
	int y = radius;
	int delta = 1 - 2 * radius;
	int error = 0;
	while (y >= 0) {
		memcpy(&buffer[x0 + x][y0 + y], &colour, pixelSize);
		memcpy(&buffer[x0 + x][y0 - y], &colour, pixelSize);
		memcpy(&buffer[x0 - x][y0 + y], &colour, pixelSize);
		memcpy(&buffer[x0 - x][y0 - y], &colour, pixelSize);
		error = 2 * (delta + y) - 1;
		if (delta < 0 && error <= 0) {
			++x;
			delta += 2 * x + 1;
			continue;
		}
		error = 2 * (delta - x) - 1;
		if (delta > 0 && error > 0) {
			--y;
			delta += 1 - 2 * y;
			continue;
		}
		++x;
		delta += 2 * (x - y);
		--y;
	}
}

// fill the background
void drawBackground(uint32_t backgroundColour)
{
	uint32_t* ptr = &buffer[0][0];

	for (int i = 0; i < SCREEN_HEIGHT; i++)
	{
		for (int j = 0; j < SCREEN_WIDTH; j++)
		{
			memcpy(ptr, &backgroundColour, pixelSize);
			ptr++;
		}
	}
}
*/