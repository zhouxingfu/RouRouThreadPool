#include "RouRouThreadPool.h"

#include <iostream>

int main(int argc, char* argv[])
{
    RouRouThreadPool pool(8);
    // thread_pool.enqueue([](int a)->int{std::cout << "test" << std::endl; return 100;}, 100);
    pool.enqueue([](void) {return 100;})
    return 0;
}