#ifndef CALL_AFTER_DELAY_H
#define CALL_AFTER_DELAY_H

#include <functional>
#include <vector>

class CallAfterDelay {

    struct FunctionData {
		bool operator==(const FunctionData& a_Other) const
		{
			return m_Id == a_Other.m_Id;
		}
		int m_Id = 0;
		std::function<void()> m_Function;
		std::chrono::time_point<std::chrono::steady_clock> m_TargetTime;
	};

public:
    static CallAfterDelay& getInstance() {
        static CallAfterDelay instance;
        return instance;
    }

    void AddFunction(const std::function<void()>& a_Function, long a_Delay)
    {
	    FunctionData data;
		data.m_Function = a_Function;
		data.m_TargetTime = std::chrono::steady_clock::now() + std::chrono::seconds(a_Delay);
		int temp = rand();
		for(int i = 0; i < m_Functions.size(); i++)
		{
			if(m_Functions[i].m_Id == temp)
			{
				temp = rand();
				i = 0;
			}
		}
		m_Functions.push_back(data);
    }
    void LoopThroughFunctions()
    {
	    for (auto& function : m_Functions)
	    {
		    if (std::chrono::steady_clock::now() > function.m_TargetTime)
		    {
			    function.m_Function();
				m_Functions.erase(std::remove(m_Functions.begin(), m_Functions.end(), function), m_Functions.end());
		    }
	    }
	
    }

private:
    std::vector<FunctionData> m_Functions;
};

#endif // FUNCTION_CONTAINER_H