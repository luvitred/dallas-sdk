//---------------------------------------------------------------------------
// Copyright (C) 2000 Dallas Semiconductor Corporation, All Rights Reserved.
//
// Permission is hereby granted, free of charge, to any person obtaining a
// copy of this software and associated documentation files (the "Software"),
// to deal in the Software without restriction, including without limitation
// the rights to use, copy, modify, merge, publish, distribute, sublicense,
// and/or sell copies of the Software, and to permit persons to whom the
// Software is furnished to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included
// in all copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS
// OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
// MERCHANTABILITY,  FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
// IN NO EVENT SHALL DALLAS SEMICONDUCTOR BE LIABLE FOR ANY CLAIM, DAMAGES
// OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE,
// ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR
// OTHER DEALINGS IN THE SOFTWARE.
//
// Except as contained in this notice, the name of Dallas Semiconductor
// shall not be used except as stated in the Dallas Semiconductor
// Branding Policy.
//---------------------------------------------------------------------------

#include <unistd.h>
#include "ownet.h"
#include "cortexow.h"

// exportable link-level functions
SMALLINT owTouchReset(int);
SMALLINT owTouchBit(int, SMALLINT);
SMALLINT owTouchByte(int, SMALLINT);
SMALLINT owWriteByte(int, SMALLINT);
SMALLINT owReadByte(int);
SMALLINT owSpeed(int, SMALLINT);
SMALLINT owLevel(int, SMALLINT);
SMALLINT owProgramPulse(int);
void msDelay(int);
long msGettick(void);
SMALLINT owWriteBytePower(int, SMALLINT);
SMALLINT owReadBitPower(int, SMALLINT);
SMALLINT owHasPowerDelivery(int);
SMALLINT owHasOverDrive(int);
SMALLINT owHasProgramPulse(int);
SMALLINT owReadBytePower(int portnum);

//--------------------------------------------------------------------------
// Reset all of the devices on the 1-Wire Net and return the result.
//
// 'portnum'    - number 0 to MAX_PORTNUM-1.  This number is provided to
//                indicate the symbolic port number.
//
// Returns: TRUE(1):  presense pulse(s) detected, device(s) reset
//          FALSE(0): no presense pulses detected
//
SMALLINT owTouchReset(int portnum)
{
    unsigned long result;

    ioctl(portnum, IOCTL_CORTEX_OW_TOUCH_RESET, &result);

    return (result ? 1 : 0);
}

//--------------------------------------------------------------------------
// Send 1 bit of communication to the 1-Wire Net and return the
// result 1 bit read from the 1-Wire Net.  The parameter 'sendbit'
// least significant bit is used and the least significant bit
// of the result is the return bit.
//
// 'portnum'    - number 0 to MAX_PORTNUM-1.  This number is provided to
//                indicate the symbolic port number.
// 'sendbit'    - the least significant bit is the bit to send
//
// Returns: 0:   0 bit read from sendbit
//          1:   1 bit read from sendbit
//
SMALLINT owTouchBit(int portnum, SMALLINT sendbit)
{
    unsigned long result;

    ioctl(portnum, IOCTL_CORTEX_OW_TOUCH_BIT, &result);

    return (result ? 1 : 0);
}

//--------------------------------------------------------------------------
// Send 8 bits of communication to the 1-Wire Net and return the
// result 8 bits read from the 1-Wire Net.  The parameter 'sendbyte'
// least significant 8 bits are used and the least significant 8 bits
// of the result is the return byte.
//
// 'portnum'    - number 0 to MAX_PORTNUM-1.  This number is provided to
//                indicate the symbolic port number.
// 'sendbyte'   - 8 bits to send (least significant byte)
//
// Returns:  8 bits read from sendbyte
//
SMALLINT owTouchByte(int portnum, SMALLINT sendbyte)
{
    unsigned long result = sendbyte;

    ioctl(portnum, IOCTL_CORTEX_OW_TOUCH_BYTE, &result);

    return (SMALLINT)result;
}

//--------------------------------------------------------------------------
// Send 8 bits of communication to the 1-Wire Net and verify that the
// 8 bits read from the 1-Wire Net is the same (write operation).
// The parameter 'sendbyte' least significant 8 bits are used.
//
// 'portnum'    - number 0 to MAX_PORTNUM-1.  This number is provided to
//                indicate the symbolic port number.
// 'sendbyte'   - 8 bits to send (least significant byte)
//
// Returns:  TRUE: bytes written and echo was the same
//           FALSE: echo was not the same
//
SMALLINT owWriteByte(int portnum, SMALLINT sendbyte)
{
    return (owTouchByte(portnum, sendbyte) == sendbyte) ? TRUE : FALSE;
}

//--------------------------------------------------------------------------
// Send 8 bits of read communication to the 1-Wire Net and and return the
// result 8 bits read from the 1-Wire Net.
//
// 'portnum'    - number 0 to MAX_PORTNUM-1.  This number is provided to
//                indicate the symbolic port number.
//
// Returns:  8 bytes read from 1-Wire Net
//
SMALLINT owReadByte(int portnum)
{
    return owTouchByte(portnum, 0xFF);
}

//--------------------------------------------------------------------------
// Set the 1-Wire Net communucation speed.
//
// 'portnum'    - number 0 to MAX_PORTNUM-1.  This number is provided to
//                indicate the symbolic port number.
// 'new_speed'  - new speed defined as
//                MODE_NORMAL     0x00
//                MODE_OVERDRIVE  0x01
//
// Returns:  current 1-Wire Net speed
//
SMALLINT owSpeed(int portnum, SMALLINT new_speed)
{
    return 0;
}

//--------------------------------------------------------------------------
// Set the 1-Wire Net line level.  The values for NewLevel are
// as follows:
//
// 'portnum'    - number 0 to MAX_PORTNUM-1.  This number is provided to
//                indicate the symbolic port number.
// 'new_level'  - new level defined as
//                MODE_NORMAL     0x00
//                MODE_STRONG5    0x02
//                MODE_PROGRAM    0x04
//                MODE_BREAK      0x08
//
// Returns:  current 1-Wire Net level
//
// Note: Strong and Program not supported on 550 target.
SMALLINT owLevel(int portnum, SMALLINT new_level)
{
//TODO what does this mean???
    unsigned long nl = new_level;
    switch (new_level) {
        case MODE_NORMAL:
            break;
        case MODE_STRONG5:
            break;
        case MODE_PROGRAM:
        case MODE_OVERDRIVE:
        case MODE_BREAK:
            return 0;
    }
    ioctl(portnum, IOCTL_CORTEX_OW_SPU, &nl);
    return (SMALLINT) nl;
}

//--------------------------------------------------------------------------
// This procedure creates a fixed 480 microseconds 12 volt pulse
// on the 1-Wire Net for programming EPROM iButtons.
//
// 'portnum'    - number 0 to MAX_PORTNUM-1.  This number is provided to
//                indicate the symbolic port number.
//
// Returns:  TRUE  successful
//           FALSE program voltage not available
//
SMALLINT owProgramPulse(int portnum)
{
//TODO do we support this ??????
    return FALSE;
}

//--------------------------------------------------------------------------
//  Description:
//     Delay for at least 'delay' ms
//
void msDelay(int delay)
{
    usleep(delay * 1000);
}

//--------------------------------------------------------------------------
//  Description:
//     Delay for at least 'delay' microseconds
//
//
void usDelay(unsigned int delay)
{
    usleep(delay);
}

//--------------------------------------------------------------------------
// Get the current millisecond tick count.  Does not have to represent
// an actual time, it just needs to be an incrementing timer.
//
long msGettick(void)
{
    /* TODO
    struct timespec ts;
    if (clock_gettime(CLOCK_MONOTONIC, &ts) != 0) {
        return 0; //????
    }*/
    return 0;
}

//--------------------------------------------------------------------------
// Send 8 bits of communication to the 1-Wire Net and verify that the
// 8 bits read from the 1-Wire Net is the same (write operation).
// The parameter 'sendbyte' least significant 8 bits are used.  After the
// 8 bits are sent change the level of the 1-Wire net.
//
// 'portnum'  - number 0 to MAX_PORTNUM-1.  This number was provided to
//              OpenCOM to indicate the port number.
// 'sendbyte' - 8 bits to send (least significant byte)
//
// Returns:  TRUE: bytes written and echo was the same
//           FALSE: echo was not the same
//
SMALLINT owWriteBytePower(int portnum, SMALLINT sendbyte)
{
    if (owLevel(portnum, MODE_STRONG5) != MODE_STRONG5)
        return FALSE;

    if (owTouchByte(portnum, sendbyte) != sendbyte)
        return FALSE;

    return TRUE;
}

//--------------------------------------------------------------------------
// Send 1 bit of communication to the 1-Wire Net and verify that the
// response matches the 'applyPowerResponse' bit and apply power delivery
// to the 1-Wire net.  Note that some implementations may apply the power
// first and then turn it off if the response is incorrect.
//
// 'portnum'  - number 0 to MAX_PORTNUM-1.  This number was provided to
//              OpenCOM to indicate the port number.
// 'applyPowerResponse' - 1 bit response to check, if correct then start
//                        power delivery
//
// Returns:  TRUE: bit written and response correct, strong pullup now on
//           FALSE: response incorrect
//
SMALLINT owReadBitPower(int portnum, SMALLINT applyPowerResponse)
{
    if (owLevel(portnum, MODE_STRONG5) != MODE_STRONG5)
        return FALSE;

    if (owTouchBit(portnum, 0x01) != applyPowerResponse)
        return FALSE;

    return TRUE;
}

//--------------------------------------------------------------------------
// This procedure indicates wether the adapter can deliver power.
//
// 'portnum'  - number 0 to MAX_PORTNUM-1.  This number was provided to
//              OpenCOM to indicate the port number.
//
// Returns:  TRUE  if adapter is capable of delivering power.
//
SMALLINT owHasPowerDelivery(int portnum)
{
    return FALSE;
}

//--------------------------------------------------------------------------
// This procedure indicates wether the adapter can deliver power.
//
// 'portnum'  - number 0 to MAX_PORTNUM-1.  This number was provided to
//              OpenCOM to indicate the port number.
//
// Returns:  TRUE  if adapter is capable of over drive.
//
SMALLINT owHasOverDrive(int portnum)
{
    portnum = 0;
    return FALSE;
}

//--------------------------------------------------------------------------
// This procedure creates a fixed 480 microseconds 12 volt pulse
// on the 1-Wire Net for programming EPROM iButtons.
//
// 'portnum'  - number 0 to MAX_PORTNUM-1.  This number was provided to
//              OpenCOM to indicate the port number.
//
// Returns:  TRUE  program volatage available
//           FALSE program voltage not available
SMALLINT owHasProgramPulse(int portnum)
{
    portnum = 0;
    return FALSE;
}

