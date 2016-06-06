/**
 * @file LCDgraphic.h
 * @brief Library with utility functions for graphical LCD based on KS0108
 *
 * @author Davide Lucchi
 * @version 1.0
 * @since 1.0
 */

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
void _lcd_enable(void);

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
unsigned char _lcd_status(void);

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
void _lcd_waitbusy(void);

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
void LCDinit(void);

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
void LCDcmd (unsigned char data);

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
void LCDreset(void);

/**
 * @brief Selects a page from one of the two sides.
 *
 * @detail The LCD screen is divided into two sides each 
 * consisting of 64x64 pixels. Each side is horizontally 
 * divided in 8 pages each 8 pixels high. 
 * 
 * @param side Selects the side. \c 0 for left side,
 * \c 1 for right side and \c 2 for  both sides.
 * @param page Selects one of the \c 8 pages. Accepts values 
 * from \c 0 to \c 7
 * @return \c 0 if success \c -1 if it fails
 *
 * @author Davide Lucchi
 * @version 1.0
 * @since 1.0
 */
char LCDpage(unsigned char side, unsigned char page);

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
void LCDon(unsigned char on);

/**
 * @brief Writes an ASCII character to the display
 *
 * @detail Writes an ASCII character to the formerly 
 * specified position with LCDpage() and LCDy() or 
 * to the position specified by the position parameter
 * 
 * @param c The character to write
 * @param position \c 0 use formerly specified position; \c 1  left; 
 * \c 2 center; \c 3 right.
 * @return \c 0 if success \c -1 if it fails
 *
 * @author Davide Lucchi
 * @version 1.0
 * @since 1.0
 */
char LCDchar(unsigned char c, unsigned char position);

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
char LCDy(unsigned char side, unsigned char value);

/**
 * @brief Clears all the data in the display
 * 
 * @return nothing
 *
 * @author Davide Lucchi
 * @version 1.0
 * @since 1.0
 */
void LCDclear();

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
void emptycolumn();

/**
 * @brief Draws the xy axis grid
 *
 * @return nothing
 *
 * @author Davide Lucchi
 * @version 1.0
 * @since 1.0
 */
void xyaxis ();

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
char lcdplotpixel2(char rx, char ry,int blank);

/**
 * @brief Writes a string to the LCD
 *
 * @detail Writes the string at the formerly specified
 * position or at the \c p defined position
 * 
 * @param p \c 0 formerly specified position; 
 * \c 1 left aligned; \c 2 centered; \c 3 right aligned
 * @param buffer The string to print
 * @return \c 0 if success \c -1 if it fails
 *
 * @author Davide Lucchi
 * @version 1.0
 * @since 1.0
 */
char LCDstring (char p, const char *buffer);

/**
 * @brief Set LCD start line
 *
 * @param side Selects the side. \c 0 for left side,
 * \c 1 for right side and \c 2 for  both sides.
 * @param value The display start line value between 
 * \c 0 and \c 64
 * @return \c 0 if success \c -1 if it fails
 *
 * @author Davide Lucchi
 * @version 1.0
 * @since 1.0
 */
char LCDstartline (unsigned char side, unsigned char value);

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
unsigned char LCDread();

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
void write_image(const char* image, char top_to_bottom);