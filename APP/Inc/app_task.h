/*******************************************************
 Copyright (C), HangZou Jianjia Co.,Ltd.
 File name:			app_task.h
 Author: PENG		Version: V1.0		Date:2026/05/11
 Description:    	C6板子任务
 Function List:     1.接收UR上位机的灯带指令
                    2.上传按键指令
 History:
*******************************************************/

#ifndef __APP_TASK_H_
#define __APP_TASK_H_

#define blink_fre_100ms   100//led 闪烁频率
#define BRIGHTNESS 100   //亮度

void task_init();
void task();



#endif /* __APP_TASK_H_ */
