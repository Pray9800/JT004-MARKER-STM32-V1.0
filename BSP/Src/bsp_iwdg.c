  #include "bsp_iwdg.h"

    #include "stdio.h"


  void IWDG_Refresh()
  {
        HAL_IWDG_Refresh(&hiwdg); //刷新独立看门狗
        // if (__HAL_RCC_GET_FLAG(RCC_FLAG_IWDGRST) != RESET) 
        //     {
        //         printf("\r\n[ALARM] System recovered from IWDG (Watchdog) Reset!\r\n"); 
        //     }
  }
  