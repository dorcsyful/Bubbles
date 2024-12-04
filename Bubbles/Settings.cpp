#include "Settings.h"

#include <fstream>
#include <iostream>
#include <string>

#include "Audio.h"

void Settings::LoadSettings()
{
	std::ifstream file("Assets/Settings.save");
    if (file.is_open())
    {
        std::string line;
        while (getline(file, line))
        {
            std::string name = line.substr(0, line.find('|'));
            std::string value = line.substr(line.find('|') + 1, line.length());

        	if (name == "WINDOW_WIDTH")
                m_WindowWidth = stof(value);
            if (name == "WINDOW_HEIGHT")
                m_WindowHeight = stof(value);
            if (name ==  "CONTAINER_WIDTH")
                m_ContainerWidth = stof(value);
            if (name == "CONTAINER_HEIGHT")
                m_ContainerHeight = stof(value);
            if (name == "POINTER_MOVE_SPEED")
                m_PointerMoveSpeed = stof(value);
            if (name == "PIXEL_TO_METER")
                m_PixelToMeter = stof(value);
            if (name == "LOADING_TIME")
                m_LoadingTime = stof(value);
            if (name == "LOADING_NUMBER_OF_FRAMES")
                m_LoadingFrames = stoi(value);
            if (name == "BUBBLE_FRAME_NUMBER")
                m_BubbleFrameNumber = stoi(value);
            if (name == "HIGH_SCORE_ITEM_WIDTH")
                m_HighScoreItemWidth = stof(value);
            if (name == "HIGH_SCORE_ITEM_HEIGHT")
                m_HighScoreItemHeight = stof(value);
            if (name == "SIZES")
                LoadBubbleSizes(value);
            if (name == "COMBO_EXTRA_SCORE")
                m_ComboScore = stoi(value);
            if (name == "GAME_OVER_ANIMATION_TOTAL_TIME")
                m_GameOverAnimationTime = stoi(value);
            if (name == "BUBBLE_ANIMATION_TOTAL_TIME")
                m_BubbleAnimationTotalTime = stof(value);
            if (name == "MUSIC_VOLUME")
                m_MusicVolume = stof(value);
            if (name == "EFFECT_VOLUME")
                m_EffectVolume = stof(value);
            if (name == "ANTI_ALIASING")
                m_AntiAliasingLevel = stoi(value);
            if (name == "DUCK_WIDTH")
                m_DuckWidth = stof(value);
            if (name == "DUCK_HEIGHT")
                m_DuckHeight = stof(value);
            if (name == "NEXT_UP_WIDTH")
                m_NextUpWidth = stof(value);
            if (name == "NEXT_UP_HEIGHT")
                m_NextUpHeight = stof(value);
            if (name == "CONTAINER_FRAME_WIDTH")
                m_FrameWidth = stof(value);
            if (name == "CONTAINER_FRAME_HEIGHT")
                m_FrameHeight = stof(value);
            if (name == "CONFIRMATION_WIDTH")
                m_ConfirmationWidth = stof(value);
            if (name == "CONFIRMATION_HEIGHT")
                m_ConfirmationHeight = stof(value);
            if (name == "FULLSCREEN")
                m_FullScreen = value == "YES";
            if (name == "MENU_BUTTON_WIDTH")
                m_MenuButtonWidth = stof(value);
            if (name == "MENU_BUTTON_HEIGHT")
                m_MenuButtonHeight = stof(value);
            if (name == "REGULAR_BUTTON_WIDTH")
                m_ButtonWidth = stof(value);
            if (name == "REGULAR_BUTTON_HEIGHT")
                m_ButtonHeight = stof(value);
            if (name == "TITLE_WIDTH")
                m_TitleWidth = stof(value);
            if(name == "TITLE_HEIGHT")
                m_TitleHeight = stof(value);
            if (name == "SCORE_TITLE_SIZE")
                m_ScoreTitleWidth = stof(value);
        }
        file.close();
    }
}

void Settings::LoadBubbleSizes(const std::string& a_Length)
{
    std::string temp;
    for(size_t i = 0; i < a_Length.length(); i++)
    {
	    if(a_Length[i] == ',')
	    {
            m_BubbleSizes.insert(std::pair(static_cast<EBUBBLE_TYPE>(m_BubbleSizes.size()), stof(temp)));
            temp = "";
	    }
        else
        {
            temp += a_Length[i];
        }
    }
}

void Settings::SetSoundEnabled(float a_Music, float a_Effects)
{
    m_MusicVolume = a_Music;
    Audio::getInstance().SetMusicVolume(a_Music);
    Audio::getInstance().SetEffectsVolume(a_Effects);

    std::string musicVolumeAsString = std::to_string(std::min(a_Music, 100.f));
    std::string effectsVolumeAsString = std::to_string(std::min(a_Effects, 100.f));

    std::vector<std::string> lines;
    std::ifstream infile("Assets/Settings.save");
    std::string line;
    int musicLineIndex = INFINITY;
    int effectsLineIndex = INFINITY;
    int counter = 0;
    while (getline(infile, line)) {
        if(line.starts_with("MUSIC_VOLUME|"))
        {
            musicLineIndex = counter;
        }
        if (line.starts_with("EFFECT_VOLUME|"))
        {
            effectsLineIndex = counter;
        }
        lines.push_back(line);
        counter++;
    }
    infile.close();

    // Modify the last line
    lines[musicLineIndex] = "MUSIC_VOLUME|" + musicVolumeAsString;
    lines[effectsLineIndex] = "EFFECT_VOLUME|" + effectsVolumeAsString;

    std::ofstream outfile("Assets/Settings.save");
    for (const std::string& nline : lines) {
        outfile << nline << '\n';
    }
    outfile.close();
}
