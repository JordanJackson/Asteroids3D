#include "Game.h"

int main()
{
	glsh::InitRandom();
    Game game;

    glsh::System::Run(game, "Assteroids", 800, 600);
}
