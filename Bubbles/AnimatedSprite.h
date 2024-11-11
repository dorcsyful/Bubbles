#pragma once
#include <chrono>
#include <memory>
#include <SFML/Graphics/Sprite.hpp>

class AnimatedSprite
{
public:
	AnimatedSprite(sf::Texture* a_Texture, const float a_TotalTime, const int a_FrameCount, bool a_Animate = false, bool a_Looping = false);

	void SetNextFrame(const std::chrono::steady_clock::time_point a_Now);

	void UpdateFrameByTime();

	void SetPosition(const sf::Vector2f& a_Position) const { m_Sprite->setPosition(a_Position); }

	void SetRotation(const float a_Rotation) const { m_Sprite->setRotation(a_Rotation); }

	void SetTotalTime(float a_Time) { m_FrameTime = a_Time; }

	void SetAnimate(bool a_Enable, bool a_Loop) { m_Animate = a_Enable; m_IsLooping = a_Loop; }
	bool IsAnimated() const { return m_Animate; }
	bool IsAnimFinished() const;

	sf::Sprite* GetSprite() const { return m_Sprite.get(); }
	sf::Vector2f GetPosition() const { return m_Sprite->getPosition(); }

	void Draw(sf::RenderTarget& a_Target);

private:

	bool m_Animate;
	std::unique_ptr<sf::Sprite> m_Sprite;
	sf::Texture* m_Texture;
	float m_FrameTime;
	int m_CurrentFrame;
	int m_FrameCount;
	bool m_IsLooping;
	std::chrono::time_point<std::chrono::steady_clock> m_LastSwitch;
};
