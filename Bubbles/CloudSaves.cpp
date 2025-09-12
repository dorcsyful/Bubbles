#define _CRT_SECURE_NO_WARNINGS

#include "CloudSaves.h"
#include <iostream>
#include <algorithm>
#include <steam_api.h>
const char* CLOUD_FILE = "personal_top10.dat";
const uint32_t XOR_KEY = 0xA5A5A5A5; // simple obfuscation key

CloudSaves::CloudSaves()
{
}

void CloudSaves::LoadPersonalTop10()
{
    if (SteamRemoteStorage()->FileExists(CLOUD_FILE)) {
        int32 fileSize = SteamRemoteStorage()->GetFileSize(CLOUD_FILE);
        if (fileSize != 4 * 10 + 4) { // 10 ints + 4-byte checksum
            m_TopScores = std::vector<unsigned>(10, 0);
            return;
        }

        std::vector<uint8_t> buffer(fileSize);
        SteamRemoteStorage()->FileRead(CLOUD_FILE, buffer.data(), fileSize);

        // Extract scores
        m_TopScores.clear();
        for (int i = 0; i < 10; ++i) {
            int val = buffer[i * 4] | (buffer[i * 4 + 1] << 8) | (buffer[i * 4 + 2] << 16) | (buffer[i * 4 + 3] << 24);
            val ^= XOR_KEY; // reverse obfuscation
            m_TopScores.push_back(val);
        }

        // Validate checksum
        uint32_t storedChecksum = buffer[40] | (buffer[41] << 8) | (buffer[42] << 16) | (buffer[43] << 24);
        if (ComputeChecksum(m_TopScores) != storedChecksum) {
            // Tampered file, reset
            m_TopScores = std::vector<unsigned>(10, 0);
        }
    }
    else {
        m_TopScores = std::vector<unsigned>(10, 0);
    }

}

void CloudSaves::SubmitScore(int score)
{
    m_TopScores.push_back(score);
    std::sort(m_TopScores.begin(), m_TopScores.end(), std::greater<unsigned>());
    if (m_TopScores.size() > 10)
        m_TopScores.resize(10);

    SavePersonalTop10();

}

void CloudSaves::PrintTop10()
{
    std::cout << "Top 10:\n";
    for(int i = 0; i < 10; i++)
        std::cout << m_TopScores[i] << "\n";

}

uint32_t CloudSaves::ComputeChecksum(const std::vector<unsigned>& scores)
{
    uint32_t sum = 0;
    for (int s : scores) {
        sum += static_cast<uint32_t>(s);
    }
    return sum ^ XOR_KEY;
}

void CloudSaves::SavePersonalTop10()
{
    std::vector<uint8_t> buffer(44); // 10 ints * 4 bytes + 4 byte checksum

    for (size_t i = 0; i < 10; ++i) {
        int val = m_TopScores[i] ^ XOR_KEY;
        buffer[i * 4] = val & 0xFF;
        buffer[i * 4 + 1] = (val >> 8) & 0xFF;
        buffer[i * 4 + 2] = (val >> 16) & 0xFF;
        buffer[i * 4 + 3] = (val >> 24) & 0xFF;
    }

    uint32_t checksum = ComputeChecksum(m_TopScores);
    buffer[40] = checksum & 0xFF;
    buffer[41] = (checksum >> 8) & 0xFF;
    buffer[42] = (checksum >> 16) & 0xFF;
    buffer[43] = (checksum >> 24) & 0xFF;

    SteamRemoteStorage()->FileWrite(CLOUD_FILE, buffer.data(), buffer.size());

}
