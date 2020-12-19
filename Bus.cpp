#include "Bus.h"

void Bus::ConnectBus(cpu6502* cpuPtr, NesPPU* ppuPtr)
{
	cpu = cpuPtr;
	cpu->ConnectCPU(ram);
	ppu = ppuPtr;
	ppu->ConnectPPU(ram);
}

uint16_t Bus::BusRead(uint16_t addr)
{
	uint16_t data = ram[addr];
	if (addr >= 0x000 && addr < 0x2000)
	{
		return ram[addr];
	}
	else if (addr >= 0x2000 && addr < 0x4000)
	{ 
		switch (addr % 0x2008)
		{
		case 0x2000:
			break;
		case 0x2001:
			break;
		case 0x2002:
			//clear nmi_occured
			ppu->ppu_clear_vblank();
			break;
		case 0x2003:
			break;
		case 0x2004:
			break;
		case 0x2005:
			break;
		case 0x2006:
			break;
		case 0x2007:
		{
			data = ppu_buffer;
			ppu_buffer = ppu->ppu_read(vram_addr);
			if (vram_addr > 0x3f00)
				data = ppu_buffer;
			//increment vram addr per read/write of ppudata
			BusRead(0x2000) & 0x3 ? vram_addr += 32 : vram_addr += 1;
			break;
		}
		}
	}
	else if (addr >= 0x4000 && addr < 0x4020)
	{
		printf("\nError! APU region\n");
	}
	else
	{
		printf("\nError! Cartridge region\n");
	}
	return data;
}

void Bus::BusWrite(uint16_t addr, uint8_t data)
{
	if (addr >= 0x000 && addr < 0x2000)
	{
		ram[addr & 0x7FF] = data;
	}
	else if (addr >= 0x2000 && addr < 0x4000)
	{
		switch (addr % 0x2008)
		{
		case 0x2000:
			nmi_output = true;//(BusRead(0x2000) & 0x80) ? true : false;
			break;
		case 0x2001:
			break;
		case 0x2002:
			break;
		case 0x2003:
			break;
		case 0x2004:
			break;
		case 0x2005:
			break;
		case 0x2006://ppuaddr
		{
			//$2006 holds no memory
			if (address_latch)
			{
				//high byte
				temporary_register += data << 8;
				address_latch = false;
			}
			else
			{
				//low byte
				temporary_register += data;
				address_latch = true;
				vram_addr = temporary_register;
			}
			break;
		}
		case 0x2007://ppudata
		{
			// increment vram addr per read/write of ppudata
			BusRead(0x2000) & 0x3 ? vram_addr += 32 : vram_addr += 1;
			ram[addr & 0x2007] = data;
			break;
		}
		}
	}
	else if (addr >= 0x4000 && addr < 0x4020)
	{
		printf("\nError! APU region\n");
	}
	else
	{
		printf("\nError! Cartridge region\n");
	}
}

void Bus::init()
{
	cpu->reset();
}

void Bus::run()
{
	nmi_output = true;
	while (true)
	{
		if (nmi_output && ppu->nmi_check()) {
			cpu->nmi();
		}
		int cycles = cpu->step_instruction();
		for (int i = 0; i < cycles * 3; i++) {
			ppu->step_ppu();
		}
	}
}