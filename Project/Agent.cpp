#include "Global Header.h"

class Agent
{
private:
	uint8_t m_Flags;

	void Initialization()
	{
		
	}
	void NetWorkInIt() 
	{

	}
	void Destructor() 
	{

	}
public:
	Agent() { Initialization(); }
	~Agent() { Destructor(); }
	Agent(Agent &o_Agent) {  }
	Agent(Agent&& o_Agent) noexcept{ }
	Agent& operator=(Agent&& other) noexcept 
	{ 
		if (this != &other) 
		{

		}
		return *this;
	}

};