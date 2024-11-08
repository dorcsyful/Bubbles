#pragma once
#include <memory>
#include <SFML/Audio.hpp>

#include "Helpers.h"

class Audio
{
public:
	Audio() = default;
	static Audio& getInstance() {
		static Audio instance;
		return instance;
	}
	void Initialize();
	void PlayClick() const { m_Click->play(); }
	void PlayBubbleDrop() const { m_BubbleDrop->play(); }
	void PlaySadGameOver() const
	{
		m_BackgroundMusic->pause();
		m_SadGameOver->play();
		CallAfterDelay::getInstance().AddFunction([this] {m_BackgroundMusic->play(); }, m_SadGameOverBuffer->getDuration().asSeconds(), false);
	}

private:

	std::unique_ptr<sf::Sound> m_Click;
	std::unique_ptr<sf::Sound> m_BubbleDrop;
	std::unique_ptr<sf::Sound> m_SadGameOver;

	std::unique_ptr<sf::SoundBuffer> m_ClickBuffer;
	std::unique_ptr<sf::SoundBuffer> m_BubbleDropBuffer;
	std::unique_ptr<sf::SoundBuffer> m_SadGameOverBuffer;
	std::unique_ptr<sf::Music> m_BackgroundMusic;
};

