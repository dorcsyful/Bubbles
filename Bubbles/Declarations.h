#pragma once

//UI specific
#define WINDOW_WIDTH 600
#define WINDOW_HEIGHT 800
#define CONTAINER_LINE_THICKNESS 5
#define CONTAINER_LINE_COLOR_R 0
#define CONTAINER_LINE_COLOR_G 0
#define CONTAINER_LINE_COLOR_B 255
#define CONTAINER_LINE_TOP_COLOR_R 0
#define CONTAINER_LINE_TOP_COLOR_G 0
#define CONTAINER_LINE_TOP_COLOR_B 128
#define CONTAINER_LINE_TOP_COLOR_A 128

#define BLUE_FILENAME "Assets/Shapes/blue.png"
#define GREEN_FILENAME "Assets/Shapes/green.png"
#define PINK_FILENAME "Assets/Shapes/pink.png"
#define PURPLE_FILENAME "Assets/Shapes/red.png"
#define YELLOW_FILENAME "Assets/Shapes/yellow.png"
#define RED_FILENAME "Assets/Shapes/red.png"

#define CONTAINER_WIDTH 500
#define CONTAINER_HEIGHT 600
#define POINTER_MOVE_SPEED 100
#define GRAVITY -90.81
#include <map>

enum class EOBJECT_TYPE
{
	TYPE_NO_TYPE,
	TYPE_LINE,
	TYPE_CIRCLE
};

enum class EBUBBLE_TYPE
{
	TYPE_BLUE = 0,
	TYPE_YELLOW = 1,
	TYPE_GREEN = 2,
	TYPE_PINK = 3,
	TYPE_PURPLE = 4,
	TYPE_RED = 5
};

static const std::map<EBUBBLE_TYPE, float> bubble_sizes = { {EBUBBLE_TYPE::TYPE_BLUE, 50}, {EBUBBLE_TYPE::TYPE_YELLOW, 70}, {EBUBBLE_TYPE::TYPE_GREEN, 90},
												   {EBUBBLE_TYPE::TYPE_PINK, 110},{EBUBBLE_TYPE::TYPE_PURPLE, 130}, {EBUBBLE_TYPE::TYPE_RED, 150} };

static const std::map<EBUBBLE_TYPE, float> bubble_weights = { {EBUBBLE_TYPE::TYPE_BLUE, 10}, {EBUBBLE_TYPE::TYPE_YELLOW, 20}, {EBUBBLE_TYPE::TYPE_GREEN, 30},
												   {EBUBBLE_TYPE::TYPE_PINK, 40},{EBUBBLE_TYPE::TYPE_PURPLE, 50}, {EBUBBLE_TYPE::TYPE_RED, 60} };
