#pragma once
// NOTE: don't include "Global Header.h" here - it already includes this file,
// so including it back creates a circular include. Pull in what we need directly.
#include <cstdint>
#include <string>
#include <memory>
#include <utility>
#include <type_traits>
#include <functional>   // std::invoke, std::move_only_function (C++23)
#include <thread>       // std::jthread
#include <stop_token>
#include <mutex>
#include <condition_variable>
#include <queue>
#include <vector>
#include <unordered_map>

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
		explicit Event(const std::string& name, bool o_nt_ALL);

		void _SetManual(uint32_t bits);
		void _Signal(uint32_t bits);
		void _manualReset(uint32_t bits);

		uint32_t Wait(uint32_t mask, std::stop_token _ST);
	};


	class ThreadMng
	{
	private:
		using ThrdEntry = std::pair<std::jthread, std::shared_ptr<Event>>;

		std::mutex _lock;
		std::vector<std::string> c_EVNTName;
		std::unordered_map<std::string, ThrdEntry> _thrds;

		std::condition_variable_any _CV;
		std::queue<std::move_only_function<void()>> _cmds;

		// declared LAST so it's destroyed FIRST (stop + join the manager thread
		// before the queue / thread map it uses get destroyed)
		std::jthread _worker;

		void Post(std::move_only_function<void()> cmd);
		void Run(std::stop_token ST);

	public:
		//two constucrts cuz we have a piar so in one raw data not in pair and in other alrady alloceted data
		ThreadMng(std::string&& _SRCNAME, std::jthread _SRCTHREAD, std::shared_ptr<Event> _SRCEVENT);
		ThreadMng(std::string&& _SRCNAME, ThrdEntry&& _PAIR);

		//destructor
		~ThreadMng() = default;
		//no copying!!!!!
		ThreadMng(const ThreadMng& o) = delete;
		ThreadMng& operator=(const ThreadMng& o) = delete;
		//no moveeeeeee))))
		ThreadMng(ThreadMng&& o) noexcept = delete;
		ThreadMng& operator=(ThreadMng&& o) noexcept = delete;

		// templates MUST stay in the header (the compiler needs the body where they're used)
		template<typename F, typename... Args>
		void Submit(std::string name, F&& fn, Args&&... args)
		{
			SubmitEvent(std::move(name), nullptr, std::forward<F>(fn), std::forward<Args>(args)...);
		}

		template<typename F, typename... Args>
		void SubmitEvent(std::string name, std::shared_ptr<Event> evnt, F&& fn, Args&&... args)
		{
			auto task = [f = std::forward<F>(fn), ...a = std::forward<Args>(args)](std::stop_token st) mutable
				{
					// if the function wants a stop_token as its first arg, hand it one
					if constexpr (std::is_invocable_v<std::decay_t<F>&, std::stop_token, std::decay_t<Args>&...>)
						std::invoke(f, st, a...);
					else
						std::invoke(f, a...);
				};

			Post([this, name = std::move(name), evnt = std::move(evnt), task = std::move(task)]() mutable
				{
					_thrds.emplace(std::move(name), ThrdEntry(std::jthread(std::move(task)), std::move(evnt)));
					//TODO: Logging
				});
		}

		void start();

		//stop a thread
		void Stop(std::string name);
		void _DTCH(std::string name);

		//TODO: write the CV's, event logic and write down the event and thread logic in the README.md
		void ThrowEvent(std::string target, uint32_t mask);

		std::shared_ptr<Event> GetEvent(const std::string& _NAME);
	};
}
