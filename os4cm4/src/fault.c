#include <ARMCM4_FP.h>
#include <os4cm4/printk.h>
#include <stdlib.h>

void HardFault_Handler (void) {
        printk("in HardFault_Handler\n");
        printk(
                "HFSR:\n%s%s%s",
                (SCB->HFSR & SCB_HFSR_FORCED_Msk) ? "\tFORCED\n" : "",
                (SCB->HFSR & SCB_HFSR_DEBUGEVT_Msk) ? "\tDEBUGEVT\n" : "",
                (SCB->HFSR & SCB_HFSR_VECTTBL_Msk) ? "\tVECTTBL\n" : ""
        );
        printk(
                "CFSR.UFSR:\n%s%s%s%s%s%s",
                (SCB->CFSR & SCB_CFSR_DIVBYZERO_Msk) ? "\tDIVBYZERO\n" : "",
                (SCB->CFSR & SCB_CFSR_UNALIGNED_Msk) ? "\tUNALIGNED\n" : "",
                (SCB->CFSR & SCB_CFSR_NOCP_Msk) ? "\tNOCP\n" : "",
                (SCB->CFSR & SCB_CFSR_INVPC_Msk) ? "\tINVPC\n" : "",
                (SCB->CFSR & SCB_CFSR_INVSTATE_Msk) ? "\tINVSTATE\n" : "",
                (SCB->CFSR & SCB_CFSR_UNDEFINSTR_Msk) ? "\tUNDEFINSTR\n" : ""
        );
        /*printk(
                "SHCSR.*PENDED:\n%s%s%s%s",
                (SCB->SHCSR & SCB_SHCSR_SVCALLPENDED_Msk) ? "\tSVCALL\n" : "",
                (SCB->SHCSR & SCB_SHCSR_BUSFAULTPENDED_Msk) ? "\tBUSFAULT\n" : "",
                (SCB->SHCSR & SCB_SHCSR_MEMFAULTPENDED_Msk) ? "\tMEMFAULT\n" : "",
                (SCB->SHCSR & SCB_SHCSR_USGFAULTPENDED_Msk) ? "\tUSGFAULT\n" : ""
        );*/
        abort();
}

void MemManage_Handler (void) {
        printk("in MemManage_Handler\n");
}

void BusFault_Handler (void) {
        printk("in BusFault_Handler\n");
}

void UsageFault_Handler (void) {
        printk("in UsageFault_Handler\n");
}
