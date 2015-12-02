/**
  EUSART Generated Driver File

  @Company
    Microchip Technology Inc.

  @File Name
    eusart.c

  @Summary
    This is the generated driver implementation file for the EUSART driver using MPLAB® Code Configurator

  @Description
    This header file provides implementations for driver APIs for EUSART.
    Generation Information :
        Product Revision  :  MPLAB® Code Configurator - v2.25.2
        Device            :  PIC16F1509
        Driver Version    :  2.00
    The generated drivers are tested against the following:
        Compiler          :  XC8 v1.34
        MPLAB             :  MPLAB X v2.35 or v3.00
 */

/*
Copyright (c) 2013 - 2015 released Microchip Technology Inc.  All rights reserved.

Microchip licenses to you the right to use, modify, copy and distribute
Software only when embedded on a Microchip microcontroller or digital signal
controller that is integrated into your product or third party product
(pursuant to the sublicense terms in the accompanying license agreement).

You should refer to the license agreement accompanying this Software for
additional information regarding your rights and obligations.

SOFTWARE AND DOCUMENTATION ARE PROVIDED "AS IS" WITHOUT WARRANTY OF ANY KIND,
EITHER EXPRESS OR IMPLIED, INCLUDING WITHOUT LIMITATION, ANY WARRANTY OF
MERCHANTABILITY, TITLE, NON-INFRINGEMENT AND FITNESS FOR A PARTICULAR PURPOSE.
IN NO EVENT SHALL MICROCHIP OR ITS LICENSORS BE LIABLE OR OBLIGATED UNDER
CONTRACT, NEGLIGENCE, STRICT LIABILITY, CONTRIBUTION, BREACH OF WARRANTY, OR
OTHER LEGAL EQUITABLE THEORY ANY DIRECT OR INDIRECT DAMAGES OR EXPENSES
INCLUDING BUT NOT LIMITED TO ANY INCIDENTAL, SPECIAL, INDIRECT, PUNITIVE OR
CONSEQUENTIAL DAMAGES, LOST PROFITS OR LOST DATA, COST OF PROCUREMENT OF
SUBSTITUTE GOODS, TECHNOLOGY, SERVICES, OR ANY CLAIMS BY THIRD PARTIES
(INCLUDING BUT NOT LIMITED TO ANY DEFENSE THEREOF), OR OTHER SIMILAR COSTS.
 */

/**
  Section: Included Files
 */
#include "eusart.h"

/**
  Section: Macro Declarations
 */
#define EUSART_TX_BUFFER_SIZE 16
#define EUSART_RX_BUFFER_SIZE 16

/**
  Section: Global Variables
 */

// static uint8_t eusartTxHead = 0;
static uint8_t eusartTxTail = 0;
static uint8_t eusartTxBuffer[EUSART_TX_BUFFER_SIZE+1];
volatile uint8_t eusartTxBufferRemaining=0;
volatile uint8_t eusartTxBufferWorkingFlag=0;
static int eusartTxBufferSize;

static uint8_t eusartRxHead = 0;
static uint8_t eusartRxTail = 0;
static uint8_t eusartRxBuffer[EUSART_RX_BUFFER_SIZE];
volatile uint8_t eusartRxCount;

/**
  Section: EUSART APIs
 */

void EUSART_Initilize(void) {
    // disable interrupts before changing states
    PIE1bits.RCIE = 0;
    PIE1bits.TXIE = 0;

    // Set the EUSART module to the options selected in the user interface.

    // ABDOVF no_overflow; SCKP async_noninverted_sync_fallingedge; RCIDL idle; BRG16 16bit_generator; WUE disabled; ABDEN disabled;
    BAUDCON = 0x48;

    // SPEN enabled; OERR no_error; RX9 8-bit; RX9D 0x0; CREN disabled; ADDEN disabled; SREN disabled; FERR no_error;
    RCSTA = 0x80;

    // TRMT TSR_empty; TX9 8-bit; TX9D 0x0; SENDB sync_break_complete; TXEN enabled; SYNC asynchronous; BRGH hi_speed; CSRC slave_mode;
    TXSTA = 0x26;

    // Baud Rate = 9600; SPBRGL 160;
    SPBRGL = 0xA0;

    // Baud Rate = 9600; SPBRGH 1;
    SPBRGH = 0x01;


    // initializing the driver state
    //eusartTxHead = 0;
    eusartTxTail = 0;
    eusartTxBufferRemaining = sizeof (eusartTxBuffer);

    eusartRxHead = 0;
    eusartRxTail = 0;
    eusartRxCount = 0;

    eusartTxBuffer[EUSART_TX_BUFFER_SIZE] = '\0';

    // enable receive interrupt
    PIE1bits.RCIE = 1;
}


uint8_t EUSART_Read(void) {
    uint8_t readValue = 0;

    while (0 == eusartRxCount) {
    }

    PIE1bits.RCIE = 0;

    readValue = eusartRxBuffer[eusartRxTail++];
    if (sizeof (eusartRxBuffer) <= eusartRxTail) {
        eusartRxTail = 0;
    }
    eusartRxCount--;
    PIE1bits.RCIE = 1;

    return readValue;
}

void EUSART_Read_BufClear(void) {
  PIE1bits.RCIE = 0;
  eusartRxTail=0;
  eusartRxCount=0;
  eusartRxHead=0;
  PIE1bits.RCIE = 1;
}
void EUSART_Write(uint8_t txData){
    return;
}

void EUSART_Str_Write(uint8_t *txData)
{
  int i=0;
  int loopflag=1;
  uint8_t *txDataPoint = txData;

  while(loopflag){
    while (eusartTxBufferWorkingFlag) {
    }

    PIE1bits.TXIE = 0;
    while(1){
      eusartTxBuffer[i] = txDataPoint[i];
      if(txData[i++]=='\0'){
        // tx data end.
        eusartTxBufferSize = i;
        loopflag=0;
        break;
      }
      if(i>=EUSART_TX_BUFFER_SIZE){
        eusartTxBufferSize = EUSART_TX_BUFFER_SIZE;
        txDataPoint += i;
        i=0;
        RC3=1;
        break;
      }
    }
    // send data
    eusartTxBufferWorkingFlag = 1;
    eusartTxTail = 0;
    PIE1bits.TXIE = 1;
  }

}

void EUSART_Data_Write(uint8_t *txData,int len)
{
  int i=0; // point for txData
  int dataCount=0; // number of being copyed to txBuffer
  int loopflag=1;

  while(loopflag){
    //wait sending data
    while (eusartTxBufferWorkingFlag) {
    }
    //set up tx buffer
    PIE1bits.TXIE = 0;
    while(1){
      // copy txData to tx buffer
      eusartTxBuffer[dataCount++] = txData[i++];
      // finish to copy sending data
      if(i>=len){
        // tx data end.
        eusartTxBufferSize = dataCount;
        loopflag=0;
        break;
      }

      // tx buffer full
      if(i>=EUSART_TX_BUFFER_SIZE){
        eusartTxBufferSize = EUSART_TX_BUFFER_SIZE;
        dataCount=0;
        RC3=1;
        break;
      }
    }
    // send data
    eusartTxBufferWorkingFlag = 1;
    eusartTxTail = 0;
    PIE1bits.TXIE = 1;
  }
}


void EUSART_Transmit_ISR(void) {

    // add your EUSART interrupt custom code
    if(!eusartTxBufferWorkingFlag){
      PIE1bits.TXIE = 0;
      eusartTxBufferWorkingFlag = 0;
      // for debug
      RC3=1;
      return;
    }

    TXREG = eusartTxBuffer[eusartTxTail++];
    if(eusartTxTail>=eusartTxBufferSize){
      PIE1bits.TXIE = 0;
      eusartTxBufferWorkingFlag = 0;
    }

    return;
}

void EUSART_Receive_ISR(void) {
    if (1 == RCSTAbits.OERR) {
        // EUSART error - restart

        RCSTAbits.CREN = 0;
        RCSTAbits.CREN = 1;
    }

    // buffer overruns are ignored
    eusartRxBuffer[eusartRxHead++] = RCREG;
    if (sizeof (eusartRxBuffer) <= eusartRxHead) {
        eusartRxHead = 0;
    }
    eusartRxCount++;
}
/**
  End of File
 */
