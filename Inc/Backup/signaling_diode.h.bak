#ifndef __SIGNALING_DIODE_H
#define __SIGNALING_DIODE_H

typedef enum {
        CONNECTING,
        CONNECTED,
        FAILURE
} SD_DiodeState;

#define SD_TASK "SIGNALING_DIODE_TASK"

// Task function of signaling diode
void SD_TaskFunc(void* param);

// Changes state of signaling diode
void SD_SetState(SD_DiodeState diodeState);

#endif