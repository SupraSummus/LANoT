#include <ARMCM4_FP.h>
#include <assert.h>
#include <os4cm4/mem.h>

void mpu_setup (void) {
        assert(MPU->TYPE == (8 << MPU_TYPE_DREGION_Pos));
        ARM_MPU_Enable(
                MPU_CTRL_PRIVDEFENA_Msk  // privileged mode can access non-mapped areas
        );
}

void mpu_map () {

}
