#pragma once
#include <string>
#include <algorithm>
#include <string>
#include <vector>
#include <cstdint>
#include <filesystem>
#include <chrono>
#include <fstream>
#include <iostream>

#include "Declarations.h"


class Settings
{
public:
    static Settings& get() {
        static Settings instance;
        return instance;
    }

    void LoadSettings();
    void LoadBubbleSizes(const std::string& a_Length);
    void LoadResolutions(const std::string& a_String);
    [[nodiscard]] float GetWindowWidth() const { return m_WindowWidth; }
    [[nodiscard]] float GetWindowHeight() const { return m_WindowHeight; }
    [[nodiscard]] float GetContainerWidth() const { return m_ContainerWidth; }
    [[nodiscard]] float GetContainerHeight() const { return m_ContainerHeight; }
    [[nodiscard]] float GetFrameWidth() const { return m_FrameWidth; }
    [[nodiscard]] float GetFrameHeight() const { return m_FrameHeight; }
    float GetFrameCorrection() const { return m_Correction; }
    [[nodiscard]] float GetConfirmationWidth() const { return m_ConfirmationWidth; }
    [[nodiscard]] float GetConfirmationHeight() const { return m_ConfirmationHeight; }
    [[nodiscard]] bool IsFullscreen() const { return m_FullScreen; }
    void SetFullscreen(const bool a_Fullscreen) { m_FullScreen = a_Fullscreen; }

	[[nodiscard]] int GetAaLevel() const { return m_AntiAliasingLevel; }

    [[nodiscard]] float GetPointerMoveSpeed() const { return m_PointerMoveSpeed; }
    [[nodiscard]] float GetPixelToMeter() const { return m_PixelToMeter; }
    [[nodiscard]] float GetLoadTime() const { return m_LoadingTime; }

    [[nodiscard]] unsigned int GetLoadingFrames() const { return m_LoadingFrames; }
    [[nodiscard]] unsigned int GetBubbleFrames() const { return m_BubbleFrameNumber; }
    [[nodiscard]] unsigned int GameOverAnimTime() const { return m_GameOverAnimationTime; }
    [[nodiscard]] float GetBubbleAnimationTotalTime() const { return m_BubbleAnimationTotalTime; }

    [[nodiscard]] unsigned int GetAddedScore() const { return m_AddedScore; }
    [[nodiscard]] unsigned int GetComboScore() const { return m_ComboScore; }

    [[nodiscard]] float GetHighScoreItemWidth() const { return m_HighScoreItemWidth; }
    [[nodiscard]] float GetHighScoreItemHeight() const { return m_HighScoreItemHeight; }

    [[nodiscard]] float GetDuckWidth() const { return m_DuckWidth; }
    [[nodiscard]] float GetDuckHeight() const { return m_DuckHeight; }

    [[nodiscard]] float GetNextUpWidth() const { return m_NextUpWidth; }
    [[nodiscard]] float GetNextUpHeight() const { return m_NextUpHeight; }

    [[nodiscard]] float BubbleSize(const EBUBBLE_TYPE a_Type) const { return m_BubbleSizes.at(a_Type); }

    [[nodiscard]] float GetMusicVolume() const { return m_MusicVolume; }
    [[nodiscard]] float GetSoundEffectsVolume() const { return m_EffectVolume; }
    void SetSoundEnabled(float a_Music, float a_Effects);

    [[nodiscard]] float GetMenuButtonWidth() const { return m_MenuButtonWidth; }
    [[nodiscard]] float GetMenuButtonHeight() const { return m_MenuButtonHeight; }
    [[nodiscard]] float GetButtonHeight() const { return m_ButtonHeight; }
    [[nodiscard]] float GetButtonWidth() const { return m_ButtonWidth; }
    [[nodiscard]] int GetMainButtonFontSize() const { return m_MainButtonFontSize; }

    [[nodiscard]] float GetTitleWidth() const { return m_TitleWidth; }
    [[nodiscard]] float GetTitleHeight() const { return m_TitleHeight; }
    [[nodiscard]] float GetScoreTitleWidth() const { return m_ScoreTitleWidth; }

    [[nodiscard]] float GetStorageBoxWidth() const { return m_StorageSpriteWidth; }
    [[nodiscard]] float GetStorageBoxHeight() const { return m_StorageSpriteHeight; }

    float GetContainerBottom() { return m_ContainerBottom; }
    float GetContainerLeft() {return m_ContainerLeft; }
    void UpdateContainerPos(float newBottom, float newLeft) { m_ContainerBottom = newBottom; m_ContainerLeft = newLeft; }

    [[nodiscard]] float GetScale() const { return m_Scale; }
    std::string GetResSizeAsString(int a_Index);
    int GetCurrentRes() const { return m_SelectedResOption; }
    int FindResIndex(int a_X) const
    { for (int i = 0; i < m_ResolutionOptions.size(); i++) { if (m_ResolutionOptions[i].first == a_X) return i; } return -1; }
    std::pair<int, int> GetResSizeAsPair(int a_Index) const { return m_ResolutionOptions[a_Index]; }

    void IncreaseIfFullScreen(float a_WindowX, float a_WindowY);


    void UpdateSettings(bool a_Fullscreen, std::string a_Resolution)
    {
        for (int i = 0; i < m_ResolutionOptions.size();i++)
        {
	        if (a_Resolution.substr(0,4) == std::to_string(m_ResolutionOptions[i].first))
	        {
                m_SelectedResOption = i;
	        }
        }


        std::vector<std::string> lines;
        std::ifstream infile("Assets/Settings.save");
        std::string line;
        int i;
        int counter = 0;
        int resLine;
        while (getline(infile, line)) {
            if (line.starts_with("FULLSCREEN|"))
            {
                i = counter;
            }
            if (line.starts_with("SELECTED_RESOLUTION|"))
            {
                resLine = counter;
            }
            lines.push_back(line);
            counter++;
        }
        infile.close();

        // Modify the last line
        std::string asString = a_Fullscreen ? "YES" : "NO";
        lines[i] = "FULLSCREEN|" + asString;
        lines[resLine] = "SELECTED_RESOLUTION|" + std::to_string(m_SelectedResOption);
        std::ofstream outfile("Assets/Settings.save");
        for (const std::string& nline : lines) {
            outfile << nline << '\n';
        }
        outfile.close();
    }
private:
    bool m_FullScreen = false;
    float m_Scale = 1;
    float m_WindowWidth = 0;
    float m_WindowHeight = 0;
    float m_ContainerWidth = 0;
    float m_ContainerHeight = 0;
    float m_FrameWidth = 0;
    float m_FrameHeight = 0;
    float m_Correction = 15;
    float m_ConfirmationWidth = 0;
    float m_ConfirmationHeight = 0;
    float m_TitleWidth = 0;
    float m_TitleHeight = 0;
    float m_MenuButtonWidth = 0;
    float m_MenuButtonHeight = 0;
    float m_ButtonWidth = 0;
    float m_ButtonHeight = 0;
    float m_ScoreTitleWidth = 0;
    float m_StorageSpriteWidth = 0;
    float m_StorageSpriteHeight = 0;
    float m_PointerMoveSpeed = 0;

    float m_PixelToMeter = 0;
    float m_LoadingTime = 0;

    unsigned int m_LoadingFrames = 0;

    unsigned int m_BubbleFrameNumber = 0;
    float m_BubbleAnimationTotalTime = 0;
    unsigned m_GameOverAnimationTime = 0;

    unsigned int m_AddedScore = 0;
    unsigned int m_ComboScore = 0;

    float m_HighScoreItemWidth = 0;
    float m_HighScoreItemHeight = 0;
    float m_MusicVolume = 100;
    float m_EffectVolume = 100;
    int m_AntiAliasingLevel = 0;

    float m_DuckHeight = 0;
    float m_DuckWidth = 0;

    float m_NextUpWidth = 0;
    float m_NextUpHeight = 0;
    int m_MainButtonFontSize = 0;

	std::map<EBUBBLE_TYPE, float> m_BubbleSizes;
    std::vector<std::pair<int, int>> m_ResolutionOptions;
    int m_SelectedResOption;

    float m_ContainerBottom;
    float m_ContainerLeft;
};

