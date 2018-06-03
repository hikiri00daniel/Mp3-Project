
#include <stdio.h>
#include "FreeRTOS.h"
#include "tasks.hpp"
#include "queue.h"
#include "semphr.h"
#include "utilities.h"
#include "io.hpp"
#include "LABGPIO_0.hpp"
#include "LABGPIO_1.hpp"
#include "math.h"
#include "LCD.hpp"
#include "Decoder.hpp"
#include "ssp1.h"

bool main_menu = true; //Navigate back to start of main menu
bool menu = true;  //Arrow printing at menu
bool up = false;   //Navigate upward at menu
bool down = false; //Navigate downward at menu
bool play = false; // lcd_print bool
bool mode = true;
bool vol_up = false; 
bool vol_down = false;
bool pause = true; //song pause
bool next, prev = false; //Next/Previous Song
bool song_sel = false;
bool press = false;
char str_opt[6][16] = {"Start of List", "SimpleAndClean" , "GasGasGas", "GravityWall", "JustForNow", "Pi"};
int menu_index = 0; //Keeping track of menu index between 0 and 5

 TaskHandle_t music_task;

FIL read_song()
{
	FIL file;
	FRESULT res_status;
	if(menu_index == 1)
	{
	    res_status = f_open(&file, "1:SimpleAndClean.mp3", (FA_READ | FA_OPEN_EXISTING));
	}
	else if(menu_index == 2)
	{
	    res_status = f_open(&file, "1:Gas Gas Gas.mp3", (FA_READ | FA_OPEN_EXISTING));
	}
	else if(menu_index == 3)
	{
	    res_status = f_open(&file, "1:gravityWall.mp3", (FA_READ | FA_OPEN_EXISTING));
	}
	else if(menu_index == 4)
	{
	    res_status = f_open(&file, "1:Just For Now Marshvll Remix.mp3", (FA_READ | FA_OPEN_EXISTING));
	}
	if (res_status != FR_OK) 
    	{
      //printf("open error #%i (0x%02X).\n",res_status,res_status);
      f_close(&file);
	}
	delay_ms(1);
	return file;
}

SemaphoreHandle_t press_play, next_song, prev_song;

void music(void *pvParameters)
{
  xSemaphoreTake(press_play, portMAX_DELAY);
  unsigned char buffer[512];
  unsigned int bytesRead = 0;
  uint8_t old_index;
  uint16_t volume = 0x40;
  FIL file;
  Decoder decode;
  while(1)
  {
    file = read_song();

    while(1)
    {
	if(song_sel == false){
		bytesRead = 0;
		vTaskSuspend(music_task);
		file = read_song();
	}
	if(pause == true){
		old_index = menu_index;
		vTaskSuspend(music_task);
		if(old_index != menu_index)
		{
			file = read_song();
		}
	}

      f_read(&file, buffer, sizeof(buffer), &bytesRead);
      int i = 0;
      while(i < bytesRead)
      {
	if(next == true)
	{
		bytesRead = 0;
		file = read_song();
		next = false;
	}
	else if(prev == true)
	{
		bytesRead = 0;
		file = read_song();
		prev = false;
	}
	else
	{
	if(vol_down == true)
	{
		if(volume > 0xFF)
		{
			volume = 0xFE;
		}
		
		volume += 0xF;
		decode.setVolume(volume);
		vol_down = false;
	}
	//music task
	if(vol_up == true)
	{
		if(volume <= 0xF)
		{
			volume = 0x0;
		}

		volume -= 0xF;
		decode.setVolume(volume);
		vol_up = false;
	}
        if(decode.wait_dreq())
        {
          decode.data_on();
          ssp1_exchange_byte(buffer[i++]);
          decode.data_off();
        }
        else 
        {
          vTaskDelay(1);
        }
       }
      }
	
      if(sizeof(buffer) > bytesRead)
      {
        break;
      }
      vTaskDelay(1);
    }
    f_close(&file);
    //playChunk(HelloMP3,sizeof(HelloMP3));
    vTaskDelay(355);
  }
}

void vReadSwitch(void * pvParameters)
{ 
LabGPIO1 extsw3(22), extsw4(23), extsw5(28), extsw6(29);
// External switches 
LabGPIO extsw1(0), extsw2(1);

extsw1.setAsInput();
extsw2.setAsInput();
extsw3.setAsInput();
extsw4.setAsInput();
extsw5.setAsInput();
extsw6.setAsInput();

bool state, old;
bool next_state, old_next;
bool prev_state, old_prev;
// External button init	
	while(1)
    	{
		if(extsw1.getLevel() == true) //Return to main menu
		{	
			song_sel = false;
			main_menu = true;
			mode = true;
			press = false;
			menu_index = 0;
		}
		else if(press == false && extsw2.getLevel() == true) //Play//Pause//
		{
			if(menu_index == 0)
			{
				//do nothing
			}
			else
			{
				song_sel = true;
				vTaskResume(music_task);
				press = true;
				play = true;
				mode = false;		
			}
		}
	    
		switch(mode)
		{
		case true: //MENU MODE
			if(extsw4.getLevel() == true) //DOWN/
			{
				down = true;
			}
			else if(extsw3.getLevel() == true) //UP/
			{
				up = true;
			}
		break;
		case false: //MP3 MODE
			state = extsw2.getLevel();
			if(state == false && old == true)
			{
				if(!pause)
				{
					pause = true;
				}
				else 
				{
					pause = false; 
					vTaskResume(music_task);
				}
			}
			old = state;
			if(extsw3.getLevel())//Volume Up
			{
				vol_up = true;
			}
			if(extsw4.getLevel())//Volume Down
			{
				vol_down = true;
			}
			next_state = extsw5.getLevel();
			if(extsw5.getLevel() == true) //Next Song
			{
				menu_index++;
				next = true;
				if(menu_index > 4)
				{
					menu_index = 1;
				}	
			}
			else if(extsw6.getLevel() == true) //Prev Song
			{
				menu_index--;
				prev = true;
				if(menu_index < 1)
				{
					menu_index = 4;
				}
			}
		break;
		}
		vTaskDelay(3000);	// vTaskDelay(375) works;
	}
}

void vNowPlaying(void* pvParameters)
{
    LCD_Drive lcd;
	while(1)
	{
		if(play)
		{
			if(menu_index == 5)
			{
				lcd.Pi();
			}
			else 
			{
			xSemaphoreGive(press_play);
			lcd.command_to_LCD(Clear);
			lcd.command_to_LCD(LINE1);
			lcd.string_to_LCD("Now Playing:");
			lcd.command_to_LCD(LINE2);
			lcd.string_to_LCD(str_opt[menu_index]);
			}
			play = false;
		}
		if(next)
		{	
			xSemaphoreGive(press_play);
			lcd.command_to_LCD(Clear);
			lcd.command_to_LCD(LINE1);
			lcd.string_to_LCD("Now Playing:");
			lcd.command_to_LCD(LINE2);
			lcd.string_to_LCD(str_opt[menu_index]);
		}
		if(prev)
		{
			xSemaphoreGive(press_play);
			lcd.command_to_LCD(Clear);
			lcd.command_to_LCD(LINE1);
			lcd.string_to_LCD("Now Playing:");
			lcd.command_to_LCD(LINE2);
			lcd.string_to_LCD(str_opt[menu_index]);
		}
			next = false;
			prev = false;
		vTaskDelay(355);
	}
	
}

void vControlLCD_Menu(void * pvParameters)
{
     LCD_Drive lcd;
     while(1)
	{
	  	if(main_menu && !menu)
	  	{
			menu_index = 0;
			lcd.command_to_LCD(Clear);
			lcd.arrow_up();
			lcd.command_to_LCD(LINE1+2);//LINE0+2
			lcd.string_to_LCD(str_opt[menu_index]);
			lcd.command_to_LCD(LINE2+2);
			lcd.string_to_LCD(str_opt[menu_index+1]);	
			main_menu = !main_menu;
		}
		else if(menu && !play)
		{
			if(fmod(menu_index, 2) == 0){
				lcd.arrow_up();
				lcd.command_to_LCD(LINE1+2); 
				lcd.string_to_LCD(str_opt[menu_index]);
				lcd.command_to_LCD(LINE2+2);
				lcd.string_to_LCD(str_opt[menu_index+1]);
			}
			if(fmod(menu_index,2) == 1){
				lcd.arrow_down();
				lcd.command_to_LCD(LINE1+2);
				lcd.string_to_LCD(str_opt[menu_index-1]);
				lcd.command_to_LCD(LINE2+2);
				lcd.string_to_LCD(str_opt[menu_index]);
			}
			menu = !menu;
		}
		else if(up && !play)//Switching to upward page checking
		{
			if((fmod(menu_index,2) == 0)) //CHECK even numbered menu index when going UP
			{
				if(menu_index > 0){
				menu_index--;
				}
				else if(menu_index == 0){
					menu_index = 5;
				}
				lcd.command_to_LCD(Clear);
			}
			else
			{
				menu_index--;
			}
			menu = true;
			up = !up;
		}	
		else if(down && !play)//Switching to downward page checking
		{
			if((fmod(menu_index,2) == 1)) //CHECK odd numbered menu index when going DOWN
			{
				if(menu_index < 5){
					menu_index++;
				}
				else if(menu_index == 5){
					menu_index = 0;
				}
	              			lcd.command_to_LCD(Clear);
			}
			else
			{
			menu_index++;
			}
			
			menu = true;
			down = !down;
		}
			vTaskDelay(355);
	}	
}

int main(void)
{
	void ssp1_init(); // init spi
	LCD_Drive lcd;

	Decoder decode;
	/*Special Initialization in 8-bit mode first*/
	lcd.LCD_reset();
	/*Now intialize LCD in 4-bit mode*/
	lcd.LCD_init();
	decode.begin();
	decode.modeSwitch();
	decode.setVolume(0x40);
	press_play = xSemaphoreCreateBinary();
	next_song = xSemaphoreCreateBinary();
	prev_song = xSemaphoreCreateBinary();
	scheduler_add_task(new terminalTask(PRIORITY_HIGH));
	xTaskCreate(music, "mp3 task", STACK_BYTES(1024*8), NULL, 2, &music_task);
	xTaskCreate(vReadSwitch,"Switches", STACK_BYTES(1024*1), NULL , 1 , NULL);
	xTaskCreate(vControlLCD_Menu,"LCD Control Menu", STACK_BYTES(1024*1), NULL , 1, NULL);
	xTaskCreate(vNowPlaying, "LCD Now Playing", STACK_BYTES(1024*1), NULL, 3, NULL);
	scheduler_start();
	return 0;
}


