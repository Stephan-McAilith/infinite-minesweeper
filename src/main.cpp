#include "Game.hpp"
#include <iostream>

int main(int ac, char** av)
{
    Game game;
    if (ac == 2)
        game.load(av[1]);
    game.start();
    return 0;
}