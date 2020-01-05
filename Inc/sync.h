#ifndef __SYNC_H
#define __SYNC_H

#include "semphr.h"

#define SY_TASK "SYNC_TASK"

extern SemaphoreHandle_t connectionEnableSemaphore;
extern SemaphoreHandle_t syncSemaphore;

void SY_TaskFunc(void* param);

typedef struct {
        uint32_t seconds;
        uint32_t seconds_fraction;
} SNTP_Timestamp;

typedef struct {
        uint8_t header;
        uint8_t stratum;
        uint8_t poll;
        uint8_t precision;
        uint32_t root_delay;
        uint32_t root_dispersion;
        uint32_t reference_identifier;
        SNTP_Timestamp reference_timestamp;
        SNTP_Timestamp originate_timestamp;
        SNTP_Timestamp receive_timestamp;
        SNTP_Timestamp transit_timestamp;
} SNTP_Frame;

RTC_DateTypeDef SNTP_TimestampToDate(SNTP_Timestamp timestamp);
RTC_TimeTypeDef SNTP_TimestampToTime(SNTP_Timestamp timestamp);

#endif