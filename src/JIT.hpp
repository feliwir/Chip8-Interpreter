#pragma once
#include <stdint.h>

class JIT
{

	static void TranslateCode(uint8_t* buffer);
	static void RunCode(int* code);

};