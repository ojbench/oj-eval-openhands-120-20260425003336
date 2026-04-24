
#pragma once

#include "Task.hpp"
#include <vector>
// 你不可以添加任何头文件

class TaskNode {
    friend class TimingWheel;
    friend class Timer;
public:
    TaskNode(Task* t, int tm) : task(t), time(tm), next(nullptr), prev(nullptr) {}

private:
    Task* task;
    TaskNode* next, *prev;
    int time;
};

class TimingWheel {
    friend class Timer;
public:
    TimingWheel(size_t size, size_t interval) : size(size), interval(interval), current_slot(0) {
        slots = new TaskNode*[size];
        for (size_t i = 0; i < size; ++i) {
            slots[i] = nullptr;
        }
    }
    
    ~TimingWheel() {
        for (size_t i = 0; i < size; ++i) {
            TaskNode* curr = slots[i];
            while (curr) {
                TaskNode* next = curr->next;
                if (next == curr) {
                    delete curr;
                    break;
                }
                curr->prev->next = curr->next;
                curr->next->prev = curr->prev;
                TaskNode* to_delete = curr;
                curr = next;
                delete to_delete;
            }
            slots[i] = nullptr;
        }
        delete[] slots;
    }
    
    void addNode(TaskNode* node) {
        if (!node) return;
        
        size_t target_slot = (current_slot + node->time / interval - 1) % size;
        node->time = node->time % interval;
        
        node->next = node->prev = nullptr;
        
        if (!slots[target_slot]) {
            slots[target_slot] = node;
            node->next = node->prev = node;
        } else {
            TaskNode* head = slots[target_slot];
            TaskNode* tail = head->prev;
            
            tail->next = node;
            node->prev = tail;
            node->next = head;
            head->prev = node;
        }
    }
    
    std::vector<TaskNode*> advance() {
        std::vector<TaskNode*> result;
        TaskNode* head = slots[current_slot];
        
        if (head) {
            slots[current_slot] = nullptr;
            TaskNode* curr = head;
            
            do {
                TaskNode* next = curr->next;
                curr->next = curr->prev = nullptr;
                result.push_back(curr);
                curr = next;
            } while (curr != head);
        }
        
        current_slot = (current_slot + 1) % size;
        return result;
    }
    
private:
    const size_t size, interval;
    size_t current_slot;
    TaskNode** slots;
};

class Timer {
public:    
    Timer(const Timer&) = delete;
    Timer& operator=(const Timer&) = delete;
    Timer(Timer&&) = delete;
    Timer& operator=(Timer&&) = delete;

    Timer() : second_wheel(60, 1), minute_wheel(60, 60), hour_wheel(24, 3600) {}

    ~Timer() {}

    TaskNode* addTask(Task* task) {
        if (!task) return nullptr;
        
        size_t first_interval = task->getFirstInterval();
        TaskNode* node = new TaskNode(task, first_interval);
        
        if (first_interval < 60) {
            second_wheel.addNode(node);
        } else if (first_interval < 3600) {
            minute_wheel.addNode(node);
        } else {
            hour_wheel.addNode(node);
        }
        
        return node;
    }

    void cancelTask(TaskNode *p) {
        if (!p) return;
        
        if (p->prev && p->next) {
            if (p->next != p) {
                p->prev->next = p->next;
                p->next->prev = p->prev;
            }
        }
        
        delete p;
    }

    std::vector<Task*> tick() {
        std::vector<Task*> result;
        
        std::vector<TaskNode*> second_nodes = second_wheel.advance();
        
        for (TaskNode* node : second_nodes) {
            if (node->time == 0) {
                result.push_back(node->task);
                
                size_t period = node->task->getPeriod();
                if (period > 0) {
                    node->time = period;
                    if (period < 60) {
                        second_wheel.addNode(node);
                    } else if (period < 3600) {
                        minute_wheel.addNode(node);
                    } else {
                        hour_wheel.addNode(node);
                    }
                } else {
                    delete node;
                }
            } else {
                if (node->time < 60) {
                    second_wheel.addNode(node);
                } else if (node->time < 3600) {
                    minute_wheel.addNode(node);
                } else {
                    hour_wheel.addNode(node);
                }
            }
        }
        
        if (second_wheel.current_slot == 0) {
            std::vector<TaskNode*> minute_nodes = minute_wheel.advance();
            
            for (TaskNode* node : minute_nodes) {
                if (node->time == 0) {
                    result.push_back(node->task);
                    
                    size_t period = node->task->getPeriod();
                    if (period > 0) {
                        node->time = period;
                        if (period < 60) {
                            second_wheel.addNode(node);
                        } else if (period < 3600) {
                            minute_wheel.addNode(node);
                        } else {
                            hour_wheel.addNode(node);
                        }
                    } else {
                        delete node;
                    }
                } else {
                    if (node->time < 60) {
                        second_wheel.addNode(node);
                    } else if (node->time < 3600) {
                        minute_wheel.addNode(node);
                    } else {
                        hour_wheel.addNode(node);
                    }
                }
            }
            
            if (minute_wheel.current_slot == 0) {
                std::vector<TaskNode*> hour_nodes = hour_wheel.advance();
                
                for (TaskNode* node : hour_nodes) {
                    if (node->time == 0) {
                        result.push_back(node->task);
                        
                        size_t period = node->task->getPeriod();
                        if (period > 0) {
                            node->time = period;
                            if (period < 60) {
                                second_wheel.addNode(node);
                            } else if (period < 3600) {
                                minute_wheel.addNode(node);
                            } else {
                                hour_wheel.addNode(node);
                            }
                        } else {
                            delete node;
                        }
                    } else {
                        if (node->time < 60) {
                            second_wheel.addNode(node);
                        } else if (node->time < 3600) {
                            minute_wheel.addNode(node);
                        } else {
                            hour_wheel.addNode(node);
                        }
                    }
                }
            }
        }
        
        return result;
    }

private:
    TimingWheel second_wheel;
    TimingWheel minute_wheel;
    TimingWheel hour_wheel;
};
