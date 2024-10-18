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

    void AddFunction(const std::function<void()>& a_Function, double a_Delay, bool a_Repeating)
    {
	    FunctionData data;
		data.m_IsRepeating = a_Repeating;
		data.m_Function = a_Function;
		data.m_TargetTime = std::chrono::steady_clock::now() + std::chrono::milliseconds((long)(a_Delay * 1000.f));
		data.m_Delay = a_Delay;
		int temp = rand();
		for(size_t i = 0; i < m_Functions.size(); i++)
		{
			if(m_Functions[i].m_Id == temp)
			{
				temp = rand();
				i = 0;
			}
		}
		m_Functions.push_back(data);
    }

	void RemoveFunction(int a_Id)
    {
	    for (auto& function : m_Functions)
	    {
		    if (function.m_Id == a_Id)
		    {
			    m_Functions.erase(std::remove(m_Functions.begin(), m_Functions.end(), function), m_Functions.end());
		    }
	    }
    }

	void RemoveFunction(const std::function<void()>& a_Function)
	{
	    for (auto& function : m_Functions)
	    {
		    if ((function.m_Function).target<void()>() == (a_Function).target<void()>())
		    {
			    m_Functions.erase(std::remove(m_Functions.begin(), m_Functions.end(), function), m_Functions.end());
		    }
	    }
	}

    void LoopThroughFunctions()
    {
	    for (auto& function : m_Functions)
	    {
		    if (std::chrono::steady_clock::now() > function.m_TargetTime)
		    {
			    function.m_Function();
				if(!function.m_IsRepeating) m_Functions.erase(std::remove(m_Functions.begin(), m_Functions.end(), function), m_Functions.end());
				else
				{
					function.m_TargetTime = std::chrono::steady_clock::now() + std::chrono::milliseconds((long)(function.m_Delay * 1000.f));
				}
		    }
	    }
	
    }

private:

    std::vector<FunctionData> m_Functions;
};

#endif // FUNCTION_CONTAINER_H