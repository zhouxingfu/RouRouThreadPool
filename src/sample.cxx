#include "RouRouThreadPool.h"

#include <iostream>

int main(int argc, char* argv[])
{
    RouRouThreadPool pool(8);
    pool.enqueue([](void) {std::cout << "hello" << std::endl;});
    return 0;
}