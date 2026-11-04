#pragma once
#include "ThrdMng.h"


class ThredMng 
{
private:
	std::mutex m_lock;
	std::unordered_map<std::string, std::jthread> m_thrds;

public:
	~ThredMng() = default;
	template<typename F>
	void Start(const std::string& name, F&& fn)
	{
		std::lock_guard g(m_lock);
		m_thrds.emplace(name, std::forward(std::jthread(fn));
	}
	void Stop(std::string name)
	{
		std::lock_guard g(m_lock);
		m_thrds.erase(name);
	}
	// TODO: write an event handler class 
	bool _PingThrd()
	{

	}
};