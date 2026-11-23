// sbi.h — asking the firmware (OpenSBI) for help.
//
// Some hardware settings are so powerful that even the kernel isn't allowed
// to touch them directly. Only the firmware (OpenSBI), which runs at an even
// higher power level, can. So the kernel ASKS the firmware, using the
// `ecall` instruction. This is called an "SBI call".
//
// It's the same idea as a program asking the kernel for help (a system
// call), just one level higher:   program → kernel → firmware
#pragma once
#include "types.h"

// "Firmware: please wake me up (with a timer interrupt) when the
//  clock reaches `when`."
void sbi_set_timer(uint64 when);
