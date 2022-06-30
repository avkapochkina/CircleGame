#include "Engine.h"
#include <memory.h>
#include <windows.h>

#include <vector>
#include <ctime>
#include <stdlib.h>

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
// uint32_t Colour = (int(b) << 24) + (int(g) << 16) + (int(r) << 8) + int(a);
const uint32_t black = (255 << 24) + (0 << 16) + (0 << 8) + 0;
const uint32_t white = (255 << 24) + (255 << 16) + (255 << 8) + 255;
const uint32_t red = (255 << 24) + (255 << 16) + (0 << 8) + 0;
const uint32_t green = (255 << 24) +(0 << 16) + (255 << 8) + 0;
const uint32_t blue = (255 << 24) + 255;
const int pixelSize = sizeof(uint32_t);
const float keyCooldownMax = 0.15; // bounce defence
const float timerMax = 1; // max time between projectile spawn


// globals
int gameStage = 0; // 0 - start, 1 - game, 2 - game over
float timer = 0; // current time after last projectile spawned
float keyCooldown = 0.0; // current time after space was pressed
uint32_t backgroundColour = blue;

// player info
struct Player
{
	const uint32_t orbiteRadius = 100;
	const uint32_t orbiteColour = (int(255) << 24) + (int(50) << 16) + (int(50) << 8) + int(50);
	const uint32_t radius = 20; // moving circles radius
	const uint32_t colour = red; // moving circles colour
	uint32_t velocity = 1;
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
	const uint32_t defaultVelocity = 1;
	uint32_t currentVelocity = defaultVelocity;
	int direction = 1; // -1 / 1

	uint32_t pathRadius;
	const uint32_t minPathRadius = SCREEN_WIDTH / 2;
	const uint32_t maxPathRadius = SCREEN_WIDTH * 2;

	//float spawnDelay = 1.0; // delay before next projectile spawn
	int type; //0 - harmless, 1 - dangerous
	uint32_t radius = 20;
	uint32_t x;
	uint32_t y;
	uint32_t colour;

	Projectile()
	{
		x = radius + (rand() * (int)(SCREEN_HEIGHT - 2 * radius) / RAND_MAX);
		y = radius + 1;
		type = rand() % 2;
		direction *= -1;
		pathRadius = minPathRadius + (rand() * (int)(maxPathRadius - minPathRadius) / RAND_MAX);
		if (type)
			colour = black;
		else 
			colour = white;
	}

	void updateProjectile()
	{
		if (y < SCREEN_WIDTH && x < SCREEN_HEIGHT)
		{
			x += currentVelocity;
			y += currentVelocity;
		}
	}

	Projectile& operator=(Projectile& prj)
	{
		x = prj.x;
		y = prj.y;
		type = prj.type;
		direction = prj.direction;
		currentVelocity = prj.currentVelocity;
		pathRadius = prj.pathRadius;
		colour = prj.colour;
		return *this;
	}

	int hitCheck()
	{
		// 0 - no hit
		// 1 - hit with harmless projectile
		// 2 - hit with dangerous projectile
		// 3 - out for screen
		if ((x + currentVelocity >= SCREEN_HEIGHT - radius) || (x + currentVelocity >= SCREEN_WIDTH - radius))
		{
			// if circle will be out of the screen on next step
			return 3;
		}
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
	//player.x0 = SCREEN_HEIGHT / 2;
	//player.y0 = SCREEN_WIDTH / 2 + player.orbiteRadius;
	//player.x1 = SCREEN_HEIGHT / 2;
	//player.y1 = SCREEN_WIDTH / 2 - player.orbiteRadius;
	int x = player.x0 - SCREEN_HEIGHT / 2;
	int y = player.y0 - SCREEN_WIDTH / 2 + player.orbiteRadius;
	int delta = 1 - 2 * player.orbiteRadius;
	int error = 2 * (delta + y) - 1;
	while (y >= (player.y0 - SCREEN_WIDTH / 2)) {
		if (delta < 0 && error <= 0)
		{
			x += player.direction;
			delta += 2 * x + 1;
		}
		else
		{
			error = 2 * (delta - x) - 1;
			if (delta > 0 && error > 0) {
				y -= player.direction;
				delta += 1 - 2 * y;
			}
			else
			{
				x += player.direction;
				delta += 2 * (x - y);
				y -= player.direction;
			}
		}
	}
	player.x0 = SCREEN_HEIGHT / 2 + x;
	player.y0 = SCREEN_WIDTH / 2 + y - player.orbiteRadius;
	player.x1 = SCREEN_HEIGHT / 2 - x;
	player.y1 = SCREEN_WIDTH / 2 - y + player.orbiteRadius;
}

// Bresenham's algorithm
void drawCircle(int x0, int y0, int radius, uint32_t colour)
{
	int x = 0;
	int y = radius;
	int delta = 1 - 2 * radius;
	int error = 0;
	while (y >= 0) {
		colour = (255 << 24) + (0 << 16) + (0 << 8) + 0;
		memcpy(&buffer[x0 + x][y0 + y], &colour, pixelSize);
		memcpy(&buffer[x0 + x][y0 - y], &colour, pixelSize);
		memcpy(&buffer[x0 - x][y0 + y], &colour, pixelSize);
		memcpy(&buffer[x0 - x][y0 - y], &colour, pixelSize);
		error = 2 * (delta + y) - 1;
		if (delta < 0 && error <= 0) {
			++x;
			if ((x0 + x == player.x0 + player.direction && y0 + y == player.y0 + player.direction)
				|| (x0 + x == player.x1 + player.direction && y0 + y == player.y1 + player.direction)
				|| (x0 + x == player.x0 + player.direction && y0 - y == player.y0 + player.direction)
				|| (x0 + x == player.x1 + player.direction && y0 - y == player.y1 + player.direction))
			{
				player.x0 += player.direction;
				player.x1 += player.direction;
			}
			delta += 2 * x + 1;
			continue;
		}
		error = 2 * (delta - x) - 1;
		if (delta > 0 && error > 0) {
			--y;
			if ((x0 + x == player.x0 + player.direction && y0 + y == player.y0 + player.direction)
				|| (x0 + x == player.x1 + player.direction && y0 + y == player.y1 + player.direction)
				|| (x0 + x == player.x0 + player.direction && y0 - y == player.y0 + player.direction)
				|| (x0 + x == player.x1 + player.direction && y0 - y == player.y1 + player.direction))
			{
				player.y0 -= player.direction;
				player.y1 -= player.direction;
			}
			delta += 1 - 2 * y;
			continue;
		}
		++x;
		delta += 2 * (x - y);
		--y;
		if ((x0 + x == player.x0 + player.direction && y0 + y == player.y0 + player.direction)
			|| (x0 + x == player.x1 + player.direction && y0 + y == player.y1 + player.direction)
			|| (x0 + x == player.x0 + player.direction && y0 - y == player.y0 + player.direction)
			|| (x0 + x == player.x1 + player.direction && y0 - y == player.y1 + player.direction))
		{
			player.y0 -= player.direction;
			player.x0 += player.direction;
			player.y1 -= player.direction;
			player.x1 += player.direction;
		}
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
	backgroundColour = blue;
	drawBackground();
	//char text[256];
	//char startMessage[] = "<PRESS SPACE TO START>";
	//printf_s("%s \n", startMessage);
}

void start()
{
	projectiles.resize(0);
	gameStage = 1;
	timer = 0;
	backgroundColour = green;

	player.score = 0;
	player.direction = 1;
	player.x0 = SCREEN_HEIGHT / 2;
	player.y0 = SCREEN_WIDTH / 2 + player.orbiteRadius;
	player.x1 = SCREEN_HEIGHT / 2;
	player.y1 = SCREEN_WIDTH / 2 - player.orbiteRadius;

	projectiles.push_back(Projectile());
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
		//updateCircles();
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
		backgroundColour = red;
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