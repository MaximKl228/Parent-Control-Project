#include "Global Header.h"

//dont even fcking try to change that line!!!!!!!!!!!!!!
static std::string MY_LOVE = "TALLY";

class Agent
{
private:
	uint8_t m_Flags;
	

	//All basic the Initializers TODO: the initializers
	void _ThrdMngInit()
	{

	}
	void _NetInit()
	{

	}
	void _LogInit()
	{

	}
	//Camera init and check
	bool _CameraInit()
	{

	}
	//microphone init and check
	bool _MicInit()
	{

	}
public:
	Agent() { }
	~Agent() = default;
	Agent(Agent& o_Agent) = delete;
	Agent(Agent&& o_Agent) noexcept = delete;
	Agent& operator=(Agent&& other) noexcept = delete;

};