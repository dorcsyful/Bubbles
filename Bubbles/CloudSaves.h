#pragma once
#include <vector>
class CloudSaves
{
public:
	CloudSaves();
	void LoadPersonalTop10();      // call once at startup
	void SubmitScore(int score);         // call on end of run
	void PrintTop10();             // debug

	std::vector<unsigned>& GetAllScores() { return m_TopScores; }
	int GetScoresAtIndex(int index) { return m_TopScores[index]; }
private:
	uint32_t ComputeChecksum(const std::vector<unsigned>& scores);

	void SavePersonalTop10();
	std::vector<unsigned int> m_TopScores;

};

