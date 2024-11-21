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
    [[nodiscard]] float GetFrameWidth() const { return m_FrameWidth; }
    [[nodiscard]] float GetFrameHeight() const { return m_FrameHeight; }
    [[nodiscard]] float GetConfirmationWidth() const { return m_ConfirmationWidth; }
    [[nodiscard]] float GetConfirmationHeight() const { return m_ConfirmationHeight; }


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

private:
    float m_WindowWidth = 0;
    float m_WindowHeight = 0;
    float m_ContainerWidth = 0;
    float m_ContainerHeight = 0;
    float m_FrameWidth = 0;
    float m_FrameHeight = 0;
    float m_ConfirmationWidth = 0;
    float m_ConfirmationHeight = 0;

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

	std::map<EBUBBLE_TYPE, float> m_BubbleSizes;
};

