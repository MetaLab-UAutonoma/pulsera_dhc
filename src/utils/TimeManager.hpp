#ifndef TIMEMANAGER_HPP
#define TIMEMANAGER_HPP

#include <ctime>

class TimeManager {
public:
    static TimeManager& instance();

    void begin();
    time_t getCurrentTime() const;
    bool isTimeSet() const;

private:
    TimeManager() = default;
    TimeManager(const TimeManager&) = delete;
    TimeManager& operator=(const TimeManager&) = delete;
};

#endif // TIMEMANAGER_HPP
