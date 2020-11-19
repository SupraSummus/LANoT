#include <os4cm4/printk.h>
#include <ARMCM4_FP.h>

void HardFault_Handler (void) {
        printk(
                "in HardFault_Handler, flags:\n%s%s%s",
                (SCB->HFSR & SCB_HFSR_FORCED_Msk) ? "\tHFSR.FORCED\n" : "",
                (SCB->HFSR & SCB_HFSR_DEBUGEVT_Msk) ? "\tHFSR.DEBUGEVT\n" : "",
                (SCB->HFSR & SCB_HFSR_VECTTBL_Msk) ? "\tHFSR.VECTTBL\n" : ""
        );
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
