
#include <exception>
#include <iostream>
#include "platform.h"

using namespace MusicBox;

int main(int argc, char *argv[])
{

    std::cout << "Welcome." << std::endl;

    try {

        Platform platform;
        platform.run();

    } catch (std::exception &e) {
        std::cout << e.what() << std::endl ;
        return 100;
    }

    std::cout << "Bye." << std::endl;

    return 0;

}
