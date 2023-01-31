// Copyright xingfu.zhou@zoho.com 
// Author zhouxingfu
// 


#ifndef ROUROU_THREADPOOL_H__
#define ROUROU_THREADPOOL_H__

#include <thread>
#include <condition_variable>
#include <mutex>
#include <queue>
#include <functional>
#include <future>

class RouRouThreadPool
{
public:
    RouRouThreadPool(const RouRouThreadPool& pool) = delete;
    RouRouThreadPool(RouRouThreadPool&& pool);
    RouRouThreadPool(int thread_num);
    ~RouRouThreadPool();

public:
    template<class F, typename... Args>
    auto enqueue(F&& f, Args&&... args);

    std::queue<std::function<void()>>   tasks_;
    std::mutex                          mutex_;
    std::condition_variable             cv_;
    bool                                bStop_;
private:

    std::vector<std::thread>            threads_;
};


#endif