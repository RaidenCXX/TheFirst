#include "GameEngine.h"



int main()
{
	GameEngine TheFirst("resource/config.txt");
	TheFirst.run();
  
/*
  sf::RenderWindow Window(sf::VideoMode(sf::Vector2u(100, 100)), "aa");
  Window.setFramerateLimit(10);
  while (Window.isOpen())
  {
    Window.clear();
     
    Window.display();
  }
	*/
  return 0;
}
