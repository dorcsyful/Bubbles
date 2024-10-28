#include "Rendering.h"

#include <functional>
#include <iostream>
#include <SFML/Window/Event.hpp>

#include "BubbleObject.h"
#include "Declarations.h"
#include "LineObject.h"

Rendering::Rendering(const int a_X, const int a_Y, std::vector<std::unique_ptr<AnimatedSprite>>& a_Wrapper):
	m_RenderedBubbles(a_Wrapper)
{
	m_Window = std::make_unique<sf::RenderWindow>(sf::VideoMode(a_X, a_Y), "Bubbles!",
	                                              sf::Style::Titlebar | sf::Style::Close);
	m_Window->setSize(sf::Vector2u(a_X, a_Y));
	LoadBackground();
	LoadBubbleTextures();
	CreateContainerLines();
	CreatePointer();
	CreateMenuSprites();
	CreateGameOverSprite();
	CreateScoreText();
	CreateHighScoreSprites();
}

void Rendering::PlayDraw() const
{
	for (const auto& element : m_Container)
	{
		m_Window->draw(*element);
	}

	m_Window->draw(*m_Line);
	m_Window->draw(*m_PreviewBubbles.at(m_ActiveBubble));
	for (auto& element : m_RenderedBubbles)
	{
		m_Window->draw(*element);
	}
	m_Window->draw(*m_ScoreBackground);
	m_Window->draw(*m_ScoreText);
}

void Rendering::MenuDraw() const
{
	m_Window->draw(*m_Title);

	m_Window->draw(*m_MenuButtons.at("Play"));
	m_Window->draw(*m_MenuButtons.at("High_Score"));
}

void Rendering::GameOverAnimationDraw() const
{
	for (const auto& element : m_Container)
	{
		m_Window->draw(*element);
	}

	m_Window->draw(*m_Line);
	for (auto& element : m_RenderedBubbles)
	{
		//element->UpdateFrameByTime();
		m_Window->draw(*element);
	}

	m_Window->draw(*m_GameOver);
}

void Rendering::HighScoreDraw() const
{
	m_Window->draw(*m_HighScoreTitle);

	for (int i = 0; i < 10; i++)
	{
		m_Window->draw(*(m_HighScoreSprites[i]));
	}
}

void Rendering::Draw(const EGAME_STATE a_State) const
{
	m_Window->clear();

	m_Window->draw(*m_BackgroundSprite);

	if (a_State == EGAME_STATE::STATE_PLAY)
	{
		PlayDraw();
	}
	if (a_State == EGAME_STATE::STATE_MENU) MenuDraw();
	if(a_State == EGAME_STATE::STATE_LOADING)
	{
		m_Window->draw(*m_Loading);
		m_Loading->UpdateFrameByTime();
	}
	if(a_State == EGAME_STATE::STATE_GAME_OVER_ANIMATION)
	{
		GameOverAnimationDraw();
	}
	if(a_State == EGAME_STATE::STATE_GAME_OVER)
	{
		m_Window->draw(*m_GameOver);
		m_Window->draw(*m_MenuButtons.at("PlayAgain"));
	}
	if(a_State == EGAME_STATE::STATE_HIGH_SCORE)
	{
		HighScoreDraw();
	}

	m_Window->display();
}

void Rendering::CreateSprite(const EBUBBLE_TYPE a_Type, const sf::Vector2f& a_Position, const float a_Rotation, std::unique_ptr<AnimatedSprite>& a_NewSprite) const
{
	a_NewSprite = std::make_unique<AnimatedSprite>(m_BubbleTextures.at(a_Type).get(),BUBBLE_FRAME_TIME,4);

	sf::Vector2f size = BubbleMath::ToVector2f(m_BubbleTextures.at(a_Type)->getSize());
	size.x /= 4;
	a_NewSprite->GetSprite()->setScale(bubble_sizes.at(a_Type) * PIXEL_TO_METER * 2 / size.x, bubble_sizes.at(a_Type) * PIXEL_TO_METER * 2 / size.y);

	float x = bubble_sizes.at(a_Type) * PIXEL_TO_METER / a_NewSprite->GetSprite()->getScale().x;
	float y = bubble_sizes.at(a_Type) * PIXEL_TO_METER / a_NewSprite->GetSprite()->getScale().y;
	a_NewSprite->GetSprite()->setOrigin(x, y);

	a_NewSprite->SetPosition(a_Position);
	a_NewSprite->SetRotation(a_Rotation);
}


void Rendering::MovePointerLine(const float a_X) const
{
	sf::Vector2f temp = m_Line->getPosition();
	temp.x = a_X;
	m_Line->setPosition(temp);
}

void Rendering::MovePreviewBubble(const EBUBBLE_TYPE a_NewPreview)
{
	m_ActiveBubble = a_NewPreview;
	m_PreviewBubbles.at(m_ActiveBubble)->SetPosition(m_Line->getPosition());
}

void Rendering::ResetButtons() const
{
	for(const auto& element : m_MenuButtons)
	{
		element.second->ApplyBaseTexture();
	}
}

void Rendering::UpdateHighScores(const std::vector<int>& a_Scores)
{
}

std::vector<std::unique_ptr<LineObject>> Rendering::ConvertToLine() const
{
	std::vector<std::unique_ptr<LineObject>> lines = std::vector<std::unique_ptr<LineObject>>();
	sf::Vector2f start = m_Container[0]->getPosition();
	start.x /= PIXEL_TO_METER;
	start.y /= PIXEL_TO_METER;
	start.y *= -1;
	sf::Vector2f end = sf::Vector2f(start.x, (-(m_Container[0]->getPosition().y + CONTAINER_HEIGHT) / PIXEL_TO_METER));
	std::unique_ptr<LineObject> temp = std::make_unique<LineObject>(start, end);
	lines.push_back(std::move(temp));

	start = m_Container[1]->getPosition();
	start.x /= PIXEL_TO_METER;
	start.y /= PIXEL_TO_METER;
	start.y *= -1;
	end = sf::Vector2f(start.x, (-(m_Container[1]->getPosition().y + CONTAINER_HEIGHT) / PIXEL_TO_METER));
	temp = std::make_unique<LineObject>(start, end);
	lines.push_back(std::move(temp));

	start = m_Container[2]->getPosition();
	start.x /= PIXEL_TO_METER;
	start.y /= PIXEL_TO_METER;
	start.y *= -1;
	end = sf::Vector2f((m_Container[2]->getPosition().x + CONTAINER_WIDTH) / PIXEL_TO_METER,start.y);
	temp = std::make_unique<LineObject>(start, end);
	lines.push_back(std::move(temp));

	return lines;
}

std::unique_ptr<LineObject> Rendering::ConvertTopLine() const
{
	std::vector<std::shared_ptr<LineObject>> lines = std::vector<std::shared_ptr<LineObject>>();
	sf::Vector2f start = m_Container[3]->getPosition();
	start.x /= PIXEL_TO_METER;
	start.y /= PIXEL_TO_METER;
	start.y *= -1;
	sf::Vector2f end = sf::Vector2f((m_Container[3]->getPosition().x + CONTAINER_WIDTH) / PIXEL_TO_METER, start.y);
	return std::make_unique<LineObject>(start, end);
}

void Rendering::Reset()
{
	m_ActiveBubble = EBUBBLE_TYPE::TYPE_BLUE;
	m_ScoreText->setString("Score:\n 0");
}

void Rendering::LoadBackground()
{
	m_BackgroundTexture = std::make_unique<sf::Texture>();


	if (!m_BackgroundTexture->loadFromFile("Assets/background.jpg"))
	{
		throw std::exception("Failed to load background texture");
	}
	m_BackgroundTexture->setRepeated(true);

	m_BackgroundSprite = std::make_unique<sf::RectangleShape>();
	m_BackgroundSprite->setTexture(m_BackgroundTexture.get());
	sf::Vector2f v = sf::Vector2f(static_cast<float>(m_Window->getSize().x), static_cast<float>(m_Window->getSize().y));
	
	m_BackgroundSprite->setTextureRect(sf::IntRect(0, 600, static_cast<int>(m_Window->getSize().x), static_cast<int>(m_Window->getSize().y)));

	m_BackgroundSprite->setSize(v);
}

void Rendering::LoadBubbleTextures()
{
	m_BubbleTextures = std::map<EBUBBLE_TYPE,std::unique_ptr<sf::Texture>>();
	m_BubbleTextures.insert(std::pair<EBUBBLE_TYPE,std::unique_ptr<sf::Texture>>(EBUBBLE_TYPE::TYPE_BLUE,std::make_unique<sf::Texture>()));
	m_BubbleTextures[EBUBBLE_TYPE::TYPE_BLUE]->loadFromFile(BLUE_FILENAME);

	m_BubbleTextures.insert(std::pair<EBUBBLE_TYPE, std::unique_ptr<sf::Texture>>(EBUBBLE_TYPE::TYPE_YELLOW, std::make_unique<sf::Texture>()));
	m_BubbleTextures[EBUBBLE_TYPE::TYPE_YELLOW]->loadFromFile(YELLOW_FILENAME);

	m_BubbleTextures.insert(std::pair<EBUBBLE_TYPE, std::unique_ptr<sf::Texture>>(EBUBBLE_TYPE::TYPE_GREEN,std::make_unique<sf::Texture>()));
	m_BubbleTextures[EBUBBLE_TYPE::TYPE_GREEN]->loadFromFile(GREEN_FILENAME);

	m_BubbleTextures.insert(std::pair<EBUBBLE_TYPE, std::unique_ptr<sf::Texture>>(EBUBBLE_TYPE::TYPE_PINK,std::make_unique<sf::Texture>()));
	m_BubbleTextures[EBUBBLE_TYPE::TYPE_PINK]->loadFromFile(PINK_FILENAME);

	m_BubbleTextures.insert(std::pair<EBUBBLE_TYPE, std::unique_ptr<sf::Texture>>(EBUBBLE_TYPE::TYPE_PURPLE,std::make_unique<sf::Texture>()));
	m_BubbleTextures[EBUBBLE_TYPE::TYPE_PURPLE]->loadFromFile(PURPLE_FILENAME);

	m_BubbleTextures.insert(std::pair<EBUBBLE_TYPE, std::unique_ptr<sf::Texture>>(EBUBBLE_TYPE::TYPE_RED,std::make_unique<sf::Texture>()));
	m_BubbleTextures[EBUBBLE_TYPE::TYPE_RED]->loadFromFile(RED_FILENAME);


}

void Rendering::CreateContainerLines()
{
	sf::Vector2f basePos = sf::Vector2f((static_cast<float>(m_Window->getSize().x) / 2.f) - (CONTAINER_WIDTH / 2.f),((static_cast<float>(m_Window->getSize().y) - CONTAINER_HEIGHT) /2.f));
	m_Container = std::vector<std::unique_ptr<sf::RectangleShape>>(4);

	m_Container[0] = std::make_unique<sf::RectangleShape>(sf::Vector2f(CONTAINER_LINE_THICKNESS,CONTAINER_HEIGHT));
	m_Container[0]->setFillColor(sf::Color(CONTAINER_LINE_COLOR_R, CONTAINER_LINE_COLOR_G, CONTAINER_LINE_COLOR_B));
	m_Container[0]->setPosition(basePos);

	m_Container[1] = std::make_unique<sf::RectangleShape>(sf::Vector2f(CONTAINER_LINE_THICKNESS, CONTAINER_HEIGHT));
	m_Container[1]->setFillColor(sf::Color(CONTAINER_LINE_COLOR_R, CONTAINER_LINE_COLOR_G, CONTAINER_LINE_COLOR_B));
	sf::Vector2f pos1 = basePos;
	pos1.x += CONTAINER_WIDTH;
	m_Container[1]->setPosition(pos1);

	m_Container[2] = std::make_unique<sf::RectangleShape>(sf::Vector2f(CONTAINER_WIDTH + CONTAINER_LINE_THICKNESS, CONTAINER_LINE_THICKNESS));
	m_Container[2]->setFillColor(sf::Color(CONTAINER_LINE_COLOR_R, CONTAINER_LINE_COLOR_G, CONTAINER_LINE_COLOR_B));
	sf::Vector2f pos2 = basePos;
	pos2.y += CONTAINER_HEIGHT;
	m_Container[2]->setPosition(pos2);

	m_Container[3] = std::make_unique<sf::RectangleShape>(sf::Vector2f(CONTAINER_WIDTH + CONTAINER_LINE_THICKNESS, CONTAINER_LINE_THICKNESS));
	m_Container[3]->setFillColor(sf::Color(CONTAINER_LINE_TOP_COLOR_R, CONTAINER_LINE_TOP_COLOR_G, CONTAINER_LINE_TOP_COLOR_B, CONTAINER_LINE_TOP_COLOR_A));
	sf::Vector2f pos3 = basePos;
	//pos3.y -= CONTAINER_HEIGHT;
	m_Container[3]->setPosition(pos3);

}

void Rendering::CreatePointer()
{
	sf::Vector2f position = sf::Vector2f((static_cast<float>(m_Window->getSize().x) / 2.f) - (CONTAINER_WIDTH / 2.f), ((static_cast<float>(m_Window->getSize().y) - CONTAINER_HEIGHT) / 2.f));
	m_Line = std::make_unique<sf::RectangleShape>(sf::Vector2f(CONTAINER_LINE_THICKNESS / 2.f, CONTAINER_HEIGHT));
	m_Line->setFillColor(sf::Color(255, 0, 0, 255));
	m_Line->setPosition(position);

	for(int i = 0; i < bubble_sizes.size(); i++)
	{
		std::unique_ptr<AnimatedSprite> newSprite;
		CreateSprite(static_cast<EBUBBLE_TYPE>(i), sf::Vector2f(position), 0, newSprite);
		m_PreviewBubbles.insert(std::pair<EBUBBLE_TYPE, std::unique_ptr<AnimatedSprite>>(static_cast<EBUBBLE_TYPE>(i), std::move(newSprite)));
	}
}

void Rendering::CreateTitleSprite()
{
	m_TitleTexture = std::make_unique<sf::Texture>();
	m_TitleTexture->loadFromFile(TITLE_FILENAME);
	m_Title = std::make_unique<sf::RectangleShape>();
	m_Title = std::make_unique<sf::RectangleShape>();
	m_Title->setTexture(m_TitleTexture.get());
	m_Title->setSize(BubbleMath::ToVector2f(m_TitleTexture->getSize()));
	m_Title->setOrigin(m_TitleTexture->getSize().x / 2.f, m_TitleTexture->getSize().y / 2.f);
	sf::Vector2f basePos = sf::Vector2f((static_cast<float>(m_Window->getSize().x) / 2.f), ((static_cast<float>(m_Window->getSize().y) - CONTAINER_HEIGHT) ));
	m_Title->setPosition(basePos);
}

void Rendering::CreateGameOverSprite()
{
	m_GameOverTexture = std::make_unique<sf::Texture>();
	m_GameOverTexture->loadFromFile(GAME_OVER_FILENAME);
	m_GameOver = std::make_unique<sf::RectangleShape>();
	m_GameOver->setTexture(m_GameOverTexture.get());
	m_GameOver->setSize(BubbleMath::ToVector2f(m_GameOverTexture->getSize()));

	sf::Vector2f basePos = BubbleMath::ToVector2f(m_GameOverTexture->getSize());
	m_GameOver->setOrigin(basePos.x / 2, basePos.y / 2);

	basePos = BubbleMath::ToVector2f(m_Window->getSize());
	m_GameOver->setPosition(sf::Vector2f(basePos.x / 2, basePos.y / 4));

	basePos = m_GameOver->getPosition();
	basePos.y += m_GameOver->getSize().y;
	std::unique_ptr<Button> newButton = std::make_unique<Button>(basePos, *m_Font, m_BaseButtonTexture.get(), m_ClickedButtonTexture.get());
	newButton->SetText("Play again");
	m_MenuButtons.insert(m_MenuButtons.begin(), std::pair<std::string, std::unique_ptr<Button>>("PlayAgain", std::move(newButton)));

}

void Rendering::CreateMenuSprites()
{
	CreateTitleSprite();
	CreateMenuButtonSprites();
}

void Rendering::CreateMenuButtonSprites()
{
	m_Font = std::make_unique<sf::Font>();
	m_Font->loadFromFile(FONT_FILENAME);

	sf::Vector2f basePos = sf::Vector2f(m_Title->getPosition());
	basePos.y += m_Title->getSize().y + 10;
	m_BaseButtonTexture = std::make_unique<sf::Texture>();
	m_BaseButtonTexture->loadFromFile(BUTTON_FILENAME);
	m_ClickedButtonTexture = std::make_unique<sf::Texture>();
	m_ClickedButtonTexture->loadFromFile(BUTTON_CLICKED_FILENAME);

	m_MenuButtons = std::map<std::string, std::unique_ptr<Button>>();

	std::unique_ptr<Button> newButton = std::make_unique<Button>(basePos,*m_Font, m_BaseButtonTexture.get(), m_ClickedButtonTexture.get());
	newButton->SetText("Play");
	m_MenuButtons.insert(m_MenuButtons.begin(),std::pair<std::string, std::unique_ptr<Button>>("Play",std::move(newButton)));

	basePos.y += m_BaseButtonTexture->getSize().y * 1.5f;
	std::unique_ptr<Button> newButton1 = std::make_unique<Button>(basePos, *m_Font, m_BaseButtonTexture.get(), m_ClickedButtonTexture.get());
	newButton1->SetText("High Scores");
	m_MenuButtons.insert(m_MenuButtons.begin(), std::pair<std::string, std::unique_ptr<Button>>("High_Score", std::move(newButton1)));

	m_LoadingTexture = std::make_unique<sf::Texture>();
	m_LoadingTexture->loadFromFile(LOADING_FILENAME);
	m_Loading = std::make_unique<AnimatedSprite>(m_LoadingTexture.get(), LOADING_FRAME_TIME, LOADING_NUMBER_OF_FRAMES);
	sf::Vector2f windowSize = BubbleMath::ToVector2f(m_Window->getSize());
	sf::Vector2f loadingTextureSize = BubbleMath::ToVector2f(m_LoadingTexture->getSize());
	m_Loading->SetPosition(sf::Vector2f(windowSize.x - loadingTextureSize.x / 8, windowSize.y - loadingTextureSize.y));
}

void Rendering::CreateScoreText()
{
	m_ScoreText = std::make_unique<sf::Text>();
	m_ScoreText->setFont(*m_Font);
	m_ScoreText->setCharacterSize(50);
	m_ScoreText->setFillColor(sf::Color::Black);
	m_ScoreText->setStyle(sf::Text::Bold);
	m_ScoreText->setString("Score:\n 0");
	sf::Vector2f position = m_Container[3]->getPosition();
	position.x = CONTAINER_WIDTH / 4;
	position.y += 300;
	m_ScoreText->setPosition(position);

	m_ScoreBackgroundTexture = std::make_unique<sf::Texture>();
	m_ScoreBackgroundTexture->loadFromFile(SCORE_FILENAME);

	m_ScoreBackground = std::make_unique<sf::RectangleShape>(sf::Vector2f(200, 100));
	m_ScoreBackground->setTexture(m_ScoreBackgroundTexture.get());
	sf::Vector2<float> size = m_ScoreText->getGlobalBounds().getSize();
	size.x += 40;
	size.y += 40;
	m_ScoreBackground->setSize(size);
	position.x -= 20;
	m_ScoreBackground->setPosition(position);
}

void Rendering::CreateHighScoreSprites()
{
	m_HighScoreSprites = std::vector < std::unique_ptr<SpriteWithText>>(10);
	sf::Vector2f basePos = sf::Vector2f(m_Title->getPosition());

	m_HighScoreTexture = std::make_unique<sf::Texture>();
	m_HighScoreTexture->loadFromFile(HIGH_SCORE_TITLE_FILENAME);

	m_HighScoreTitle = std::make_unique<sf::RectangleShape>();
	m_HighScoreTitle->setTexture(m_HighScoreTexture.get());
	sf::Vector2f temp = sf::Vector2f(static_cast<float>(m_HighScoreTexture->getSize().x), static_cast<float>(m_HighScoreTexture->getSize().y));
	m_HighScoreTitle->setSize(temp);
	m_HighScoreTitle->setOrigin(static_cast<float>(m_HighScoreTexture->getSize().x) / 2.f, static_cast<float>(m_HighScoreTexture->getSize().y) / 2.f);
	m_HighScoreTitle->setPosition(m_Title->getPosition());

	basePos.y += m_Title->getSize().y + 10;

	for(int i = 0; i < 10; i ++)
	{
		sf::Color textColor = i % 2 == 0 ? sf::Color::Blue : sf::Color::Green;
		sf::Color shapeColor = i % 2 == 0 ? sf::Color::Green : sf::Color::Blue;

		m_HighScoreSprites[i] = std::make_unique<SpriteWithText>(std::to_string(rand()), *m_Font, sf::Vector2f(HIGH_SCORE_ITEM_WIDTH, HIGH_SCORE_ITEM_HEIGHT),
							basePos, textColor,shapeColor);
		basePos.y += HIGH_SCORE_ITEM_HEIGHT ;
	}
}
