/**
 * @file LCDgraphic.c
 * @brief Library with functions for graphical LCD based on KS0108
 *
 * @author Davide Lucchi
 * @version 1.0
 * @since 1.0
 */

/* lib include for the LCD functions */
#include "LCDgraphic.h"
/* lib include for the delay functions */
#include <xc.h>
/* libc include for the string functions */
#include <string.h>

// bus direction I/O
#define LCD_TRIS      TRISD
// bus data
#define LCD_DATA      PORTD
// enable pin
#define EN            LATCbits.LATC0
// read/write pin 
#define RW            LATCbits.LATC1
// read/write from LCD RAM pin
#define RS            LATCbits.LATC2
// left page 
#define CS1           LATEbits.LATE0
// right page
#define CS2           LATEbits.LATE1
// reset pin
#define RESET_LCD     LATCbits.LATC6
// oscillator frequency 
#define _XTAL_FREQ    40000000

// stores the currently selected side
static int selected_side;
// stores the currently selected page in the currently selected side
static int curr_page;
// stores the current column in the currently selected side
static int curr_column;
// stores pixels that have been written after last LCDclear() (64+64)*8 = 1024
static char pixels[1025];

// ASCII table without first 32 symbols 
char const pic[][5] = {
        {0x00, 0x00, 0x00, 0x00, 0x00},            // Code for char  
        {0x00, 0x00, 0xBE, 0x00, 0x00},            // Code for char !
        {0x00, 0x0E, 0x00, 0x0E, 0x00},            // Code for char "
        {0x44, 0xFE, 0x44, 0xFE, 0x44},            // Code for char #
        {0x48, 0x54, 0xFE, 0x54, 0x24},            // Code for char $
        {0x46, 0x26, 0x10, 0xC8, 0xC4},            // Code for char %
        {0x6C, 0x92, 0xAA, 0x44, 0x80},            // Code for char &
        {0x00, 0x08, 0x06, 0x00, 0x00},            // Code for char '
        {0x00, 0x00, 0x7C, 0x82, 0x00},            // Code for char (
        {0x00, 0x82, 0x7C, 0x00, 0x00},            // Code for char )
        {0x28, 0x10, 0x7C, 0x10, 0x28},            // Code for char *
        {0x10, 0x10, 0x7C, 0x10, 0x10},            // Code for char +
        {0x00, 0x80, 0x60, 0x00, 0x00},            // Code for char ,
        {0x10, 0x10, 0x10, 0x10, 0x10},            // Code for char -
        {0x00, 0xC0, 0xC0, 0x00, 0x00},            // Code for char .
        {0x40, 0x20, 0x10, 0x08, 0x04},            // Code for char /
        {0x7C, 0x82, 0x82, 0x7C, 0x00},            // Code for char 0
        {0x00, 0x84, 0xFE, 0x80, 0x00},            // Code for char 1
        {0x84, 0xC2, 0xA2, 0x92, 0x8C},            // Code for char 2
        {0x44, 0x82, 0x92, 0x92, 0x6C},            // Code for char 3
        {0x30, 0x28, 0x24, 0xFE, 0x20},            // Code for char 4
        {0x4E, 0x8A, 0x8A, 0x8A, 0x72},            // Code for char 5
        {0x78, 0x94, 0x92, 0x92, 0x60},            // Code for char 6
        {0x02, 0xE2, 0x12, 0x0A, 0x06},            // Code for char 7
        {0x6C, 0x92, 0x92, 0x92, 0x6C},            // Code for char 8
        {0x0C, 0x92, 0x92, 0x52, 0x3C},            // Code for char 9
        {0x00, 0x66, 0x66, 0x00, 0x00},            // Code for char :
        {0x00, 0x46, 0x36, 0x00, 0x00},            // Code for char ;
        {0x10, 0x28, 0x44, 0x82, 0x00},            // Code for char <
        {0x28, 0x28, 0x28, 0x28, 0x28},            // Code for char =
        {0x00, 0x82, 0x44, 0x28, 0x10},            // Code for char >
        {0x04, 0x02, 0xA2, 0x12, 0x0C},            // Code for char ?
        {0x64, 0x92, 0xF2, 0x82, 0x7C},            // Code for char @
        {0xFC, 0x22, 0x22, 0x22, 0xFC},            // Code for char A
        {0xFE, 0x92, 0x92, 0x92, 0x6C},            // Code for char B
        {0x7C, 0x82, 0x82, 0x82, 0x44},            // Code for char C
        {0xFE, 0x82, 0x82, 0x82, 0x7C},            // Code for char D
        {0xFE, 0x92, 0x92, 0x92, 0x92},            // Code for char E
        {0xFE, 0x12, 0x12, 0x12, 0x12},            // Code for char F
        {0x7C, 0x82, 0x92, 0x92, 0x74},            // Code for char G
        {0xFE, 0x10, 0x10, 0x10, 0xFE},            // Code for char H
        {0x00, 0x00, 0xFE, 0x00, 0x00},            // Code for char I
        {0x40, 0x80, 0x82, 0x7E, 0x02},            // Code for char J
        {0xFE, 0x10, 0x28, 0x44, 0x82},            // Code for char K
        {0xFE, 0x80, 0x80, 0x80, 0x80},            // Code for char L
        {0xFE, 0x04, 0x08, 0x04, 0xFE},            // Code for char M
        {0xFE, 0x04, 0x08, 0x10, 0xFE},            // Code for char N
        {0x7C, 0x82, 0x82, 0x82, 0x7C},            // Code for char O
        {0xFE, 0x12, 0x12, 0x12, 0x0C},            // Code for char P
        {0x7C, 0x82, 0xA2, 0x42, 0xBC},            // Code for char Q
        {0xFE, 0x12, 0x32, 0x52, 0x8C},            // Code for char R
        {0x8C, 0x92, 0x92, 0x92, 0x62},            // Code for char S
        {0x02, 0x02, 0xFE, 0x02, 0x02},            // Code for char T
        {0x7E, 0x80, 0x80, 0x80, 0x7E},            // Code for char U
        {0x3E, 0x40, 0x80, 0x40, 0x3E},            // Code for char V
        {0x7E, 0x80, 0x7E, 0x80, 0x7E},            // Code for char W
        {0xC6, 0x28, 0x10, 0x28, 0xC6},            // Code for char X
        {0x0E, 0x10, 0xF0, 0x10, 0x0E},            // Code for char Y
        {0xC2, 0xA2, 0x92, 0x8A, 0x86},            // Code for char Z
        {0x00, 0x00, 0xFE, 0x82, 0x00},            // Code for char [
        {0x04, 0x08, 0x10, 0x20, 0x40},            // Code for char BackSlash
        {0x00, 0x82, 0xFE, 0x00, 0x00},            // Code for char ]
        {0x08, 0x04, 0x02, 0x04, 0x08},            // Code for char ^
        {0x80, 0x80, 0x80, 0x80, 0x80},            // Code for char _
        {0x00, 0x00, 0x01, 0x02, 0x04},            // Code for char `
        {0x40, 0xA8, 0xA8, 0xA8, 0xF0},            // Code for char a
        {0xFE, 0x88, 0x88, 0x88, 0x70},            // Code for char b
        {0x70, 0x88, 0x88, 0x88, 0x40},            // Code for char c
        {0x70, 0x88, 0x88, 0x90, 0xFE},            // Code for char d
        {0x70, 0xA8, 0xA8, 0xA8, 0x30},            // Code for char e
        {0x10, 0xFC, 0x12, 0x02, 0x04},            // Code for char f
        {0x18, 0xA4, 0xA4, 0xA4, 0x7C},            // Code for char g
        {0xFE, 0x10, 0x08, 0x08, 0xF0},            // Code for char h
        {0x00, 0x00, 0xFA, 0x00, 0x00},            // Code for char i
        {0x00, 0x80, 0x7A, 0x00, 0x00},            // Code for char j
        {0xFE, 0x20, 0x50, 0x88, 0x00},            // Code for char k
        {0x00, 0xFE, 0x00, 0x00, 0x00},            // Code for char l
        {0xF8, 0x08, 0x10, 0x08, 0xF0},            // Code for char m
        {0xF8, 0x10, 0x08, 0x08, 0xF0},            // Code for char n
        {0x70, 0x88, 0x88, 0x88, 0x70},            // Code for char o
        {0xF8, 0x28, 0x28, 0x28, 0x10},            // Code for char p
        {0x10, 0x28, 0x28, 0x30, 0xFC},            // Code for char q
        {0xF8, 0x10, 0x08, 0x08, 0x10},            // Code for char r
        {0x90, 0xA8, 0xA8, 0xA8, 0x40},            // Code for char s
        {0x08, 0x7E, 0x88, 0x80, 0x40},            // Code for char t
        {0x78, 0x80, 0x80, 0x40, 0xF8},            // Code for char u
        {0x38, 0x40, 0x80, 0x40, 0x38},            // Code for char v
        {0x78, 0x80, 0x78, 0x80, 0x78},            // Code for char w
        {0x88, 0x50, 0x20, 0x50, 0x88},            // Code for char x
        {0x18, 0xA0, 0xA0, 0xA0, 0x78},            // Code for char y
        {0x88, 0xC8, 0xA8, 0x98, 0x88},            // Code for char z
        {0x10, 0x6C, 0x82, 0x00, 0x00},            // Code for char {
        {0x00, 0x00, 0x7E, 0x00, 0x00},            // Code for char |
        {0x00, 0x82, 0x6C, 0x10, 0x00},            // Code for char }
        {0x20, 0x10, 0x10, 0x08, 0x00},            // Code for char ~   
};
       
/**
 * @brief Sends the enable pulse.
 *
 * @detail It sends an enable pulse to the LCD. On  
 * the rising edge the LCD puts/reads the data to/from
 * the bus accordingly to the read/write bit value
 * 
 * @return nothing
 *
 * @author Davide Lucchi
 * @version 1.0
 * @since 1.0
 */
void _lcd_enable(void)
{
    EN = 1;
    __delay_us(2);
    EN = 0;
}

/**
 * @brief Initializes the LCD
 *
 * @detail Waits until LCD is no longer busy, then sends reset signal
 * and turns LCD ON.
 * 
 * @return nothing
 *
 * @author Davide Lucchi
 * @version 1.0
 * @since 1.0
 */
void LCDinit()
{
    // wait for VDD stabilization and LCD internal turn on sequence 150ms
    for (int i = 0; i < 15; i++)
    {
        __delay_ms(10);
    }
    // wait until LCD initialization is complete
    // busy and reset flags cleared  
    while (_lcd_status() & 0b10010000 );
    
    // send reset command
    LCDreset();
    // turn on LCD
    LCDon(1);
    // clear LCD 
    LCDclear();
}

/**
 * @brief Reads the status of the LCD
 *
 * @detail It sends a request to the LCD to make it 
 * put its status on the data bus. The MSB of the status 
 * contains the BUSY flag, the 3rd MSB the display ON/OFF 
 * flag and the 4th MSB the RESET flag.
 * 
 * @return The LCD status
 *
 * @author Davide Lucchi
 * @version 1.0
 * @since 1.0
 */
unsigned char _lcd_status(void)
{
    unsigned char status;
    // save current sides' statuses
    unsigned char cs1, cs2;
    cs1 = CS1; 
    cs2 = CS2;
    
    // write 0 to the I/O bus
    LCD_DATA = 0x00;
    // set PORTD to be input
    LCD_TRIS = 0xFF;
    
    // Prepare LCD for reading status 
    RS=0; RW=1; CS1 = 1; CS2 = 1;
    __delay_us(1);
    
    //send enable pulse
    _lcd_enable();
    
    // save status data
    status = LCD_DATA;
    
    // Restore normal operation mode
    RW = 0; CS1 = cs1; CS2 = cs2;
    
    // wait 10us to make sure LCD has updated its output port direction
    // minimizes risk of logic contentions on I/O bus
    __delay_us(10);
    
    // set PORTD to be output
    LCD_TRIS = 0x00;
   
    return status;
}

/**
 * @brief Sends a command to the LCD.
 *
 * @detail It writes an 8bits value on the data bus 
 * and sends an enable pulse to make the LCD read the
 * value.
 * 
 * @param data the 8bits value to send to the LCD
 * @return nothing
 *
 * @author Davide Lucchi
 * @version 1.0
 * @since 1.0
 */
void LCDcmd (unsigned char data)
{
    // put data on I/O bus
    LCD_DATA = data;
    // send enable pulse
    _lcd_enable();
}   

/**
 * @brief Resets the LCD.
 *
 * @detail It sends a zero pulse to the LCD to make it 
 * reset and waits until LCD BUSY and RESETS flags are
 * cleared.
 * 
 * @return nothing
 *
 * @author Davide Lucchi
 * @version 1.0
 * @since 1.0
 */
void LCDreset(void) 
{
    // write 0 to I/O bus
    LCD_DATA = 0x00;
    
    // send Reset Pulse
    RESET_LCD = 0;
    __delay_us(20);
    RESET_LCD = 1;
    
    // wait until reset is completed 
    while (_lcd_status() & 0b10010000);
}

/**
 * @brief Waits until the LCD can accept new commands.
 *
 * @detail It keeps requesting the LCD status until the 
 * BUSY and RESET flags are cleared.
 * 
 * @return nothing
 *
 * @author Davide Lucchi
 * @version 1.0
 * @since 1.0
 */
void _lcd_waitbusy(void)
{
    // wait until busy and reset flags are cleared
    while (_lcd_status() & 0b10010000);
}

/**
 * @brief Selects a page from one of the two sides.
 *
 * @detail The LCD screen is divided into two pages each 
 * consisting of 64x64 pixels. Each page is horizontally 
 * divided in 8 rows each 8 pixels high. 
 * 
 * @param side Selects the side. 0 for left side,
 * 1 for right side and 2 for  both sides.
 * @param page Selects one of the 8 pages. Accepts values 
 * from \c 0 to \c 7
 * @return \c 0 if success \c -1 if it fails
 *
 * @author Davide Lucchi
 * @version 1.0
 * @since 1.0
 */
char LCDpage(unsigned char side, unsigned char page)
{
    if (side < 0 || side > 2 || page < 0 || page > 7)
        return -1; 

    //wait if LCD is busy
    _lcd_waitbusy();
    
    // store in the static variable the current page
    curr_page = page;

    RS=0;
    RW=0;

    switch(side)
    {
        case 0:
            CS1=1;
            CS2=0;
            break;

        case 1:
            CS1=0;
            CS2=1;
            break;

        case 2:
            CS1=1;
            CS2=1;
            break;
    }
    // send command where 3LSB are the page value
    LCDcmd(0b10111000 | page);
    
    return 0;
}

/**
 * @brief Turns ON/OFF the LCD
 * 
 * @param on \c 1 ON; \c 0 OFF
 * @return nothing
 *
 * @author Davide Lucchi
 * @version 1.0
 * @since 1.0
 */
void LCDon(unsigned char on)
{
    //wait if busy
    _lcd_waitbusy();
    //1 on
    RS=0;
    RW=0;
    CS1=1;
    CS2=1;
    if(on) 
    {
        LCDcmd(0b00111111);
    }
    else
    {
        LCDcmd(0b00111110);
    } 
}

/**
 * @brief Specifies a column in the selected page.
 *
 * @detail Each page contains 64 columns of 8 pixels each. 
 * It selects one of these columns.
 * 
 * @param side Selects the side. \c 0 for left side,
 * \c 1 for right side and \c 2 for  both sides.
 * @param value The column value between \c 0 and \c 63
 * @return \c 0 if success \c -1 if it fails
 *
 * @author Davide Lucchi
 * @version 1.0
 * @since 1.0
 */
char LCDy (unsigned char side, unsigned char value)
{
    if (side < 0 || side > 2 || value < 0 || value > 63)
        return -1;
    
    // store in static variable current side
    selected_side = side;
    // store in static variable current y position in selected side
    curr_column = value;
    
    _lcd_waitbusy();
    
    RS=0;
    RW=0;
    switch(side)
    {
        case 0: 
            CS1=1;
            CS2=0;
            break;
        case 1:
            CS1=0;
            CS2=1;
            break;
        case 2:
            CS1=1;
            CS2=1;
            break;
    }

    LCDcmd(0b01000000 | value);
    return 0;
}

/**
 * @brief Writes space between adjacent letters
 *
 * @detail Writes an empty column after the last written letter
 * 
 * @return nothing
 *
 * @author Davide Lucchi
 * @version 1.0
 * @since 1.0
 */
void emptycolumn(void) {
    
    int char_index;
    
    RS=1;
    RW=0;
    
    if(selected_side==0) 
    {
        char_index = (128*curr_page) + curr_column;
    }
    // side 1
    else 
    {
        char_index = (128*curr_page) + curr_column+64;
    }
    
    // letters are 5 pixels wide 
    LCDcmd (0x00 | pixels[char_index+5]); 

    curr_column = curr_column+6; 
}

/**
 * @brief Writes an ASCII character to the display
 *
 * @detail Writes an ASCII character to the formerly 
 * specified position with LCDpage() and LCDy() or 
 * to the position specified by the position parameter
 * 
 * @param c the character to write
 * @param position \c 0 use formerly specified position; \c 1  left; 
 * \c 2 center; \c 3 right.
 * @return \c 0 if success, \c -1 if fail
 *
 * @author Davide Lucchi
 * @version 1.0
 * @since 1.0
 */
char LCDchar(unsigned char c, unsigned char position)
{
    if (position < 0 || position > 3)
    {
        return 1;
    }
    // first 32 ASCII chars are not in pic array 
    int index = c-32;
    int char_index;
    unsigned char tmp;

    _lcd_waitbusy();

    switch (position)
    {
        case 0:
            if(selected_side==0) 
            {
                char_index=(128*curr_page)+curr_column;
            }
            // side 1
            else 
            {
                char_index=(128*curr_page)+64+curr_column;
            }

            RS=1;
            RW=0;
            for (int i=0; i<5; i++)
            {
                tmp = pixels[char_index+i] = pixels[char_index+i] | pic[index][i];
                LCDcmd(tmp);
            }

            break;

        case 1: 
            LCDpage(0,curr_page);      
            LCDy(0,1);
            
            char_index=(128*curr_page)+curr_column;
           
            RS=1;
            RW=0;
            for (int i=0; i<5; i++)
            {
                tmp = pixels[char_index+i] = pixels[char_index+i] | pic[index][i];
                LCDcmd(tmp);
            }
            break;
        // centered letter
        case 2: 
            LCDpage(0,curr_page);
            LCDy(0,61);
            
            char_index=(128*curr_page)+curr_column;
           
            RS=1;
            RW=0;
            for (int i=0; i<3; i++)
            {
                tmp = pixels[char_index+i] = pixels[char_index+i] | pic[index][i];
                LCDcmd(tmp); 
            }

            LCDpage(1,curr_page);
            LCDy(1,0);
            // page 0
            
            RS=1;
            RW=0;
            for (int i=3; i<5; i++)
            {
                tmp = pixels[char_index+i] = pixels[char_index+i] | pic[index][i];
                LCDcmd(tmp);
            }

            break;
        // letter adjusted right
        case 3:
            LCDpage(1,curr_page); 
            LCDy(1,59);     
            
            char_index=(128*curr_page)+curr_column+64;
           
            RS=1;
            RW=0;
            for (int i=0; i<5; i++)
            {
                tmp = pixels[char_index+i] = pixels[char_index+i] | pic[index][i];
                LCDcmd(tmp); 
            }    
            break;
    }
    return 0;
}

/**
 * @brief Clears all the data in the display
 * 
 * @return nothing
 *
 * @author Davide Lucchi
 * @version 1.0
 * @since 1.0
 */
void LCDclear(void)
{
    _lcd_waitbusy();

    // clear array that keeps track of written pixels
    for(int i = 0; i < 1025; i++)
    {
        pixels[i] = 0x00;
    }
    // write ' ' to entire display
    for(int i = 0; i < 8;i++)
    {
        LCDpage(2,i);
        for(int j = 0; j < 13; j++)
        {
            LCDy(2,j*5);
            LCDchar(' ',0);
        }
    }
}

/**
 * @brief Writes a string to the LCD
 *
 * @detail Writes the string at the formerly specified
 * position or at the \c p defined position
 * 
 * @param p \c 0 formerly specified position; 
 * \c 1 left aligned; \c 2 centered; \c 3 right aligned
 * @param buffer the string to print
 * @return \c 0 if success, \c -1 if fail
 *
 * @author Davide Lucchi
 * @version 1.0
 * @since 1.0
 */
char LCDstring (char p, const char *buffer) {
    
    if (p < 0 || p > 3 || buffer == 0)
    {
        return -1;
    }
    
    _lcd_waitbusy();
    
    int length = strlen(buffer);
    
    // left justified string
    if(p==0) 
    {
        // Write data to LCD up to \0
        while(*buffer) 
        {
            LCDchar(*buffer++,0);
            emptycolumn();
       }
    }
    else if(p == 1)
    {
        //set position
        LCDpage(0,curr_page);
        LCDy(0,1);
        // the string fits the first half of the display
        if(length < 11) 
        {
            // Write characters to LCD
            for(int i=0; i<length; i++)
            {
                LCDchar(*buffer++,0);
                //space between characters
                emptycolumn();
            }
        }
        // the string fits both halves of the display
        else
        {
            // write first half (10 chars)
            for(int i=0; i<10; i++)
            {
                LCDchar(*buffer++,0);
                emptycolumn();
            }
            // write central character
            LCDpage(0,curr_page);
            LCDchar(*buffer++,2);
            // write second half
            // set position
            LCDpage(1,curr_page);
            LCDy(1,3);
            for(int i=11;i<length;i++)
            {
                LCDchar(*buffer,0);
                emptycolumn();
                buffer++;      
            }
        }
    }
    // centered string
    else if(p==2)
    {
        int l1=length/2;
        // length is even
        if(length % 2 == 0) 
        {
            // set position 5 pixels + emptycolumn = 6
            LCDpage(0,curr_page);
            LCDy(0,63-(6*l1));
            // write first characters
            for(int i=0; i<l1; i++)
            {
                // Write character to LCD
                LCDchar(*buffer++,0);
                emptycolumn();
            }
            // write the others characters
            LCDpage(1,curr_page);
            LCDy(1,0);
            for(int i=l1; i < length; i++)
            {
                // Write character to LCD
                LCDchar(*buffer++,0);
                emptycolumn();
            }
        }
        // length is odd 
        else 
        {
            // write first characters
            LCDpage(0,curr_page);
            LCDy(0,61-(6*l1));

            for(int i = 0; i<l1; i++)
            {
                // Write character to LCD
                LCDchar(*buffer++,0);
                emptycolumn();
            }

            // write central character
            LCDpage(0,curr_page);
            LCDchar(*buffer++,2);

            // write the others characters
            LCDpage(1,curr_page);
            LCDy(1,3);

            for(int i=l1+1; i<length; i++)
            {
                // Write character to LCD
                LCDchar(*buffer++,0);
                emptycolumn();
            }
        }
    }
    // right justified string p = 3
    else
    {
        // the string fits only the second half of the display
        if(length < 11) 
        {
            // set position
            LCDpage(1,curr_page);
            LCDy(1,63-(6*length));
            // Write characters to LCD
            for(int i = 0; i<length; i++)
            {
                LCDchar(*buffer,0);
                emptycolumn();
                buffer++;      
            }
        }
        // the string fits both halves of the display
        else
        {
            if(length==11)
            {
                // write central character
                LCDpage(0,curr_page);
                LCDchar(*buffer++,2);   
                LCDpage(1,curr_page);
                LCDy(1,3);
                // write the other characters
                for(int i=0; i<10; i++)
                {
                    LCDchar(*buffer++,0);
                    emptycolumn();     
                }

            }
            else //length>11
            {
                // set position
                LCDpage(0,curr_page);
                LCDy(0,126-(6*length));
                // write the first half 
                for(int i=0; i<length-11; i++)
                {
                    LCDchar(*buffer++,0);
                    emptycolumn();      
                }
                // write central character
                LCDpage(0,curr_page);
                LCDchar(*buffer++,2);  
                LCDpage(1,curr_page);
                LCDy(1,3);
                // write the other characters
                for(int i = length-10; i<length; i++)
                {
                    LCDchar(*buffer++,0);
                    emptycolumn();    
                }
            }
        }
    }
    return 0;
}

/**
 * @brief Turns on or off pixels accordingly to blank parameter.
 * 
 * @param rx  x coordinate
 * @param ry  y coordinate
 * @param blank \c -1 turns off all pixels of the column \c 0  turns on specified pixel
 * \c 1 turns off only specified pixel
 * @return \c 0 if success \c -1 if it fails
 *
 * @author Davide Lucchi
 * @version 1.0
 * @since 1.0
 */
char lcdplotpixel2(char rx, char ry, int blank)
{
    char pixel;
    int index,value;
    const char data[]={0x01, 0x02, 0x04, 0x08, 0x10, 0x20, 0x40, 0x80,0x00};

    // set page and column
    if(rx < 64) 
    {
        LCDpage(0,(int)ry/8);
        LCDy(0,rx);
    }
    else 
    {
        LCDpage(1,(int)ry/8);
        LCDy(1,rx-64);
    }

    // find index of pixel to turn on
    pixel = ry-8*(ry/8);
    index = rx+128*(ry/8);

    if(blank > 0)
    {
        value = ~data[pixel] & pixels[index];
    }
    else if (blank < 0)
    {
        value = data[8] & pixels[index];
    }
    else 
    {
        value = data[pixel] | pixels[index];
    }

    RS=1;
    RW=0;

    LCDcmd(value);

    pixels[index]=value;  
    return 0;
}

/**
 * @brief Draws the xy axis grid
 *
 * @return nothing
 *
 * @author Davide Lucchi
 * @version 1.0
 * @since 1.0
 */
void xyaxis ()
{
    for(int i=0;i<55;i++)
    {
        lcdplotpixel2(64,i,0);
    }

    for(int i=0;i<=126;i++)
    {
        lcdplotpixel2(i,32,0);
    }
    
    // y arrow
    lcdplotpixel2(63,1,0);
    lcdplotpixel2(62,2,0);
    lcdplotpixel2(61,3,0);
    lcdplotpixel2(65,1,0);
    lcdplotpixel2(66,2,0);
    lcdplotpixel2(67,3,0);

    // x arrow
    lcdplotpixel2(127,32,0);
    lcdplotpixel2(126,31,0);
    lcdplotpixel2(126,33,0);
    lcdplotpixel2(125,30,0);
    lcdplotpixel2(125,34,0);
    lcdplotpixel2(124,29,0);
    lcdplotpixel2(124,35,0);
}

/**
 * @brief Set LCD start line
 *
 * @param side Selects the side. \c 0 for left side,
 * \c 1 for right side and \c 2 for  both sides.
 * @param value The display start line value between 
 * \c 0 and \c 64
 * @return \c 0 on success, \c -1 on fail
 *
 * @author Davide Lucchi
 * @version 1.0
 * @since 1.0
 */
char LCDStartline (unsigned char side, unsigned char value)
{
    if (side < 0 || side > 2 || value < 0 || value > 63)
        return -1;
    
    RS=0;
    RW=0;

    switch(side)
    {
        case 0:
            CS1=1;
            CS2=0;
            break;

        case 1:
            CS1=0;
            CS2=1;
            break;

        case 2:
            CS1=1;
            CS2=1;
            break;
    }

    LCDcmd(0b11000000 | value);

    CS1=0;
    CS2=0;
    
    return 0;
}

/**
 * @brief Returns the data written at the previously set
 * page and column
 *
 * @return Returns the data written at the previously set
 * page and column
 *
 * @author Davide Lucchi
 * @version 1.0
 * @since 1.0
 */
unsigned char LCDread()
{
    // write 0 to the I/O bus
    LCD_DATA = 0x00;
    // set PORTD to be input
    LCD_TRIS = 0xFF;
    
    // Prepare LCD for reading status 
    RS=1; RW=1;
    __delay_us(1);
    
    //send enable pulse
    _lcd_enable();
    
    // save status data
    unsigned char data = LCD_DATA;
    
    // Restore normal operation mode
    RW = 0; RS = 0; 
    
    // wait 10us to make sure LCD has updated its output port direction
    // minimizes risk of logic contentions on I/O bus
    __delay_us(10);
    
    // set PORTD to be output
    LCD_TRIS = 0x00;
   
    return data;
}

/**
 * @brief Writes an image to the screen
 *
 * @detail It writes the image passed as argument to the screen. The argument must
 * meet certain requirements. It must contain 128*8 8bit values, one for each column 
 * of each page of the display. The array is written to the LCD from left to 
 * right starting at page 0 end ending at page 7. 
 * 
 * @param image An array with the image to write
 * @param top_to_bottom \c 0 if the MSB of the 8 bit values is the top pixel, \c 1 otherwise
 * @return nothing
 *
 * @author Davide Lucchi
 * @version 1.0
 * @since 1.0
 */
void write_image(const char* image, char top_to_bottom)
{   
    for (int page = 0; page < 8; page++)
    {
        LCDpage(0,page);
        LCDy(0,0);
        RS=1;
        RW=0;
        for (int i = 0; i < 64; i++)
        {
            unsigned char val =  image[page*128+i];
            unsigned char n_val = val;
            /*
             * Pixels are printed such that LSB is top pixel, so if 
             * top_to_bottom is 1 the bits must be put in the right position
             */
            if (top_to_bottom)
            {
                for (int j = 0; j < 8; j++)
                {
                    n_val = (val << 7) | ((val & 0b00000010) << 5)
                            | ((val & 0b00000100) << 3) | ((val & 0b00001000) << 1)
                            | ((val & 0b10000000) >> 7) | ((val & 0b01000000) >> 5)
                            | ((val & 0b00100000) >> 3) | ((val & 0b00010000) >> 1);
                }
            }
            LCDcmd(n_val);
        }
        LCDpage(1,page);
        LCDy(1,0);
        RS=1;
        RW=0;
        for (int i = 0;i < 64; i++)
        {
            unsigned char val =  image[page*128+64+i];
            unsigned char n_val = val;
            /*
             * Pixels are printed such that LSB is top pixel, so if 
             * top_to_bottom is 1 the bits must be put in the right position
             */
            if (top_to_bottom)
            {
                for (int j = 0; j < 8; j++)
                {
                    n_val = (val << 7) | ((val & 0b00000010) << 5)
                            | ((val & 0b00000100) << 3) | ((val & 0b00001000) << 1)
                            | ((val & 0b10000000) >> 7) | ((val & 0b01000000) >> 5)
                            | ((val & 0b00100000) >> 3) | ((val & 0b00010000) >> 1);
                }
            }
            LCDcmd(n_val);
        }
    }
}


