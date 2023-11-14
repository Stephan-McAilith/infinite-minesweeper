#include "Game.hpp"

int main(int ac, char** av)
{
    std::cout << "Start aled" << std::endl;
    Game game;

    game.start(ac == 2 ? av[1] : "ws://localhost:9002");

    return 0;
}