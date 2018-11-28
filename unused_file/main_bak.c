//main.c
//authored by Jared Hull
//
//tasks 1 and 2 blink the ACT LED
//main initialises the devices and IP tasks

#include <FreeRTOS.h>
#include <task.h>

#include "interrupts.h"
#include "gpio.h"
// #include "video.h"
#include <ili9340.h>
#include "FreeRTOS_IP.h"
#include "FreeRTOS_Sockets.h"


//Only for debug, normally should not 
//include private header
#include "FreeRTOS_IP_Private.h"

#define GPPUD       0x20200094
#define GPPUDCLK0   0x20200098

typedef unsigned char uint8_t;
typedef unsigned short uint16_t;
typedef long int32_t;

extern void PUT32 ( unsigned int, unsigned int );
extern unsigned int GET32 ( unsigned int );
extern void dummy ( unsigned int );

unsigned int ra;

void task1() {
	int i = 0;
	while(1) {
		i++;
		SetGpio(47, 1);
		vTaskDelay(200);
	}
}

void task2() {
	int i = 0;
	while(1) {
		i++;
		vTaskDelay(100);
		SetGpio(47, 0);
		vTaskDelay(100);
	}
}

void task3() {
	int i = 0;
	int rb;
	while(1) {
		i++;
		vTaskDelay(5000);
		if (ReadGpio(18) == 0) {
			// println("KEY1 is pressed", BLUE_TEXT); 
			rb = (~ReadGpio(47))&0x00000001;
			SetGpio(47, rb);
			// vTaskDelay(5000);    
		}
	}
}

//server task does not work in this build, it fails to accept a connection
void serverTask() {
	//setup a socket structure
	Socket_t listenfd = FreeRTOS_socket(FREERTOS_AF_INET, FREERTOS_SOCK_STREAM, FREERTOS_IPPROTO_TCP);
	struct freertos_sockaddr server, client;
	server.sin_port = FreeRTOS_htons(4242);
	server.sin_addr = FreeRTOS_inet_addr("192.168.1.42");

	socklen_t cli_size = sizeof(client);

	FreeRTOS_bind(listenfd, (struct freertos_sockaddr*)&server, sizeof(server));
	FreeRTOS_listen(listenfd, 3);

	while(1) {
		loaded = 2;
		Socket_t connectfd = FreeRTOS_accept(listenfd, (struct freertos_sockaddr*)&client, &cli_size);
		loaded = 1;
		println("    accepted", 0xFFFFFFFF);
		char recieved[9000];
		FreeRTOS_recv(connectfd, recieved, 9000, 0);
		println(recieved, 0xFFFFFFFF);

		char *message = "Forty-Two";
		FreeRTOS_send(connectfd, message, sizeof(message), 0);
		println(message, 0xFFFFFFFF);

		FreeRTOS_shutdown(connectfd, FREERTOS_SHUT_RDWR);
	}
	FreeRTOS_shutdown(listenfd, FREERTOS_SHUT_RDWR);
}

#undef CREATE_SOCK_TASK
#define tcpechoSHUTDOWN_DELAY	(pdMS_TO_TICKS(5000))

//server task DOES work in this build, it DOES accept a connection
void serverListenTask() {
	int status = 0;
	static const portTickType xReceiveTimeOut = portMAX_DELAY;
	const portTickType xDelay8s = pdMS_TO_TICKS(8000UL);
	const portTickType xDelay500ms = pdMS_TO_TICKS(500UL);
	const portBASE_TYPE xBacklog = 4;

	portTickType xTimeOnShutdown;
	uint8_t *pucRxBuffer;
	// for debug 
	FreeRTOS_Socket_t * sockt;

	//setup a socket structure
	//loaded = 2;
	//printAddressConfiguration();
	volatile int times = 10; 
	println("Server task starting", BLUE_TEXT);
	if (FreeRTOS_IsNetworkUp()) {
		println("Network is UP", BLUE_TEXT);    
	}
	else {
		println("Network is Down", BLUE_TEXT);
		while (!FreeRTOS_IsNetworkUp()) {
			vTaskDelay( xDelay500ms );
		}    
	}
	println("Serv tsk done wait net", BLUE_TEXT);

	Socket_t listen_sock;
	listen_sock = FreeRTOS_socket(FREERTOS_AF_INET, FREERTOS_SOCK_STREAM, FREERTOS_IPPROTO_TCP);
	printHex("listenfd sock: ", (int)listen_sock, BLUE_TEXT);
	printHex("IPPROTO_TCP val: ", FREERTOS_IPPROTO_TCP, BLUE_TEXT);
	if (listen_sock == FREERTOS_INVALID_SOCKET) {
		println("Socket is NOT valid", GREEN_TEXT);
	}
	else {
		println("Socket is valid", BLUE_TEXT);
		sockt = (FreeRTOS_Socket_t*)listen_sock;
		if (sockt->ucProtocol == FREERTOS_IPPROTO_TCP) {
			println("Proto is TCP", BLUE_TEXT);
		} 
		else {
			printHex("Proto NOT TCP: ", sockt->ucProtocol, BLUE_TEXT);
		}
	}

	/* Set a time out so accept() will just wait for a connection. */
	FreeRTOS_setsockopt(listen_sock,
						0,
						FREERTOS_SO_RCVTIMEO,
						&xReceiveTimeOut,
						sizeof( xReceiveTimeOut ) 
						);
	
	/**
	 ** If I dont set REUSE option, accept will never return
	**/
	portBASE_TYPE xReuseSocket = pdTRUE;
	FreeRTOS_setsockopt(listen_sock,
						0,
						FREERTOS_SO_REUSE_LISTEN_SOCKET,
						(void *)&xReuseSocket,
						sizeof(xReuseSocket)
						);

	struct freertos_sockaddr server, client;
	server.sin_port = FreeRTOS_htons((uint16_t)2056);
	//server.sin_addr = FreeRTOS_inet_addr("192.168.1.9");

	socklen_t cli_size = sizeof(client);
	println("Server task about to bind", BLUE_TEXT);
	status = FreeRTOS_bind(listen_sock, &server, sizeof(server));
	printHex("bind status: ", (int)status, BLUE_TEXT);
	sockt = (FreeRTOS_Socket_t*)listen_sock;
	printHex("Bind port: ", (unsigned int)sockt->usLocPort, BLUE_TEXT);

	println("Server task about to listen", BLUE_TEXT);
	status = FreeRTOS_listen(listen_sock, xBacklog);
	printHex("listen status: ", (int)status, BLUE_TEXT);

	int clients = 0;
	int32_t lBytes, lSent, lTotalSent;

	#ifdef CREATE_SOCK_TASK
		for (;;) {
			//Only seem to be able to spawn one client task that works.  Additional client tasks don't work. 
			println("Server task accepting", BLUE_TEXT);
			printHex("Serv sock TCP State: ", (unsigned int)sockt->u.xTCP.ucTCPState, BLUE_TEXT);
			Socket_t connect_sock = FreeRTOS_accept(listen_sock, (struct freertos_sockaddr*)&client, &cli_size);
			println("    accepted", BLUE_TEXT);

			xTaskCreate(prvServerConnectionInstance, "EchoServer", 4096, (void*) connect_sock, tskIDLE_PRIORITY, NULL);
		}
	#else
		println("Server task accepting", BLUE_TEXT);
		Socket_t connect_sock = FreeRTOS_accept(listen_sock, (struct freertos_sockaddr*)&client, &cli_size);
		println("    accepted", BLUE_TEXT);
		
		pucRxBuffer = (uint8_t*) pvPortMalloc(ipconfigTCP_MSS);
		for ( ;; ) {
			memset(pucRxBuffer, 0x00, ipconfigTCP_MSS);
			if ((lBytes = FreeRTOS_recv(connect_sock, pucRxBuffer, ipconfigTCP_MSS, 0)) > 0) {
				printHex("Chars Received: ", (unsigned int)lBytes, BLUE_TEXT);
				println(pucRxBuffer, BLUE_TEXT);
				lSent = 0;
				lTotalSent = 0;
				while ((lSent >= 0) && (lTotalSent < lBytes)) {
					lSent = FreeRTOS_send(connect_sock, pucRxBuffer, lBytes-lTotalSent, 0);
					lTotalSent += lSent;  
				}
				if (lSent < 0)
					break;
			}
			else {
				FreeRTOS_shutdown(connect_sock, FREERTOS_SHUT_RDWR);

				/* Wait for the shutdown to take effect, indicated by FreeRTOS_recv()
				returning an error. */
				xTimeOnShutdown = xTaskGetTickCount();
				do {
					if( FreeRTOS_recv(connect_sock, pucRxBuffer, ipconfigTCP_MSS, 0 ) < 0) {
						break;
					}
				} while( (xTaskGetTickCount() - xTimeOnShutdown ) < tcpechoSHUTDOWN_DELAY);

				vPortFree(pucRxBuffer);
				FreeRTOS_closesocket(connect_sock);

				break;
			} 
		}
	#endif

	#ifndef CREATE_SOCK_TASK
	vTaskDelay(xDelay8s);
	for (;;) {
		println("Srv Tsk spin", BLUE_TEXT);
		vTaskDelay( xDelay8s );
	}
	#endif
}

static void prvServerConnectionInstance(void *pvParameters) {
	int32_t lBytes, lSent, lTotalSent;
	Socket_t xConnectedSocket;
	static const portTickType xReceiveTimeOut = pdMS_TO_TICKS(5000);
	static const portTickType xSendTimeOut = pdMS_TO_TICKS(5000);
	portTickType xTimeOnShutdown;
	uint8_t *pucRxBuffer;

	xConnectedSocket = (Socket_t) pvParameters;

	/* Attempt to create the buffer used to receive the string to be echoed
	back.  This could be avoided using a zero copy interface that just returned
	the same buffer. */
	pucRxBuffer = (uint8_t*) pvPortMalloc(ipconfigTCP_MSS);

	if(pucRxBuffer != NULL) {
		FreeRTOS_setsockopt(xConnectedSocket, 0, FREERTOS_SO_RCVTIMEO, &xReceiveTimeOut, sizeof(xReceiveTimeOut));
		FreeRTOS_setsockopt(xConnectedSocket, 0, FREERTOS_SO_SNDTIMEO, &xSendTimeOut, sizeof(xReceiveTimeOut));

		for(;;) {
			/* Zero out the receive array so there is NULL at the end of the string
			when it is printed out. */
			memset(pucRxBuffer, 0x00, ipconfigTCP_MSS);

			/* Receive data on the socket. */
			lBytes = FreeRTOS_recv(xConnectedSocket, pucRxBuffer, ipconfigTCP_MSS, 0);

			/* If data was received, echo it back. */
			if(lBytes >= 0) {
				lSent = 0;
				lTotalSent = 0;

				/* Call send() until all the data has been sent. */
				while((lSent >= 0) && (lTotalSent < lBytes)) {
					lSent = FreeRTOS_send(xConnectedSocket, pucRxBuffer, lBytes - lTotalSent, 0);
					lTotalSent += lSent;
				}

				if(lSent < 0) {
					/* Socket closed? */
					break;
				}
			}
			else {
				/* Socket closed? */
				break;
			}
		}
	}

	/* Initiate a shutdown in case it has not already been initiated. */
	FreeRTOS_shutdown(xConnectedSocket, FREERTOS_SHUT_RDWR);

	/* Wait for the shutdown to take effect, indicated by FreeRTOS_recv()
	returning an error. */
	xTimeOnShutdown = xTaskGetTickCount();
	do {
		if(FreeRTOS_recv(xConnectedSocket, pucRxBuffer, ipconfigTCP_MSS, 0) < 0) {
			break;
		}
	} while((xTaskGetTickCount() - xTimeOnShutdown) < tcpechoSHUTDOWN_DELAY);

	/* Finished with the socket, buffer, the task. */
	vPortFree(pucRxBuffer);
	FreeRTOS_closesocket(xConnectedSocket);

	vTaskDelete(NULL);
}

int main(void) {
	// PUT32(GPPUD,2);
    // for(ra=0;ra<150;ra++) dummy(ra);
    // PUT32(GPPUDCLK0,(1<<18));
    // for(ra=0;ra<150;ra++) dummy(ra);
	// PUT32(GPPUD,0);
    // for(ra=0;ra<150;ra++) dummy(ra);
    // PUT32(GPPUDCLK0,0);

	// SetGpioFunction(18, 0);
	SetGpioFunction(47, 1);			// RDY led

	// Inizializzazione Video per Debug
	// initFB();
	ili9340_init();
	println("StuFA", ILI9340_WHITE);

	SetGpio(47, 1);
	//videotest();

	// DisableInterrupts();
	// InitInterruptController();

	//ensure the IP and gateway match the router settings!
	//const unsigned char ucIPAddress[ 4 ] = {192, 168, 1, 42};
	// const unsigned char ucIPAddress[ 4 ] = {192, 168, 137, 9};
	// const unsigned char ucNetMask[ 4 ] = {255, 255, 255, 0};
	// const unsigned char ucGatewayAddress[ 4 ] = {192, 168, 137, 1};
	// const unsigned char ucDNSServerAddress[ 4 ] = {8, 8, 8, 8};
	//const unsigned char ucMACAddress[ 6 ] = {0xB8, 0x27, 0xEB, 0x19, 0xAD, 0xA7};
	// const unsigned char ucMACAddress[ 6 ] = {0xB8, 0x27, 0xEB, 0xa5, 0x35, 0xC1};
	//FreeRTOS_IPInit(ucIPAddress, ucNetMask, ucGatewayAddress, ucDNSServerAddress, ucMACAddress);

	//xTaskCreate(serverTask, "server", 128, NULL, 0, NULL);
	//xTaskCreate(serverListenTask, "server", 128, NULL, 0, NULL);

	xTaskCreate(task1, "LED_0", 128, NULL, 0, NULL);
	xTaskCreate(task2, "LED_1", 128, NULL, 0, NULL);
	// xTaskCreate(task3, "KEY_1", 128, NULL, 0, NULL);

	//set to 0 for no debug, 1 for debug, or 2 for GCC instrumentation (if enabled in config)
	loaded = 1;

	vTaskStartScheduler();

	/*
	 *	We should never get here, but just in case something goes wrong,
	 *	we'll place the CPU into a safe loop.
	 */
	while(1) {
		;
	}
}
