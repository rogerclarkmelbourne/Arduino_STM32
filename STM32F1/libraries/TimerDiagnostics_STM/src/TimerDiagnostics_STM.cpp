/**
 * Based on data from RM0008 Reference manual:
 *  STM32F101xx, STM32F102xx, STM32F103xx, STM32F105xx and STM32F107xx advanced ARM ® -based 32-bit MCUs
 */
#include <TimerDiagnostics_STM.h>

timerdiagnostics_stm::AdvancedTimer timer1Diagnostics(HardwareTimer(1).c_dev()->regs.adv);
timerdiagnostics_stm::GeneralTimer timer2Diagnostics(HardwareTimer(2).c_dev()->regs.gen);
timerdiagnostics_stm::GeneralTimer timer3Diagnostics(HardwareTimer(3).c_dev()->regs.gen);
timerdiagnostics_stm::GeneralTimer timer4Diagnostics(HardwareTimer(4).c_dev()->regs.gen);
#ifdef STM32_HIGH_DENSITY
timerdiagnostics_stm::GeneralTimer timer5Diagnostics(HardwareTimer(5).c_dev()->regs.gen);
timerdiagnostics_stm::BasicTimer timer6Diagnostics(HardwareTimer(6).c_dev()->regs.basic);
timerdiagnostics_stm::BasicTimer timer7Diagnostics(HardwareTimer(7).c_dev()->regs.basic);
timerdiagnostics_stm::AdvancedTimer timer8Diagnostics(HardwareTimer(8).c_dev()->regs.adv);
#endif
