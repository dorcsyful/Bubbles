#ifndef CALL_AFTER_DELAY_H
#define CALL_AFTER_DELAY_H

#include <functional>
#include <utility>
#include <vector>
#include <chrono>
#include <iostream>
class CallAfterDelay {

    struct FunctionData {
		bool operator==(const FunctionData& a_Other) const
		{
			return m_Name == a_Other.m_Name;
		}
		std::string m_Name;
		bool m_IsRepeating = false;
		double m_Delay = 0;
		std::function<void()> m_Function;
		std::chrono::time_point<std::chrono::steady_clock> m_TargetTime;
	};

public:
    static CallAfterDelay& getInstance() {
        static CallAfterDelay instance;
        return instance;
    }

	void ClearQueue()
    {
		m_Functions.clear();
    }

    void AddFunction(const std::function<void()>& a_Function, std::string a_RefName, float a_Delay, bool a_Repeating)
    {
	    FunctionData data;
		data.m_IsRepeating = a_Repeating;
		data.m_Name = std::move(a_RefName);
	    data.m_Function = a_Function;
		data.m_TargetTime = std::chrono::steady_clock::now() + std::chrono::milliseconds((long)(a_Delay * 1000.f));
		data.m_Delay = a_Delay;
		int temp = rand();
		for(int i = 0; i < m_Functions.size(); i++)
		{
			if(m_Functions[i].m_Name == a_RefName)
			{
				throw new std::exception("Function with this already exists!");
			}
		}
		m_Functions.push_back(data);
    }

	void RemoveFunction(const std::string& a_Name)
	{
	    for (auto& function : m_Functions)
	    {
			if (function.m_Name == a_Name)
		    {
			    std::erase(m_Functions, function);
		    }
	    }
	}

    void LoopThroughFunctions()
    {
	    for (int i = static_cast<int>(m_Functions.size()) - 1; i >= 0; i--)
	    {
		    if (std::chrono::steady_clock::now() > m_Functions[i].m_TargetTime)
		    {
				if(m_Functions[i].m_Name.empty()) continue;
				m_Functions[i].m_Function();
				if (!m_Functions[i].m_IsRepeating) {
					m_Functions.erase(std::find(m_Functions.begin(),m_Functions.end(),m_Functions[i]));
				}
				else
				{
					m_Functions[i].m_TargetTime = std::chrono::steady_clock::now() + std::chrono::milliseconds((long)(m_Functions[i].m_Delay * 1000.f));
				}
		    }
	    }
    }

private:

    std::vector<FunctionData> m_Functions;
};

#endif // FUNCTION_CONTAINER_H