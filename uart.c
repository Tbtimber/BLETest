#include <stdio.h>
#include <stdlib.h>
#include <xc.h>
#include "general.h"
#include "uart.h"


#define FCY 16000000UL
#include "libpic30.h"




void resetBTModule() {
    //Branch the reset pin of module to ground
    TRISBbits.TRISB15 = 0;//0for output and 1 for input //RB15 is set in this case
    PORTBbits.RB15 = 0;
    __delay_ms(50);
    PORTBbits.RB15 = 1;
}

void UARTInit230400(void){
    CLKDIV = 0x0000;
//    TRISB = 0x0000;     // port B en sortie
    
    U1BRG = 0x0010; //pour mode fast
    
    
    U1MODE = 0b1000100000001000; //mode fast
    
    U1STA =  0b1000010100010000;
    
    /// timer/////
    T1CON = 0xA200;
    PR1 = 0x257F;
    /// end timer////
    
    IFS0bits.T1IF=0;
    
}

void UARTInit9600(void){
    CLKDIV = 0x0000;
//    TRISB = 0x0000;     // port B en sortie
    
    //U1BRG = 0x0010; pour mode fast
    U1BRG = 0x0067;
    
    //U1MODE = 0b1000100000001000; mode fast
    U1MODE = 0b1000100000000000;
    U1STA =  0b1000010100010000;
    
    /// timer/////
    T1CON = 0xA200;
    PR1 = 0x257F;
    /// end timer////
    
    IFS0bits.T1IF=0;
    
}

void UARTBasicCommand(){
    UARTWriteChar('A');
    UARTWriteChar('T');
    UARTWriteChar('+');
}

void UARTtestBT(){
    UARTWriteChar('T');
    UARTWriteChar('E');
    UARTWriteChar('S');
    UARTWriteChar('T');
    
}

void UARTgetBaudRate(){
    UARTBasicCommand();
    UARTWriteChar('B');
    UARTWriteChar('A');
    UARTWriteChar('U');
    UARTWriteChar('D');  
    UARTWriteChar('?');
}

void UARTConfigBaudRate215400(){
    UARTBasicCommand();
    UARTWriteChar('B');
    UARTWriteChar('A');
    UARTWriteChar('U');
    UARTWriteChar('D');
    UARTWriteChar('8');
}

void UARTGetFilter(){
    UARTBasicCommand();
    UARTWriteChar('F');
    UARTWriteChar('I');
    UARTWriteChar('L');
    UARTWriteChar('T');  
    UARTWriteChar('?');
}

void UARTGetADType(){
    UARTBasicCommand();
    UARTWriteChar('A');
    UARTWriteChar('D');
    UARTWriteChar('T');
    UARTWriteChar('Y');  
    UARTWriteChar('?');
}
void UARTSleep(){
    UARTBasicCommand();
    UARTWriteChar('S');
    UARTWriteChar('L');
    UARTWriteChar('E');
    UARTWriteChar('E');  
    UARTWriteChar('P');
}

void UARTWakeUp(){
    int i;
    for(i=0;i<83;i++){
        UARTWriteChar('C');
    }    
    UARTWriteChar('\0');
}

void UARTWriteChar(char data){
    while(U1STAbits.UTXBF == 1){}
    U1TXREG = data;
}

void UARTWriteByte(UINT8_T data){
    while(U1STAbits.UTXBF == 1){}
    U1TXREG = data;
}

//Set up UART at 9600 and wake up BT + set BT baud to 230400 then put the pic uart to 230400 and wake module again
void initBTModule() {
    UARTInit9600();
    UARTWakeUp();
    __delay_ms(10);
    UARTConfigBaudRate215400();
    __delay_ms(10);
    UARTSleep();
    __delay_ms(10);
    UARTInit230400();
    UARTWakeUp();
    __delay_ms(10);
}


/*
void UARTInit(UINT16_T baudRate) {

    TRISCbits.TRISC6 = INP;                     // RC6 is TxD (but should be declared first as input)
    TRISCbits.TRISC7 = INP;                     // RC7 is RxD
    ANSELCbits.ANSC7 = DISABLE;                 // disable analog input

    TXSTA1 = CLEAR;                             // reset USART registers
    RCSTA1 = CLEAR;

    TXSTA1bits.SYNC = CLEAR;                    // asynchronous mode
    TXSTA1bits.TX9 = CLEAR;                     // 8-bit mode
    RCSTA1bits.RX9 = CLEAR;
    RCSTA1bits.CREN = SET;                      // continous receive
    RCSTA1bits.ADDEN = SET;                     // address detect enable
    PIE1bits.RC1IE = SET;                       // interrupt on receive
    PIE1bits.TX1IE = CLEAR;                     // no interrupt on transmission

 * 
    TXSTA1bits.BRGH = SET;                      // baud rate select (high speed)
    BAUDCON1bits.BRG16 = SET;                   // see PIC18F44K22 Silicon Errata (5. Module: EUSART, page 2)
    SPBRG1 = (UINT8_T)(((_XTAL_FREQ / (4 * baudRate)) - 1));
    SPBRGH1 = 0;

//    SPBRG1 = (UINT8_T)((_XTAL_FREQ / baudRate) / 16 - 1);   // set baud rate divider

    TXSTA1bits.TXEN = ENABLE;                   // enable transmitter
    RCSTA1bits.SPEN = ENABLE;                   // enable receiver

    RCIF = CLEAR;                               // reset RX pin flag
//    RCIP = CLEAR;                               // not high priority
    RCIE = ENABLE;                              // enable RX interrupt
    PEIE = ENABLE;                              // enable pheripheral interrupt (serial port is a pheripheral)
}

void UARTWriteByte(UINT8_T data) {

    while (TXSTA1bits.TRMT == CLEAR);           // wait for transmission idle

    TXREG1 = data;
}

UINT8_T UARTReadByte(void) {

    return(RCREG1);
}

void interrupt low_priority SerialRxPinInterrupt() {

    if (PIR1bits.RCIF == 1) {               //check if the interrupt is caused by RX pin

        if (RCSTA1bits.FERR) {
            LCDGoto(0, 1);
            LCDWriteStr("Framing Error");
            exit(EXIT_FAILURE);
        } else if (RCSTA1bits.OERR) {
            LCDGoto(0, 1);
            LCDWriteStr("Overrun Error");
            exit(EXIT_FAILURE);
        } else {
            if(debut == 0){
                LCDPutChar(UARTReadByte());     // display the received character
                PIR1bits.RCIF = 0;              // clear rx flag
                }
            else{
                LCDPutByte(UARTReadByte());     // display the received character
                PIR1bits.RCIF = 0;              // clear rx flag
                }
        }
    }
}
*/

