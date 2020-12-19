#ifndef NESPPU_H
#define NESPPU_H

#include <iostream>
//#include "Bus.h"

class Bus;

class NesPPU
{
private:
	uint8_t ppu_ram[0x4000];
	int ppu_cycles, ppu_scanline;
	bool nmi_occured = false;
	Bus* busPtr;
	uint8_t *ptrToBus;
public:
	void ConnectToBus(Bus* ptr) { busPtr = ptr; }
	void ConnectPPU(uint8_t* ptr) { ptrToBus = ptr; }
	uint16_t ppu_read(uint16_t addr) { return ppu_ram[addr]; }
	void ppu_write(uint16_t addr, uint8_t data) { ppu_ram[addr] = data; }
	void ppu_set_vblank();
	void ppu_clear_vblank();
	uint16_t interleave(uint16_t, uint16_t);
	bool nmi_check() { return nmi_occured; }
	void step_ppu();
};

#endif //NESPPU_H



