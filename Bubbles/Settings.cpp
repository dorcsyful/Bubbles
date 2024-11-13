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
            if (name == "SOUND_ENABLED")
                m_SoundEnabled = stoi(value);
        }
        file.close();
    }
}

void Settings::LoadBubbleSizes(const std::string& a_Length)
{
    int lastDelimiter = 0;
    std::string temp;
    for(int i = 0; i < a_Length.length(); i++)
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

void Settings::SetSoundEnabled(bool a_Enabled)
{
    m_SoundEnabled = a_Enabled;
    Audio::getInstance().SetAudioActive(a_Enabled);

    std::string isEnabledAsString = a_Enabled ? "1" : "0";

    std::vector<std::string> lines;
    std::ifstream infile("Assets/Settings.save");
    std::string line;
    while (getline(infile, line)) {
        lines.push_back(line);
    }
    infile.close();

    // Modify the last line
    lines.back() = "SOUND_ENABLED|" + isEnabledAsString;

    std::ofstream outfile("Assets/Settings.save");
    for (const std::string& nline : lines) {
        outfile << nline << std::endl;
    }
    outfile.close();
}
