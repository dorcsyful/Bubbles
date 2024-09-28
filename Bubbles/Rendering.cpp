#include "Rendering.h"

#include <functional>
#include <SFML/Window/Event.hpp>

#include "Declarations.h"

Rendering::Rendering(const int a_x, const int a_y)
{
	m_window = std::make_shared<sf::RenderWindow>(sf::VideoMode(a_x, a_y), "Bubbles!");
	
	LoadBackground();
	LoadBubbleTextures();
	CreateContainerLines();
}

void Rendering::draw() const
{
	sf::Event event;
	while (m_window->pollEvent(event))
	{
		if (event.type == sf::Event::Closed)
			m_window->close();
	}

	m_window->clear();

	m_window->draw(*m_backgroundSprite);

	for (const auto& element : m_container)
	{
		m_window->draw(*element);
	}

	for (const auto& element : m_bubbleSprites)
	{
		m_window->draw(*element);
	}

	m_window->display();
}

std::shared_ptr<sf::RectangleShape>& Rendering::AddSprite(int a_size, const sf::Vector2f& a_position, float a_rotation)
{
	std::shared_ptr<sf::RectangleShape> newSprite = std::make_shared<sf::RectangleShape>();
	newSprite->setTexture(m_bubbleTextures[a_size].get());
	sf::Vector2f size = sf::Vector2f(static_cast<float>(m_bubbleTextures[a_size]->getSize().x), static_cast<float>(m_bubbleTextures[a_size]->getSize().y));
	newSprite->setSize(size);
	newSprite->setPosition(a_position);
	newSprite->setRotation(a_rotation);
	m_bubbleSprites.push_back(newSprite);
	return m_bubbleSprites.back();
}

void Rendering::RemoveSprite(const std::shared_ptr<sf::RectangleShape>& a_spriteToRemove)
{
	m_bubbleSprites.erase(std::find(m_bubbleSprites.begin(), m_bubbleSprites.end(), a_spriteToRemove));
}

void Rendering::LoadBackground()
{
	m_backgroundTexture = std::make_shared<sf::Texture>();


	if (!m_backgroundTexture->loadFromFile("Assets/background.jpg"))
	{
		throw std::exception("Failed to load background texture");
	}
	m_backgroundTexture->setRepeated(true);

	m_backgroundSprite = std::make_shared<sf::RectangleShape>();
	m_backgroundSprite->setTexture(m_backgroundTexture.get());
	sf::Vector2<float> v = sf::Vector2<float>(static_cast<float>(m_window->getSize().x), static_cast<float>(m_window->getSize().y));

	m_backgroundSprite->setTextureRect(sf::IntRect(0, 600, v.x, v.y));

	m_backgroundSprite->setSize(v);
}

void Rendering::LoadBubbleTextures()
{
	m_bubbleTextures = std::vector<std::shared_ptr<sf::Texture>>(6);
	m_bubbleTextures[0] = std::make_shared<sf::Texture>();
	m_bubbleTextures[0]->loadFromFile(BLUE_FILENAME);

	m_bubbleTextures[1] = std::make_shared<sf::Texture>();
	m_bubbleTextures[1]->loadFromFile(GREEN_FILENAME);

	m_bubbleTextures[2] = std::make_shared<sf::Texture>();
	m_bubbleTextures[2]->loadFromFile(PINK_FILENAME);

	m_bubbleTextures[3] = std::make_shared<sf::Texture>();
	m_bubbleTextures[3]->loadFromFile(PURPLE_FILENAME);

	m_bubbleTextures[4] = std::make_shared<sf::Texture>();
	m_bubbleTextures[4]->loadFromFile(YELLOW_FILENAME);

	m_bubbleTextures[5] = std::make_shared<sf::Texture>();
	m_bubbleTextures[5]->loadFromFile(RED_FILENAME);


}

void Rendering::CreateContainerLines()
{
	sf::Vector2f position = sf::Vector2f((static_cast<float>(m_window->getSize().x) / 2.f) - (CONTAINER_WIDTH / 2.f),((static_cast<float>(m_window->getSize().y) - CONTAINER_HEIGHT) /2.f));
	m_container = std::vector<std::shared_ptr<sf::RectangleShape>>(4);

	m_container[0] = std::make_shared<sf::RectangleShape>(sf::Vector2f(CONTAINER_LINE_THICKNESS,CONTAINER_HEIGHT));
	m_container[0]->setFillColor(sf::Color(CONTAINER_LINE_COLOR_R, CONTAINER_LINE_COLOR_G, CONTAINER_LINE_COLOR_B));
	m_container[0]->setPosition(position);

	m_container[1] = std::make_shared<sf::RectangleShape>(sf::Vector2f(CONTAINER_LINE_THICKNESS, CONTAINER_HEIGHT));
	m_container[1]->setFillColor(sf::Color(CONTAINER_LINE_COLOR_R, CONTAINER_LINE_COLOR_G, CONTAINER_LINE_COLOR_B));
	position.x += CONTAINER_WIDTH;
	m_container[1]->setPosition(position);

	m_container[2] = std::make_shared<sf::RectangleShape>(sf::Vector2f(CONTAINER_WIDTH + CONTAINER_LINE_THICKNESS, CONTAINER_LINE_THICKNESS));
	m_container[2]->setFillColor(sf::Color(CONTAINER_LINE_COLOR_R, CONTAINER_LINE_COLOR_G, CONTAINER_LINE_COLOR_B));
	position.x -= CONTAINER_WIDTH;
	position.y += CONTAINER_HEIGHT;
	m_container[2]->setPosition(position);

	m_container[3] = std::make_shared<sf::RectangleShape>(sf::Vector2f(CONTAINER_WIDTH + CONTAINER_LINE_THICKNESS, CONTAINER_LINE_THICKNESS));
	m_container[3]->setFillColor(sf::Color(CONTAINER_LINE_TOP_COLOR_R, CONTAINER_LINE_TOP_COLOR_G, CONTAINER_LINE_TOP_COLOR_B, CONTAINER_LINE_TOP_COLOR_A));
	position.y -= CONTAINER_HEIGHT;
	m_container[3]->setPosition(position);

}