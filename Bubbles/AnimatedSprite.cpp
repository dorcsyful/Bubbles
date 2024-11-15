#include "AnimatedSprite.h"
#include <SFML/Graphics/Texture.hpp>
#include <SFML/Graphics/RenderTarget.hpp>
AnimatedSprite::AnimatedSprite(sf::Texture* a_Texture, const float a_TotalTime, const int a_FrameCount, bool a_Animate, bool a_Looping):
	m_Texture(a_Texture),
	m_FrameTime(a_TotalTime),
	m_CurrentFrame(0),
	m_FrameCount(a_FrameCount)
{
	m_FrameTime /= m_FrameCount;
	m_FramesToAnimate = m_FrameCount;
	m_Sprite = std::make_unique<sf::Sprite>(*m_Texture);
	sf::Vector2i size = static_cast<sf::Vector2i>(m_Texture->getSize());
	size.x /= m_FrameCount;
	m_Sprite->setTextureRect(sf::IntRect(0, 0, size.x, size.y));
	m_LastSwitch = std::chrono::steady_clock::now();
	m_Animate = a_Animate;
	m_IsLooping = a_Looping;
}

void AnimatedSprite::SetNextFrame(const std::chrono::steady_clock::time_point a_Now)
{
	m_CurrentFrame++;
	if (m_CurrentFrame >= m_FramesToAnimate)
	{
		if (!m_IsLooping) return;
		m_CurrentFrame = 0;
	}
	sf::Vector2i size = static_cast<sf::Vector2i>(m_Texture->getSize());
	size.x /= m_FrameCount;
	m_Sprite->setTextureRect(sf::IntRect(size.x * m_CurrentFrame, 0, size.x, size.y));
	m_LastSwitch = a_Now;
}

void AnimatedSprite::UpdateFrameByTime()
{
	auto now = std::chrono::steady_clock::now();
	float duration = static_cast<float>(std::chrono::duration_cast<std::chrono::milliseconds>(now - m_LastSwitch).count()) / 1000.f;
	if (duration > m_FrameTime)
	{
		SetNextFrame(now);
	}
}

void AnimatedSprite::SetFrame(int a_Frame)
{
	sf::Vector2i size = static_cast<sf::Vector2i>(m_Texture->getSize());
	size.x /= m_FrameCount;
	m_Sprite->setTextureRect(sf::IntRect(size.x * a_Frame, 0, size.x, size.y));

}

bool AnimatedSprite::IsAnimFinished() const
{
	if (m_IsLooping) return false;
	if(m_CurrentFrame == m_FramesToAnimate) return true;
	return false;
}

void AnimatedSprite::Draw(sf::RenderTarget& a_Target)
{
	if (m_Animate) UpdateFrameByTime();

	a_Target.draw(*m_Sprite);
}
