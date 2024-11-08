#include "Rendering.h"

#include <functional>
#include <SFML/Window/Event.hpp>

#include "BubbleObject.h"
#include "Declarations.h"
#include "LineObject.h"
#include "Settings.h"

Rendering::Rendering(const int a_X, const int a_Y, std::vector<std::unique_ptr<AnimatedSprite>>& a_Wrapper):
	m_RenderedBubbles(a_Wrapper)
{
	m_Window = std::make_unique<sf::RenderWindow>(sf::VideoMode(a_X, a_Y), "Bubbles!",
	                                              sf::Style::Titlebar | sf::Style::Close);
	m_Window->setSize(sf::Vector2u(a_X, a_Y));
	LoadBackground();
	LoadBubbleTextures();
	CreatePointer();
	CreateMenuSprites();
	CreateGameOverSprite();
	CreateScoreText();
	CreateHighScoreSprites();
}

void Rendering::PlayDraw() const
{
	m_Window->draw(*m_Container);
	m_Window->draw(*m_Line);

	m_Window->draw(*m_PreviewBubbles.at(m_ActiveBubble));
	for (auto& element : m_RenderedBubbles)
	{
		m_Window->draw(*element);
	}
	m_Window->draw(*m_Score);
	m_Window->draw(*m_ComboText);
}

void Rendering::MenuDraw() const
{
	m_Window->draw(*m_Title);

	m_Window->draw(*m_MenuButtons.at("Play"));
	m_Window->draw(*m_MenuButtons.at("High_Score"));
	m_Window->draw(*m_MenuButtons.at("Settings"));
	m_Window->draw(*m_MenuButtons.at("Exit"));
}

void Rendering::GameOverAnimationDraw() const
{
	m_Window->draw(*m_Container);
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
	m_Window->draw(*m_HSBackButton);
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
	a_NewSprite = std::make_unique<AnimatedSprite>(m_BubbleTextures.at(a_Type).get(),Settings::get().GetBubbleFrames(),4);

	sf::Vector2f size = BubbleMath::ToVector2f(m_BubbleTextures.at(a_Type)->getSize());
	size.x /= 4;
	float pixelToMeter = Settings::get().GetPixelToMeter();
	float factorX = Settings::get().BubbleSize(a_Type) * pixelToMeter * 2 / size.x;
	float factorY = Settings::get().BubbleSize(a_Type) * pixelToMeter * 2 / size.y;
	a_NewSprite->GetSprite()->setScale(factorX, factorY);

	float x = Settings::get().BubbleSize(a_Type) * pixelToMeter / a_NewSprite->GetSprite()->getScale().x;
	float y = Settings::get().BubbleSize(a_Type) * pixelToMeter / a_NewSprite->GetSprite()->getScale().y;
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

void Rendering::UpdateHighScores(const std::vector<unsigned int>& a_Scores) const
{
	for(int i = 0; i < 10; i++)
	{
		m_HighScoreSprites[i]->SetText(std::to_string(a_Scores[i]));
	}
}

void Rendering::Reset()
{
	m_ActiveBubble = EBUBBLE_TYPE::TYPE_CLAM;
	//m_Score->SetText("Score:\n 0");
}

void Rendering::LoadBackground()
{
	//Game Background
	m_BackgroundTexture = std::make_unique<sf::Texture>();

	if (!m_BackgroundTexture->loadFromFile(GAME_BACKGROUND_FILENAME))
	{
		throw std::exception("Failed to load background texture");
	}
	m_BackgroundTexture->setRepeated(true);
	m_BackgroundSprite = std::make_unique<sf::RectangleShape>();
	m_BackgroundSprite->setTexture(m_BackgroundTexture.get());
	sf::Vector2f windowSize = sf::Vector2f(static_cast<float>(m_Window->getSize().x),static_cast<float>(m_Window->getSize().y));
	sf::Vector2f v = sf::Vector2f(windowSize.x, windowSize.y);
	m_BackgroundSprite->setTextureRect(sf::IntRect(0, 600, static_cast<int>(windowSize.x), static_cast<int>(windowSize.y)));
	m_BackgroundSprite->setSize(v);

	//Container
	m_ContainerTexture = std::make_unique<sf::Texture>();
	if(!m_ContainerTexture->loadFromFile(CONTAINER_FILENAME))
	{
		throw std::exception("Failed to load container texture");
	}
	m_ContainerTexture->setRepeated(true);
	m_Container = std::make_unique<sf::RectangleShape>();
	m_Container->setTexture(m_ContainerTexture.get());
	float width = Settings::get().GetContainerWidth();
	float height = Settings::get().GetContainerHeight();
	m_Container->setSize(sf::Vector2f(width, height));
	sf::Vector2f basePos = sf::Vector2f((windowSize.x / 2.f) - (width / 2.f), ((windowSize.y - height) / 2.f));
	m_Container->setPosition(basePos);
}

void Rendering::LoadBubbleTextures()
{
	m_BubbleTextures = std::map<EBUBBLE_TYPE,std::unique_ptr<sf::Texture>>();
	m_BubbleTextures.insert(std::pair(EBUBBLE_TYPE::TYPE_CLAM,std::make_unique<sf::Texture>()));
	m_BubbleTextures[EBUBBLE_TYPE::TYPE_CLAM]->loadFromFile(CLAM_FILENAME);
	
	m_BubbleTextures.insert(std::pair<EBUBBLE_TYPE, std::unique_ptr<sf::Texture>>(EBUBBLE_TYPE::TYPE_CRAB, std::make_unique<sf::Texture>()));
	m_BubbleTextures[EBUBBLE_TYPE::TYPE_CRAB]->loadFromFile(CRAB_FILENAME);

	m_BubbleTextures.insert(std::pair<EBUBBLE_TYPE, std::unique_ptr<sf::Texture>>(EBUBBLE_TYPE::TYPE_FISH,std::make_unique<sf::Texture>()));
	m_BubbleTextures[EBUBBLE_TYPE::TYPE_FISH]->loadFromFile(FISH_FILENAME);

	m_BubbleTextures.insert(std::pair<EBUBBLE_TYPE, std::unique_ptr<sf::Texture>>(EBUBBLE_TYPE::TYPE_FROG,std::make_unique<sf::Texture>()));
	m_BubbleTextures[EBUBBLE_TYPE::TYPE_FROG]->loadFromFile(FROG_FILENAME);

	m_BubbleTextures.insert(std::pair<EBUBBLE_TYPE, std::unique_ptr<sf::Texture>>(EBUBBLE_TYPE::TYPE_JELLY,std::make_unique<sf::Texture>()));
	m_BubbleTextures[EBUBBLE_TYPE::TYPE_JELLY]->loadFromFile(JELLY_FILENAME);

	m_BubbleTextures.insert(std::pair<EBUBBLE_TYPE, std::unique_ptr<sf::Texture>>(EBUBBLE_TYPE::TYPE_KRILL,std::make_unique<sf::Texture>()));
	m_BubbleTextures[EBUBBLE_TYPE::TYPE_KRILL]->loadFromFile(KRILL_FILENAME);

	m_BubbleTextures.insert(std::pair<EBUBBLE_TYPE, std::unique_ptr<sf::Texture>>(EBUBBLE_TYPE::TYPE_SEAL, std::make_unique<sf::Texture>()));
	m_BubbleTextures[EBUBBLE_TYPE::TYPE_SEAL]->loadFromFile(SEAL_FILENAME);

	m_BubbleTextures.insert(std::pair<EBUBBLE_TYPE, std::unique_ptr<sf::Texture>>(EBUBBLE_TYPE::TYPE_SHARK, std::make_unique<sf::Texture>()));
	m_BubbleTextures[EBUBBLE_TYPE::TYPE_SHARK]->loadFromFile(SHARK_FILENAME);

	m_BubbleTextures.insert(std::pair<EBUBBLE_TYPE, std::unique_ptr<sf::Texture>>(EBUBBLE_TYPE::TYPE_SQUID, std::make_unique<sf::Texture>()));
	m_BubbleTextures[EBUBBLE_TYPE::TYPE_SQUID]->loadFromFile(SQUID_FILENAME);

	m_BubbleTextures.insert(std::pair<EBUBBLE_TYPE, std::unique_ptr<sf::Texture>>(EBUBBLE_TYPE::TYPE_WHALE, std::make_unique<sf::Texture>()));
	m_BubbleTextures[EBUBBLE_TYPE::TYPE_WHALE]->loadFromFile(WHALE_FILENAME);

	for (auto& current : m_BubbleTextures)
	{
		current.second->setSmooth(true);
	}
}

void Rendering::CreatePointer()
{
	float containerHeight = Settings::get().GetContainerHeight();
	sf::Vector2f position = sf::Vector2f((static_cast<float>(m_Window->getSize().x) / 2.f) - (Settings::get().GetContainerWidth() / 2.f), ((static_cast<float>(m_Window->getSize().y) - containerHeight) / 2.f) - 25);
	m_Line = std::make_unique<sf::RectangleShape>(sf::Vector2f(5 / 2.f, containerHeight + 25));
	m_Line->setFillColor(sf::Color(255, 0, 0, 255));
	m_Line->setPosition(position);

	for(size_t i = 0; i < 10; i++)
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
	sf::Vector2f titleTextureSize = BubbleMath::ToVector2f(m_TitleTexture->getSize());

	m_Title = std::make_unique<sf::RectangleShape>();
	m_Title = std::make_unique<sf::RectangleShape>();
	m_Title->setTexture(m_TitleTexture.get());
	m_Title->setSize(titleTextureSize);
	m_Title->setOrigin(titleTextureSize.x / 2.f, titleTextureSize.y / 2.f);
	sf::Vector2f basePos = sf::Vector2f(static_cast<float>(m_Window->getSize().x) / 2.f, static_cast<float>(m_Window->getSize().y) - (Settings::get().GetContainerHeight()*1.3f));
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
	newButton->ResizeCharacters(60);
	m_MenuButtons.insert(m_MenuButtons.begin(),std::pair<std::string, std::unique_ptr<Button>>("Play",std::move(newButton)));

	basePos.y += static_cast<float>(m_BaseButtonTexture->getSize().y) * 1.2f;
	std::unique_ptr<Button> newButton1 = std::make_unique<Button>(basePos, *m_Font, m_BaseButtonTexture.get(), m_ClickedButtonTexture.get());
	newButton1->SetText("High Scores");
	newButton1->ResizeCharacters(45);
	m_MenuButtons.insert(m_MenuButtons.begin(), std::pair<std::string, std::unique_ptr<Button>>("High_Score", std::move(newButton1)));

	basePos.y += static_cast<float>(m_BaseButtonTexture->getSize().y) * 1.2f;
	std::unique_ptr<Button> newButton3 = std::make_unique<Button>(basePos, *m_Font, m_BaseButtonTexture.get(), m_ClickedButtonTexture.get());
	newButton3->SetText("Settings");
	newButton3->ResizeCharacters(45);
	m_MenuButtons.insert(m_MenuButtons.begin(), std::pair<std::string, std::unique_ptr<Button>>("Settings", std::move(newButton3)));

	basePos.y += static_cast<float>(m_BaseButtonTexture->getSize().y) * 1.2f;
	std::unique_ptr<Button> newButton2 = std::make_unique<Button>(basePos, *m_Font, m_BaseButtonTexture.get(), m_ClickedButtonTexture.get());
	newButton2->SetText("Exit");
	newButton2->ResizeCharacters(60);

	m_MenuButtons.insert(m_MenuButtons.begin(), std::pair<std::string, std::unique_ptr<Button>>("Exit", std::move(newButton2)));

	m_LoadingTexture = std::make_unique<sf::Texture>();
	m_LoadingTexture->loadFromFile(LOADING_FILENAME);
	m_Loading = std::make_unique<AnimatedSprite>(m_LoadingTexture.get(), Settings::get().GetLoadTime(), Settings::get().GetLoadingFrames());
	sf::Vector2f windowSize = BubbleMath::ToVector2f(m_Window->getSize());
	sf::Vector2f loadingTextureSize = BubbleMath::ToVector2f(m_LoadingTexture->getSize());
	m_Loading->SetPosition(sf::Vector2f(windowSize.x - loadingTextureSize.x / 8, windowSize.y - loadingTextureSize.y));
}

void Rendering::CreateScoreText()
{
	sf::Vector2f position = m_Container->getPosition();
	position.x -= Settings::get().GetContainerWidth() / 4;
	position.y += 300;
	m_ScoreBackgroundTexture = std::make_unique<sf::Texture>();
	m_ScoreBackgroundTexture->loadFromFile(SCORE_FILENAME);

	m_Score = std::make_unique<SpriteWithText>("Score: \n 0", *m_Font, sf::Vector2f(200,130),
		position, sf::Color::Black, m_ScoreBackgroundTexture.get());


	m_ComboText = std::make_unique<sf::Text>();
	m_ComboText->setFont(*m_Font);
	m_ComboText->setCharacterSize(50);
	m_ComboText->setFillColor(sf::Color::Black);
	m_ComboText->setStyle(sf::Text::Bold);
	m_ComboText->setString("Combo:\n 0");
	position = m_Container->getPosition();
	position.x += Settings::get().GetContainerWidth();
	position.y += 100;
	m_ComboText->setPosition(position);

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

		m_HighScoreSprites[i] = std::make_unique<SpriteWithText>(std::to_string(rand()), *m_Font, sf::Vector2f(Settings::get().GetHighScoreItemWidth(), Settings::get().GetHighScoreItemHeight()),
							basePos, textColor,shapeColor);
		basePos.y += Settings::get().GetHighScoreItemHeight();
	}

	sf::Vector2f buttonTextureSize = sf::Vector2f(static_cast<float>(m_BaseButtonTexture->getSize().x), static_cast<float>(m_BaseButtonTexture->getSize().y));
	sf::Vector2f buttonPos = sf::Vector2f(buttonTextureSize.x / 2, Settings::get().GetWindowHeight() - buttonTextureSize.y);
	m_HSBackButton = std::make_unique<Button>(buttonPos, *m_Font, m_BaseButtonTexture.get(), m_ClickedButtonTexture.get());
	m_HSBackButton->SetText("Back");
	m_HSBackButton->SetScale(sf::Vector2f(0.7f, 0.7f));
}
