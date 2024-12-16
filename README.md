# Arduino Microsecond Timer
When `micros()` just isn't enough.

The microsecond time resolution on 16MHz ATmega-based Arduino boards (Uno, Mega, Nano, etc.) is 4 microseconds, but if you want finer precision, you can use this little timer instead.

**Notes:**
- Precision: 1μs
- Based on [Timer/Counter2](http://ww1.microchip.com/downloads/en/DeviceDoc/Atmel-2549-8-bit-AVR-Microcontroller-ATmega640-1280-1281-2560-2561_datasheet.pdf#page=169)
- Respects and restores the current interrupts enabled status

**Installation:**
- Drop the `microsecond_time.h` and `microsecond_time.cpp` files into your Arduino project's `src` directory:
```
Project/
    Sketch.ino
    src/
        microsecond-time/
            microsecond_time.h
            microsecond_time.cpp
```

**Usage:**
```c++
// Include the header
#include "src/microsecond-time/microsecond_time.h"

void setup() {
    // Enable the microsecond timer:
    enableMicrosecondTime();
}

void loop() {
    // Retrieve a 1μs precise time snapshot:
    unsigned long currentMicrosecondTime = microsecondTime();
}

// If you want to disable the microsecond timer at any point:
disableMicrosecondTime();
```
