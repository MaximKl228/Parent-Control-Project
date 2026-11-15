#pragma once
#include "ThrdMng.h"

class Event
{
private:
	bool _signaled = false;
	bool _autoReset;
	std::string _name;
	std::mutex _mtx;
	std::condition_variable_any _CV;


public:
	explicit Event(bool o_autoReset = true, const std::string& name) : _autoReset(o_autoReset), _name(name) {}

	void _StartEVNT()
	{
		{std::lock_guard g(_mtx); _signaled = true;}
		_autoReset ? _CV.notify_one() : _CV.notify_all();
		//TODO:Loggign
	}
	void _Reset()
	{
		std::lock_guard g(_mtx);
		_signaled = false;
	}

	bool Wait(std::stop_token _ST)
	{
		std::unique_lock l(_mtx);
		if (!_CV.wait(l, _ST, [&] {return _signaled;}))
			return false;
		if (_autoReset) { _signaled = false; }
		return true;
	}
};

class ThreadMng 
{
private:
	std::mutex _lock;
	std::vector<std::string> c_EVNTName;
	std::unordered_map<std::string, std::pair<std::jthread,std::shared_ptr<Event>>> _thrds;
public:
	//two constucrts cuz we have a piar so in one raw data not in pair and in other alrady alloceted data
	ThreadMng(std::string&& _SRCNAME, std::jthread _SRCTHREAD, std::shared_ptr<Event> _SRCEVENT)
	{

		std::lock_guard g(_lock);
		_thrds.emplace(std::move(_SRCNAME), std::make_pair(std::move(_SRCTHREAD), std::move(_SRCEVENT)));

	}
	ThreadMng(std::string&& _SRCNAME, std::pair<std::jthread, std::shared_ptr<Event>>&& _PAIR)
	{
		std::lock_guard g(_lock);
		_thrds.emplace(_SRCNAME, _PAIR);
	}
	//destructor
	~ThreadMng() = default;
	//no copying!!!!!
	ThreadMng(const ThreadMng& o) = delete;
	//no moveeeeeee))))
	ThreadMng(ThreadMng&& o) noexcept = delete;
	ThreadMng& operator=(ThreadMng&& o)noexcept = delete;
	
	//start a new thread
	template<typename F>
	void Start(const std::string& name, F&& fn)
	{
		std::lock_guard g(_lock);
		std::pair<std::jthread, std::shared_ptr> t_pair;
		t_pair.first = std::move(std::jthread(fn));
		t_pair.second = nullptr;
		_thrds.emplace(name, t_pair);
		//TODO:Logging
		
	}
	//start a thread with an event in it
	template<typename F>
	void Start(const std::string& name, F&& fn, std::shared_ptr<Event> _EVNT)
	{
		std::lock_guard g(_lock);
		_thrds.emplace(name, std::make_pair<std::jthread, std::shared_ptr<Event>>(std::jthread(fn), _EVNT);
		//TODO:Logging
	}
																																																																																																																																																																																																																																					
	//stop a thread
	void Stop(std::string name)
	{
		std::lock_guard g(_lock);
		_thrds.erase(name);
		//TODO:Logging
	}
	
	bool _DTCH(const std::string& name)
	{
		std::lock_guard g(_lock);
		auto it = _thrds.find(name);
		if (it != _thrds.end())
		{
			auto w_thrd = std::move(it->second.first);
			w_thrd.detach();
			return true;
			//TODO:Logging
		}
		else
		{
			//TODO: Log here an exeption

			return false;
		}
	}

	//TODO: write the CV's, event logic and write down the event and thread logic in the README.md
	void _ThorwEvent(std::string _SRCNAME, std::string EVENTNAME)
	{
		std::lock_guard g(_lock);
		auto it = _thrds.find(EVENTNAME);
		if (it != _thrds.end())
		{
			it->second.second->_StartEVNT();
			//TODO:Logging

		}
		else
		{
			//TODO: Logging
		}
	}
	std::shared_ptr<Event> GetEvent(std::string&& _NAME)
	{

		std::lock_guard g(_lock);
		return std::make_shared<Event>(_thrds.find(_NAME)->second.second);
	}
	std::shared_ptr<Event> GetEvent(const std::string& _NAME)
	{

		std::lock_guard g(_lock);
		return std::make_shared<Event>(_thrds.find(_NAME)->second.second);
	}
	bool _AddEvent(std::string _TOADDNAME, Event _EVENT)
	{
		std::lock_guard g(_lock);
		auto it = _thrds.find(_TOADDNAME);
		if (it != _thrds.end())
		{
			if (it->second.second == nullptr)
			{
				it->second.second = std::make_shared<Event>(_EVENT);
				//TODO:Logging
				return true;
			}
			else
			{
				//TODO:Logging
				return false;
			}
		}
	}
	
};