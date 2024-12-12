#pragma once
#include <random>

class Random {
public:
    Random() : m_Seed(time(nullptr)) {
        std::seed_seq seq{ m_Seed };
        m_Gen = std::mt19937(seq);
    }

    static Random& getInstance() {
        static Random instance;
        return instance;
    }

    void SetSeed(long long a_NewSeed) {
        m_Seed = a_NewSeed;
        std::seed_seq seq{ m_Seed };
        m_Gen = std::mt19937(seq);
    }

    int GetRandomNumber(int a_Min, int a_Max) {
        std::uniform_int_distribution<int> dist(a_Min, a_Max);
        return dist(m_Gen);
    }

private:
    long long m_Seed;
    std::mt19937 m_Gen;
};