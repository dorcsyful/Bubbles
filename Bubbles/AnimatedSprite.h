#pragma once
#include <chrono>
#include <iostream>
#include <memory>
#include <SFML/Graphics/Drawable.hpp>
#include <SFML/Graphics/Sprite.hpp>
#include <SFML/Graphics/Texture.hpp>

#include <SFML/Graphics/RenderTarget.hpp>
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

	void SetNextFrame(std::chrono::steady_clock::time_point now)
	{
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

	void UpdateFrameByTime()
	{
		auto now = std::chrono::steady_clock::now();
		float duration = static_cast<float>(std::chrono::duration_cast<std::chrono::milliseconds>(now - m_LastSwitch).count()) / 1000.f;
		if (duration > static_cast<float>(m_FrameCount)/m_TotalTime)
		{
			SetNextFrame(now);
		}
	}

	void SetPosition(const sf::Vector2f& a_Position) const
	{
		m_Sprite->setPosition(a_Position);
	}

	void SetRotation(const float a_Rotation) const
	{
		m_Sprite->setRotation(a_Rotation);
	}

	std::shared_ptr<sf::Sprite> GetSprite() const { return m_Sprite; }
	sf::Vector2f GetPosition() const { return m_Sprite->getPosition(); }

private:
	void draw(sf::RenderTarget& a_Target, const sf::RenderStates a_States) const override
	{
		// You can draw other high-level objects
		a_Target.draw(*m_Sprite, a_States);


	}

	std::shared_ptr<sf::Sprite> m_Sprite;
	std::shared_ptr<sf::Texture> m_Texture;
	float m_TotalTime;
	int m_CurrentFrame;
	int m_FrameCount;
	std::chrono::time_point<std::chrono::steady_clock> m_LastSwitch;
};
