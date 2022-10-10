#include "Engine.h"
#include <memory.h>
#include <windows.h>

#include <vector>
#include <ctime>
#include <iostream>
#include <stdlib.h>
#include <string> 

using namespace std;

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
const uint32_t black = (255 << 24) + (0 << 16) + (0 << 8) + 0;
const uint32_t white = (255 << 24) + (255 << 16) + (255 << 8) + 255;
const uint32_t red = (255 << 24) + (255 << 16) + (0 << 8) + 0;
const uint32_t colour1 = (255 << 24) + (206 << 16) + (107 << 8) + 106;
//const uint32_t colour2 = (255 << 24) + (235 << 16) + (162 << 8) + 172;
//const uint32_t colour4 = (255 << 24) + (74 << 16) + (158 << 8) + 145;
const int pixelSize = sizeof(uint32_t);
const float keyCooldownMax = 0.15; // bounce defence
const float timerMax = 0.7; // max time between projectile spawn

const char start_string[256] = "PRESS <SPACE> TO START";
//char about_string[256] = "PRESS <SPACE> TO CHANGE MOVEMENT DIRECTION";
const char gameover_string[32] = "GAME OVER";

// globals
int gameStage = 0; // 0 - start, 1 - game, 2 - game over
float timer = 0; // current time after last projectile spawned
float keyCooldown = 0.0; // current time after space was pressed
uint32_t backgroundColour = white;

// player info
struct Player
{
	const uint32_t orbiteRadius = 100;
	const uint32_t orbiteColour = black;
	const uint32_t radius = 20; // moving circles radius
	const uint32_t colour = white; // moving circles colour
	float velocity = 0.013;
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
	float currentVelocity = 1;
	float velocityDelta = 0.1;
	float direction;

	int type; // 0 - harmless, 1 - dangerous
	uint32_t radius = 20;
	float x;
	float y;
	uint32_t colour;  // white - harmless, black - dangerous

	char buffer[32];

	Projectile()
	{
		x = radius + (rand() * (int)(SCREEN_HEIGHT - 2 * radius) / RAND_MAX) + 1;
		y = radius + 1;
		type = rand() % 2;
		direction = (float) rand() / RAND_MAX;
		if (type)
			colour = black;
		else 
			colour = white;
	}

	void updateProjectile()
	{
		x += currentVelocity * sin(direction);
		y += currentVelocity * cos(direction);
	}

	Projectile& operator=(Projectile& prj)
	{
		x = prj.x;
		y = prj.y;
		type = prj.type;
		direction = prj.direction;
		currentVelocity = prj.currentVelocity;
		colour = prj.colour;
		return *this;
	}

	int hitCheck()
	{
		// 0 - no hit
		// 1 - hit with harmless projectile
		// 2 - hit with dangerous projectile
		// 3 - out for screen
		if ((x + sin(direction) * currentVelocity >= SCREEN_HEIGHT - radius) 
			|| (y + cos(direction) * currentVelocity >= SCREEN_WIDTH - radius)
			|| (x + sin(direction) * currentVelocity <=  radius) 
			|| (y + cos(direction) * currentVelocity <=  radius))
		{
			// if circle will be out of the screen on next step
			return 3;
		}
		if ((x - player.x0) * (x - player.x0) + (y - player.y0) * (y - player.y0) 
			<= (radius + player.radius) * (radius + player.radius))
		{
			if (!type)
			{
				updateScore();
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
					updateScore();
					return 1;
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

	void updateScore() {
		player.score += 1;
		sprintf_s(buffer, "SCORE: %d", player.score);
		strcpy_s(screen_text, buffer);
		backgroundColour = colour1 + player.score * 10;
	}
};

std::vector<Projectile> projectiles; // container for all existing projectiles

// Bresenham's algorithm
void fillCircle(int x0, int y0, int radius, uint32_t colour)
{
	int x = 0;
	int y = radius;
	int delta = 1 - 2 * radius;
	int error = 0;
	while (y >= 0) {
		for (int i = x0 - x; i <= x + x0; i++)
		{
			memcpy(&buffer[i][y0 + y], &colour, pixelSize);
			memcpy(&buffer[i][y0 - y], &colour, pixelSize);
		}
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

void updateCircles()
{
	static long double angle = 0;

	angle = angle + player.velocity * player.direction;
	double  x = cos(angle) * player.orbiteRadius * player.direction;
	double  y = sin(angle) * player.orbiteRadius * player.direction;
	
	player.x0 = x + SCREEN_HEIGHT / 2;
	player.y0 = y  + SCREEN_WIDTH / 2;
	player.x1 = -x + SCREEN_HEIGHT / 2;
	player.y1 = -y + SCREEN_WIDTH / 2;
}

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

// initialize game data in this function
void initialize()
{
	srand(time(0));
	gameStage = 0; 
	backgroundColour = black;
	drawBackground();

	strcpy_s(screen_text, start_string);
}

void start()
{
	projectiles.resize(0);
	gameStage = 1;
	timer = 0;
	player.score = 0;
	player.direction = 1;
	player.x0 = SCREEN_HEIGHT / 2;
	player.y0 = SCREEN_WIDTH / 2 + player.orbiteRadius;
	player.x1 = SCREEN_HEIGHT / 2;
	player.y1 = SCREEN_WIDTH / 2 - player.orbiteRadius;

	backgroundColour = colour1;
	projectiles.push_back(Projectile());

	strcpy_s(screen_text, "SCORE: 0");
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
			break; 
		}
		case 1:
		{
			if (keyCooldown >= keyCooldownMax)
			{
				player.direction = -player.direction;
				keyCooldown = 0;
			}
			break; 
		}
		case 2:
		{
			Sleep(1000);
			start();
			break; 
		}
		default:
			break;
		}
	}
	if (gameStage == 1)
	{
		//backgroundColour = colour1 + player.score * 10;
		if (timer >= timerMax)
		{
			projectiles.push_back(Projectile());
			timer = 0;
		}
		for (int i = 0; i < projectiles.size(); i++)
		{
			switch (projectiles.at(i).hitCheck())
			{
			case 0:
			{
				projectiles.at(i).updateProjectile();
				break;
			}
			default: 
			{
				for (int j = i; j < projectiles.size() - 1; j++)
				{
					projectiles.at(j) = projectiles.at(j + 1);
				}
				projectiles.pop_back();
				break;
			}
			}

		}
		updateCircles();
		timer += dt;
		keyCooldown += dt;
	}
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
		//char buffer[32];
		//sprintf_s(buffer, "SCORE: %d", player.score);
		//strcpy_s(screen_text, buffer);
		drawBackground();
		drawCircle(SCREEN_HEIGHT / 2, SCREEN_WIDTH / 2, player.orbiteRadius, player.orbiteColour);
		fillCircle(player.x0, player.y0, player.radius, player.colour);
		fillCircle(player.x1, player.y1, player.radius, player.colour);
		for (int i = 0; i < projectiles.size(); i++)
		{
			fillCircle(projectiles[i].x, projectiles[i].y, projectiles[i].radius, projectiles[i].colour);
		}
		break;
	case 2:
		strcpy_s(screen_text, gameover_string);
		backgroundColour = white;
		drawBackground();
		break;
	}
}

// free game data in this function
void finalize()
{
	projectiles.clear();
	memset(buffer, 0, SCREEN_HEIGHT * SCREEN_WIDTH * sizeof(uint32_t));
}