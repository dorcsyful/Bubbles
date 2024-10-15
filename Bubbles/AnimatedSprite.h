#pragma once
#include <chrono>
#include <iostream>
#include <memory>
#include <SFML/Graphics/Drawable.hpp>
#include <SFML/Graphics/Sprite.hpp>
#include <SFML/Graphics/Texture.hpp>

class AnimatedSprite : public sf::Drawable
{
public:
	AnimatedSprite(const std::shared_ptr<sf::Texture>& a_Texture, const float a_TotalTime, const int a_FrameCount)
		: m_Texture(a_Texture), m_TotalTime(a_TotalTime), m_CurrentFrame(0), m_FrameCount(a_FrameCount)
	{
		m_Sprite = std::make_shared<sf::Sprite>(*m_Texture);
		sf::Vector2i size = static_cast<sf::Vector2i>(m_Texture->getSize());
		size.x /= m_FrameCount;
		m_Sprite->setTextureRect(sf::IntRect(0, 0, size.x, size.y));
		m_LastSwitch = std::chrono::steady_clock::now();
	}

	void UpdateFrame()
	{
		auto now = std::chrono::steady_clock::now();
		float duration = std::chrono::duration_cast<std::chrono::milliseconds>(now - m_LastSwitch).count() / 1000.f;
		if (duration > m_FrameCount/m_TotalTime)
		{
			std::cout << duration << std::endl;
			m_CurrentFrame++;
			if (m_CurrentFrame >= m_FrameCount)
			{
				m_CurrentFrame = 0;
			}
			sf::Vector2i size = static_cast<sf::Vector2i>(m_Texture->getSize());
			size.x /= m_FrameCount;
			m_Sprite->setTextureRect(sf::IntRect(size.x * m_CurrentFrame, 0, size.x, size.y));
			m_LastSwitch = now;
		}
	}

	void SetPosition(const sf::Vector2f& a_Position)
	{
		m_Sprite->setPosition(a_Position);
	}

private:
	void draw(sf::RenderTarget& a_Target, const sf::RenderStates a_States) const override
	{
		a_Target.draw(*m_Sprite, a_States);
	}



	std::shared_ptr<sf::Sprite> m_Sprite;
	std::shared_ptr<sf::Texture> m_Texture;
	float m_TotalTime;
	int m_CurrentFrame;
	int m_FrameCount;
	std::chrono::time_point<std::chrono::steady_clock> m_LastSwitch;
};
