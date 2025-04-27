#pragma once

#ifdef __cplusplus
extern "C" {
#endif

extern uint32_t SystemCoreClock;

void SystemInit(void);
void SystemCoreClockUpdate(void);

#ifdef __cplusplus
}
#endif
