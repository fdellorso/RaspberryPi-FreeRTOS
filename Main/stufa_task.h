#ifndef _STUFA_TASK_H_
#define _STUFA_TASK_H_

void prvTask_WatchDog(void *pParam);
void prvTask_UspiInit(void *pParam);
void prvTask_TicControl(void *pParam);
void prvTask_TicConsole(void *pParam);
void prvTask_8825Control(void *pParam);

#endif
