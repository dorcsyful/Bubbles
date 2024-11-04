#pragma once

//Base UI
#define WINDOW_WIDTH 1200
#define WINDOW_HEIGHT 800
#define HIGH_SCORE_ITEM_WIDTH 300
#define	HIGH_SCORE_ITEM_HEIGHT 35

//Files
#define CLAM_FILENAME "Assets/Shapes/0.png"
#define CRAB_FILENAME "Assets/Shapes/1.png"
#define FISH_FILENAME "Assets/Shapes/2.png"
#define FROG_FILENAME "Assets/Shapes/3.png"
#define JELLY_FILENAME "Assets/Shapes/4.png"
#define KRILL_FILENAME "Assets/Shapes/5.png"
#define SEAL_FILENAME "Assets/Shapes/6.png"
#define SHARK_FILENAME "Assets/Shapes/7.png"
#define SQUID_FILENAME "Assets/Shapes/8.png"
#define WHALE_FILENAME "Assets/Shapes/9.png"

#define TITLE_FILENAME "Assets/Menu/Title.png"
#define BUTTON_FILENAME "Assets/Menu/Button_Base.png"
#define BUTTON_CLICKED_FILENAME "Assets/Menu/Button_Clicked.png"
#define FONT_FILENAME "Assets/Menu/Fonts/comic.ttf"
#define LOADING_FILENAME "Assets/Menu/LoadingSheet.png"

#define GAME_BACKGROUND_FILENAME "Assets/background.jpg"
#define CONTAINER_FILENAME "Assets/Other/ContainerBackground.png"
#define GAME_OVER_FILENAME "Assets/Other/GameOver.png"
#define SCORE_FILENAME "Assets/Other/ScoreBackground.png"
#define HIGH_SCORE_TITLE_FILENAME "Assets/Other/HighScore.png"

#define HIGH_SCORE_DATA_FILENAME "Assets/Slot00001.save"

//Gameplay
#define CONTAINER_WIDTH 400
#define CONTAINER_HEIGHT 600
#define POINTER_MOVE_SPEED 100
#define GRAVITY (-9.81f)

#define PIXEL_TO_METER 100
#define LOADING_TIME 0.2
#define LOADING_NUMBER_OF_FRAMES 8
#define LOADING_FRAME_TIME 0.5f

#define BUBBLE_FRAME_NUMBER 4
#define BUBBLE_FRAME_TIME 1

#define GAME_OVER_ANIMATION_TOTAL_TIME 10

#define NAIL_LENGTH 0.5f
#define NAIL_WEIGHT 0.5f

#include <map>

enum class EGAME_STATE
{
	STATE_START,
	STATE_PLAY,
	STATE_GAME_OVER_ANIMATION,
	STATE_GAME_OVER,
	STATE_MENU,
	STATE_LOADING,
	STATE_HIGH_SCORE
};

enum class EOBJECT_TYPE
{
	TYPE_NO_TYPE,
	TYPE_LINE,
	TYPE_CIRCLE,
	TYPE_NAIL
};

enum class EBUBBLE_TYPE
{
	TYPE_CLAM = 0,
	TYPE_CRAB = 1,
	TYPE_FISH = 2,
	TYPE_FROG = 3,
	TYPE_JELLY = 4,
	TYPE_KRILL = 5,
	TYPE_SEAL = 6,
	TYPE_SHARK = 7,
	TYPE_SQUID = 8,
	TYPE_WHALE = 9
};

static const std::map<EBUBBLE_TYPE, float> bubble_sizes = { {EBUBBLE_TYPE::TYPE_CLAM, 0.4}, {EBUBBLE_TYPE::TYPE_CRAB, 0.45}, {EBUBBLE_TYPE::TYPE_FISH, 0.55},
												   {EBUBBLE_TYPE::TYPE_FROG, 0.7},{EBUBBLE_TYPE::TYPE_JELLY, 0.80}, {EBUBBLE_TYPE::TYPE_KRILL, 0.9},
															{EBUBBLE_TYPE::TYPE_SEAL, 1}, {EBUBBLE_TYPE::TYPE_SHARK, 1.1}, {EBUBBLE_TYPE::TYPE_SQUID, 1.2},
												   {EBUBBLE_TYPE::TYPE_WHALE, 1.3}};

static const std::map<EBUBBLE_TYPE, float> bubble_weights = { {EBUBBLE_TYPE::TYPE_CLAM, 1}, {EBUBBLE_TYPE::TYPE_CRAB, 1.5}, {EBUBBLE_TYPE::TYPE_FISH, 2},
												   {EBUBBLE_TYPE::TYPE_FROG, 3},{EBUBBLE_TYPE::TYPE_JELLY, 3.5}, {EBUBBLE_TYPE::TYPE_KRILL, 4},
															{EBUBBLE_TYPE::TYPE_SEAL, 5}, {EBUBBLE_TYPE::TYPE_SHARK, 6}, {EBUBBLE_TYPE::TYPE_SQUID, 7},
												   {EBUBBLE_TYPE::TYPE_WHALE, 8} };