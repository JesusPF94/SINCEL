#include "Structure.h"

static unsigned char taskPosition=0;
static unsigned char actualPosition=10;           //This is the indexOf the process executing
static unsigned char schedulerAdd1=0;
static unsigned char schedulerAdd2=0;
static unsigned char schedulerAdd3=0;
static unsigned char schedulerStatus = ON;
static unsigned long *r_pointer;
//Global modifications
static unsigned char i;
static unsigned char i_mic;
static unsigned char i_wmb;
static unsigned char i_rmb;
static unsigned char i_dw;
static unsigned char i_dr;

static unsigned char index=0;
static unsigned char index_mic=0;
static unsigned char index_wmb;
static unsigned char index_rmb;
static unsigned char index_dw;
static unsigned char index_dr;

static unsigned char minPrior=255;
static unsigned char cont=0;
static unsigned char iAmDone=0;



struct Task{
    unsigned char Add1;
    unsigned char Add2;                          //This is where it starts
    unsigned char Add3;
    unsigned char state;
    unsigned char Id;                           //THIS IS THE SAME AS THE INDEX
    unsigned char Priority;
    unsigned char currentAdd1;
    unsigned char currentAdd2;                    //donde se quedo
    unsigned char currentAdd3;  
    
    //Interrupciones
    unsigned char backupStatus;
    unsigned char backupWreg;                   //INTERRUP REGISTERS SAVED
    unsigned char backupBsr;
    
    //Tick y delays
    unsigned int DelayTime;
    unsigned char Delayed;      //Waiting for time o como le quieran poner
    
    unsigned char WaitMessage;
    
    
    unsigned char MessageWritter;
    unsigned char MessageReader;
    unsigned char MessageData;
    unsigned char readInterruptedIndex;
    unsigned char writeInterruptedIndex;
    
    unsigned long *r_dataPTR;
};

struct Task TaskArray[TASKNUMBER];

struct MailBox {
    unsigned char Reader;
    unsigned char Writer;
    unsigned char Available;
    unsigned char Data;
    unsigned char WriterDone;
    unsigned char ReaderDone;
    unsigned char Connected;
    unsigned char ActualConection;
};

struct MailBox MailBoxArray[TASKNUMBER];

void mailBox_init_conection(unsigned char reader, unsigned char writer ){
    GIE = OFF;
    index_mic=10;
    for(i_mic=0;i_mic<TASKNUMBER;i_mic++){
        if(MailBoxArray[i_mic].Available==OFF && MailBoxArray[i_mic].Connected==OFF){
            index_mic=i_mic;   
            i_mic=100;
        }
    }
    TaskArray[actualPosition].MessageReader=reader;
    MailBoxArray[index_mic].Reader=reader;
    MailBoxArray[index_mic].Writer=writer;
    MailBoxArray[index_mic].Connected=ON;  
}


char mailBoxHasData(char mailboxIndex)
{
    return MailBoxArray[mailboxIndex].Available ;
}
void WritingMailBox(unsigned char data, unsigned char reader){
    GIE = OFF;
    index_wmb=10;
    for(i_wmb=0;i_wmb<TASKNUMBER;i_wmb++){
        if(MailBoxArray[i_wmb].Writer==actualPosition && 
           MailBoxArray[i_wmb].Reader==reader){            
            index_wmb=i_wmb;
            i_wmb=100; // para salir de for.
            TaskArray[actualPosition].writeInterruptedIndex = index_wmb;
            TaskArray[actualPosition].MessageData=data;
            if(MailBoxArray[index_wmb].Available==ON){
            //if( MailBoxArray[  TaskArray[actualPosition].writeInterruptedIndex  ].Available==ON){
                TaskArray[actualPosition].MessageReader=reader;
                TaskArray[actualPosition].MessageData=data;
                TaskArray[actualPosition].WaitMessage = WAITING_TO_WRITE;
               
                waitForBuffer();
            }
        }
    }
    
    MailBoxArray[ TaskArray[actualPosition].writeInterruptedIndex ].Data=  TaskArray[actualPosition].MessageData;
    MailBoxArray[  TaskArray[actualPosition].writeInterruptedIndex  ].Available=ON;
    if(TaskArray[ MailBoxArray[  TaskArray[actualPosition].writeInterruptedIndex  ].Reader ].state == WAIT && TaskArray[ MailBoxArray[  TaskArray[actualPosition].writeInterruptedIndex  ].Reader].WaitMessage == WAITING_TO_READ){
        TaskArray[ MailBoxArray[  TaskArray[actualPosition].writeInterruptedIndex  ].Reader ].state = READY;
        TaskArray[ MailBoxArray[  TaskArray[actualPosition].writeInterruptedIndex  ].Reader].WaitMessage = NOT_WAITING;
        
        TaskArray[actualPosition].currentAdd3=TOSU;
        TaskArray[actualPosition].currentAdd2=TOSH;
        TaskArray[actualPosition].currentAdd1=TOSL;
        STKPTR--;
        OSRun();
       
    }
}
void ReadingMailBox(unsigned char writter, unsigned long *r){
    GIE = OFF;
    //TaskArray[actualPosition].r_dataPTR = r;
    r_pointer = r;
    for(i_rmb=0;i_rmb<TASKNUMBER;i_rmb++){
        if(MailBoxArray[i_rmb].Reader==actualPosition && 
           MailBoxArray[i_rmb].Writer==writter){
            index_rmb=i_rmb;
            i_rmb=100; // para salir de for.
            TaskArray[actualPosition].readInterruptedIndex = index_rmb;
            TaskArray[actualPosition].MessageData=MailBoxArray[index_rmb].Data;
            if(MailBoxArray[index_rmb].Available==OFF){
                TaskArray[actualPosition].MessageWritter=writter;
                TaskArray[actualPosition].WaitMessage = WAITING_TO_READ;
                TaskArray[actualPosition].state = WAIT;
                waitForBuffer();
            }
           MailBoxArray[ TaskArray[actualPosition].readInterruptedIndex ].Available=OFF;
           TaskArray[actualPosition].MessageData = MailBoxArray[ TaskArray[actualPosition].readInterruptedIndex ].Data;
           i_rmb = TaskArray[actualPosition].readInterruptedIndex;
           //r = 0x3B;
           asm("nop");
           //*TaskArray[actualPosition].r_dataPTR =MailBoxArray[i_rmb].Data;
           *r_pointer = MailBoxArray[i_rmb].Data;
           
           if(  (TaskArray[MailBoxArray[i_rmb].Writer].MessageReader == actualPosition)  && TaskArray[ MailBoxArray[  i_rmb  ].Writer ].state == WAIT && TaskArray[ MailBoxArray[  i_rmb  ].Writer].WaitMessage == WAITING_TO_WRITE){
               TaskArray[ MailBoxArray[  i_rmb  ].Writer ].state = READY;
               TaskArray[ MailBoxArray[  i_rmb  ].Writer].WaitMessage = NOT_WAITING;
               //STKPTR--;
               TaskArray[actualPosition].currentAdd3=TOSU;
                TaskArray[actualPosition].currentAdd2=TOSH;
                TaskArray[actualPosition].currentAdd1=TOSL;
                STKPTR--;
               OSRun();
           }
           
           //MailBoxArray[ TaskArray[actualPosition].readInterruptedIndex ].Data= TaskArray[actualPosition].MessageData;
           //return MailBoxArray[ TaskArray[actualPosition].readInterruptedIndex ].Data;
        }
    }
}

void waitForBuffer(void){
    GIE = OFF;
    TaskArray[actualPosition].state = WAIT;
    //STKPTR -=1;
    TaskArray[actualPosition].currentAdd3 = TOSU;
    TaskArray[actualPosition].currentAdd2 = TOSH;
    TaskArray[actualPosition].currentAdd1 = TOSL;
    STKPTR++;
    TOS = &OSRun;
}

void DoneWritting(unsigned char reader){
    GIE = OFF;
    index_dw=10;
    for(i_dw=0;i_dw<TASKNUMBER;i_dw++){
        if(MailBoxArray[i_dw].Reader==reader && MailBoxArray[i_dw].Writer==actualPosition){
            index_dw=i_dw;
        }   
    }
    MailBoxArray[index_dw].WriterDone=ON;
    if(MailBoxArray[index_dw].ReaderDone==ON){
        MailBoxArray[index_dw].Connected=OFF;
    }
}

void DoneReading(unsigned char writter){
    GIE = OFF;
    index_dr=10;
    for(i_dr=0;i_dr<TASKNUMBER;i_dr++){
        if(MailBoxArray[i_dr].Writer==writter && MailBoxArray[i_dr].Reader==actualPosition){
            index_dr=i_dr;
        }   
    }
    MailBoxArray[index_dr].ReaderDone=ON;
    if(MailBoxArray[index_dr].WriterDone==ON){
        MailBoxArray[index_dr].Connected=OFF;
    }
}

void vTaskDelay(unsigned int delayms){
    INTCONbits.PEIE_GIEL=OFF;
    GIE=OFF;
    TaskArray[actualPosition].currentAdd3=TOSU;
    TaskArray[actualPosition].currentAdd2=TOSH;
    TaskArray[actualPosition].currentAdd1=TOSL;
     
    TaskArray[actualPosition].state=WAIT;
    TaskArray[actualPosition].DelayTime=delayms;
    TaskArray[actualPosition].Delayed=ON;
    cont++;
    T1CONbits.TMR1ON=1;          //Timer1 enable 
    TMR1IE=ON;
    STKPTR--;
    OSRun();
}

void reductionTime(void){
    for(i=0;i<TASKNUMBER;i++){
        if(TaskArray[i].Delayed==ON){
            TaskArray[i].DelayTime--;
            if(TaskArray[i].DelayTime==0){
                cont--;
                TaskArray[i].state=READY;
                TaskArray[i].Delayed=OFF;
            }
        }
    }
    if(cont==0){
        TMR1IE=OFF;
        T1CONbits.TMR1ON=1;          //Timer1 enable 
    }
   //STKPTR--;
   TOS = &OSRun; 
   asm("RETFIE");
    //OSRun();     
}

void saveInterruptedState(){
     // asm("nop");
      //asm("movwf 0x36");          //wreg is at 36
      //asm("movff 0x0FD8,0x35");   //status is 35
      //asm("movff 0x0FE0,0x34");   //BSR is 34
    TaskArray[actualPosition].backupWreg   = s_WREGBackup;          //8bits    
    TaskArray[actualPosition].backupStatus = s_STATUSBackup;        //8bits
    TaskArray[actualPosition].backupBsr    = s_BSRBackup;           //8bits
    
    //save PC the stack should be a position less due to comming from int
    TaskArray[actualPosition].state = INTERRUPTED;
    STKPTR--;
    TaskArray[actualPosition].currentAdd3 = TOSU;
    TaskArray[actualPosition].currentAdd2 = TOSH;
    TaskArray[actualPosition].currentAdd1 = TOSL;
    STKPTR++;
    //("RETFIE");
}

void xTaskCreate(unsigned long pvTaskCode, unsigned char Id, unsigned char Prior){
    INTCONbits.PEIE_GIEL=OFF;
    GIE=OFF;
    //HEre we save the lastest current PC
    TaskArray[actualPosition].currentAdd3=TOSU;
    TaskArray[actualPosition].currentAdd2=TOSH;
    TaskArray[actualPosition].currentAdd1=TOSL;
    
    taskPosition=Id;
    TaskArray[taskPosition].Id=Id;
    TaskArray[taskPosition].state=READY;
    TaskArray[taskPosition].Add3=(pvTaskCode & 0x00ff0000) >> 16;
    TaskArray[taskPosition].Add2=(pvTaskCode & 0x0000ff00) >> 8;
    TaskArray[taskPosition].Add1=(pvTaskCode & 0x000000ff);
    TaskArray[taskPosition].currentAdd3=(pvTaskCode & 0x00ff0000) >> 16;
    TaskArray[taskPosition].currentAdd2=(pvTaskCode & 0x0000ff00) >> 8;
    TaskArray[taskPosition].currentAdd1=(pvTaskCode & 0x000000ff);
    TaskArray[taskPosition].Priority=Prior;
   // if(TaskArray[actualPosition].Priority >= Prior){
       
   // }
    STKPTR--;
    OSRun();
}

void xInterruptTaskCreate(unsigned long pvTaskCode, unsigned char Id, unsigned char Prior){
    INTCONbits.PEIE_GIEL=OFF;
    GIE=OFF;
  //  asm("MOVWF xInterruptTaskCreate@A");
  //  asm("MOVFF STATUS, xInterruptTaskCreate@B");
  //  asm("MOVFF BSR, xInterruptTaskCreate@C");
    
   // TaskArray[actualPosition].currentAdd3=TOSU;
   // TaskArray[actualPosition].currentAdd2=TOSH;
   // TaskArray[actualPosition].currentAdd1=TOSL;
    //HEre we save the lastest current PC
    TaskArray[Id].Id=Id;
    TaskArray[Id].state=READY;
    TaskArray[Id].Add3=(pvTaskCode & 0x00ff0000) >> 16;
    TaskArray[Id].Add2=(pvTaskCode & 0x0000ff00) >> 8;
    TaskArray[Id].Add1=(pvTaskCode & 0x000000ff);
    TaskArray[Id].currentAdd3=(pvTaskCode & 0x00ff0000) >> 16;
    TaskArray[Id].currentAdd2=(pvTaskCode & 0x0000ff00) >> 8;
    TaskArray[Id].currentAdd1=(pvTaskCode & 0x000000ff);
    TaskArray[Id].Priority=Prior;
 /*   if(TaskArray[actualPosition].Priority >= Prior    && TaskArray[actualPosition].state == RUNNING){
        TaskArray[actualPosition].currentAdd3=TOSU;
        TaskArray[actualPosition].currentAdd2=TOSH;
        TaskArray[actualPosition].currentAdd1=TOSL;
    }*/
    //STKPTR--;
    TOS = &OSRun;
 asm("RETFIE");
    //push the OSRun Direction
    //OSRun();
}


void vTaskDelete(void){
     INTCONbits.PEIE_GIEL=OFF;
    GIE=OFF;
    TaskArray[actualPosition].state=SUSPEND;
   // PCLATU = TaskArray[schedulerAdd3].currentAdd3;                  //Regresamos al Scheduler en posición más alta.
   // PCLATH = TaskArray[schedulerAdd2].currentAdd2;                 //Scheduler posición de enmedio.
   STKPTR--;
    OSRun();
   // STKPTR -= 3;
   // PCL = TaskArray[schedulerAdd1].currentAdd1;
    //Here we should adjust the }

}

void OSInit(){
    INTCONbits.PEIE_GIEL=OFF;
    GIE=OFF;
    for(i=0;i<TASKNUMBER ; i++){
        TaskArray[i].Priority=255;
        TaskArray[i].Id=i;
        TaskArray[i].state=SUSPEND;
        MailBoxArray[i].Available=OFF;
        MailBoxArray[i].Connected=OFF;
    }
    
    OSCCONbits.IDLEN=0;//idle mode in sleep
    OSCCONbits.IRCF2=1;//Usamos el reloj de X MHz/KHz
    OSCCONbits.IRCF1=1;//Usamos el reloj de X MHz/KHz
    OSCCONbits.IRCF0=1;//Usamos el reloj de X MHz/KHz
    OSCCONbits.OSTS=0;//Oscillator Start-up Timer time-out is running; primary oscillator is not ready
    OSCCONbits.IOFS=1;//INTOSC frequency is not stable
    OSCCONbits.SCS1=1;//Select clock Source
    OSCCONbits.SCS0=1;//Select clock Source
    
    //Timmer
    /*
    T0CONbits.TMR0ON=ON;
    T0CONbits.T08BIT=Bits8;
    T0CONbits.T0CS=InternalClk;
    T0CONbits.T0SE=LowtoHigh;
    T0CONbits.PSA=PreescalerOn;
    T0CON|= DivBy256;              //Dividimos en 256 el Preescaler
    T0CONbits.T0PS=MHz8;
    T0CONbits.PEIE=1;     //Enable Peripheral Interrupt
    T0CONbits.TMR0ON=1;      //Now start the timer!
     * */
    T1CON = 0;
    T1CONbits.RD16=1;       //Enables register read/write of Timer1 in one 16-bit operation
    T1CONbits.T1RUN=0;      //Device clock is derived from another source
    T1CONbits.T1CKPS1= 0;  // T1CKPS = 11 = 1:8 prescaler
    T1CONbits.T1CKPS0= 0;  // so timer clock = 1MHz / 8 = 125kHz
    T1CONbits.T1OSCEN=0;    //Timer1 oscillator is ahut off
    T1CONbits.T1SYNC=1;     //Do not synchronize external clock input
    T1CONbits.TMR1CS=0;     //Internal clock (FOSC/4)
    
    TMR1H = 0xFA;           // 65,535 - 12,500 = 53035 : 0xCF2B
    TMR1L = 0x00;           // to count for 100ms (0.100 seconds) till timer rollover
    
   
    //INTCONbits.GIE_GIEH = 1;     // Enable global interrupts
    //INTCONbits.PEIE_GIEL = 1;    // Enable peripheral interrupts
    
    PIR1bits.TMR1IF = 0;         // Clear Timer 1 interrupt flag
    PIE1bits.TMR1IE = 1;         // Enable Timer 1 interrupt  
    
  
}


void OSRun(void){
    INTCONbits.PEIE_GIEL=OFF;
    GIE=OFF;
    index = 10;
    minPrior = 255;
    if(schedulerStatus == ON){    
    for(i=0;i<TASKNUMBER ; i++){
        if(TaskArray[i].Priority < minPrior && (TaskArray[i].state==READY || TaskArray[i].state==RUNNING || TaskArray[i].state==INTERRUPTED)){  
            //The previous IF could been change to state!=WAIT or suspend 
            index=i;
            minPrior = TaskArray[index].Priority;
        } 
    }

 /*   if(TaskArray[index].state==INTERRUPTED){
        WREG   = TaskArray[index].backupWreg;
        STATUS = TaskArray[index].backupStatus;
        BSR    = TaskArray[index].backupBsr;
    }*/
       TaskArray[index].state=RUNNING; 
       //STKPTR--;                                     //Each time we jump to osRun, the previous PC is saved into the stack, to prevent overflow make adjust
       actualPosition=index;
       //PCLATU = TaskArray[index].currentAdd3;
       //PCLATH = TaskArray[index].currentAdd2;         //Here we should jump
       TOSH = TaskArray[index].currentAdd2;   
       TOSU = TaskArray[index].currentAdd3;
       TOSL = TaskArray[index].currentAdd1;
    
    if(TaskArray[index].state==INTERRUPTED){
        STATUS = TaskArray[index].backupStatus;
        BSR    = TaskArray[index].backupBsr;
        WREG   = TaskArray[index].backupWreg;
    }
       INTCONbits.PEIE_GIEL=ON;
       GIE = ON;


       //asm();
       //PCL = TaskArray[index].currentAdd1;    
    //we save the pc which is pointing the following app, we increase it by two to point 
    //here we jump to the next task  
    }
}



