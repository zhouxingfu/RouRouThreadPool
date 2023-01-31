#include "RouRouThreadPool.h"

using namespace std;


RouRouThreadPool::RouRouThreadPool(int thread_num)
{
    for(int i=0; i< thread_num; ++i)
    {
        threads_.push_back(thread([this](){
            while(true)
            {
                unique_lock<mutex> lck(this->mutex_);
                this->cv_.wait(lck, [this](){return this->bStop_ || !tasks_.empty();});//如果运行到此代码时，直接能获取到锁，同时满足条件，那么不需要notify触发，也会往下执行；如果运行到此代码时，无法获取到锁，或者能获取到锁但不满足条件，此时wait阻塞，此时必须被notify触发才能往下执行。notify的作用是触发已阻塞的wait。
                if(this->bStop_ && tasks_.empty())
                    return;
  
                auto task = std::move(tasks_.front());
                task();
                tasks_.pop();
            
            }
        }));
    }
}


template<class F, class... Args>
auto RouRouThreadPool::enqueue(F&& f, Args&&... args)
{
    auto R = using std::result_of<F(Args...)>::type;
    auto f = std::make_shared<std::packaged_task<R()>>(std::bind(std::forward<F>(f), std::forward<Args>(args)...)));
    std::future<R> res = f.get_future();
    {
        lock_guard<mutex> lck(mutex_);
        tasks_.push([f]()->void{(*f)();});
    }
    cv_.notify_one();
    return res;
}

RouRouThreadPool::~RouRouThreadPool()
{
    {
        std::lock_guard<mutex> lck(mutex_);
        bStop_ = true;
    }

    for(auto& t : threads_)
    {
        t.join();
    }
}