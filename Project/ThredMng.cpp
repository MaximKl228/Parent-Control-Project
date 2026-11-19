#pragma once
#include "ThrdMng.h"

namespace ThrdMng {

	enum BitMask : uint32_t
	{
		//TODO: Write down all the events
		_FIRED = 1u << 0

	};


	class Event
	{
	private:
		std::string _name;
		std::mutex _mtx;
		std::condition_variable_any _CV;

		uint32_t _calledEvents = 0;
		uint32_t _manualEvents = 0;


	public:
		explicit Event(const std::string& name, bool o_nt_ALL) : _name(name) {}

		void _SetManual(uint32_t bits) { std::lock_guard g(_mtx); _manualEvents |= bits; }

		void _Signal(uint32_t bits)
		{
			{ std::lock_guard g(_mtx); _calledEvents |= bits; }
			_CV.notify_one();
			//TODO:Loggign
		}
		void _manualReset(uint32_t bits)
		{
			std::lock_guard g(_mtx);
			_manualEvents &= ~bits;
		}

		uint32_t Wait(uint32_t mask, std::stop_token _ST)
		{
			std::unique_lock l(_mtx);
			if (!_CV.wait(l, _ST, [&] {return (_calledEvents & _manualEvents) != 0;}))
				return false;
			uint32_t fired = _calledEvents & mask;
			_calledEvents &= ~(fired & ~_manualEvents);
			return true;
		}
	};

	class ThreadMng
	{
	private:
		std::mutex _lock;
		std::vector<std::string> c_EVNTName;
		std::unordered_map<std::string, std::pair<std::jthread, std::shared_ptr<Event>>> _thrds;

		std::condition_variable_any _CV;
		std::queue<std::move_only_function<void()>> _cmds;

		std::jthread _worker;

		void Post(std::move_only_function<void()> cmd)
		{
			{ std::lock_guard g(_lock); _cmds.push(std::move(cmd)); }
			_CV.notify_one();
		}

		void Run(std::stop_token ST)
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

		template<typename F, typename... Args>
		void Submit(std::string name, F&& fn, Args... args)
		{
			SubmitEvent(std::move(name), nullptr, std::forward<F>(fn), std::forward<Args>(args)...);
		}

		template<typename F, typename... Args>
		void SubmitEvent(std::string name, std::shared_ptr<Event> evnt, F&& fn, Args... args)
		{
			auto task = [f = std::forward<F> fn, ...a = std::forward<Args>(args)](std::stop_token st) mutable
				{
					if constexpr (std::is_invocable_v<std::decay_t<F>>&, std::stop_token, std::decay<Args>&...)
						std::invoke(fn, st, a...);
					else
						std::invoke(fn, a...);

				}
				Post([this, name = std::move(name), event = std::move(event), task = std::move(task)] mutable
					{
						_thrds.emplace(std::move(name), std::make_pair(std::jthread(std::move(task)), std::move(event)));
						//TODO: Logging
				});
		}

		void start()
		{
			_worker = std::jthread([this](std::stop_token st) {Run(st); });
		}


		//stop a thread
		void Stop(std::string name)
		{
			Post([this, name = std::move(name)]()
				{
					_thrds.erase(name);
				});
		}

		void _DTCH(std::string name)
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

		//TODO: write the CV's, event logic and write down the event and thread logic in the README.md
		void ThrowEvent(std::string target, uint32_t mask)
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
		

	};
}