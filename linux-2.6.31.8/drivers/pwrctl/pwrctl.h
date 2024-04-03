#ifndef USI_POWER_CTL_H
#define USI_POWER_CTL_H

/* Base of Internal Registers Address Map */
//#define BASE_INTERNAL_REGISTER_ADDRESS 0xD0000000
#define BASE_INTERNAL_REGISTER_ADDRESS 0xF1000000

#define REG(x) ((volatile unsigned int*)(BASE_INTERNAL_REGISTER_ADDRESS + x))

/*******************************************/
/* Main Interrupt Controller Registers Map */
/*******************************************/

#define CPU_MAIN_INT_CAUSE_REG                  0x20200
#define CPU_MAIN_IRQ_MASK_REG                   0x20204
#define CPU_MAIN_FIQ_MASK_REG                   0x20208
#define CPU_ENPOINT_MASK_REG                    0x2020C
#define CPU_MAIN_INT_CAUSE_HIGH_REG             0x20210
#define CPU_MAIN_IRQ_MASK_HIGH_REG              0x20214
#define CPU_MAIN_FIQ_MASK_HIGH_REG              0x20218
#define CPU_ENPOINT_MASK_HIGH_REG               0x2021C


#endif /* USI_POWER_CTL_H */
