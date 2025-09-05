/* Timers.c
 */
#include <ti/devices/msp/msp.h>
#include "../inc/Clock.h"

void (*PeriodicTask0)(void);   // user function
void (*PeriodicTask12)(void);   // user function

// void TimerG0_IntArm(void(*task)(void), uint32_t period, uint32_t priority){
//   uint32_t prescale = 80;
//   TIMG0->GPRCM.RSTCTL = 0xB1000003;
//   TIMG0->GPRCM.PWREN = 0x26000001;
//   Clock_Delay(24); // time for TimerG0 to power up
//   TIMG0->CLKSEL = 0x08; // bus clock
//   TIMG0->CLKDIV = 0x00; // divide by 1
//   TIMG0->COMMONREGS.CPS = prescale-1;     // divide by prescale,
//   TIMG0->COUNTERREGS.LOAD  = period-1;     // set reload register
//   TIMG0->COUNTERREGS.CTRCTL = 0x02;
//   // bits 5-4 CM =0, down
//   // bits 3-1 REPEAT =001, continue
//   // bit 0 EN enable (0 for disable, 1 for enable)
//   //TIMG0->INT_EVENT[0].IMASK |= 1; // zero event mask
//   TIMG0->CPU_INT.IMASK = 1; // zero event mask
//  // TIMG0->GEN_EVENT1.IMASK |= 1; // zero event mask
//   TIMG0->COMMONREGS.CCLKCTL = 1;
//   NVIC->ISER[0] = 1 << 16; // TIMG0 interrupt
//   NVIC->IP[4] = (NVIC->IP[4]&(~0x000000FF))|(priority<<6);    // set priority (bits 7,6) IRQ 16
//   TIMG0->COUNTERREGS.CTRCTL |= 0x01;
//   PeriodicTask0 =task;
// }
// power Domain PD0
// initialize G0/G8 for periodic interrupt
// for 32MHz bus clock, Timer clock is 32MHz
// for 40MHz bus clock, Timer clock is ULPCLK 20MHz
// for 80MHz bus clock, Timer clock is ULPCLK 40MHz
// frequency = TimerClock/prescale/period
void TimerG0_IntArm(uint16_t period, uint32_t prescale, uint32_t priority){
  TIMG0->GPRCM.RSTCTL = 0xB1000003;
  TIMG0->GPRCM.PWREN = 0x26000001;
  Clock_Delay(24); // time for TimerG0 to power up
  TIMG0->CLKSEL = 0x08; // bus clock
  TIMG0->CLKDIV = 0x00; // divide by 1
  TIMG0->COMMONREGS.CPS = prescale-1;     // divide by prescale,
  TIMG0->COUNTERREGS.LOAD  = period-1;     // set reload register
  TIMG0->COUNTERREGS.CTRCTL = 0x02;
  // bits 5-4 CM =0, down
  // bits 3-1 REPEAT =001, continue
  // bit 0 EN enable (0 for disable, 1 for enable)
  //TIMG0->INT_EVENT[0].IMASK |= 1; // zero event mask
  TIMG0->CPU_INT.IMASK = 1; // zero event mask
 // TIMG0->GEN_EVENT1.IMASK |= 1; // zero event mask
  TIMG0->COMMONREGS.CCLKCTL = 1;
  NVIC->ISER[0] = 1 << 16; // TIMG0 interrupt
  NVIC->IP[4] = (NVIC->IP[4]&(~0x000000FF))|(priority<<6);    // set priority (bits 7,6) IRQ 16
  TIMG0->COUNTERREGS.CTRCTL |= 0x01;
}






// initialize G12 for periodic interrupt, 32 bit
// no prescale
// frequency = TimerClock/period
void TimerG12_IntArm(uint32_t period, uint32_t priority){
  TIMG12->GPRCM.RSTCTL = (uint32_t)0xB1000003;
  TIMG12->GPRCM.PWREN = (uint32_t)0x26000001;
  Clock_Delay(24); // time for TimerG12 to power up
  TIMG12->CLKSEL = 0x08; // bus clock
  TIMG12->CLKDIV = 0x00; // divide by 1
  TIMG12->COUNTERREGS.LOAD  = period - 1;     // set reload register
  TIMG12->COUNTERREGS.CTRCTL = 0x02;
    // bits 5-4 CM =0, down
    // bits 3-1 REPEAT =001, continue
    // bit 0 EN enable (0 for disable, 1 for enable)
  TIMG12->CPU_INT.IMASK |= 1; // zero event mask
  TIMG12->COMMONREGS.CCLKCTL = 1;
  NVIC->ISER[0] = 1 << 21; // TIMG12 interrupt
  NVIC->IP[5] = (NVIC->IP[5]&(~0x0000FF00))|(priority<<14);    // set priority (bits 15,14) IRQ 21
  TIMG12->COUNTERREGS.CTRCTL |= 0x01;
}






// void TIMG0_IRQHandler(void){

//   if((TIMG0->CPU_INT.IIDX) == 1){ // this will acknowledge
//     (*PeriodicTask0)();                // execute user task
//   }
// }

//  void TIMG12_IRQHandler(void){

//    if((TIMG12->CPU_INT.IIDX) == 1){ // this will acknowledge
//      (*PeriodicTask12)();                // execute user task
//    }
//  }

void Timer1TimerG12_Stop(){
  TIMG12->COUNTERREGS.LOAD = 0;
}

void Timer1TimerG0_Stop(){
  TIMG0->COUNTERREGS.LOAD = 0;
}

// initialize G7 for periodic interrupt
// frequency = TimerClock/prescale/period
void TimerG7_IntArm(uint16_t period, uint32_t prescale, uint32_t priority){
  TIMG7->GPRCM.RSTCTL = (uint32_t)0xB1000003;
  TIMG7->GPRCM.PWREN = (uint32_t)0x26000001;
  Clock_Delay(24); // time for TimerG7 to power up
  TIMG7->CLKSEL = 0x08; // bus clock
  TIMG7->CLKDIV = 0x00; // divide by 1
  TIMG7->COMMONREGS.CPS = prescale-1;     // divide by prescale
  TIMG7->COUNTERREGS.LOAD  = period-1;    // set reload register
  TIMG7->COUNTERREGS.CTRCTL = 0x02;
    // bits 5-4 CM =0, down
    // bits 3-1 REPEAT =001, continue
    // bit 0 EN enable (0 for disable, 1 for enable)
  TIMG7->CPU_INT.IMASK |= 1; // zero event mask
  TIMG7->COMMONREGS.CCLKCTL = 1;
  NVIC->ISER[0] = 1 << 20; // TIMG7 interrupt
  NVIC->IP[5] = (NVIC->IP[5]&(~0x000000FF))|(priority<<6);    // set priority (bits 7,6) IRQ 20
  TIMG7->COUNTERREGS.CTRCTL |= 0x01;
}


