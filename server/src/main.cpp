#include "Game.hpp"
#include <iostream>

int main(int ac, char** av)
{
    std::cout << "Start client" << std::endl;
    Game game;
    if (ac == 2)
        game.load(av[1]);
    game.start();
    return 0;
}