#pragma once

#include <stdint.h>

inline const char* code_name(uintptr_t code)
{
	switch(code)
	{
		case 0: return "Instruction address misaligned";
		case 1: return "Instruction access fault";
		case 2: return "Illegal instruction";
		case 3: return "Breakpoint";
		case 4: return "Load address misaligned";
		case 5: return "Load access fault";
		case 6: return "Store/AMO address misaligned";
		case 7: return "Store/AMO access fault";
		case 8: return "Environment call from U-mode";
		case 9: return "Environment call from S-mode";
		case 10: return "Reserved";
		case 11: return "Environment call from M-mode";
		case 12: return "Instruction page fault";
		case 13: return "Load page fault";
		case 14: return "Reserved";
		case 15: return "Store/AMO page fault";
	}
	return "Unknown Exception";
}
