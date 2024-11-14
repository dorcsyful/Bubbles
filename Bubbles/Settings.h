#pragma once
#include <string>

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

    [[nodiscard]] float GetWindowWidth() const { return m_WindowWidth; }
    [[nodiscard]] float GetWindowHeight() const { return m_WindowHeight; }
    [[nodiscard]] float GetContainerWidth() const { return m_ContainerWidth; }
    [[nodiscard]] float GetContainerHeight() const { return m_ContainerHeight; }

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

    int GetAaLevel() const { return m_AntiAliasingLevel; }

    [[nodiscard]] float BubbleSize(const EBUBBLE_TYPE a_Type) const { return m_BubbleSizes.at(a_Type); }

    [[nodiscard]] bool IsSoundEnabled() const { return m_SoundEnabled; }
    void SetSoundEnabled(bool a_Enabled);

private:
    float m_WindowWidth = 0;
    float m_WindowHeight = 0;
    float m_ContainerWidth = 0;
    float m_ContainerHeight = 0;

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
    bool m_SoundEnabled = true;
    int m_AntiAliasingLevel = 0;;
	std::map<EBUBBLE_TYPE, float> m_BubbleSizes;
};

