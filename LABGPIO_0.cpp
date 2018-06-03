


#include "LABGPIO_0.hpp"

LabGPIO::LabGPIO(uint8_t pinNum)
{
	pin = pinNum;
}

void LabGPIO::setAsInput(void)//Should alter the hardware registers to set the pin as an input
{
    LPC_GPIO0->FIODIR &= ~(1 << pin);
}

void LabGPIO::setAsOutput(void)// Should alter the hardware registers to set the pin as an output
{
   LPC_GPIO0->FIODIR |= (1 << pin);
}

void LabGPIO::setDirection(bool output)// Should alter the set the direction output or input depending on the input.
{	
 if(output)setAsOutput();
 else setAsInput();
}

void LabGPIO::setHigh(void)
{
  LPC_GPIO0->FIOSET |= (1 << pin);
}

void LabGPIO::setLow(void)
{
  LPC_GPIO0->FIOCLR |= (1 << pin);
}

void LabGPIO::set(bool high)
{
  if(high)setHigh();
  else setLow();	
}

bool LabGPIO::getLevel()
{
  bool level = false;
  
  if(LPC_GPIO0->FIOPIN & (1 << pin) ){level = true;}
  else level = false;

  return level;
}
LabGPIO::~LabGPIO()
{
}

