#include "Decoder.hpp"

Decoder::Decoder()
{

}

 bool Decoder::wait_dreq(void) //const
{
LabGPIO1 dreq_pin(19); // DREQ
	if(dreq_pin.getLevel() == 1)
  {
    return 1;
  }
  else
  {
    return 0;
  }
}

 void Decoder::control_on(void) //const
{
    LabGPIO cs_pin(29); // XCS
    LabGPIO dcs_pin(30); // XDCS
    dcs_pin.setHigh();
    cs_pin.setLow();
}

void Decoder::control_off(void) //const
{
	LabGPIO cs_pin(29); // XCS
	cs_pin.setHigh();
}

void Decoder::data_on(void) //const
{
	LabGPIO cs_pin(29); // XCS
	LabGPIO dcs_pin(30); // XDCS
	cs_pin.setHigh();
	dcs_pin.setLow();
}

void Decoder::data_off(void) //const
{

	LabGPIO dcs_pin(30); // XDCS
	dcs_pin.setHigh();
}

void Decoder::writeSci(uint8_t addr, uint16_t data)
{
  control_on();
  ssp1_exchange_byte(0x2);
  ssp1_exchange_byte(addr);
  ssp1_exchange_byte(data >> 8);
  ssp1_exchange_byte(data & 0xFF);
  control_off();
}

void Decoder::begin(void)
{
LabGPIO cs_pin(29); // XCS
LabGPIO dcs_pin(30); // XDCS
LabGPIO1 dreq_pin(19); // DREQ
LabGPIO1 reset_pin(20); // RST
	// Keep the chip in reset until we are ready
	reset_pin.setAsOutput();
	reset_pin.setLow();

	// The SCI and SDI will start deselected
	cs_pin.setAsOutput();
	cs_pin.setHigh();
	dcs_pin.setAsOutput();
	dcs_pin.setHigh();

	// DREQ is an input
	dreq_pin.setAsInput();

	// Boot VS1053D
	//printf("Booting VS1053...\n");

	delay_ms(10);

	reset_pin.setHigh();

	writeSci(SCI_VOL,0xffff); // VOL

	writeSci(SCI_AUDATA,10);

	delay_ms(10);

	writeSci(SCI_VOL,0xfefe); // VOL

	//printf("VS1053 still booting\n");

	writeSci(SCI_AUDATA,44101); // 44.1kHz stereo

	//Mp3SetVolume(20,20); // Set initial volume (20 = -10dB)
	writeSci(SCI_VOL,0x2020); // VOL

	writeSci(SCI_MODE, (1<<SM_SDINEW) | (1<<SM_RESET));
	delay_ms(10);
  	ssp1_set_max_clock(14);
	writeSci(SCI_CLOCKF,0xB800); // Experimenting with higher clock settings
	delay_ms(10);
	
	//printf("VS1053 Set\n");

	//printf("VS1053 OK\n");
}


void Decoder::setVolume(uint8_t vol) //const
{
	uint16_t value = vol;
	value <<= 8;
	value |= vol;

	writeSci(SCI_VOL,value); // VOL
}

void Decoder::modeSwitch(void)
{
	//GPIO_DDR
	writeSci(SCI_WRAMADDR, 0xc017);
	writeSci(SCI_WRAM, 0x0003);
	//GPIO_ODATA
	writeSci(SCI_WRAMADDR, 0xc019);
	writeSci(SCI_WRAM, 0x0000);

	delay_ms(10);
	writeSci(SCI_MODE, (1<<SM_SDINEW) | (1<<SM_RESET));
	delay_ms(10);
  //printf("mp3 mode\n");
}

Decoder::~Decoder()
{
	
}


