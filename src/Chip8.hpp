#pragma once
#include <stdint.h>
#include <string>
#include <SFML/Window.hpp>
#include <SFML/Audio.hpp>

class Chip8
{
	public:
		Chip8();
		void EmulateCycle();
		bool LoadROM(const std::string& name);
		bool Draw();
		void HandleKey(sf::Event::KeyEvent& key);

		inline uint8_t* GetScreen()
		{
				return m_gfx;
		}
	
	private:
		uint16_t m_opcode;
		uint8_t m_memory[4096];
		uint8_t m_reg[16];
		uint16_t m_pc;
		uint16_t m_index;
		uint8_t	m_gfx[64*32];
		uint8_t m_delayTimer;
		uint8_t m_soundTimer;
		uint16_t m_stack[16];
		uint16_t m_sp;
		uint8_t m_key[16];

		sf::SoundBuffer m_soundwave;
		sf::Sound m_beep;
		bool m_drawFlag;
};
