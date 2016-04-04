/* Microchip Technology Inc. and its subsidiaries.  You may use this software 
 * and any derivatives exclusively with Microchip products. 
 * 
 * THIS SOFTWARE IS SUPPLIED BY MICROCHIP "AS IS".  NO WARRANTIES, WHETHER 
 * EXPRESS, IMPLIED OR STATUTORY, APPLY TO THIS SOFTWARE, INCLUDING ANY IMPLIED 
 * WARRANTIES OF NON-INFRINGEMENT, MERCHANTABILITY, AND FITNESS FOR A 
 * PARTICULAR PURPOSE, OR ITS INTERACTION WITH MICROCHIP PRODUCTS, COMBINATION 
 * WITH ANY OTHER PRODUCTS, OR USE IN ANY APPLICATION. 
 *
 * IN NO EVENT WILL MICROCHIP BE LIABLE FOR ANY INDIRECT, SPECIAL, PUNITIVE, 
 * INCIDENTAL OR CONSEQUENTIAL LOSS, DAMAGE, COST OR EXPENSE OF ANY KIND 
 * WHATSOEVER RELATED TO THE SOFTWARE, HOWEVER CAUSED, EVEN IF MICROCHIP HAS 
 * BEEN ADVISED OF THE POSSIBILITY OR THE DAMAGES ARE FORESEEABLE.  TO THE 
 * FULLEST EXTENT ALLOWED BY LAW, MICROCHIP'S TOTAL LIABILITY ON ALL CLAIMS 
 * IN ANY WAY RELATED TO THIS SOFTWARE WILL NOT EXCEED THE AMOUNT OF FEES, IF 
 * ANY, THAT YOU HAVE PAID DIRECTLY TO MICROCHIP FOR THIS SOFTWARE.
 *
 * MICROCHIP PROVIDES THIS SOFTWARE CONDITIONALLY UPON YOUR ACCEPTANCE OF THESE 
 * TERMS. 
 */

/* 
 * File:   
 * Author: 
 * Comments:
 * Revision history: 
 */

// This is a guard condition so that contents of this file are not included
// more than once.  
#ifndef XC_HEADER_TEMPLATE_H
#define	XC_HEADER_TEMPLATE_H

#include <xc.h> // include processor files - each processor file is guarded.  
#include <pic18f4550.h>

#define TASKNUMBER 11

#define SUSPEND         0
#define WAIT            1
#define READY           2
#define INTERRUPTED     3
#define RUNNING         4

#define ON              1
#define OFF             0

#define     NOT_WAITING         0
#define     WAITING_TO_WRITE    1
#define     WAITING_TO_READ     2


#define vTaskStartScheduler() schedulerStatus=1
#define vTaskEndScheduler()   schedulerStatus=0

////////Prescale Divisor Select
#define DivBy2 			0x00
#define DivBy4			0x01
#define DivBy8			0x02
#define DivBy16			0x03
#define DivBy32 		0x04
#define DivBy64			0x05
#define DivBy128		0x06
#define DivBy256		0x07

////////Clk Select
#define MHz8 			0x70
#define MHz4			0x60
#define MHz2			0x50
#define MHz1			0x40
#define KHz500          0x30
#define KHz260         	0x20
#define KHz125      	0x10
#define KHz31   		0x00

//System Clock Select
#define InterOsc        0x02;
#define Timer1Osc       0x01;
#define PrimaryOsc      0x00;

////////Timer0 others bits
#define Bits8               1
#define ExternalClk         1
#define Bits16              0
#define InternalClk         0
#define LowtoHigh           0
#define HightoLow           1
#define PreescalerOff       1
#define PreescalerOn        0

void vTaskDelay(unsigned int delayms);

void xTaskCreate(unsigned long pvTaskCode, unsigned char Id, unsigned char Prior);

void xInterruptTaskCreate(unsigned long pvTaskCode, unsigned char Id, unsigned char Prior);

void vTaskDelete(void);

void OSRun(void);

void OSInit();

void mailBox_init_conection(unsigned char reader, unsigned char writer);

void WritingMailBox(unsigned char data, unsigned char reader);

void ReadingMailBox(unsigned char writter, unsigned long *r);

void waitForBuffer(void);

void DoneWritting(unsigned char reader);

void DoneReading(unsigned char writter);

char mailBoxHasData(char mailboxIndex);

    static char s_STATUSBackup=255;
    static char s_WREGBackup=255;
    static char s_BSRBackup=255;
// TODO Insert appropriate #include <>

// TODO Insert C++ class definitions if appropriate

// TODO Insert declarations

// Comment a function and leverage automatic documentation with slash star star
/**
    <p><b>Function prototype:</b></p>
  
    <p><b>Summary:</b></p>

    <p><b>Description:</b></p>

    <p><b>Precondition:</b></p>

    <p><b>Parameters:</b></p>

    <p><b>Returns:</b></p>

    <p><b>Example:</b></p>
    <code>
 
    </code>

    <p><b>Remarks:</b></p>
 */
// TODO Insert declarations or function prototypes (right here) to leverage 
// live documentation

#ifdef	__cplusplus
extern "C" {
#endif /* __cplusplus */

    // TODO If C++ is being used, regular C code needs function names to have C 
    // linkage so the functions can be used by the c code. 

#ifdef	__cplusplus
}
#endif /* __cplusplus */

#endif	/* XC_HEADER_TEMPLATE_H */
