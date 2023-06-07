#include "../sylar/uri.h"
#include <iostream>

int main(int argc, char** argv) {
    sylar::Uri::ptr uri = sylar::Uri::Create("http://www.sylar.top:80/blog");
    std::cout << uri->toString() << std::endl;
    std::cout << uri->getFragment() << std::endl;
    auto addr = uri->createAddress();
    std::cout << *addr << std::endl;
    return 0;
}