#define _CRT_SECURE_NO_WARNINGS

#pragma once
#include <string>
#include <iostream>
#include <steam_api.h>
class LeaderBoard
{
public:
    LeaderBoard(const std::string& name = "High Scores")
        : leaderboardName(name), g_leaderboard(0), g_leaderboardFound(false) {
        FindOrCreateLeaderboard();
    }
    void SubmitScore(int score) {
        if (!g_leaderboardFound) return;

        SteamAPICall_t hCall = SteamUserStats()->UploadLeaderboardScore(
            g_leaderboard,
            k_ELeaderboardUploadScoreMethodKeepBest, // only keep best score
            score,
            nullptr,
            0
        );

        callResultUploadScore.Set(hCall, this, &LeaderBoard::OnScoreUploaded);
    }
    void DownloadTopScores(int count = 10);

private:
    std::string leaderboardName;
    SteamLeaderboard_t g_leaderboard;
    bool g_leaderboardFound;

    // Async callback handlers
    CCallResult<LeaderBoard, LeaderboardFindResult_t> callResultFindLeaderboard;
    CCallResult<LeaderBoard, LeaderboardScoreUploaded_t> callResultUploadScore;
    CCallResult<LeaderBoard, LeaderboardScoresDownloaded_t> callResultDownload;


    void FindOrCreateLeaderboard() {
        SteamAPICall_t hCall = SteamUserStats()->FindOrCreateLeaderboard(
            leaderboardName.c_str(),
            k_ELeaderboardSortMethodDescending,
            k_ELeaderboardDisplayTypeNumeric
        );

        callResultFindLeaderboard.Set(hCall, this, &LeaderBoard::OnLeaderboardFound);
    }

    void OnLeaderboardFound(LeaderboardFindResult_t* pResult, bool bIOFailure) {
        if (bIOFailure || !pResult->m_bLeaderboardFound) {
            std::cerr << "Failed to find/create leaderboard\n";
            return;
        }
        g_leaderboard = pResult->m_hSteamLeaderboard;
        g_leaderboardFound = true;
    }

    void OnScoreUploaded(LeaderboardScoreUploaded_t* pResult, bool bIOFailure) {
        if (bIOFailure || !pResult->m_bSuccess) {
            std::cerr << "Failed to upload score\n";
            return;
        }
    }

    void OnScoresDownloaded(LeaderboardScoresDownloaded_t* pResult, bool bIOFailure) {
        if (bIOFailure) {
            return;
        }

        int numEntries = pResult->m_cEntryCount;
        for (int i = 0; i < numEntries; ++i) {
            LeaderboardEntry_t entry;
            SteamUserStats()->GetDownloadedLeaderboardEntry(pResult->m_hSteamLeaderboardEntries, i, &entry, nullptr, 0);
        }
    }
};

