#pragma once
#include "Logger.h"
#include <iostream>
#include <mutex>

void Logger::Write(LogLvl level, std::string_view message) {
    static std::mutex mutex;
    std::lock_guard lock(mutex);
    std::cout << message << '\n'; // add some more info bitch
}
