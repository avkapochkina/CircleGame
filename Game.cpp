#include "Engine.h"
#include <stdlib.h>
#include <stdio.h>
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
const uint32_t black = (255 << 24) + (0 << 16) + (0 << 8) + 0;
const uint32_t white = (255 << 24) + (255 << 16) + (255 << 8) + 255;
const uint32_t red = (255 << 24) + (255 << 16);
const uint32_t green = (255 << 24) + (255 << 8);
const uint32_t blue = (255 << 24) + 255;
const int pixelSize = sizeof(uint32_t);
const int delay = 150;

// globals
int gameStage = 0; // 0 - start, 1 - game, 2 - game over
uint32_t backgroundColour = blue;

// player info
struct Player
{
	const uint32_t orbiteRadius = 100;
	const uint32_t orbiteColour = (int(255) << 24) + (int(50) << 16) + (int(50) << 8) + int(50);
	const uint32_t radius = 20; // moving circles radius
	float velocity = 10.0;
	int direction = 1; //1 / -1
    uint32_t x0 = SCREEN_HEIGHT / 2;
    uint32_t y0 = SCREEN_WIDTH / 2 + orbiteRadius;
    uint32_t x1 = SCREEN_HEIGHT / 2;
    uint32_t y1 = SCREEN_WIDTH / 2 - orbiteRadius;
    uint32_t score = 0;
} player;

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

	Projectile()
	{
		x = 0;
		y = rand() % SCREEN_WIDTH;
		type = rand() % 2;
		currentVelocity = defaultVelocity;
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

	int hitCheck()
	{
		//0 - no hit
		//1 - hit with harmless projectile
		//2 - hit with dangerous projectile
		if ((x - player.x0) * (x - player.x0) + (y - player.y0) * (y - player.y0) 
			<= (radius + player.radius) * (radius + player.radius))
		{
			if (!type)
			{
				player.score += 1;
				return 1;
			}
			else
			{
				gameStage = 2;
				return 2;
			}
		}
		else
		{
			if ((x - player.x1) * (x - player.x1) + (y - player.y1) * (y - player.y1)
				<= (radius + player.radius) * (radius + player.radius))
			{
				if (!type)
				{
					return 1;
					player.score += 1;
				}
				else
				{
					gameStage = 2;
					return 2;
				}
			}
			else
				return 0;
		}
	}
};
Projectile *projectiles; // container for all existing projectiles

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
void drawBackground()
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

void updateCircles()
{
	//drawCircle(player.x, player.y, player.radius, blue);
	player.x0 += player.velocity * player.direction;
	player.y0 += player.velocity * player.direction;
}

// initialize game data in this function
void initialize()
{
	gameStage = 0;
	backgroundColour = blue;
	drawBackground();
	//char text[256];
	//char startMessage[] = "<PRESS SPACE TO START>";
	//printf_s("%s \n", startMessage);
}

void start()
{
	gameStage = 1;

	player.score = 0;
	player.direction = 1;

	uint32_t x0 = SCREEN_HEIGHT / 2;
	uint32_t y0 = SCREEN_WIDTH / 2 + player.orbiteRadius;
	uint32_t x1 = SCREEN_HEIGHT / 2;
	uint32_t y1 = SCREEN_WIDTH / 2 - player.orbiteRadius;

	Projectile projectile;
	projectile.currentVelocity = projectile.defaultVelocity;
	//projectiles.
}

// this function is called to update game data,
// dt - time elapsed since the previous update (in seconds)
void act(float dt)
{
	if (is_key_pressed(VK_ESCAPE))
		schedule_quit_game();
	if (is_key_pressed(VK_SPACE))
	{
		switch (gameStage)
		{
		case 0: 
		{
			start();
			Sleep(delay);
			break; 
		}
		case 1:
		{
			player.direction = -player.direction;
			Sleep(delay);
			break; 
		}
		case 2:
		{
			start();
			Sleep(delay);
			break; 
		}
		default:
			break;
		}
	}
	//updateCircles(); 
	//updateProjectile();
}

// fill buffer in this function
// uint32_t buffer[SCREEN_HEIGHT][SCREEN_WIDTH] - is an array of 32-bit colors (8 bits per R, G, B)
void draw()
{
	switch (gameStage)
	{
	case 0:
		
        break;
	case 1:
		drawBackground();
		drawCircle(SCREEN_HEIGHT / 2, SCREEN_WIDTH / 2, player.orbiteRadius, player.orbiteColour);
		//drawCircle(player.x, player.y, player.radius, blue);
		//drawCircle(projectile.x, projectile.y, projectile.radius, projectile.colour);
		break;
	case 2:
		backgroundColour = red;
		drawBackground();
		break;
	}
}

// free game data in this function
void finalize()
{
	// clear backbuffer
	//memset(buffer, 0, SCREEN_HEIGHT * SCREEN_WIDTH * sizeof(uint32_t));
}