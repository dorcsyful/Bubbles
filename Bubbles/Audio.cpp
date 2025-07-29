#include "Audio.h"

#include "Declarations.h"
#include "FilePaths.h"

void Audio::Initialize()
{
	m_BubbleDropBuffer = std::make_unique<sf::SoundBuffer>();
	m_BubbleDropBuffer->loadFromFile(AUDIO_BUBBLE_DROP);

	m_ClickBuffer = std::make_unique<sf::SoundBuffer>();
	m_ClickBuffer->loadFromFile(AUDIO_CLICK);

	m_BackgroundMusic = std::make_unique<sf::Music>();
	m_BackgroundMusic->openFromFile(AUDIO_BACKGROUND_MUSIC);
	m_BackgroundMusic->setPitch(1.0f); // Set pitch to normal speed
	m_BackgroundMusic->setLooping(true);
	m_BackgroundMusic->setVolume(30);
	m_BackgroundMusic->play();

	m_Click = std::make_unique<sf::Sound>(*m_ClickBuffer);

	m_BubbleDrop = std::make_unique<sf::Sound>(*m_BubbleDropBuffer);

	m_SadGameOverBuffer = std::make_unique<sf::SoundBuffer>();
	m_SadGameOverBuffer->loadFromFile(AUDIO_SAD_GAME_OVER);

	m_SadGameOver = std::make_unique<sf::Sound>(*m_SadGameOverBuffer);
}
