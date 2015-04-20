#pragma once
#include <stdint.h>
#include <string>
#include <chrono>
#include <SFML/Window.hpp>
#include <SFML/Audio.hpp>
#include "flextGL.h"

#define CPU_FREQ 60

class Chip8
{
	private:
		const GLchar* m_vs =
			"#version 330\n"
			"const vec2 pos[4] = vec2[4](vec2(-1.0,1.0),vec2(-1.0,-1.0),\n"
			"							 vec2(1.0,1.0) ,vec2(1.0,-1.0));\n"
			"const vec2 tex[4] = vec2[4](vec2(0.0,0.0),vec2(0.0,1.0),\n"
			"							 vec2(1.0,0.0),vec2(1.0,1.0));\n"
			"out vec2 outCoord;\n"
			"void main(void){\n"
			"outCoord = tex[gl_VertexID];\n"
			"gl_Position = vec4(pos[gl_VertexID],0,1);\n"
			"}";

		const GLchar* m_fs =
			"#version 330\n"
			"uniform sampler2D sampler;\n"
			"in vec2 coord;\n"
			"out vec4 color;\n"
			"void main(void){\n"
			"vec3 tmp = vec3(texture(sampler, coord)).rrr*255;\n"
			"color = vec4(tmp,1.0);\n"
			"}\n";

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

		uint32_t m_freq;

		std::chrono::time_point<std::chrono::high_resolution_clock> m_last;
		std::chrono::microseconds m_cycleInterval;
		sf::SoundBuffer m_soundwave;
		sf::Sound m_beep;
		GLuint m_tex;
		GLuint m_program,m_vs_id,m_fs_id;
		GLuint m_vao;
		bool m_drawFlag;
};
