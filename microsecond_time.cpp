/*
 * microsecond_time.cpp
 */

#include "microsecond_time.h"
#include <Arduino.h>

class MicrosecondTimer {
    private:
        volatile unsigned long overflowCount;

    public:
        MicrosecondTimer() {}

        void enable() {
            // Store current interrupt enabled status
            byte copiedSREG = SREG;

            // Temporarily disable interrupts
            noInterrupts();

            overflowCount = 0;

            // Clear TCCR2A for "Normal" mode (datasheet pgs. 172, 182)
            // http://ww1.microchip.com/downloads/en/DeviceDoc/Atmel-2549-8-bit-AVR-Microcontroller-ATmega640-1280-1281-2560-2561_datasheet.pdf#page=172
            TCCR2A = 0b00000000;

            // Clear TCCR2B for "Normal" mode and set CS21 = 1 for prescaling of 8 (0.5 us) (datasheet pg. 185)
            // http://ww1.microchip.com/downloads/en/DeviceDoc/Atmel-2549-8-bit-AVR-Microcontroller-ATmega640-1280-1281-2560-2561_datasheet.pdf#page=185
            TCCR2B = 0b00000010;

            // Clear TIMSK2 and set TOIE = 1 to enable Timer 2 overflow interrupt (datasheet pg. 188)
            // http://ww1.microchip.com/downloads/en/DeviceDoc/Atmel-2549-8-bit-AVR-Microcontroller-ATmega640-1280-1281-2560-2561_datasheet.pdf#page=188
            TIMSK2 = 0b00000001;

            // Restore interrupt enabled status to previous value
            SREG = copiedSREG;
        }

        unsigned long microsecondTime() {
            // Store current interrupt enabled status
            byte copiedSREG = SREG;

            // Temporarily disable interrupts
            noInterrupts();

            byte copiedTCNT2 = TCNT2;
            boolean overflowFlag = bitRead(TIFR2, 0);

            if (overflowFlag) {
                // TCNT2 could have just overflowed from 255 to 0 without being handled by the overflow ISR yet.
                // In that case, copy TCNT2 again (to avoid an erroneous 255),
                // manually increment the overflow count, and clear the overflow flag.
                copiedTCNT2 = TCNT2;
                overflowCount++;

                // Clear TIFR2 and set TOV2 = 1 to clear Timer 2 overflow flag (datasheet pg. 188)
                // http://ww1.microchip.com/downloads/en/DeviceDoc/Atmel-2549-8-bit-AVR-Microcontroller-ATmega640-1280-1281-2560-2561_datasheet.pdf#page=188
                TIFR2 = 0b00000001;
            }

            unsigned long totalCount = overflowCount * 256 + copiedTCNT2;

            // Restore interrupt enabled status to previous value
            SREG = copiedSREG;

            // Divide by 2 since `totalCount` refers to 0.5 us (see prescaling in `enable()`)
            return totalCount / 2;
        }

        void handleOverflow() {
            overflowCount++;
        }

        void disableMicrosecondTime() {
            // Store current interrupt enabled status
            byte copiedSREG = SREG;

            // Temporarily disable interrupts
            noInterrupts();

            // Clear TIMSK2 and set TOIE = 0 to disable Timer 2 overflow interrupt (datasheet pg. 188)
            // http://ww1.microchip.com/downloads/en/DeviceDoc/Atmel-2549-8-bit-AVR-Microcontroller-ATmega640-1280-1281-2560-2561_datasheet.pdf#page=188
            TIMSK2 = 0b00000000;

            // Reset overflow count
            overflowCount = 0;

            // Restore interrupt enabled status to previous value
            SREG = copiedSREG;
        }
};

/*
 * Single Timer Instance
 */
MicrosecondTimer microsecondTimer;

/*
 * Timer 2 Overflow Handler
 */
ISR(TIMER2_OVF_vect) {
  microsecondTimer.handleOverflow();
}

/*
 * Public Interface
 */
void enableMicrosecondTime() {
    microsecondTimer.enable();
}

unsigned long microsecondTime() {
    return microsecondTimer.microsecondTime();
}

void disableMicrosecondTime() {
    microsecondTimer.disableMicrosecondTime();
}
