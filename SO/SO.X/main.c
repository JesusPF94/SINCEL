/*
 * File:   OSMain.c
 * Author: David
 *
 * Created on 16 de enero de 2016, 10:40
 */


#include <xc.h>
#include <pic18f4550.h>
#include "Structure.h"
#include <stdio.h>
#include "types.h"
#include "configBits.h"

union myFloat{
    u8 c4[4];
    u16 int2[2];
    float flt;
};

static volatile unsigned long *y, *z;
static volatile unsigned long x;
static unsigned char dutyCycle1 = 1;
static unsigned char dutyCycle2 = 2;
static unsigned char dutyCycle3 = 3;
static unsigned char dutyCycle4 = 4;
static unsigned char dutyCycle5 = 5;
static unsigned char dutyCycle6 = 6;

void FunctionE(void){
    asm("nop");
    asm("nop");
    asm("nop");
    asm("nop");
    vTaskDelay(5);
    asm("nop");
    vTaskDelete();
}

void FunctionD(void){
    asm("nop");
    asm("nop");
    asm("nop");
    xTaskCreate(FunctionE,4,0);
    vTaskDelay(1);
    asm("nop");
    vTaskDelete();  
}

void FunctionC(void){
    asm("nop");
    asm("nop");
    ReadingMailBox(0,y); 
    x=*y;
    //xTaskCreate(FunctionD,3,2);
    //vTaskDelay(5);
    asm("nop");
    asm("nop");
    vTaskDelete();
}

void FunctionB(void){
    asm("nop");
    asm("nop");
    //xTaskCreate(FunctionC,2,10);
    //vTaskDelay(1);
    ReadingMailBox(0,y); 
    x=*y;
    asm("nop");
    asm("nop");
    ReadingMailBox(0,z);
    x=*z;
    asm("nop");
    DoneWritting(0);
    vTaskDelete();

}

void FunctionA(void){
    asm("nop");
    asm("nop");
    asm("nop");
    //vTaskDelay(1);
    xTaskCreate(FunctionB,1,10);
    mailBox_init_conection(1,0);
    //ReadingMailBox(1,y);
    WritingMailBox(1,1);
    WritingMailBox(2,1);
    x=*y;
    //WritingMailBox(68,1);
 
    //mailBox_init_conection(2,0);
    //WritingMailBox(6,2);
    //xTaskCreate(FunctionC,2,5);
    //WritingMailBox(96,1);
    DoneReading(1);
    //DoneWritting(2);
    
   // vTaskDelay(4);
    asm("nop");
    asm("nop");
    vTaskDelete();
}




void high_priority interrupt High_Priority_Interrupt(void){
        saveInterruptedState();
    if(INT0IE && INT0IF){ //interrupt CREATE
        //saveInterruptedState();
        INTCON &=  0xFD;
        asm("nop");
        xInterruptTaskCreate(&FunctionA,0,1);
    }
    
    if(TMR0IE && TMR0IF){ // interrupt TICK
        //saveInterruptedState();
        TMR0IF=0;
        reductionTime(); 
   }  
}



void PWM_init(){
    LATB = 0x00;
    TRISB = 0x00;
}

void PWM1(void){
    //Time base will be 100 micro secs 
    while(1){
        LATBbits.LATB0= 1;
        //PORTBbits.RB0 = 1;
        //LATBbits.LATB0 = 1;   // RB-0 to High   
		//LATBbits.LATB1 = 1;   // RB-1 to High 
        vTaskDelay(dutyCycle1); 
        
        LATBbits.LATB0= 0;
        //PORTBbits.RB0 = 0;
        vTaskDelay(10-dutyCycle1);   
    }
    vTaskDelete();
}

void PWM2(void){
    //Time base will be 100 micro secs 
    while(1){
        //PORTBbits.RB1 = 1;
        LATBbits.LATB1= 1;
        vTaskDelay(dutyCycle2); 
        
        //PORTBbits.RB1 = 0;
        LATBbits.LATB1= 0;
        vTaskDelay(10-dutyCycle2);   
    }
    vTaskDelete();
}

void PWM3(void){
    //Time base will be 100 micro secs 
    while(1){
        //PORTBbits.RB2 = 1;
        LATBbits.LATB2= 1;
        vTaskDelay(dutyCycle3); 
        
        //PORTBbits.RB2 = 0;
        LATBbits.LATB2= 0;
        vTaskDelay(10-dutyCycle3);   
    }
    vTaskDelete();
}

void PWM4(void){
    //Time base will be 100 micro secs 
    while(1){
        //PORTBbits.RB3 = 1;
        LATBbits.LATB3= 1;
        vTaskDelay(dutyCycle4); 
        
        //PORTBbits.RB3 = 0;
                LATBbits.LATB3= 0;
        vTaskDelay(10-dutyCycle4);   
    }
    vTaskDelete();
}

void PWM5(void){
    //Time base will be 100 micro secs 
    while(1){
       // PORTBbits.RB4 = 1;
               LATBbits.LATB4= 1;
        vTaskDelay(dutyCycle5); 
        
        //PORTBbits.RB4 = 0;
                LATBbits.LATB4= 0;
        vTaskDelay(10-dutyCycle5);   
    }
    vTaskDelete();
}

void floatMultiplication(unsigned long *result, myFloat data1, myFloat data2  ){
    myFloat resultado;
    resultado.c4[3] = ( data1.c4[3] & 0x80) ^ ( data2.c4[3] & 0x80)  ;      //signo
    
    
    
}

void PWM6(void){
    //Time base will be 100 micro secs 
    while(1){
      //  PORTBbits.RB5 = 1;
                LATBbits.LATB5= 1;
        vTaskDelay(dutyCycle6); 
        
        //PORTBbits.RB5 = 0;
                LATBbits.LATB5= 0;
        vTaskDelay(10-dutyCycle6);   
    }
    vTaskDelete();
}

void PWM_activator(void){
    xTaskCreate(&PWM1, 1,5);
    xTaskCreate(&PWM2, 2,5);
    xTaskCreate(&PWM3, 3,5);
    xTaskCreate(&PWM4, 4,5);
    xTaskCreate(&PWM5, 5,5);
    xTaskCreate(&PWM6, 6,5);
    asm("nop");
    vTaskDelete();
}

void low_priority interrupt Low_Priority_Interrupt(void) {
    //DUMMY call
   FunctionA();     FunctionB();     FunctionC();    FunctionD();    FunctionE();
    
    PWM_init();
    PWM1();PWM2();PWM3();PWM4();PWM5();PWM6();
    PWM_activator();
    
}

void main(void) {
    
    /*TRISB = 0xF0 ; // PORT B Setting: Set all the pins in port B to Output. 
    TRISA = 0xF0 ; // PORT B Setting: Set all the pins in port B to Output. 
    PORTBbits.RB0 = 1;
    LATBbits.LATB0 = 1;   // RB-0 to High   
	LATBbits.LATB1 = 1;   // RB-1 to High 
    LATAbits.LATA0 = 1;   // RB-0 to High   */

    
    OSInit();
    GIE=ON;
    //PWM_init();
    //xTaskCreate(&PWM_activator,0,0);
    //INTCON = 0x02;    
    
   INTCON |=  0x12;
    asm("nop");
    asm("nop");
    do{
        asm("nop");
    }while(1);    
    return;
}
