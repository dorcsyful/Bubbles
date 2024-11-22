#include <SFML/Window/Event.hpp>
#include <SFML/Graphics/CircleShape.hpp>
#include "Audio.h"
#include "BubbleGame.h"

int main()
{

    Settings::get().LoadSettings();
    Audio::getInstance().Initialize();
    std::unique_ptr<BubbleGame>  bubble_game = std::make_unique<BubbleGame>();

    bubble_game->Initialize();

    bubble_game->Update();

    return 0;
}

