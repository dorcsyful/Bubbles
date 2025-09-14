#pragma once
#include <steam_api.h>
#include <string>
#include <iostream>
class Achievments
{
public:
	Achievments()
	{

	}


    // Unlock an achievement by its API name (set in Steamworks)
    void Unlock(const std::string& id) {
        bool achieved = false;
        if (SteamUserStats()->GetAchievement(id.c_str(), &achieved) && !achieved) {
            SteamUserStats()->SetAchievement(id.c_str());
            SteamUserStats()->StoreStats();
            std::cout << "Unlocked achievement: " << id << "\n";
        }
        else {
            std::cout << "Achievement already unlocked or error: " << id << "\n";
        }
    }

    // Reset all achievements (useful for testing)
    void ResetAll() {
        if (!statsInitialized) return;
        SteamUserStats()->ResetAllStats(true);

        std::cout << "All achievements reset\n";
    }

private:
    bool statsInitialized;

    CCallResult<Achievments, UserStatsReceived_t> callResultUserStatsReceived;

    void OnUserStatsReceived(UserStatsReceived_t* pCallback, bool bIOFailure) {
        if (bIOFailure || pCallback->m_eResult != k_EResultOK) {
            std::cerr << "Failed to receive user stats/achievements\n";
            return;
        }
        statsInitialized = true;
        std::cout << "Achievements initialized and ready!\n";
    }
};

