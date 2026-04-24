#pragma once

#include "Task.hpp"
#include <vector>
// 你不可以添加任何头文件

class TaskNode {
    friend class TimingWheel;
    friend class Timer;
public:
    // 你需要完成这个类，并且可以添加任意成员变量或者函数（STL容器除外）
    // TODO

private:
    Task* task; // 需要触发的任务，你无需关心其内存管理
    TaskNode* next, *prev; // 双向链表，你可以按你自己的想法修改
    int time; // 任务距离触发的时间，可以参考示例中的伪代码
};

class TimingWheel {
    friend class Timer;
public:
    // 你需要完成这个类，并且可以添加任意成员变量或者函数（STL容器除外）
    // TODO

    TimingWheel(size_t size, size_t interval);
    ~TimingWheel();
    
private:
    const size_t size, interval; // 时间轮的大小，以及每个槽位代表的时间间隔
    size_t current_slot = 0; // 当前指向的槽位
};

class Timer {
public:    
    // 不需要考虑拷贝构造和赋值运算符
    Timer(const Timer&) = delete;
    Timer& operator=(const Timer&) = delete;
    Timer(Timer&&) = delete;
    Timer& operator=(Timer&&) = delete;

    // 你需要完成下面的函数
    // 你需要在里面添加秒，分，时三个时间轮
    // 你可以添加任意成员变量或者函数（STL容器除外）
    // TODO

    Timer();

    ~Timer();

    // 添加一个任务，并且返回指向该任务对应的 TaskNode 的指针。    
    TaskNode* addTask(Task* task);

    // 通过指向 TaskNode 的指针取消一个任务，将其从时间轮上移除。
    void cancelTask(TaskNode *p);

    // 最低时间轮前进一格，返回在此时刻需要执行的任务列表，无需关心列表中的任务顺序。需要注意时间轮的进位。
    std::vector<Task*> tick();

private:

};
