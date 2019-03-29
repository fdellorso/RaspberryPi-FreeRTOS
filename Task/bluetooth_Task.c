// bluetooth_Task.c
// authored by Francesco Dell'Orso
//
// tasks for StuFA Project

#include "stufa_Task.h"

#include <prvlib/stdlib.h>

extern TimerHandle_t		xWatchDogTimer;
extern QueueHandle_t		xQueueBLTiProc;

#define INQUIRY_SECONDS		20

void prvTask_BluetoothInitialize(void *pParam) {
	int i = 0;

	/* Stop warnings. */
	( void ) pParam;

	xTimerChangePeriod( xWatchDogTimer, initWATCHDOG_TIMER_PERIOD, 0 );

	prvFunc_Print("\nBluetooth Initialize...\t\t     Started");
	prvFunc_Print("--------------------------------------------");

	TBTSubSystem *m_Bluetooth = (TBTSubSystem *) malloc (sizeof(TBTSubSystem));
	BTSubSystem(m_Bluetooth, 0, NULL);

	if(BTSubSystemInitialize(m_Bluetooth)) {
		prvFunc_Print("Bluetooth Initialize...\t\t    Finished");

		xTimerChangePeriod( xWatchDogTimer, mainWATCHDOG_TIMER_PERIOD, 0 );
	}

	prvFunc_Print("Inquiry is running for %u seconds", INQUIRY_SECONDS);

	// TBTInquiryResults *pInquiryResults = BTSubSystemInquiry(m_Bluetooth, INQUIRY_SECONDS);
	// if (pInquiryResults == 0)
	// {
	// 	prvFunc_Print("Inquiry failed");
	// }

	// prvFunc_Print("Inquiry complete, %u device(s) found", BTInquiryResultsGetCount(pInquiryResults));

	// if (BTInquiryResultsGetCount(pInquiryResults) > 0)
	// {
	// 	prvFunc_Print("# BD address        Class  Name");

	// 	for (unsigned nDevice = 0; nDevice < BTInquiryResultsGetCount(pInquiryResults); nDevice++)
	// 	{
	// 		const u8 *pBDAddress = BTInquiryResultsGetBDAddress(pInquiryResults, nDevice);
	// 		assert (pBDAddress != 0);
			
	// 		const u8 *pClassOfDevice = BTInquiryResultsGetClassOfDevice(pInquiryResults, nDevice);
	// 		assert (pClassOfDevice != 0);
			
	// 		prvFunc_Print("%u %02X:%02X:%02X:%02X:%02X:%02X %02X%02X%02X %s",
	// 				nDevice+1,
	// 				(unsigned) pBDAddress[5],
	// 				(unsigned) pBDAddress[4],
	// 				(unsigned) pBDAddress[3],
	// 				(unsigned) pBDAddress[2],
	// 				(unsigned) pBDAddress[1],
	// 				(unsigned) pBDAddress[0],
	// 				(unsigned) pClassOfDevice[2],
	// 				(unsigned) pClassOfDevice[1],
	// 				(unsigned) pClassOfDevice[0],
	// 				BTInquiryResultsGetRemoteName(pInquiryResults, nDevice));
	// 	}
	// }

	// free(pInquiryResults);

	while(1) {
		i++;

		vTaskDelay(100);
	}
}

void prvTask_BluetoothProcess(void *pParam) {
	int i = 0;

	/* Stop warnings. */
	( void ) pParam;
	
	TBTSubSystem *m_pBTSubSystem = NULL;

	if(xQueueReceive(xQueueBLTiProc, &m_pBTSubSystem, portMAX_DELAY) == pdPASS) {
		vQueueDelete(xQueueBLTiProc);
		prvFunc_Print("%cSubSystemFromInit...\t\t    Received", 0x3e);
	}

	while(1) {
		i++;

		BTSubSystemProcess(m_pBTSubSystem);

		// prvFunc_Print("BTPROC");

		taskYIELD();
	}
}
