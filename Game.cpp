#include "Engine.h"
#include <stdlib.h>
#include <memory.h>
#include <iostream>
#include <windows.h>

//
//  You are free to modify this file
//

//  is_key_pressed(int button_vk_code) - check if a key is pressed,
//                                       use keycodes (VK_SPACE, VK_RIGHT, VK_LEFT, VK_UP, VK_DOWN, 'A', 'B')
//
//  get_cursor_x(), get_cursor_y() - get mouse cursor position
//  is_mouse_button_pressed(int button) - check if mouse button is pressed (0 - left button, 1 - right button)
//  clear_buffer() - set all pixels in buffer to 'black'
//  is_window_active() - returns true if window is active
//  schedule_quit_game() - quit game after act()

// constants
//uint32_t Colour = (int(a) << 24) + (int(b) << 16) + (int(r) << 8) + int(g);
const uint32_t Black = (255 << 24) + (0 << 16) + (0 << 8) + 0;
const uint32_t White = (255 << 24) + (255 << 16) + (255 << 8) + 255;
const uint32_t Red = (255 << 24) + (255 << 16);
const uint32_t Green = (255 << 24) + (255 << 8);
const uint32_t Blue = (255 << 24) + 255;
const int PixelSize = sizeof(uint32_t);

// globals
int GameStage = 0; // 0 - start, 1 - game, 2 - game over

struct Projectile
{
    float Velocity = 0.0;
    float Direction = 0.0;
    int Type = 0; //0 - harmless, 1 - dangerous
    uint32_t x = 0;
    uint32_t y = 0;
    uint32_t Colour;
};

// player info
struct Player
{
    int Direction = 0; //0 - clockwise
    uint32_t x = SCREEN_HEIGHT / 2;
    uint32_t y = SCREEN_WIDTH / 2;
    uint32_t Score = 0;
};

// Bresenham's algorithm
void drawCircle(int x0, int y0, int Radius, uint32_t Colour)
{
	int x = 0;
	int y = Radius;
	int Delta = 1 - 2 * Radius;
	int Error = 0;
	while (y >= 0) {
		memcpy(&buffer[x0 + x][y0 + y], &Colour, PixelSize);
		memcpy(&buffer[x0 + x][y0 - y], &Colour, PixelSize);
		memcpy(&buffer[x0 - x][y0 + y], &Colour, PixelSize);
		memcpy(&buffer[x0 - x][y0 - y], &Colour, PixelSize);
		Error = 2 * (Delta + y) - 1;
		if (Delta < 0 && Error <= 0) {
			++x;
			Delta += 2 * x + 1;
			continue;
		}
		Error = 2 * (Delta - x) - 1;
		if (Delta > 0 && Error > 0) {
			--y;
			Delta += 1 - 2 * y;
			continue;
		}
		++x;
		Delta += 2 * (x - y);
		--y;
	}
}

// initialize game data in this function
void initialize()
{
	uint32_t* ptr = &buffer[0][0];

	for (int i = 0; i < SCREEN_HEIGHT; i++)
	{
		for (int j = 0; j < SCREEN_WIDTH; j++)
		{
			memcpy(ptr, &White, PixelSize);
			ptr++;
			//memset(&buffer[i][j], i, size / 4);
		}
	}
}

void drawBackground(uint32_t BackgroundColour, uint32_t CircleColour)
{
   uint32_t* ptr = &buffer[0][0];

   for (int i = 0; i < SCREEN_HEIGHT; i++)
   {
       for (int j = 0; j < SCREEN_WIDTH; j++)
       {
           memcpy(ptr, &BackgroundColour, PixelSize);
           ptr++;
       }
   }
   drawCircle(SCREEN_HEIGHT / 2, SCREEN_WIDTH / 2, 100, CircleColour);
 }

// this function is called to update game data,
// dt - time elapsed since the previous update (in seconds)
void act(float dt)
{
  if (is_key_pressed(VK_ESCAPE))
    schedule_quit_game();
  if (is_key_pressed(VK_SPACE))
  {
	  switch (GameStage)
	  {
	  case 0: 
		  Start();
	  case 1:
		 
	  case 2:
		  Restart();
	  }
  }
}

// fill buffer in this function
// uint32_t buffer[SCREEN_HEIGHT][SCREEN_WIDTH] - is an array of 32-bit colors (8 bits per R, G, B)
void draw()
{
	switch (GameStage)
	{
	case 0:
	case 1:
	case 2:
	}
  // clear backbuffer
  //memset(buffer, 0, SCREEN_HEIGHT * SCREEN_WIDTH * sizeof(uint32_t));
  //drawBackground(Black, White);
}

// free game data in this function
void finalize()
{
}

void Start()
{
	drawBackground(Black, White);
}

void Restart()
{
}