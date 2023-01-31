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
    RouRouThreadPool(int thread_num)
    {
        for(int i=0; i< thread_num; ++i)
        {
            threads_.push_back(std::thread([this](){
                while(true)
                {
                    std::function<void()> task;
                    {
                        std::unique_lock<std::mutex> lck(this->mutex_);
                        this->cv_.wait(lck, [this](){return this->bStop_ || !tasks_.empty();});//如果运行到此代码时，直接能获取到锁，同时满足条件，那么不需要notify触发，也会往下执行；如果运行到此代码时，无法获取到锁，或者能获取到锁但不满足条件，此时wait阻塞，此时必须被notify触发才能往下执行。notify的作用是触发已阻塞的wait。
                        if(this->bStop_ && tasks_.empty())
                            return;
        
                        task = std::move(tasks_.front());
                        tasks_.pop();
                    }
                    task();
                }
            }));
        }
    }
    ~RouRouThreadPool()
    {
        {
            std::lock_guard<std::mutex> lck(mutex_);
            bStop_ = true;
        }
        cv_.notify_all();
        for(auto& t : threads_)
        {
            t.join();
        }
    }

public:
    template<class F, typename... Args>
    auto enqueue(F&& f, Args&&... args)
    {
        using R = typename std::result_of<F(Args...)>::type;
        auto task = std::make_shared<std::packaged_task<R()>>(std::bind(std::forward<F>(f), std::forward<Args>(args)...));
        std::future<R> res = task->get_future();
        {
            std::lock_guard<std::mutex> lck(mutex_);
            tasks_.push([task]()->void{(*task)();});
        }
        cv_.notify_one();
        return res;
    }

    std::queue<std::function<void()>>   tasks_;
    std::mutex                          mutex_;
    std::condition_variable             cv_;
    bool                                bStop_;
private:

    std::vector<std::thread>            threads_;
};


#endif