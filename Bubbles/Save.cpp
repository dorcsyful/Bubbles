#include "Save.h"

#include <fstream>

#include "FilePaths.h"

Save::Save()
{
    if (!DoesSaveFileExist())
    {
        CreateSaveFile();
    }
    m_Scores = LoadData();
    std::ranges::sort(m_Scores, GreaterThan);
}

bool Save::SaveIfHighScore(unsigned a_Score)
{
    if (a_Score >= m_Scores[29].m_Score)
    {
        m_Scores[29].m_Score = a_Score;
        const auto now = std::chrono::system_clock::now();
        m_Scores[29].m_Date = std::format("{:%d-%m-%Y}", now);
        std::ranges::sort(m_Scores, GreaterThan);
        SaveData(m_Scores);

        return true;
    }

    return false;
}

void Save::UpdateSettings(bool a_Fullscreen)
{
    std::vector<std::string> lines;
    std::ifstream infile("Assets/Settings.save");
    std::string line;
    int i;
    int counter = 0;
    while (getline(infile, line)) {
        if (line.starts_with("FULLSCREEN|"))
        {
            i = counter;
        }
        lines.push_back(line);
        counter++;
    }
    infile.close();

    // Modify the last line
    std::string asString = a_Fullscreen ? "YES" : "NO";
    lines[i] = "FULLSCREEN|" + asString;

    std::ofstream outfile("Assets/Settings.save");
    for (const std::string& nline : lines) {
        outfile << nline << '\n';
    }
    outfile.close();
}

uint64_t Save::CalculateHash(const Data& a_Data)
{
    std::hash<unsigned int> intHash;
    std::hash<std::string> stringHash;
    uint64_t hashValue = intHash(a_Data.m_Score) ^ stringHash(a_Data.m_Date);
    return hashValue;
}

void Save::SaveData(const std::vector<Data>& a_Data)
{
    std::ofstream file(HIGH_SCORE_DATA_FILENAME);

    if (!file.is_open()) {
        return;
    }

    for (const Data& item : a_Data) {
        uint64_t hash = CalculateHash(item);
        file << hash << " " << item.m_Score << " " << item.m_Date << "\n";
    }

    file.close();
}

std::vector<Save::Data> Save::LoadData()
{
    std::vector<Data> data;
    std::ifstream file(HIGH_SCORE_DATA_FILENAME);

    if (!file.is_open()) {
        return data;
    }

    std::string line;
    while (std::getline(file, line)) {
        std::istringstream iss(line);
        uint64_t hash;
        unsigned int value;
        std::string date;

        iss >> hash >> value >> date;

        uint64_t calculateHash = CalculateHash(Data(value, date));
        // Verify hash
        if (calculateHash == hash) {
            data.emplace_back(value, date);
        }
    }

    file.close();

    if(data.size() < 30)
    {
	    for(size_t i = data.size(); i < 30; i++)
	    {
            data.emplace_back(0);
	    }
        SaveData(data);
    }
    return data;
}

bool Save::DoesSaveFileExist()
{
    std::filesystem::path path(HIGH_SCORE_DATA_FILENAME);
    std::filesystem::file_status status = std::filesystem::file_status{};
    return (std::filesystem::status_known(status) ? std::filesystem::exists(status) : std::filesystem::exists(path));
}

void Save::CreateSaveFile()
{
    std::vector<Data> emptyData;
    emptyData.reserve(30);
    for (int i = 0; i < 30; i++)
    {
        emptyData.emplace_back(0);
    }
    SaveData(emptyData);
}