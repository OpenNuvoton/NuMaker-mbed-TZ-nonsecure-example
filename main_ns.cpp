#include <arm_cmse.h>
#include "mbed.h"

/* Non-Secure main() */
int main(void)
{
    printf("\r\n");
    printf("+---------------------------------------------+\r\n");
    printf("|    Non-secure code is running ...           |\r\n");
    printf("+---------------------------------------------+\r\n");

    DigitalOut led(LED1);

    unsigned count = 0;
    while (1) {
        ThisThread::sleep_for(500);

        led = ! led;
        printf("Non-secure main thread: %d \r\n", count ++);
    }
}
