#include "ThrdMng.h"

namespace ThrdMng {

	// ============================ Event ============================

	Event::Event(const std::string& name, bool o_nt_ALL) : _name(name) {}

	void Event::_SetManual(uint32_t bits) { std::lock_guard g(_mtx); _manualEvents |= bits; }

	void Event::_Signal(uint32_t bits)
	{
		{ std::lock_guard g(_mtx); _calledEvents |= bits; }
		_CV.notify_one();
		//TODO:Loggign
	}

	void Event::_manualReset(uint32_t bits)
	{
		_manualEvents &= ~bits;
	}

	uint32_t Event::Wait(uint32_t mask, std::stop_token _ST)
	{
		std::unique_lock l(_mtx);
		if (!_CV.wait(l, _ST, [&] {return (_calledEvents | _manualEvents) != 0;}))
			return 0;
		uint32_t firedNonManual = _calledEvents & mask;
		_calledEvents &= ~(firedNonManual & ~_manualEvents);
		uint32_t firedManual = _manualEvents;
		return firedManual | firedNonManual;
	}


	// ========================== ThreadMng ==========================

	ThreadMng::ThreadMng(std::string&& _SRCNAME, std::jthread _SRCTHREAD, std::shared_ptr<Event> _SRCEVENT)
	{
		std::lock_guard g(_lock);
		_thrds.emplace(std::move(_SRCNAME), std::make_pair(std::move(_SRCTHREAD), std::move(_SRCEVENT)));
	}

	ThreadMng::ThreadMng(std::string&& _SRCNAME, ThrdEntry&& _PAIR)
	{
		std::lock_guard g(_lock);
		_thrds.emplace(std::move(_SRCNAME), std::move(_PAIR));
	}

	void ThreadMng::Post(std::move_only_function<void()> cmd)
	{
		{ std::lock_guard g(_lock); _cmds.push(std::move(cmd)); }
		_CV.notify_one();
	}

	void ThreadMng::Run(std::stop_token ST)
	{
		while (true)
		{
			std::move_only_function<void()> cmd;
			{
				std::unique_lock _l(_lock);
				if (!_CV.wait(_l, ST, [&] {return !_cmds.empty();}) || ST.stop_requested())
				{
					break;
				}
				cmd = std::move(_cmds.front());
				_cmds.pop();
			}
			cmd();
			//TODO: Logger
		}
		_thrds.clear();
	}

	void ThreadMng::start()
	{
		_worker = std::jthread([this](std::stop_token st) {Run(st); });
	}

	//stop a thread
	void ThreadMng::Stop(std::string name)
	{
		Post([this, name = std::move(name)]()
			{
				_thrds.erase(name);
			});
	}

	void ThreadMng::_DTCH(std::string name)
	{
		Post([this, name = std::move(name)]()
			{
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
			});
	}

	void ThreadMng::ThrowEvent(std::string target, uint32_t mask)
	{
		Post([this, target = std::move(target), mask]
			{
				// manager thread
				auto it = _thrds.find(target);
				if (it != _thrds.end() && it->second.second)
					it->second.second->_Signal(mask);
				//TODO: Logging (not found / no event)
			});
	}

	void ThreadMng::GetEvent(std::string _NAME, std::atomic<bool>& flag,std::shared_ptr<Event>& e_var)
	{
		Post([this, name = std::move(_NAME), &flag ,&e_var]()
			{
				auto it = _thrds.find(name);
				if (it == _thrds.end())
				{
					e_var = nullptr;
					flag = true;
					return;
				}
				//TODO: Logger
				e_var = it->second.second;
				flag = true;
			});
	}

	void ThreadMng::_ResetManual(std::string name, uint32_t bitToReset) // unused!!!
	{
		Post([this, name = name, bitToReset = bitToReset]() 
			{
			auto it = _thrds.find(name);
			if (it == _thrds.end())
				//TODO: Logger
				return;
			if (it->second.second == nullptr)
				return;
			it->second.second->_manualReset(bitToReset);
			});
	}
	
}
