#include <chrono>
#include <iostream>

class Timer
{
private:
    std::chrono::time_point<std::chrono::high_resolution_clock> d_start;

public:
    //构造函数构造一个启动计时器
    Timer()
    {
        d_start = std::chrono::high_resolution_clock::now();
    }

    ~Timer()
    {
        stop();
    }

    void stop()
    {
        auto d_end = std::chrono::high_resolution_clock::now();
        auto _start = std::chrono::time_point_cast<std::chrono::microseconds>(d_start)
            .time_since_epoch()
            .count();
        auto _end = std::chrono::time_point_cast<std::chrono::microseconds>(d_end)
            .time_since_epoch()
            .count();
        auto duration = _end - _start;
        double ms = duration * 0.001;

        std::cout << "耗时:" << duration << "us /  " << ms << "ms\n";
    }
};