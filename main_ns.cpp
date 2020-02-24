#include <arm_cmse.h>
#include "mbed.h"
#include "cmsis.h"

extern "C" void CLK_EnableModuleClock_S(uint32_t u32ModuleIndex);

/* Non-Secure main() */
int main(void)
{
    printf("Check if called-saved registers are corrupted across NSC call...\r\n");
    printf("if we see error message or see nothing below, it means called-saved registers have been corrupted across NSC call!!!\r\n");
    /* Flush STDOUT message above */
    fflush(stdout);
    wait_ms(1000);

    const uint32_t r8_b_nsc = 0x12345678;
    uint32_t r8_a_nsc;
    uint32_t spi0_module = SPI0_MODULE;

    __asm volatile (
#if defined(__GNUC__)
        ".syntax  unified                                           \n"
#endif
        "mov    R8, %[R8_B_NSC]                                     \n"
        "mov    R0, %[Rn]                                           \n"
        "bl     CLK_EnableModuleClock_S                             \n"
        "mov    %[R8_A_NSC], R8                                     \n"
        : [R8_A_NSC] "=&r" (r8_a_nsc)                               /* comma-separated list of output operands */
        : [R8_B_NSC] "r" (r8_b_nsc), [Rn] "r" (spi0_module)         /* comma-separated list of input operands */
        : "cc", "r0", "r1", "r2", "r3", "r8", "r12", "lr", "memory" /* comma-separated list of clobbered resources */
    );

    /* Check if called-saved register is corrupted across NSC call.
     * The following printf(...) can also fail when the above occurs. */
    printf("R8(before NSC)=0x%08x, R8(after NSC)=0x%08x\r\n", r8_b_nsc, r8_a_nsc);
    if (r8_b_nsc != r8_a_nsc) {
        printf("ERROR: Callee-saved R8 corrupted in NSC\r\n");
    } else {
        printf("OK: Callee-saved R8 not corrupted in NSC\r\n");
    }

    while (1);
}
