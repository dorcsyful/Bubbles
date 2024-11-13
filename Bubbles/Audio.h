#pragma once
#include <memory>
#include <SFML/Audio.hpp>

#include "Declarations.h"
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
	void PlayBackgroundMusic() const
	{
		m_BackgroundMusic->play();
	}
	void PlaySadGameOver() const
	{
		m_BackgroundMusic->stop();
		m_SadGameOver->play();	
	}
	void SetAudioActive(const bool a_Enabled)
	{
		float newVolume = a_Enabled ? 100 : 0;
		m_Click->setVolume(newVolume);
		m_BubbleDrop->setVolume(newVolume);
		m_BackgroundMusic->setVolume(newVolume);
		m_SadGameOver->setVolume(newVolume);
		m_Enabled = a_Enabled;
	}

	bool IsAudioEnabled() const { return m_Enabled; }

private:

	bool m_Enabled = true;
	std::unique_ptr<sf::Sound> m_Click;
	std::unique_ptr<sf::Sound> m_BubbleDrop;
	std::unique_ptr<sf::Sound> m_SadGameOver;
	std::unique_ptr<sf::Music> m_BackgroundMusic;

	std::unique_ptr<sf::SoundBuffer> m_ClickBuffer;
	std::unique_ptr<sf::SoundBuffer> m_BubbleDropBuffer;
	std::unique_ptr<sf::SoundBuffer> m_SadGameOverBuffer;
};

