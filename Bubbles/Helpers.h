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

    void AddFunction(const std::function<void()>& a_Function, std::string a_RefName, float a_Delay, bool a_Repeating)
    {
	    FunctionData data;
		data.m_IsRepeating = a_Repeating;
		data.m_Name = std::move(a_RefName);
	    data.m_Function = a_Function;
		data.m_TargetTime = std::chrono::steady_clock::now() + std::chrono::milliseconds((long)(a_Delay * 1000.f));
		data.m_Delay = a_Delay;
		int temp = rand();
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
		std::vector<FunctionData> markedForDelete;
	    for (auto& function : m_Functions)
	    {
		    if (std::chrono::steady_clock::now() > function.m_TargetTime)
		    {
				if(function.m_Name.empty()) continue;
			    function.m_Function();
				if (!function.m_IsRepeating) {
					markedForDelete.push_back(function);
				}
				else
				{
					function.m_TargetTime = std::chrono::steady_clock::now() + std::chrono::milliseconds((long)(function.m_Delay * 1000.f));
				}
		    }
	    }
		for (auto& function : markedForDelete)
		{
			auto functionData = std::ranges::find(m_Functions, function);
			m_Functions.erase(functionData);
		}
	
    }

private:

    std::vector<FunctionData> m_Functions;
};

#endif // FUNCTION_CONTAINER_H