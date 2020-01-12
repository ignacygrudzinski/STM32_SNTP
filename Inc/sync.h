#ifndef __SYNC_H
#define __SYNC_H

#include "semphr.h"

#define SY_TASK "SYNC_TASK"

#define DEBUG
#ifdef DEBUG
        #define debug_only(command) command;
#else
        #define debug_only(command)
#endif

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

#define SNTP_MODE_CLIENT 3
#define SNTP_MODE_SERVER 4

#define SNTP_VERSION_4 4 << 3

#define TIME_H_DIFF 59958144000
#define SNTP_UNIX_TIMESTAMP_DIFF 2208988800UL


#define SNTP_SECONDS_FRACTION_SHIFT 8
#define SUBSECONDS_PER_SECOND (2 << SNTP_SECONDS_FRACTION_SHIFT)

// SNTP functions -> sntp.h

int Connect(struct netconn **conn);
int SNTP_GetTime(SNTP_Timestamp *currentTimestamp, struct netconn *conn);
int32_t SNTP_GetDelay(struct netconn *conn);
int32_t CalculateClockOffset(SNTP_Timestamp *originate_timestamp, SNTP_Timestamp *receive_timestamp, SNTP_Timestamp *transmit_timestamp, SNTP_Timestamp *destination_timestamp);

// Synchronization functions

void SetRemoteTime(struct netconn *conn);
void Sync(struct netconn *conn);

// SNTP <-> RTC conversion functions -> conversion.h

void SNTP_TimestampToTime(SNTP_Timestamp *timestamp, RTC_TimeTypeDef *rtc_time);
void SNTP_TimestampToDate(SNTP_Timestamp *timestamp, RTC_DateTypeDef *rtc_date);
void LocalDateTimeToSNTP(SNTP_Timestamp *sntp_time, RTC_DateTypeDef *rtc_date, RTC_TimeTypeDef *rtc_time);

// SNTP <-> RTC utils

void SetRTCTime(SNTP_Timestamp *timestamp);
void GetRTCTimeInSNTPFormat(SNTP_Timestamp* timestamp);

// Debug functions

void PrintCurrentTime();


#endif