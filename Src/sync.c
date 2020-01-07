#include <string.h>

#include "lwip.h"
#include "lwip/inet.h"
#include "lwip/netdb.h"
#include "lwip/api.h"
#include "lwip/ip_addr.h"
#include <stm32f4xx_hal_gpio.h>
#include <stm32f4xx_hal_rtc.h>

#include <time.h>
#include "sync.h"
#include "signaling_diode.h"

#define SNTP_ADDR "192.168.0.88"
#define SNTP_PORT 123

SemaphoreHandle_t connectionEnableSemaphore;
SemaphoreHandle_t syncSemaphore;
extern RTC_HandleTypeDef hrtc;

void hang() {
        SD_SetState(FAILURE);
        for (;;) {
                osDelay(420);
        }
}

void HAL_GPIO_EXTI_Callback (uint16_t GPIO_Pin) {
        if (GPIO_Pin == USER_Btn_Pin) xSemaphoreGiveFromISR(syncSemaphore, NULL);
}

void SY_TaskFunc(void *param){
        SD_SetState(CONNECTING);
        
        printf("Waiting for LWIP to be initialized...");
        if (xSemaphoreTake(connectionEnableSemaphore, 30000) != pdTRUE){
                hang();
        };
        printf(" done \r\n");


        struct netconn* conn;
        Connect(&conn);
        SD_SetState(CONNECTED);

        SetRemoteTime(conn);
        PrintCurrentTime();

        while (1) {
                if (xSemaphoreTake(syncSemaphore, 100) == pdTRUE)
                {
                        Sync(conn);
                        // debug_only(PrintCurrentTime());

                }
                debug_only(PrintCurrentTime());
        }
}

int Connect(struct netconn **conn) {
        struct ip4_addr server_ip;
        int result;

        ipaddr_aton(SNTP_ADDR, &server_ip); 
        
        printf("creating new LWIP connection...");
        *conn = netconn_new(NETCONN_UDP);
        if (*conn == NULL) {
                return -1;
        }
        printf(" done \r\n");

        printf("connecting to %s...", SNTP_ADDR);
        if ((result = netconn_connect(*conn, &server_ip, SNTP_PORT)) != ERR_OK) {
                return result;
        }
        printf(" done \r\n");
        
        return 0;
}

void SNTP_TimestampToTime(SNTP_Timestamp *timestamp, RTC_TimeTypeDef *rtc_time){
        rtc_time->Hours = (timestamp->seconds / 3600) % 24;
        rtc_time->Minutes = (timestamp->seconds / 60) % 60;
        rtc_time->Seconds = timestamp->seconds % 60;
        rtc_time->SubSeconds = 107;
}

void SNTP_TimestampToSubSeconds(SNTP_Timestamp *timestamp, uint32_t *subseconds) {
        *subseconds = timestamp->seconds_fraction >> (32 - SNTP_SECONDS_FRACTION_SHIFT);
}

void SNTP_TimestampToDate(SNTP_Timestamp *timestamp, RTC_DateTypeDef *rtc_date) {
        struct tm datetime;
        time_t seconds_time_t = timestamp->seconds - SNTP_UNIX_TIMESTAMP_DIFF + TIME_H_DIFF;
        localtime_r(&seconds_time_t, &datetime);

        debug_only(printf("yer: %d\r\n", datetime.tm_year));

        rtc_date->Date = datetime.tm_mday;
        rtc_date->Month = datetime.tm_mon + 1;
        rtc_date->Year = datetime.tm_year % 100;
}

int SNTP_GetTime(SNTP_Timestamp *currentTimestamp, struct netconn *conn) {
        struct netbuf *sendbuf, *recvbuf;
        SNTP_Frame client_sntp_frame = {0}, *server_sntp_frame;
        int send_result, recv_result;
        u16_t recv_size;

        sendbuf = netbuf_new();
        client_sntp_frame.header = SNTP_MODE_CLIENT;
        netbuf_ref(sendbuf, &client_sntp_frame, sizeof(client_sntp_frame));

        send_result = netconn_send(conn, sendbuf);
        if (send_result != ERR_OK) {
                printf("aaa\r\n");
                return -1;
        } 
        debug_only(printf("UDP message sent \r\n"));
        // osDelay(100);

        recv_result = netconn_recv(conn, &recvbuf);
        if (recv_result != ERR_OK) {
                return -1;
        } 
        debug_only(printf("UDP message recieved \r\n"));
        netbuf_data(recvbuf, (void**) &server_sntp_frame, &recv_size);

        currentTimestamp->seconds = ntohl(server_sntp_frame->receive_timestamp.seconds);
        currentTimestamp->seconds_fraction = ntohl(server_sntp_frame->receive_timestamp.seconds_fraction);

        netbuf_delete(sendbuf);
        netbuf_delete(recvbuf);
        return 0;
}

void SetRTCTime(SNTP_Timestamp *timestamp) {
        RTC_DateTypeDef date = {0};
        RTC_TimeTypeDef time = {0};

        SNTP_TimestampToDate(timestamp, &date);
        SNTP_TimestampToTime(timestamp, &time);

        HAL_RTC_SetDate(&hrtc, &date, RTC_FORMAT_BIN);
        HAL_RTC_SetTime(&hrtc, &time, RTC_FORMAT_BIN);
}

void SetRemoteTime(struct netconn *conn) {
        SNTP_Timestamp timestamp = {0};

        printf("Synchronizing time... \r\n");
        SNTP_GetTime(&timestamp, conn);
        SetRTCTime(&timestamp);
}

void PrintCurrentTime() {
        RTC_TimeTypeDef time;
        RTC_DateTypeDef date;
        HAL_RTC_GetTime(&hrtc, &time, RTC_FORMAT_BIN);
        HAL_RTC_GetDate(&hrtc, &date, RTC_FORMAT_BIN);
        
        printf("%d-%d-%d %d:%d:%d %ld %ld \r\n", date.Date, date.Month, date.Year, time.Hours, time.Minutes, time.Seconds, time.SubSeconds, time.SecondFraction);
}

void LocalDateTimeToSNTP(SNTP_Timestamp *sntp_time, RTC_DateTypeDef *rtc_date, RTC_TimeTypeDef *rtc_time){
        struct tm datetime = {0};
        //not Y2.1K compliant:
        datetime.tm_year = rtc_date->Year + 2000;
        datetime.tm_mon = rtc_date->Month - 1;
        datetime.tm_mday = rtc_date->Date;
        datetime.tm_hour = rtc_time->Hours;
        datetime.tm_min = rtc_time->Minutes;
        datetime.tm_sec = rtc_time->Seconds;
        time_t unixDate = mktime(&datetime) - TIME_H_DIFF;
        sntp_time->seconds = unixDate + SNTP_UNIX_TIMESTAMP_DIFF;
        sntp_time->seconds_fraction = (255 - rtc_time->SubSeconds) << 24;
}

void SNTP_Timestamps_Add(SNTP_Timestamp* op1, SNTP_Timestamp* op2) {
        uint32_t overflow = 0;
        if ((op1->seconds_fraction / 2 + op2->seconds_fraction / 2) > 0x7FFFFFFF) {
                overflow = 1;
        }

        op1->seconds_fraction += op2->seconds_fraction;
        op1->seconds += op2->seconds + overflow;
}

void SNTP_Timestamps_Subtract(SNTP_Timestamp* op1, SNTP_Timestamp* op2) {
        if (op2->seconds_fraction > op1->seconds_fraction) {
                op1->seconds_fraction = 0xFFFFFFFF - (op2->seconds_fraction - op1->seconds_fraction) + 0x1;
                op1->seconds -= (op2->seconds + 1);
        } else {
                op1->seconds_fraction -= op2->seconds_fraction;
                op1->seconds -= op2->seconds;
        }
}

void Sync(struct netconn* conn) {
        SNTP_Timestamp sntp_time, rtc_time = {0};
        uint32_t sntp_subseconds;
        int32_t seconds_fractions_diff, seconds_diff, shift;
        RTC_DateTypeDef rtc_date_in_rtc_format;
        RTC_TimeTypeDef rtc_time_in_rtc_format;

        SNTP_GetTime(&sntp_time, conn);
        HAL_RTC_GetTime(&hrtc, &rtc_time_in_rtc_format, RTC_FORMAT_BIN);
        HAL_RTC_GetDate(&hrtc, &rtc_date_in_rtc_format, RTC_FORMAT_BIN);
        LocalDateTimeToSNTP(&rtc_time, &rtc_date_in_rtc_format, &rtc_time_in_rtc_format);

        //delete lateor
        printf("SNTP: %lu %lu\r\n", sntp_time.seconds, sntp_time.seconds_fraction);
        printf("RTC: %lu %lu\r\n", rtc_time.seconds, rtc_time.seconds_fraction);
        
        seconds_fractions_diff = (sntp_time.seconds_fraction >> (32 - SNTP_SECONDS_FRACTION_SHIFT)) - (rtc_time.seconds_fraction >> (32 - SNTP_SECONDS_FRACTION_SHIFT));
        seconds_diff = sntp_time.seconds - rtc_time.seconds;
        shift = seconds_fractions_diff + (seconds_diff << SNTP_SECONDS_FRACTION_SHIFT);

        printf("Shift: %li\r\n", shift);

        if (shift > 0) {
                HAL_RTCEx_SetSynchroShift(&hrtc, RTC_SHIFTADD1S_SET, SUBSECONDS_PER_SECOND - shift);
        } else if (shift < 0) {
                HAL_RTCEx_SetSynchroShift(&hrtc, RTC_SHIFTADD1S_RESET, -shift);
        }
}