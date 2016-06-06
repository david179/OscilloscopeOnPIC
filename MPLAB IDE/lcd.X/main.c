/**
 * @file main.c
 * @brief Oscilloscope based on PIC18F4550 and 128x64 Graphic LCD
 *
 * @author Davide Lucchi
 * @version 1.0
 * @since 1.0
 */

/* lib include for the delay functions */
#include <xc.h>
/* lib include for the PIC18F4550 functions */
#include <p18f4550.h>
/* libc include for the string functions */
#include <string.h>
/* lib include for the LCD functions */
#include "LCDgraphic.h"
/* libc include for the sprintf functions */
#include <stdio.h>

#pragma config FOSC = HS
// Turn off whatchdog timer
#pragma config WDT = OFF
// Turn off low voltage programmation
#pragma config LVP = OFF
// Set RB0-RB4 as digital inputs
#pragma config PBADEN = OFF
// oscillator frequency 
#define _XTAL_FREQ  40000000
// One state button wired to RB0 
#define SCREEN_FREEZE PORTBbits.RB0
// Switch wired to RB1 for horizontal/vertical cursors selection
#define H_V PORTBbits.RB1

// Return absolute value of a
int abs(int a)
{
    return a >= 0 ? a : -a;
}

// welcome screen
char const image[]= {
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xfe, 0x02,
    0x02, 0x00, 0x26, 0x2a, 0x2a, 0x1e, 0x00, 0xfe, 0x22, 0x22, 0x22, 0x1c, 0x00, 0x1c, 0x22, 0x22,
    0x1c, 0x00, 0x00, 0x3e, 0x10, 0x20, 0x00, 0x26, 0x2a, 0x2a, 0x1e, 0x00, 0x20, 0x7c, 0x22, 0x00,
    0x1c, 0x22, 0x22, 0x1c, 0x00, 0x00, 0x3e, 0x10, 0x20, 0x00, 0x20, 0xbe, 0x00, 0x1c, 0x22, 0x22,
    0x1c, 0x00, 0x00, 0x00, 0x00, 0x00, 0xfe, 0x00, 0x3e, 0x20, 0x20, 0x1e, 0x00, 0x1a, 0x26, 0x26,
    0x3b, 0x00, 0x1c, 0x2a, 0x2a, 0x1a, 0x00, 0x1a, 0x26, 0x26, 0x3b, 0x00, 0x3e, 0x20, 0x20, 0x1e,
    0x00, 0x1c, 0x2a, 0x2a, 0x1a, 0x00, 0x00, 0x3e, 0x10, 0x20, 0x00, 0x20, 0xbe, 0x00, 0x26, 0x2a,
    0x2a, 0x1e, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x07, 0x00, 0x01, 0x01, 0x01, 0x00, 0x00, 0x01, 0x03, 
    0x05, 0x04, 0x00, 0x01, 0x01, 0x00, 0x00, 0x00, 0x01, 0x00, 0x01, 0x00, 0x01, 0x01, 0x01, 0x01,
    0x01, 0x01, 0x00, 0x00, 0x01, 0x01, 0x01, 0x00, 0x00, 0x01, 0x03, 0x01, 0x00, 0x81, 0x85, 0x80,
    0x00, 0x01, 0x01, 0x01, 0x00, 0x01, 0x01, 0x81, 0x80, 0x80, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xf0, 0x00, 0xf0, 0x00, 0x00, 0xf0, 0x00, 0x00, 0xf0, 
    0x00, 0x00, 0xe0, 0x10, 0x10, 0xe0, 0x00, 0x00, 0xf0, 0x80, 0x00, 0x00, 0xf0, 0x00, 0x00, 0xf0, 
    0x00, 0x00, 0xf0, 0x00, 0x30, 0x50, 0x50, 0xf0, 0x00, 0x00, 0xe0, 0x10, 0x00, 0x00, 0xf0, 0x00,
    0xe0, 0x10, 0x10, 0x10, 0x00, 0x30, 0x50, 0x50, 0xf0, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01, 0x00, 0x00, 0x00, 0x00,
    0x01, 0x01, 0x01, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0xfc, 0x04, 0x04, 0x00, 0x7c, 0x04, 0x04, 0x7c, 0x00, 0x38, 0x44, 0x44, 0x44, 0x00,
    0x38, 0x44, 0x44, 0x44, 0x00, 0xfc, 0x40, 0x40, 0x3c, 0x00, 0x40, 0x7c, 0x00, 0x00, 0x00, 0x00,
    0xfc, 0x04, 0x04, 0x04, 0xf8, 0x00, 0x4c, 0x54, 0x54, 0x3c, 0x00, 0x40, 0x38, 0x04, 0x38, 0x40,
    0x40, 0x7c, 0x00, 0x38, 0x44, 0x44, 0x44, 0xfc, 0x00, 0x38, 0x54, 0x54, 0x34, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00
};

/**
 * @brief Oscilloscope based on PIC18F4550 and 128x64 Graphic LCD
 * 
 * @return nothing
 *
 * @author Davide Lucchi
 * @version 1.0
 * @since 1.0
 */
void main(void) 
{
    // Set PORTA as input
    TRISA = 0xff;
    // Set PORTB as input
    TRISB = 0xff;
    // Set PORTC/D/E as output
    TRISC = 0x00;
    TRISD = 0x00;
    TRISE = 0x00;
    
    // Write 0 to all outputs 
    PORTC = 0x00;
    PORTD = 0x00;
    PORTE = 0x00;
    
    /************************
     *                      *
     *  ADC Configuration   *
     *                      *
     ************************/
    // Details from page 267 of datasheet
    // Set 0v as minimum voltage reference
    ADCON1bits.VCFG1=0;
    // Set VDD = 5V as maximum voltage reference
    ADCON1bits.VCFG0=0;
    // Set AN0-AN4 as analog inputs
    ADCON1bits.PCFG0=1;
    ADCON1bits.PCFG1=0;
    ADCON1bits.PCFG2=1;
    ADCON1bits.PCFG3=0;
    // Right justified result
    ADCON2bits.ADFM=1;
    // acquisition time 2*TAD
    ADCON2bits.ACQT2=0;
    ADCON2bits.ACQT1=0;
    ADCON2bits.ACQT0=1;
    // ADC clock Fosc/16
    ADCON2bits.ADCS2=1;
    ADCON2bits.ADCS1=0;
    ADCON2bits.ADCS0=1;
    // Select channel AN0 and enable ADC module
    ADCON0=0x01;
    
    // Array for sampled points
    unsigned int data[128];
    // initial value for peak-to-peak input voltage
    float Vpp = 0;
    
    float amplitude, s_time_pot;
    float y_div,s_time;
    // flag for freeze screen
    int freeze = 0;
   
    // Initialize LCD
    LCDinit();
    
    write_image(image,1);
    
    // 3 seconds delay 
    for (int i = 0; i < 300; i++)
    {
        __delay_ms(10);
    }
    LCDclear();
    
    while(1)
    {
        // check if freeze button was pushed
        if (SCREEN_FREEZE == 1)
        {
            // set freeze flag
            freeze = 1;
            // When the screen is frozen two cursors appear and 
            // they allow the user to take some voltage and 
            // period measurements. Cursors can be horizontal or
            // vertical but not both at the same time
            // initial values for cursors. Cursors' values can vary
            // between -63 to 63 being 0 the central value.
            int cur1_y = 0;
            int cur2_y = 0;
            int cur1_x = 0;
            int cur2_x = 0;
            
            while(freeze)
            {
                // horizontal/vertical switch set to horizontal 
                if (H_V == 1)
                {
                    // delete old cursors if any
                    if (cur1_x != 0 || cur2_x != 0)
                    {
                        // calculate sample points printed at cursors x coordinates
                        int y1 = (int)(((((float)5/1024)*data[64-cur1_x])-2.50f)*amplitude*2);
                        int y2 = (int)(((((float)5/1024)*data[64-cur2_x])-2.50f)*amplitude*2);
                        // delete all cursors' points but the sample points
                        for (int i = 8; i < 56; i++)
                        {
                            if (i != (32-y1) && i != 32 )
                            {
                                lcdplotpixel2(64-cur1_x,i,1);
                            }
                            if (i != (32-y2) && i != 32 )
                            {
                                lcdplotpixel2(64-cur2_x,i,1);
                            }
                        }
                        // reset default values
                        cur1_x = cur2_x = 0;
                    }
                    // temporary variables for current values of horizontal cursors
                    int cur1_y_new = 0;
                    int cur2_y_new = 0;
                    // sample cursors position 2.5V => 2^10/2 => center => cur1_y = 0
                    // sample from channel AN3 connected to cursor1 POT
                    ADCON0 = 0b00001101;
                    // start conversion
                    ADCON0bits.GO = 1;
                    // wait until it completes
                    while(ADCON0bits.GO == 1);
                    // The maximum horizonal range is 48 pixels, while the max
                    // sampled value is 5V => 2^10
                    // 1024/48 = 21 
                    int s_val = ((unsigned int)(ADRESH << 8) | ADRESL);
                    // new cursor1 position. All positions are relative to the main axis
                    cur1_y_new = ((1024/2)-s_val)/21; 
                    // cursor needs to be moved to a new position
                    if (cur1_y_new != cur1_y)
                    {
                        // delete old cursor from LCD
                        for (int i = 0; i < 128; i++)
                        {
                            int y = (int)(((((float)5/1024)*data[i])-2.50f)*amplitude*2);
                            // the two cursors 1 and 2 don't overlap => cur1 can be safely deleted
                            if (y != cur1_y && i != 64 && cur1_y != cur2_y )
                            {
                                lcdplotpixel2(i,32-cur1_y,1);
                            }
                            // write new cursor points
                            lcdplotpixel2(i,32-cur1_y_new,0);
                        }
                        cur1_y = cur1_y_new;
                        // delete old delta value printed on the LCD with delta being
                        // the difference between the two cursors multiplied by the 
                        // voltage per pixel value. In other words delta is the voltage 
                        // difference between the two cursors
                        for (int i = 70; i < 128; i++)
                        {
                            lcdplotpixel2(i,0,-1);
                        }
                        xyaxis();
                    }
                    
                    // sample from channel AN4 connected to cursor2 POT
                    ADCON0=0b00010001;
                    // start conversion
                    ADCON0bits.GO=1;
                    // wait until it completes
                    while(ADCON0bits.GO == 1);
                    // The maximum horizonal range is 48 pixels, while the max
                    // sampled value is 5V => 2^10
                    // 1024/48 = 21 
                    int s_val = ((unsigned int)(ADRESH << 8) | ADRESL);
                    cur2_y_new = ((1024/2)-s_val)/21; 
                    // cursor needs to be moved to a new position
                    if (cur2_y_new != cur2_y)
                    {
                        // delete old cursor from LCD
                        for (int a = 0; a < 128; a++)
                        {
                            int y = (int)(((((float)5/1024)*data[a])-2.50f)*amplitude*2);
                            // the two cursors 1 and 2 don't overlap => cur1 can be safely deleted
                            if (y != cur2_y && a != 64 && cur2_y != cur1_y )
                            {
                                lcdplotpixel2(a,32-cur2_y,1);
                            }
                            // write new cursor
                            lcdplotpixel2(a,32-cur2_y_new,0);
                        }
                        cur2_y = cur2_y_new;
                        // delete old delta value
                        for (int i = 70; i < 128; i++)
                        {
                            lcdplotpixel2(i,0,-1);
                        }
                        xyaxis();
                    }
                    // calculate delta value
                    float delta = abs(cur1_y-cur2_y) * y_div; 
                    char buf[10];
                    if (delta >= 1)
                    {
                        sprintf(buf, "%.2fV", delta);
                    }
                    else
                    {//use mV
                        sprintf(buf, "%.2fmV", delta*1000);
                    }
                    // print delta value on top right corner of LCD
                    LCDpage(1,0);
                    LCDstring(3,buf);
                } 
                // horizontal/vertical switch set to vertical
                else
                {
                    // delete old cursors if any
                    if (cur1_y != 0 || cur2_y != 0)
                    {
                        // delete all cursors' points but the sample points
                        for (int i = 0; i < 128; i++)
                        {
                            // sample point coordinate
                            int y = (int)(((((float)5/1024)*data[i])-2.50f)*amplitude*2);
                            if (y != cur1_y && i != 64)
                            {
                                lcdplotpixel2(i,32-cur1_y,1);
                            }
                            if (y != cur2_y && i != 64)
                            {
                                lcdplotpixel2(i,32-cur2_y,1);
                            }
                        }
                        cur1_y = cur2_y = 0;
                        xyaxis();
                    }
                    // temporary variables for current values of horizontal cursors
                    int cur1_x_new = 0;
                    int cur2_x_new = 0;
                    // sample cursors position 2.5V => 2^10/2 => center => cur1_y = 0
                    // sample from channel AN3 connected to cursor1 POT
                    ADCON0 = 0b00001101;
                    // start conversion
                    ADCON0bits.GO = 1;
                    // wait until it completes
                    while(ADCON0bits.GO == 1);
                    // The maximum vertical range is 128 pixels, while the max
                    // sampled value is 5V => 2^10
                    // 1024/128 = 8
                    int s_val = ((unsigned int)(ADRESH << 8) | ADRESL);
                    // new cursor1 position. All positions are relative to the main axis
                    cur1_x_new = ((1024/2)-s_val)/8; 
                    // cursor needs to be moved to a new position
                    if (cur1_x_new != cur1_x)
                    {
                        // delete old cursor from LCD
                        // calculate coordinate of sample pixel
                        int y = (int)(((((float)5/1024)*data[64-cur1_x])-2.50f)*amplitude*2);
                        for (int i = 8; i < 56; i++)
                        {
                            if (i != (32-y) && i != 32 && cur1_x != cur2_x )
                            {
                                lcdplotpixel2(64-cur1_x,i,1);
                            }
                            // write new cursor
                            lcdplotpixel2(64-cur1_x_new,i,0);
                        }
                        cur1_x = cur1_x_new;
                        // delete old delta value
                        for (int i = 70; i < 128; i++)
                        {
                            lcdplotpixel2(i,1,-1);
                        }
                        xyaxis();
                    }
                    // sample from channel AN4 connected to cursor2 POT
                    ADCON0=0b00010001;
                    // start conversion
                    ADCON0bits.GO=1;
                    // wait until it completes
                    while(ADCON0bits.GO == 1);
                    // The maximum vertical range is 128 pixels, while the max
                    // sampled value is 5V => 2^10
                    // 1024/128 = 8
                    int s_val = ((unsigned int)(ADRESH << 8) | ADRESL);
                    // new cursor2 position. All positions are relative to the main axis
                    cur2_x_new = ((1024/2)-s_val)/8; 
                    // cursor needs to be moved to a new position
                    if (cur2_x_new != cur2_x)
                    {
                        // delete last cursor from LCD
                        int y = (int)(((((float)5/1024)*data[64-cur2_x])-2.50f)*amplitude*2);
                        for (int i = 8; i < 56; i++)
                        {
                            // delete old
                            if (i != (32-y)  && i != 32 && cur2_x != cur1_x )
                            {
                                lcdplotpixel2(64-cur2_x,i,1);
                            }
                            // write new
                            lcdplotpixel2(64-cur2_x_new,i,0);
                        }
                        cur2_x = cur2_x_new;
                        // delete old delta value
                        for (int i = 70; i < 128; i++)
                        {
                            lcdplotpixel2(i,0,-1);
                        }
                        xyaxis();
                    }
                    // calculate delta value
                    float delta = abs(cur1_x-cur2_x) * s_time; 
                    char buf[10];
                    if (delta >= 1000)
                    {//use mS
                        sprintf(buf, "%.2fmS", delta/1000);
                    }
                    else
                    {// use uS
                        sprintf(buf, "%.2fuS", delta);
                    }
                    // print on top right corner of LCD
                    LCDpage(1,0);
                    LCDstring(3,buf);
                }
                for (int i = 0; i < 10; i++)
                {
                    __delay_ms(10);
                }
                // if button is pressed again quit frozen screen mode
                if (PORTBbits.RB0 == 1)
                {
                    freeze = 0;
                }
            }
        }
        
        
        // sample from channel wired to amplitude control potentiometer
        ADCON0=0x01;
        // start conversion
        ADCON0bits.GO=1;
        // wait until it completes
        while(ADCON0bits.GO == 1);
        // save amplitude sampled value
        amplitude = ((float)5/1024)*((unsigned int)(ADRESH << 8) | ADRESL); 
        
        
        // sample from channel wired to sample time potentiometer
        ADCON0 = 0b00000101;
        // start conversion
        ADCON0bits.GO=1;
        // wait until it completes  
        while(ADCON0bits.GO == 1);
        
        int s_time_pot = ((unsigned int)(ADRESH << 8) | ADRESL); 
        // save sample-time sampled value
        //s_time_pot = ((float)5/1024)*tmp;
        
        // sample the input signal
        ADCON0=0b00001001;
        
        // Take 128 samples of the input signal. Each sample will be plot as a pixel 
        // on the LCD.
        for (int i = 0; i < 128; i++)
        {
            // start conversion
            ADCON0bits.GO=1;
            while(ADCON0bits.GO == 1);
            // save the sample 
            data[i] =  ((unsigned int)(ADRESH << 8) | ADRESL); 
            // Delay accordingly to s_time_pot. In this way each sample is
            // taken every s_time seconds
            for (int j = 0; j <s_time_pot; j++)
            {
                ;
            }
        }
        
        char bottom_buf[40];
        // sample time in seconds. The formula has been derived by looking
        // at the ASM code and taking into account the instructions cycles 
        // required for the various instructions as reported on the datasheet.
        s_time = ( 99+19*(s_time_pot-2))*0.1;//= tmp*2.078f; tmp = 100; 206us
        //div = (79 + 15*0.0000008 + (tmp-2)*16)*0.1;
        if (s_time > 1000)
        {// use mS
            sprintf(bottom_buf,"%.2f mS/p",s_time/1000);
        }
        else
        {// use uS
            sprintf(bottom_buf,"%.2f uS/p",s_time);
        }  
        
        // Clear all LCD and print xyaxis
        LCDclear();
        xyaxis();
        
        // print data points and calculate peak-to-peak voltage
        int i = 0;
        // calculate peak-to-peak voltage
        unsigned int min = 1025;
        unsigned int max = 0;
        
        char min_y = 0;
        char max_y = 0;
        while (i < 128)
        {
            int y = (int)(((((float)5/1024)*data[i])-2.50f)*amplitude*2);
            if (data[i] < min)
            {
                // take minimum of sampled data
                min = data[i];
                // take minimum of sampled data after processing 
                min_y = abs(y);
            }
            if (data[i] > max)
            {
                // take maximum of sampled data
                max = data[i];
                // take maximum of sampled data after processing 
                max_y = abs(y);
            }
            // plot pixel
            lcdplotpixel2(i,32-y,0);
            i++;
        }
        
        // peak-to-peak voltage
        Vpp = ((float)5/1024)*(max-min);
        // voltage associated to each pixel on the vertical axis. This value is a 
        // function of amplitude variable
        y_div = Vpp/(max_y + min_y +1);
        
        
        char buf[13];
        if (y_div >= 1)
        {//use mV
            sprintf(buf, "%.2fV/p", y_div);
        }
        else
        {
            sprintf(buf, "%.2fmV/p", y_div*1000);
        }
        LCDpage(0,0);
        LCDstring(1,buf);
        
        sprintf(buf," %.3fVpp",Vpp);
        strcat(bottom_buf,buf);
        
        LCDpage(0,7);
        LCDstring(1,bottom_buf);
        
        // put small delay between each screen refresh
        for (int i = 0; i < 5; i++)
        {
            __delay_ms(10);
        }
    }
}



