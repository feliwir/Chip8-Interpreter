#include <SFML/Window.hpp>
#include <SFML/Graphics.hpp>
#include <iostream>
#include "Chip8.hpp"

int main(int argc,char* argv[])
{
	const std::string romname = "PONG2";
	
	sf::Window win(sf::VideoMode(640, 320, 8), "Chip-8", sf::Style::Default, sf::ContextSettings(0, 0, 0, 3, 0));

	Chip8 emu;
	if (!emu.LoadROM(romname))
	{
		std::cout << "Failed to load ROM named: " << romname << std::endl;
		return -1;
	}
	else
		std::cout << "Loaded ROM: " << romname << std::endl; 
		


	sf::Texture tex;
	tex.create(64, 32);
	while(win.isOpen())
	{
		
		sf::Event event;
	
		while (win.pollEvent(event))
		{
			switch (event.type)
			{
			case sf::Event::Closed:
				win.close();
				break;
			case sf::Event::KeyPressed:
				emu.HandleKey(event.key,true);
				break;
			case sf::Event::KeyReleased:
				emu.HandleKey(event.key,false);
				break;
			default:
				break;
			}
		}	
		
		emu.EmulateCycle();		
		win.display();
	}
	
	return 0;
}
