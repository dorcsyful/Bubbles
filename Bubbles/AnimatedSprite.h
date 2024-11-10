#pragma once
#include <chrono>
#include <memory>
#include <SFML/Graphics/Sprite.hpp>

class AnimatedSprite : public sf::Drawable
{
public:
	AnimatedSprite(sf::Texture* a_Texture, const float a_TotalTime, const int a_FrameCount);

	void SetNextFrame(const std::chrono::steady_clock::time_point a_Now);

	void UpdateFrameByTime();

	void SetPosition(const sf::Vector2f& a_Position) const { m_Sprite->setPosition(a_Position); }

	void SetRotation(const float a_Rotation) const { m_Sprite->setRotation(a_Rotation); }

	void SetTotalTime(float a_Time) { m_FrameTime = a_Time; }

	sf::Sprite* GetSprite() const { return m_Sprite.get(); }
	sf::Vector2f GetPosition() const { return m_Sprite->getPosition(); }

private:
	void draw(sf::RenderTarget& a_Target, sf::RenderStates a_States) const override;

	std::unique_ptr<sf::Sprite> m_Sprite;
	sf::Texture* m_Texture;
	float m_FrameTime;
	int m_CurrentFrame;
	int m_FrameCount;
	std::chrono::time_point<std::chrono::steady_clock> m_LastSwitch;
};
