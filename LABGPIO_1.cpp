
#include "LABGPIO_1.hpp"

LabGPIO1::LabGPIO1(uint8_t pinNum)
{
	pin = pinNum;
}

void LabGPIO1::setAsInput(void)//Should alter the hardware registers to set the pin as an input
{
    LPC_GPIO1->FIODIR &= ~(1 << pin);
}

void LabGPIO1::setAsOutput(void)// Should alter the hardware registers to set the pin as an output
{
   LPC_GPIO1->FIODIR |= (1 << pin);
}

void LabGPIO1::setDirection(bool output)// Should alter the set the direction output or input depending on the input.
{	
 if(output)setAsOutput();
 else setAsInput();
}

void LabGPIO1::setHigh(void)
{
  LPC_GPIO1->FIOSET |= (1 << pin);
}

void LabGPIO1::setLow(void)
{
  LPC_GPIO1->FIOCLR |= (1 << pin);
}

void LabGPIO1::set(bool high)
{
  if(high)setHigh();
  else setLow();	
}

bool LabGPIO1::getLevel()
{
  bool level = false;
  
  if(LPC_GPIO1->FIOPIN & (1 << pin) ){level = true;}
  else level = false;

  return level;
}
LabGPIO1::~LabGPIO1()
{

}

