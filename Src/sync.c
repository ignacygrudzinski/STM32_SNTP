#include <string.h>

#include "lwip.h"
#include "lwip/inet.h"
#include "lwip/netdb.h"
#include "lwip/api.h"
#include "lwip/ip_addr.h"

#include "sync.h"
#include "signaling_diode.h"

#define SNTP_ADDR "10.0.0.2"

SemaphoreHandle_t connectionEnableSemaphore;


void hang() {
        SD_SetState(FAILURE);
        for (;;) {
                osDelay(420);
        }
}

void SY_TaskFunc(void* param){
        SD_SetState(CONNECTING);
        
        // MX_LWIP_Init(); // todo: decancerify
        // int sock;
        // sock = socket(AF_INET, SOCK_DGRAM, 0);
        // if (sock == -1) hang();

        // int status;
        // struct addrinfo hints;
        // memset(&hints, 0, sizeof(hints));
        // struct addrinfo* addr;
        // if (getaddrinfo("10.1.1.2", "2137", &hints, &addr) != 0) {
        //         hang();
        // }

        // if (connect(sock, addr->ai_addr, addr->ai_addrlen) != 0) {
        //         hang();
        // }

        // SD_SetState(CONNECTED);

        // for(;;) {        
        // }
        printf("Waiting for LWIP to be initialized...");
        if (xSemaphoreTake(connectionEnableSemaphore, 30000) != pdTRUE){
                hang();
        };
        printf(" done \r\n");

        printf("Delay (TBD: do not wait here)...");
        osDelay(30000);
        printf(" done \r\n");

        struct netconn* conn;
        struct ip4_addr server_ip;
        // server_ip.addr = 3232235908;
        ipaddr_aton(SNTP_ADDR, &server_ip); 
        
        printf("creating new LWIP connection...");
        conn = netconn_new(NETCONN_TCP);
        if (conn == NULL) {
                printf(" error \r\n");
                hang();
        }
        printf(" done \r\n");

        printf("connecting to %s...", SNTP_ADDR);
        int conn_result = netconn_connect(conn, &server_ip, 2137);
        if (conn_result != ERR_OK) {
                printf(" error %d\r\n", conn_result);
                hang();
        }
        printf(" done \r\n");

        SD_SetState(CONNECTED);

        for(;;) {
                osDelay(420);   
        }
}