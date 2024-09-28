#include <SFML/Graphics.hpp>

#include "BubbleGame.h"

int main()
{

    BubbleGame* bubble_game = new BubbleGame();

    bubble_game->Initialize();


    bubble_game->Update();

    delete bubble_game;

    return 0;
}
