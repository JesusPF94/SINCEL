#include <xc.h>
#include <pic18f4550.h>
#include "Structure.h"
#include <stdio.h>
/*
#pragma config FOSC = INTOSC_HS // Oscilador interno, uso de pin RA6 activado, El USB usa Clock Externo.
#pragma config WDT = OFF //desactivamos watchdog timer
#pragma config PWRT = OFF // activamos power up 
#pragma config LVP = OFF 
#pragma STVREN = ON //
*/

// CONFIG1L
#pragma config PLLDIV = 1       // PLL Prescaler Selection bits (No prescale (4 MHz oscillator input drives PLL directly))
#pragma config CPUDIV = OSC4_PLL6// System Clock Postscaler Selection bits ([Primary Oscillator Src: /4][96 MHz PLL Src: /6])
#pragma config USBDIV = 1       // USB Clock Selection bit (used in Full-Speed USB mode only; UCFG:FSEN = 1) (USB clock source comes directly from the primary oscillator block with no postscale)

// CONFIG1H
#pragma config FOSC = INTOSCIO_EC// Oscillator Selection bits (Internal oscillator, port function on RA6, EC used by USB (INTIO))
#pragma config FCMEN = OFF      // Fail-Safe Clock Monitor Enable bit (Fail-Safe Clock Monitor disabled)
#pragma config IESO = OFF       // Internal/External Oscillator Switchover bit (Oscillator Switchover mode disabled)

// CONFIG2L
#pragma config PWRT = OFF       // Power-up Timer Enable bit (PWRT disabled)
#pragma config BOR = OFF        // Brown-out Reset Enable bits (Brown-out Reset disabled in hardware and software)
#pragma config BORV = 3         // Brown-out Reset Voltage bits (Minimum setting)
#pragma config VREGEN = OFF     // USB Voltage Regulator Enable bit (USB voltage regulator disabled)

// CONFIG2H
#pragma config WDT = OFF        // Watchdog Timer Enable bit (WDT disabled (control is placed on the SWDTEN bit))
#pragma config WDTPS = 32768    // Watchdog Timer Postscale Select bits (1:32768)

// CONFIG3H
#pragma config CCP2MX = ON      // CCP2 MUX bit (CCP2 input/output is multiplexed with RC1)
#pragma config PBADEN = OFF     // PORTB A/D Enable bit (PORTB<4:0> pins are configured as digital I/O on Reset)
#pragma config LPT1OSC = OFF    // Low-Power Timer 1 Oscillator Enable bit (Timer1 configured for higher power operation)
#pragma config MCLRE = ON       // MCLR Pin Enable bit (MCLR pin enabled; RE3 input pin disabled)

// CONFIG4L
#pragma config STVREN = OFF     // Stack Full/Underflow Reset Enable bit (Stack full/underflow will not cause Reset)
#pragma config LVP = OFF        // Single-Supply ICSP Enable bit (Single-Supply ICSP disabled)
#pragma config XINST = OFF      // Extended Instruction Set Enable bit (Instruction set extension and Indexed Addressing mode disabled (Legacy mode))



static volatile unsigned long *y, *z;
static volatile unsigned long x;

//PWM
static unsigned char dutyCycle1 = 1;
static unsigned char dutyCycle2 = 2;
static unsigned char dutyCycle3 = 3;
static unsigned char dutyCycle4 = 4;
static unsigned char dutyCycle5 = 5;
static unsigned char dutyCycle6 = 6;

//Serial
static unsigned char CN1 = 8;
static unsigned char valor1 = 0;

static unsigned char CN2 = 8;
static unsigned char valor2 = 0;




void extInt(void){
    //enable
    INTCONbits.INT0IE=0;//disable
    INTCON3bits.INT1IE=1;
    INTCON3bits.INT2IE=1;
    
    //priority
    INTCON3bits.INT1IP=0;
    INTCON3bits.INT2IP=0;
    
    INTCON2bits.INTEDG0=0;
    INTCON2bits.INTEDG1=0;
    INTCON2bits.INTEDG2=0;
    //INTCON2bits.RBPU = 0;
}


void ports_Init(){
    ADCON1bits.PCFG = 0xFF; // all portB digital
    LATB = 0x00; //dato 0
    TRISB = 0x00;// outputs
    
    //inputs
    TRISBbits.RB0=1;//ext interrupt 0
    TRISBbits.RB1=1;//ext interrupt 1
    TRISBbits.RB2=1;//ext interrupt 2
        
}


void RX1(void){
    CN1 = 8;
    valor1 = 0;
    vTaskDelay(1);
    do
    {
        valor1>>=1;
        if(PORTBbits.RB1)
        {
            valor1|=(0x80);
        }
       vTaskDelay(4);
    }
    while(CN1--);
    vTaskDelay(4);
    WritingMailBox(valor1,0);
    INTCON3bits.INT1IF = 0; //clear ext interrupt 1 flag
    INTCON3bits.INT1IE = 1; //enable ext Interrupt 1 
    /*do{
    LATB3=1;
    vTaskDelay(1);
    LATB3=0;
    vTaskDelay(1);
    }while(1);*/
    vTaskDelete();
}

void PWM1(void){
    //Time base will be 100 micro secs 
   mailBox_init_conection(0,1);
   ReadingMailBox(1,y);
   asm("nop");
   dutyCycle1=y;
    while(1){
        LATB3= 1;
        //PORTBbits.RB0 = 1;
        //LATBbits.LATB0 = 1;   // RB-0 to High   
		//LATBbits.LATB1 = 1;   // RB-1 to High 
        vTaskDelay(dutyCycle1); 
        
        LATB3= 0;
        //PORTBbits.RB0 = 0;
        vTaskDelay(100-dutyCycle1);   
    }
    vTaskDelete();
}

void high_priority interrupt High_Priority_Interrupt(void){
    saveInterruptedState();
    if(TMR1IE && TMR1IF){ // interrupt TICK
        TMR1IF=0;
        TMR1H = 0xFA;           // for 
        TMR1L = 0x00; 
        reductionTime(); 
   }  
}


void interrupt low_priority Low_Priority_Interrupt(void) {
    saveInterruptedState();
    
    if(INT1IE && INT1IF){
        //LATB3=1; // prueba que entre
        INT1IF = 0; //clear interrupt 1 flag
        INT1IE = 0; //Disable interrupt 1 (RB1 back to normal)
        //LATB3=0;
        xInterruptTaskCreate(RX1,1,1);
    }
    
    if(INT2IE && INT2IF){
        INT2IF = 0; //clear interrupt 1 flag
        INT2IE = 0; //Disable interrupt 1 (RB1 back to normal)
        asm("nop");
        //xInterruptTaskCreate(RX2,3,2);
    }

    
}



void main(void){
   asm("nop");
   OSInit();
   ports_Init();
   extInt();
   RCONbits.IPEN=1;
   INTCONbits.PEIE_GIEL=1;
   INT1IF=0;
   //INT2IF=0;
   GIE=ON;
   xTaskCreate(PWM1,0,1);
   //xTaskCreate(PWM2,2,1);
   //INT1IF=1;
   
    do{
        asm("nop");
    }while(1); 
     //DUMMY call
    RX1(); 
    PWM1(); 
    //RX2(); PWM2();
    return;
}


