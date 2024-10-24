#pragma once
#include <algorithm>
#include <fstream>
#include <iostream>
#include <sstream>
#include <string>
#include <vector>
#include <cstdint>
#include "Declarations.h"
#include <filesystem>
#include <chrono>
#include <random>

class Save
{
public:

    Save();

    void SaveIfHighScore(unsigned a_Score);

private:

    // Structure to store data
    struct Data {

        Data() : m_Score(0){}
        Data(unsigned int a_Score)
        {
            m_Score = a_Score;
            const auto now = std::chrono::system_clock::now();
            m_Date = std::format("{:%d-%m-%Y}", now);
        }
        Data(unsigned int a_Score, std::string a_Date) :m_Score(a_Score), m_Date(std::move(a_Date))
        {
        }

        unsigned int m_Score;
        std::string m_Date;
    };

    static uint64_t CalculateHash(const Data& a_Data);

    static void SaveData(const std::vector<Data>& a_Data);

    static std::vector<Data> LoadData();

    static bool DoesSaveFileExist();

    static void CreateSaveFile();

    static bool GreaterThan(const Data& a_First, const Data& a_Second)
    {
        return a_First.m_Score > a_Second.m_Score;
    }

     std::vector<Data> m_Scores;
};

