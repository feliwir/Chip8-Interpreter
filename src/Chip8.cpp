#include "Chip8.hpp"
#include <algorithm>
#include <fstream>
#include <cstring>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>


Chip8::Chip8() : m_opcode(0),m_pc(0x200),m_index(0),m_sp(0)
{
	const uint8_t fontset[80] =
	{ 
	  0xF0, 0x90, 0x90, 0x90, 0xF0, // 0
	  0x20, 0x60, 0x20, 0x20, 0x70, // 1
	  0xF0, 0x10, 0xF0, 0x80, 0xF0, // 2
	  0xF0, 0x10, 0xF0, 0x10, 0xF0, // 3
	  0x90, 0x90, 0xF0, 0x10, 0x10, // 4
	  0xF0, 0x80, 0xF0, 0x10, 0xF0, // 5
	  0xF0, 0x80, 0xF0, 0x90, 0xF0, // 6
	  0xF0, 0x10, 0x20, 0x40, 0x40, // 7
	  0xF0, 0x90, 0xF0, 0x90, 0xF0, // 8
	  0xF0, 0x90, 0xF0, 0x10, 0xF0, // 9
	  0xF0, 0x90, 0xF0, 0x90, 0x90, // A
	  0xE0, 0x90, 0xE0, 0x90, 0xE0, // B
	  0xF0, 0x80, 0x80, 0x80, 0xF0, // C
	  0xE0, 0x90, 0x90, 0x90, 0xE0, // D
	  0xF0, 0x80, 0xF0, 0x80, 0xF0, // E
	  0xF0, 0x80, 0xF0, 0x80, 0x80  // F
	};
	
	std::copy(fontset,fontset+80,&m_memory[0]);
	srand (time(NULL));
}


bool Chip8::LoadROM(const std::string& name)
{
	std::ifstream fin(name,std::ios::binary);
	if(fin.fail())
		return false;
	
	//get filesize	
	fin.seekg(0,std::ios::end);
	auto romsize = fin.tellg();
	fin.seekg(0,std::ios::beg);
	
	fin.read((char*)(m_memory+0x200),romsize);
	
	return true;
}

void Chip8::EmulateCycle()
{
	m_opcode = m_memory[m_pc] << 8 | m_memory[m_pc + 1];
	printf ("Current opcode: 0x%X\n", m_opcode);
	switch(m_opcode & 0xF000)
	{
		case 0x0000:
			switch(m_opcode & 0x000F)
			{
				case 0x0000: // 0x00E0: Clears the screen
					memset(m_gfx,0,sizeof(m_gfx));
					m_drawFlag = true;
					m_pc += 2;
					break;
				case 0x000E: // 0x00EE: Returns from subroutine
					--m_sp;			
					m_pc = m_stack[m_sp];				
					m_pc += 2;
				break;					
			}
			break;
		case 0x1000://0x1NNN: Jump to adress NNN
			m_pc= m_opcode & 0x0FFF;
			break;
		case 0x2000://0x2NNN: Call subroutine at NNN
			m_stack[m_sp] = m_pc;
			m_pc = m_opcode & 0x0FFF;
			++m_sp;
			break;
		case 0x3000://0x3VNN: Skip next instruction if reg V equals NN
			if(m_reg[(0x0F00&m_opcode)>>8]==(m_opcode&0x00FF))
				m_pc+=4;
			else
				m_pc+=2;
			break;
		case 0x4000://0x4VNN: Skip next instruction if reg V doesn't equals NN
			if(m_reg[(0x0F00&m_opcode)>>8]!=(m_opcode&0x00FF))
				m_pc+=4;
			else
				m_pc+=2;
			break;
		case 0x5000://0x5VY0: Skip next instruction if reg V quals reg Y
			if(m_reg[(0x0F00&m_opcode)>>8]==(m_reg[(0x00F0&m_opcode)>>4]))
				m_pc+=4;
			else
				m_pc+=2;
			break;
		case 0x6000://0x6VNN: Set the register V to NN
			m_reg[(m_opcode&0x0F00)>>8]= m_opcode&0x00FF;
			m_pc+=2;
			break;
		case 0x7000: //0x7VNN: Add NN to register V
			m_reg[(m_opcode&0x0F00)>>8] += m_opcode&0x00FF;
			m_pc+=2;
			break;
		case 0x8000:
			switch(m_opcode&0x000F)
			{
				case 0x0000://0x8XY0: set reg X to Y
					m_reg[(m_opcode & 0x0F00)>>8] =  m_reg[(m_opcode&0x00F0)>>4];
					break;
				case 0x0001://0x8XY1: set reg X to X OR Y
					m_reg[(m_opcode & 0x0F00)>>8] |= m_reg[(m_opcode&0x00F0)>>4];
					break;
				case 0x0002://0x8XY2: set reg X to X AND Y
					m_reg[(m_opcode & 0x0F00)>>8] &= m_reg[(m_opcode&0x00F0)>>4];
					break;
				case 0x0003://0x8XY3: set reg X to X XOR Y
					m_reg[(m_opcode & 0x0F00)>>8] ^= m_reg[(m_opcode&0x00F0)>>4];
					break;
				case 0x0004://0x8XY4: Adds reg Y to reg X. reg F is set to 1 when there's a carry, and to 0 when there isn't					
					if(m_reg[(m_opcode & 0x00F0) >> 4] > (0xFF - m_reg[(m_opcode & 0x0F00) >> 8])) 
						m_reg[0xF] = 1;
					else 
						m_reg[0xF] = 0;					
					m_reg[(m_opcode & 0x0F00) >> 8] += m_reg[(m_opcode & 0x00F0) >> 4];
					m_pc += 2;
					break;	
				case 0x0005://0x8XY5: reg Y is subtracted from reg X. reg F is set to 0 when there's a borrow, and 1 when there isn't			
					if(m_reg[(m_opcode & 0x00F0) >> 4] > m_reg[(m_opcode & 0x0F00) >> 8]) 
						m_reg[0xF] = 0; 
					else 
						m_reg[0xF] = 1;					
					m_reg[(m_opcode & 0x0F00) >> 8] -= m_reg[(m_opcode & 0x00F0) >> 4];
					m_pc += 2;
					break;
				case 0x0006://0x8XY6: Shifts reg X right by one. reg F is set to the value of the least significant bit of reg X before the shift
					m_reg[0xF] = m_reg[(m_opcode & 0x0F00) >> 8] & 0x1;
					m_reg[(m_opcode & 0x0F00) >> 8] >>= 1;
					m_pc += 2;
					break;
				case 0x0007://0x8XY7: Sets reg X to regY minus reg X. reg F is set to 0 when there's a borrow, and 1 when there isn't
					if(m_reg[(m_opcode & 0x0F00) >> 8] > m_reg[(m_opcode & 0x00F0) >> 4])
						m_reg[0xF] = 0;
					else
						m_reg[0xF] = 1;
					m_reg[(m_opcode & 0x0F00) >> 8] = m_reg[(m_opcode & 0x00F0) >> 4] - m_reg[(m_opcode & 0x0F00) >> 8];				
					m_pc += 2;
					break;
				case 0x000E://0x8XYE: Shifts reg X left by one. reg F is set to the value of the most significant bit of regX before the shift
					m_reg[0xF] = m_reg[(m_opcode & 0x0F00) >> 8] >> 7;
					m_reg[(m_opcode & 0x0F00) >> 8] <<= 1;
					m_pc += 2;
					break;
				default:
					printf ("Unknown opcode [0x8000]: 0x%X\n", m_opcode);
			}
		case 0x9000://0x9XY0: Skip next instruction if x doesn't equal y
			if(m_reg[(0x0F00&m_opcode)>>8]!=m_reg[(0x00F0&m_opcode)>>4])
				m_pc+=4;
			else
				m_pc+=2;
			break;
		case 0xA000://0xANNN: Set index to NNN
			m_index = m_opcode & 0x0FFF;
			m_pc+=2;
			break;
		case 0xB000://0xBNNN: Jump to NNN plus reg 0
			m_pc = (m_opcode&0x0FFF)+m_reg[0];
			break;
		case 0xC000://0xCXNN: Set reg X to a random numer masked by NN
			m_reg[(m_opcode & 0x0F00) >> 8] = (rand() % 0xFF) & (m_opcode & 0x00FF);
			m_pc+=2;
			break;
		case 0xD000:
		{
			uint16_t x = m_reg[(m_opcode & 0x0F00) >> 8];
			uint16_t y = m_reg[(m_opcode & 0x00F0) >> 4];
			uint16_t height = m_opcode & 0x000F;
			uint16_t pixel;

			m_reg[0xF] = 0;
			for (uint16_t yline = 0; yline < height; yline++)
			{
				pixel = m_memory[m_index + yline];
				for(uint16_t xline = 0; xline < 8; xline++)
				{
					if((pixel & (0x80 >> xline)) != 0)
					{
						if(m_gfx[(x + xline + ((y + yline) * 64))] == 1)
						{
							//Collision detected
							m_reg[0xF] = 1;                                    
						}
						m_gfx[x + xline + ((y + yline) * 64)] ^= 1;
					}
				}
			}
						
			m_drawFlag = true;			
			m_pc += 2;
		}
		break;
		case 0xE000:
			switch(m_opcode & 0x00FF)
			{
				case 0x009E://EX9E: Skips the next instruction if the key stored in reg X is pressed
					if(m_key[m_reg[(m_opcode & 0x0F00) >> 8]] != 0)
						m_pc += 4;
					else
						m_pc += 2;
					break;			
				case 0x00A1://EXA1: Skips the next instruction if the key stored in reg X isn't pressed
					if(m_key[m_reg[(m_opcode & 0x0F00) >> 8]] == 0)
						m_pc += 4;
					else
						m_pc += 2;
					break;
				default:
					printf ("Unknown opcode [0xE000]: 0x%X\n", m_opcode);
			}
			break;
		case 0xF000:
			switch(m_opcode & 0x00FF)
			{
				case 0x0007://FX07: Sets reg X to the value of the delay timer
					m_reg[(m_opcode & 0x0F00) >> 8] = m_delayTimer;
					m_pc += 2;
					break;								
				case 0x000A://FX0A: A key press is awaited, and then stored in reg X		
					{
					bool keyPress = false;

					for(uint8_t i = 0; i < 16; ++i)
					{
						if(m_key[i] != 0)
						{
							m_reg[(m_opcode & 0x0F00) >> 8] = i;
							keyPress = true;
						}
					}

					// If we didn't received a keypress, skip this cycle and try again.
					if(!keyPress)						
						return;

					m_pc += 2;					
					}
					break;
				case 0x0015://FX15: Sets the delay timer to reg X
					m_delayTimer = m_reg[(m_opcode & 0x0F00) >> 8];
					m_pc += 2;
					break;
				case 0x0018://FX18: Sets the sound timer to reg X
					m_soundTimer = m_reg[(m_opcode & 0x0F00) >> 8];
					m_pc += 2;
					break;
				case 0x001E://FX1E: Adds reg X to I
					if(m_index + m_reg[(m_opcode & 0x0F00) >> 8] > 0xFFF)
						m_reg[0xF] = 1;
					else
						m_reg[0xF] = 0;
					m_index += m_reg[(m_opcode & 0x0F00) >> 8];
					m_pc += 2;
					break;
				case 0x0029://FX29: Sets Index to the location of the sprite for the character in reg X. Characters 0-F (in hexadecimal) are represented by a 4x5 font
					m_index = m_reg[(m_opcode & 0x0F00) >> 8] * 0x5;
					m_pc += 2;
					break;
				case 0x0033://FX33: Stores the Binary-coded decimal representation of regX at the addresses I, I plus 1, and I plus 2
					m_memory[m_index]     = m_reg[(m_opcode & 0x0F00) >> 8] / 100;
					m_memory[m_index + 1] = (m_reg[(m_opcode & 0x0F00) >> 8] / 10) % 10;
					m_memory[m_index + 2] = (m_reg[(m_opcode & 0x0F00) >> 8] % 100) % 10;					
					m_pc += 2;
					break;
				case 0x0055://FX55: Stores reg0 to regX in memory starting at address index					
					std::copy(m_reg,m_reg+((m_opcode & 0x0F00) >> 8),&m_memory[m_index]);
					m_index += ((m_opcode & 0x0F00) >> 8) + 1;
					m_pc += 2;
					break;
				case 0x0065://FX65: Fills reg0 to regX with values from memory starting at address I					
					std::copy(&m_memory[m_index],(&m_memory[m_index])+((m_opcode & 0x0F00) >> 8),m_reg);	
					m_index += ((m_opcode & 0x0F00) >> 8) + 1;
					m_pc += 2;
					break;
				default:
					printf ("Unknown opcode [0xF000]: 0x%X\n", m_opcode);
				}
			break;			
		default:
			printf ("Unknown opcode: 0x%X\n", m_opcode);
	}
	
	if(m_delayTimer>0)
		--m_delayTimer;
		
	if(m_soundTimer>0)
		if(m_soundTimer==1)
		{
			printf ("BEEP!\n");
			--m_soundTimer;
		}
}

bool Chip8::Draw()
{
	if(m_drawFlag)
	{
			m_drawFlag=false;
			return true;
	}
	return false;
	
}


