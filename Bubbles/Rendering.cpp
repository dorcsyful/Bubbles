#include "Rendering.h"

#include <functional>
#include <iostream>
#include <SFML/Window/Event.hpp>

#include "BubbleObject.h"
#include "Declarations.h"
#include "LineObject.h"

Rendering::Rendering(const int a_X, const int a_Y)
{
	m_Window = std::make_shared<sf::RenderWindow>(sf::VideoMode(a_X, a_Y), "Bubbles!", sf::Style::Titlebar | sf::Style::Close);
	m_Window->setSize(sf::Vector2u(a_X, a_Y));
	LoadBackground();
	LoadBubbleTextures();
	CreateContainerLines();
	CreatePointer();
	CreateMenuSprites();
}

void Rendering::PlayDraw() const
{
	for (const auto& element : m_Container)
	{
		m_Window->draw(*element);
	}

	m_Window->draw(*m_Line);
	m_Window->draw(*m_PreviewBubbles.at(m_ActiveBubble));
	for (const auto& element : m_BubbleSprites)
	{
		m_Window->draw(*element);
	}
}

void Rendering::MenuDraw() const
{
	m_Window->draw(*m_Title);

	for(const auto& element : m_MenuButtons)
	{
		m_Window->draw(*element);
	}
}

void Rendering::Draw(EGAME_STATE a_State) const
{
	m_Window->clear();

	m_Window->draw(*m_BackgroundSprite);

	if (a_State == EGAME_STATE::STATE_PLAY)
	{
		PlayDraw();
	}
	if (a_State == EGAME_STATE::STATE_MENU || a_State == EGAME_STATE::STATE_LOADING) MenuDraw();
	if(a_State == EGAME_STATE::STATE_LOADING)
	{
		m_Window->draw(*m_Loading);
		m_Loading->UpdateFrame();
	}

	m_Window->display();
}

void Rendering::CreateSprite(EBUBBLE_TYPE a_Size, const sf::Vector2f& a_Position, float a_Rotation, std::shared_ptr<sf::RectangleShape>& newSprite)
{
	newSprite = std::make_shared<sf::RectangleShape>();
	newSprite->setTexture(m_BubbleTextures.at(a_Size).get());
	sf::Vector2i textureSize = sf::Vector2i(m_BubbleTextures.at(a_Size)->getSize().x, m_BubbleTextures.at(a_Size)->getSize().y);
	newSprite->setTextureRect(sf::IntRect(0, 0, textureSize.x, textureSize.y));
	sf::Vector2f bubbleSize = sf::Vector2f(bubble_sizes.at(a_Size) * PIXEL_TO_METER * 2.f, bubble_sizes.at(a_Size) * PIXEL_TO_METER * 2.f);
	newSprite->setSize(bubbleSize);
	newSprite->setOrigin(bubble_sizes.at(a_Size) * PIXEL_TO_METER,bubble_sizes.at(a_Size) * PIXEL_TO_METER);
	newSprite->setPosition(a_Position);
	newSprite->setRotation(a_Rotation);
}

std::shared_ptr<sf::RectangleShape>& Rendering::AddSprite(EBUBBLE_TYPE a_Size, const sf::Vector2f& a_Position, float a_Rotation)
{
	std::shared_ptr<sf::RectangleShape> newSprite;
	CreateSprite(a_Size, a_Position, a_Rotation, newSprite);
	m_BubbleSprites.push_back(newSprite);
	return m_BubbleSprites.back();
}

void Rendering::RemoveSprite(const std::shared_ptr<sf::RectangleShape>& a_SpriteToRemove)
{
	m_BubbleSprites.erase(std::find(m_BubbleSprites.begin(), m_BubbleSprites.end(), a_SpriteToRemove));
}

void Rendering::MovePointerLine(float a_X)
{
	sf::Vector2f temp = m_Line->getPosition();
	temp.x = a_X;
	m_Line->setPosition(temp);
}

void Rendering::MovePreviewBubble(EBUBBLE_TYPE a_NewPreview)
{
	m_ActiveBubble = a_NewPreview;
	m_PreviewBubbles.at(m_ActiveBubble)->setPosition(m_Line->getPosition());
}

std::vector<std::shared_ptr<LineObject>> Rendering::ConvertToLine()
{
	std::vector<std::shared_ptr<LineObject>> lines = std::vector<std::shared_ptr<LineObject>>();
	sf::Vector2f start = m_Container[0]->getPosition();
	start.x /= PIXEL_TO_METER;
	start.y /= PIXEL_TO_METER;
	start.y *= -1;
	sf::Vector2f end = sf::Vector2f(start.x, (-(m_Container[0]->getPosition().y + CONTAINER_HEIGHT) / PIXEL_TO_METER));
	std::shared_ptr<LineObject> temp = std::make_shared<LineObject>(start, end);
	lines.push_back(temp);

	start = m_Container[1]->getPosition();
	start.x /= PIXEL_TO_METER;
	start.y /= PIXEL_TO_METER;
	start.y *= -1;
	end = sf::Vector2f(start.x, (-(m_Container[1]->getPosition().y + CONTAINER_HEIGHT) / PIXEL_TO_METER));
	temp = std::make_shared<LineObject>(start, end);
	lines.push_back(temp);

	start = m_Container[2]->getPosition();
	start.x /= PIXEL_TO_METER;
	start.y /= PIXEL_TO_METER;
	start.y *= -1;
	end = sf::Vector2f((m_Container[2]->getPosition().x + CONTAINER_WIDTH) / PIXEL_TO_METER,start.y);
	temp = std::make_shared<LineObject>(start, end);
	lines.push_back(temp);

	return lines;
}

std::shared_ptr<LineObject> Rendering::ConvertTopLine()
{
	std::vector<std::shared_ptr<LineObject>> lines = std::vector<std::shared_ptr<LineObject>>();
	sf::Vector2f start = m_Container[3]->getPosition();
	start.x /= PIXEL_TO_METER;
	start.y /= PIXEL_TO_METER;
	start.y *= -1;
	sf::Vector2f end = sf::Vector2f((m_Container[3]->getPosition().x + CONTAINER_WIDTH) / PIXEL_TO_METER, start.y);
	return std::make_shared<LineObject>(start, end);
}

void Rendering::LoadBackground()
{
	m_BackgroundTexture = std::make_shared<sf::Texture>();


	if (!m_BackgroundTexture->loadFromFile("Assets/background.jpg"))
	{
		throw std::exception("Failed to load background texture");
	}
	m_BackgroundTexture->setRepeated(true);

	m_BackgroundSprite = std::make_shared<sf::RectangleShape>();
	m_BackgroundSprite->setTexture(m_BackgroundTexture.get());
	sf::Vector2<float> v = sf::Vector2<float>(static_cast<float>(m_Window->getSize().x), static_cast<float>(m_Window->getSize().y));

	m_BackgroundSprite->setTextureRect(sf::IntRect(0, 600, v.x, v.y));

	m_BackgroundSprite->setSize(v);
}

void Rendering::LoadBubbleTextures()
{
	m_BubbleTextures = std::map<EBUBBLE_TYPE,std::shared_ptr<sf::Texture>>();
	m_BubbleTextures.insert(std::pair<EBUBBLE_TYPE,std::shared_ptr<sf::Texture>>(EBUBBLE_TYPE::TYPE_BLUE,std::make_shared<sf::Texture>()));
	m_BubbleTextures[EBUBBLE_TYPE::TYPE_BLUE]->loadFromFile(BLUE_FILENAME);

	m_BubbleTextures.insert(std::pair<EBUBBLE_TYPE, std::shared_ptr<sf::Texture>>(EBUBBLE_TYPE::TYPE_YELLOW, std::make_shared<sf::Texture>()));
	m_BubbleTextures[EBUBBLE_TYPE::TYPE_YELLOW]->loadFromFile(YELLOW_FILENAME);

	m_BubbleTextures.insert(std::pair<EBUBBLE_TYPE, std::shared_ptr<sf::Texture>>(EBUBBLE_TYPE::TYPE_GREEN,std::make_shared<sf::Texture>()));
	m_BubbleTextures[EBUBBLE_TYPE::TYPE_GREEN]->loadFromFile(GREEN_FILENAME);

	m_BubbleTextures.insert(std::pair<EBUBBLE_TYPE, std::shared_ptr<sf::Texture>>(EBUBBLE_TYPE::TYPE_PINK,std::make_shared<sf::Texture>()));
	m_BubbleTextures[EBUBBLE_TYPE::TYPE_PINK]->loadFromFile(PINK_FILENAME);

	m_BubbleTextures.insert(std::pair<EBUBBLE_TYPE, std::shared_ptr<sf::Texture>>(EBUBBLE_TYPE::TYPE_PURPLE,std::make_shared<sf::Texture>()));
	m_BubbleTextures[EBUBBLE_TYPE::TYPE_PURPLE]->loadFromFile(PURPLE_FILENAME);

	m_BubbleTextures.insert(std::pair<EBUBBLE_TYPE, std::shared_ptr<sf::Texture>>(EBUBBLE_TYPE::TYPE_RED,std::make_shared<sf::Texture>()));
	m_BubbleTextures[EBUBBLE_TYPE::TYPE_RED]->loadFromFile(RED_FILENAME);


}

void Rendering::CreateContainerLines()
{
	sf::Vector2f basePos = sf::Vector2f((static_cast<float>(m_Window->getSize().x) / 2.f) - (CONTAINER_WIDTH / 2.f),((static_cast<float>(m_Window->getSize().y) - CONTAINER_HEIGHT) /2.f));
	m_Container = std::vector<std::shared_ptr<sf::RectangleShape>>(4);

	m_Container[0] = std::make_shared<sf::RectangleShape>(sf::Vector2f(CONTAINER_LINE_THICKNESS,CONTAINER_HEIGHT));
	m_Container[0]->setFillColor(sf::Color(CONTAINER_LINE_COLOR_R, CONTAINER_LINE_COLOR_G, CONTAINER_LINE_COLOR_B));
	m_Container[0]->setPosition(basePos);

	m_Container[1] = std::make_shared<sf::RectangleShape>(sf::Vector2f(CONTAINER_LINE_THICKNESS, CONTAINER_HEIGHT));
	m_Container[1]->setFillColor(sf::Color(CONTAINER_LINE_COLOR_R, CONTAINER_LINE_COLOR_G, CONTAINER_LINE_COLOR_B));
	sf::Vector2f pos1 = basePos;
	pos1.x += CONTAINER_WIDTH;
	m_Container[1]->setPosition(pos1);

	m_Container[2] = std::make_shared<sf::RectangleShape>(sf::Vector2f(CONTAINER_WIDTH + CONTAINER_LINE_THICKNESS, CONTAINER_LINE_THICKNESS));
	m_Container[2]->setFillColor(sf::Color(CONTAINER_LINE_COLOR_R, CONTAINER_LINE_COLOR_G, CONTAINER_LINE_COLOR_B));
	sf::Vector2f pos2 = basePos;
	pos2.y += CONTAINER_HEIGHT;
	m_Container[2]->setPosition(pos2);

	m_Container[3] = std::make_shared<sf::RectangleShape>(sf::Vector2f(CONTAINER_WIDTH + CONTAINER_LINE_THICKNESS, CONTAINER_LINE_THICKNESS));
	m_Container[3]->setFillColor(sf::Color(CONTAINER_LINE_TOP_COLOR_R, CONTAINER_LINE_TOP_COLOR_G, CONTAINER_LINE_TOP_COLOR_B, CONTAINER_LINE_TOP_COLOR_A));
	sf::Vector2f pos3 = basePos;
	//pos3.y -= CONTAINER_HEIGHT;
	m_Container[3]->setPosition(pos3);

}

void Rendering::CreatePointer()
{
	sf::Vector2f position = sf::Vector2f((static_cast<float>(m_Window->getSize().x) / 2.f) - (CONTAINER_WIDTH / 2.f), ((static_cast<float>(m_Window->getSize().y) - CONTAINER_HEIGHT) / 2.f));
	m_Line = std::make_shared<sf::RectangleShape>(sf::Vector2f(CONTAINER_LINE_THICKNESS / 2.f, CONTAINER_HEIGHT));
	m_Line->setFillColor(sf::Color(255, 0, 0, 255));
	m_Line->setPosition(position);

	for(int i = 0; i < bubble_sizes.size(); i++)
	{
		std::shared_ptr<sf::RectangleShape> newSprite;
		CreateSprite(static_cast<EBUBBLE_TYPE>(i), sf::Vector2f(position), 0, newSprite);
		m_PreviewBubbles.insert(std::pair<EBUBBLE_TYPE, std::shared_ptr<sf::RectangleShape>>(static_cast<EBUBBLE_TYPE>(i), newSprite));
	}
}

void Rendering::CreateTitleSprite()
{
	m_TitleTexture = std::make_shared<sf::Texture>();
	m_TitleTexture->loadFromFile(TITLE_FILENAME);
	m_Title = std::make_shared<sf::RectangleShape>();
	m_Title = std::make_shared<sf::RectangleShape>();
	m_Title->setTexture(m_TitleTexture.get());
	m_Title->setSize(sf::Vector2f(m_TitleTexture->getSize().x, m_TitleTexture->getSize().y));
	sf::Vector2f basePos = sf::Vector2f((static_cast<float>(m_Window->getSize().x) / 2.f) - (CONTAINER_WIDTH / 2.f), ((static_cast<float>(m_Window->getSize().y) - CONTAINER_HEIGHT) / 2.f));
	m_Title->setPosition(basePos);
}

void Rendering::CreateMenuSprites()
{
	CreateTitleSprite();
	CreateMenuButtonSprites();
}

void Rendering::CreateMenuButtonSprites()
{
	m_Font = std::make_shared<sf::Font>();
	m_Font->loadFromFile(FONT_FILENAME);

	sf::Vector2f basePos = sf::Vector2f(m_Title->getPosition());
	basePos.y += m_Title->getSize().y * 2.f;
	basePos.x += m_Title->getSize().x / 2.f;
	std::shared_ptr<sf::Texture> baseButtonTexture = std::make_shared<sf::Texture>();
	baseButtonTexture->loadFromFile(BUTTON_FILENAME);
	std::shared_ptr<sf::Texture> clickedButtonTexture = std::make_shared<sf::Texture>();
	clickedButtonTexture->loadFromFile(BUTTON_CLICKED_FILENAME);

	std::shared_ptr<Button> newButton = std::make_shared<Button>(basePos,*m_Font,baseButtonTexture,clickedButtonTexture);
	newButton->SetText("Play");
	m_MenuButtons = std::vector<std::shared_ptr<Button>>();
	m_MenuButtons.push_back(newButton);

	std::shared_ptr<sf::Texture> loadingTexture = std::make_shared<sf::Texture>();
	loadingTexture->loadFromFile(LOADING_FILENAME);
	m_Loading = std::make_shared<AnimatedSprite>(loadingTexture, LOADING_TIME * 2, 8);
	m_Loading->SetPosition(sf::Vector2f(m_Window->getSize().x - loadingTexture->getSize().x / 8, m_Window->getSize().y - loadingTexture->getSize().y));
}
