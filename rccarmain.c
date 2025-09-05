/* main.c
 * your name
 * date
 */


#include <ti/devices/msp/msp.h>
#include "../inc/LaunchPad.h"
#include "../inc/Clock.h"

#include "../inc/Timer.h"
//#include "../inc/ADC1.h" //pa18 for y
//#include "../inc/ADC0.h" //pa25 for x
#include "ADC1.h"
#include "ADC0.h"
#include "UART1.h"
#include "UART2.h"
#include "UART.h"
//#include "../inc/Dump.h"  // student's Lab 3
#include <stdio.h>
#include <string.h>
#include "FIFO1.h"
#include "PWM1.h"
#include "PWM.h"

uint32_t flag, concatenate, receive, pwm_on, pwm1, pwm, percent, temp;
int32_t y_temp;
uint16_t r_y, r_x, data_x, data_y;
uint8_t in;
uint32_t run = 0;
bool check = false;
char buffer[20];
bool fwd = false;

void turnonled(void){

  GPIOB->DOUTSET31_0 = 0x008;
}

void PLL_Init(void){ // set phase lock loop (PLL)
  // Clock_Init40MHz(); // run this line for 40MHz
  Clock_Init80MHz(0);   // run this line for 80MHz
}

uint32_t abs(int32_t val){
  if (val < 0){
    val = val * (-1);
  }
  return val;
}

void wireless_init(void){
  IOMUX->SECCFG.PINCM[PB2INDEX] = 0x00000081; //right motor init
  GPIOB->DOE31_0 |= 0x004;
  GPIOB->DOUTSET31_0 = 0x004;
}

// void sendATCommand(const char *cmd) {
//     UART_InString(cmd, 10);
//     UART_writeByte(DEBUG_UART_INSTANCE, '\r'); // HC-12 requires CR terminator
//     UART_writeByte(DEBUG_UART_INSTANCE, '\n');
// }

void led_init(void){

  IOMUX->SECCFG.PINCM[PB13INDEX] = 0x00000081; 
  GPIOB->DOE31_0 |= 0x02000;
  IOMUX->SECCFG.PINCM[PB15INDEX] = 0x00000081; 
  GPIOB->DOE31_0 |= 0x08000;
}

void led_out(uint32_t x, uint32_t y){

  if ((x < 2048) && (y<2048)) {
    GPIOB->DOUTSET31_0 = 0x01; //red
    GPIOB->DOUTCLR31_0 = 0x02;
    GPIOB->DOUTCLR31_0 = 0x04;
    GPIOB->DOUTCLR31_0 = 0x08;
  }

  if ((x < 2048) && (y > 2048)) {
    GPIOB->DOUTCLR31_0 = 0x01; //yellow
    GPIOB->DOUTSET31_0 = 0x02;
    GPIOB->DOUTCLR31_0 = 0x04;
    GPIOB->DOUTCLR31_0 = 0x08;
  }

  if ((x > 2048) && (y < 2048)){
    GPIOB->DOUTCLR31_0 = 0x01; //green
    GPIOB->DOUTCLR31_0 = 0x02;
    GPIOB->DOUTSET31_0 = 0x04;
    GPIOB->DOUTCLR31_0 = 0x08;
  }
  if ((x > 2048) && (y > 2048)){
    GPIOB->DOUTCLR31_0 = 0x01; //blue
    GPIOB->DOUTCLR31_0 = 0x02;
    GPIOB->DOUTCLR31_0 = 0x04;
    GPIOB->DOUTSET31_0 = 0x08;
  }
}

void hbridge_init(void){
  IOMUX->SECCFG.PINCM[PB1INDEX] = 0x00000081; //right motor init
  GPIOB->DOE31_0 |= 0x0002;
  IOMUX->SECCFG.PINCM[PB4INDEX] = 0x00000081; 
  GPIOB->DOE31_0 |= 0x0010;

  IOMUX->SECCFG.PINCM[PB2INDEX] = 0x00000081; //left motor init
  GPIOB->DOE31_0 |= 0x0004;
  IOMUX->SECCFG.PINCM[PB3INDEX] = 0x00000081; 
  GPIOB->DOE31_0 |= 0x0008;

}

void car_forward(void){
  GPIOB->DOUTCLR31_0 = 0x02; //both motors in one direction (forward)
  GPIOB->DOUTSET31_0 = 0x010;
  GPIOB->DOUTCLR31_0 = 0x04;
  GPIOB->DOUTSET31_0 = 0x08;
}

void car_backward(void){
  GPIOB->DOUTSET31_0 = 0x02; //both motors in one direction (backward)
  GPIOB->DOUTCLR31_0 = 0x010;
  GPIOB->DOUTSET31_0 = 0x04;
  GPIOB->DOUTCLR31_0 = 0x08;
}

void car_right(void){ //one motor is off
  GPIOB->DOUTCLR31_0 = 0x04;
  GPIOB->DOUTCLR31_0 = 0x08;
}

void car_left(void){ //the other motor is off
  GPIOB->DOUTCLR31_0 = 0x01;
  GPIOB->DOUTCLR31_0 = 0x02;
}

void car_off(void){
  GPIOB->DOUTCLR31_0 = 0x02;
  GPIOB->DOUTCLR31_0 = 0x010;
  GPIOB->DOUTCLR31_0 = 0x04;
  GPIOB->DOUTCLR31_0 = 0x08;
}
void motor_fwd(void){
  GPIOB->DOUTCLR31_0 = 0x01; //both motors in one direction (forward)
  GPIOB->DOUTSET31_0 = 0x02;
}

void motor_bckwd(void){
  GPIOB->DOUTCLR31_0 = 0x02; //both motors in one direction (forward)
  GPIOB->DOUTSET31_0 = 0x01;
}

void left_motor(bool fwd_bckwd, uint8_t pwm_x){ //
  if (fwd_bckwd){
      GPIOB->DOUTCLR31_0 = 0x01; //right motor in one direction (forward)
      GPIOB->DOUTSET31_0 = 0x02;
  }
  else{
      GPIOB->DOUTCLR31_0 = 0x02; //right motor in one direction (backward)
      GPIOB->DOUTSET31_0 = 0x01;
  }
  Clock_Delay1ms(pwm_x);
}

void right_motor(bool fwd_bckwd, uint8_t pwm_x){//
    if (fwd_bckwd){
      GPIOB->DOUTCLR31_0 = 0x04; //right motor in one direction (forward)
      GPIOB->DOUTSET31_0 = 0x08;
  }
  else{
      GPIOB->DOUTCLR31_0 = 0x08; //right motor in one direction (backward)
      GPIOB->DOUTSET31_0 = 0x04;
  }
  Clock_Delay1ms(pwm_x);
}

void rmotor_off(uint8_t pwm_x){
  GPIOB->DOUTCLR31_0 = 0x01; //right motor off
  GPIOB->DOUTCLR31_0 = 0x02;
  Clock_Delay1ms(1000-pwm_x);
}

void lmotor_off(uint8_t pwm_x){
  GPIOB->DOUTCLR31_0 = 0x08; //left motor off
  GPIOB->DOUTCLR31_0 = 0x04;
  Clock_Delay1ms(1000-pwm_x);
}


void wheel_test(void){
    car_forward();
    Clock_Delay1ms(3000);
    car_left();
    Clock_Delay1ms(3000);
    car_forward();
    Clock_Delay1ms(3000);
    car_right();
    Clock_Delay1ms(3000);
    car_backward();
    Clock_Delay1ms(3000);
    car_left();
    Clock_Delay1ms(3000);
    car_backward();
    Clock_Delay1ms(3000);
    car_right();
    Clock_Delay1ms(3000);
}

void motor_test(void){
    motor_fwd();
    Clock_Delay1ms(3000);
    motor_bckwd();
    Clock_Delay1ms(3000);
}

uint32_t conversion_straight(int32_t y){
  uint32_t convert;
  convert = (abs((2048 - y)) * 1000)/2048;
  return convert;
}

uint32_t conversion_turnleft(uint16_t x){
  uint32_t turn;
  turn = ((x-2048) * 1000)/2048;
  return turn;
}

uint32_t conversion_turnright(uint16_t x){
  uint32_t turn;
  turn = ((x)*1000)/2048;
  turn = 1-turn;
  return turn;
}

uint32_t relativewheel(uint32_t val, uint32_t straight_y){
  val = (val * straight_y)/10000;
  return val;
}

// void direction (bool fwd_bckwd, uint32_t pwm_y){ //
//   if (fwd_bckwd){
//     //car_forward();
//     pwm_y = 10000 - pwm_y;
//     PWM1_SetDuty(pwm_y, 0);
//   }
//   else if (!fwd_bckwd){
//     car_backward();
//     PWM1_SetDuty(0, pwm_y);
//   }
// }

void TIMG12_IRQHandler(void){
  if((TIMG12->CPU_INT.IIDX) == 1){ // this will acknowledge
    GPIOB->DOUTTGL31_0 = GREEN; // toggle PB27 (minimally intrusive debugging)
    GPIOB->DOUTTGL31_0 = GREEN; // toggle PB27 (minimally intrusive debugging)
    // sample 12-bit ADC0 channel 5, slidepot
    // store data into mailbox
    // set the semaphore

    data_x = ADC0_in();
    data_y = ADC1_in();

    concatenate = data_x;
    concatenate = concatenate << 12;
    concatenate += data_y;
    UART1_OutChar('<'); //very unlikely that this value would show up on ADC but I might have to change it
    UART1_OutChar(concatenate & 0xFF); //lsb 8 bits y
    UART1_OutChar((concatenate >>8) & 0xFF);
    UART1_OutChar((concatenate >>16) & 0xFF); //lsb 8 bits

    flag = 1;
    GPIOB->DOUTTGL31_0 = GREEN; // toggle PB27 (minimally intrusive debugging)
  }
}

int main(void){
  __disable_irq();
  PLL_Init();
  LaunchPad_Init();
  ADC0_init();
  ADC1_init();
  UART1_Init();
  UART2_Init();
  TimerG12_IntArm(2666667, 0);
  PWM1_Init(8,7,1000,0,0);
  PWM_Init(8,7,1000,0,0);
  wireless_init();
  __enable_irq();

  while(1){
      

        receive = 0;

        in = UART2_InChar();
        while (in!= 0x3C){
          in = UART2_InChar();
        }
        in = UART2_InChar();

        


        receive = in;
        in = UART2_InChar();
        temp = in;
        receive += ((temp << 8) & 0xFF00);
        in = UART2_InChar();
        temp = in;
        receive += ((temp << 16)& 0xFF0000);


        r_y = (receive & 0xFFF);
        r_x = ((receive & 0xFFF000)>>12);
        
        // check wireless uart with leds 
        // if (r_y > 2048){
        //   GPIOB->DOUTSET31_0 = 0x02000;        
        // }
        // else if (r_y <= 2048){
        //   GPIOB->DOUTCLR31_0 = 0x02000;
        // }

        // if (r_x > 2048){
        //   GPIOB->DOUTSET31_0 = 0x08000;
        // }
        // else if (r_x <= 2048){
        //   GPIOB->DOUTCLR31_0 = 0x08000;
        // }

        y_temp = r_y;
        if (y_temp <= 2048){
          fwd = true; // backwards
        }
        else if (y_temp > 2048){

          fwd = false; // declare going forwards
          y_temp -= 2048;
        }

        
        pwm_on = conversion_straight(y_temp);
        

        if ((r_x >2100) && (r_x <2200) && (r_y >1700) && (r_y < 1830)){
          PWM1_SetDuty(0,0);
          PWM_SetDuty(0,0);
        }
        else{
        if ((r_x >=2100) && (r_x <= 2200)){
            if (fwd){ // going backwakrds
              
              PWM1_SetDuty((pwm_on - 1), 0);
              PWM_SetDuty((pwm_on - 1), 0);
            }
            else if (!fwd){ // going forwards
              
              PWM1_SetDuty(0, (1000-pwm_on)); //right motor
              PWM_SetDuty(0, (1000-pwm_on)); //left motor
            }

        }

        else if (r_x <  2100){ // //right

          if ((r_y >1600)&& (r_y < 1900)){
            if (r_y > 1750){
              fwd = false;
            }
            if(fwd){
            PWM_SetDuty(500,0);
            }
            else if (!fwd){
            PWM_SetDuty(0,500);
            }
            PWM1_SetDuty(0,0);
          }
        
          else{ //if (r_x >2200)
            percent = pwm_on*7/10;
            if (fwd){
              PWM_SetDuty((percent-1),0);
              PWM1_SetDuty((pwm_on-1),0);
            }
            else if (!fwd){
              PWM_SetDuty(0, (700-percent));
              PWM1_SetDuty(0, (900-pwm_on));
            }
          }
        }


          else if (r_x >  2200){ 
          if ((r_y < 1900)&& (r_y >1600)){
            if (r_y > 1750){
              fwd = false;
            }
            if(fwd){
            PWM1_SetDuty(500,0);
            }
            else if (!fwd){
            PWM1_SetDuty(0,500);
            }
            PWM_SetDuty(0,0);
          }
          
          
          else{ //if (r_x < 2100)
            percent = pwm_on*7/10;
            if (fwd){
              PWM1_SetDuty((percent-1),0);
              PWM_SetDuty((pwm_on-1),0);
            }
            else if (!fwd){
              PWM1_SetDuty(0, (700-percent));
              PWM_SetDuty(0, (900-pwm_on));
            }

            }
            }
         }
        
        
        while (flag == 0){

        }

        if (flag == 1){
          flag = 0;
        }           

    GPIOB->DOUTTGL31_0 = RED;

    
    

  }
  
}


