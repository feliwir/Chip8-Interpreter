#pragma once
#include <stdint.h>
#include <string>

class Chip8
{
	public:
	Chip8();
	void EmulateCycle();
	bool LoadROM(const std::string& name);
	bool Draw();
	inline uint8_t* GetScreen()
	{
			return m_gfx;
	}
	
	private:
	uint16_t m_opcode;
	//4k memory
	uint8_t m_memory[4096];
	//15 registers + carry flag
	uint8_t m_reg[16];
	//program counter
	uint16_t m_pc;
	//index
	uint16_t m_index;
	//screen
	uint8_t	m_gfx[64*32];
	//timer
	uint8_t m_delayTimer;
	uint8_t m_soundTimer;
	//stack
	uint16_t m_stack[16];
	//stack pointer
	uint16_t m_sp;
	//key
	uint8_t m_key[16];
	
	//gfx states
	bool m_drawFlag;
};
