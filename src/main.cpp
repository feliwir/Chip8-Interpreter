#include <SFML/Window.hpp>
#include <SFML/Graphics.hpp>
#include <iostream>
#include "Chip8.hpp"

int main(int argc,char* argv[])
{
	const std::string romname = "PONG2";
	
	Chip8 emu;
	if (!emu.LoadROM(romname))
	{
		std::cout << "Failed to load ROM named: " << romname << std::endl;
		return -1;
	}
	else
		std::cout << "Loaded ROM: " << romname << std::endl; 
		

	sf::RenderWindow win(sf::VideoMode(640, 320), "Chip-8", sf::Style::Default);
	sf::Texture tex;
	tex.create(64, 32);
	win.setFramerateLimit(60);
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
				emu.HandleKey(event.key);
				break;
			default:
				break;
			}
		}

		emu.EmulateCycle();
		
		if(emu.Draw())
		{
			win.clear();
			sf::Image img;
			img.create(64,32);
			
			for(int y = 0; y < 32; ++y)		
				for(int x = 0; x < 64; ++x)
					if(emu.GetScreen()[(y * 64) + x] == 0)
						img.setPixel(x,y,sf::Color(0,0,0));
					else 
						img.setPixel(x,y,sf::Color(255,255,255));
						
			tex.update(img);
			sf::Sprite spr(tex);
			spr.scale(10.0f,10.0f);
			win.draw(spr);
			win.display();
		}
		
	}
	
	return 0;
}
