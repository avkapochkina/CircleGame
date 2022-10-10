#pragma once
#include <stdint.h>

#define SCREEN_WIDTH 1024
#define SCREEN_HEIGHT 1024

extern uint32_t buffer[SCREEN_HEIGHT][SCREEN_WIDTH];
extern char screen_text[256];

#ifndef VK_ESCAPE
#  define VK_ESCAPE 0x1B
#  define VK_SPACE  0x20
#  define VK_LEFT   0x25
#  define VK_UP     0x26
#  define VK_RIGHT  0x27
#  define VK_DOWN   0x28
#  define VK_RETURN 0x0D
#endif

bool is_key_pressed(int button_vk_code);

bool is_mouse_button_pressed(int button);

int get_cursor_x();
int get_cursor_y();

bool is_window_active();

void clear_buffer();

void initialize();
void finalize();

void act(float dt);
void draw();

void schedule_quit_game();