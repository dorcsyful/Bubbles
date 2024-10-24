#include <SFML/Graphics.hpp>

#include "BubbleGame.h"
#include "Save.h"

int main()
{
    std::unique_ptr<BubbleGame>  bubble_game = std::make_unique<BubbleGame>();

    bubble_game->Initialize();


    bubble_game->Update();



    return 0;
}
