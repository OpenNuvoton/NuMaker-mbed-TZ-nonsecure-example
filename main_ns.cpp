#include <arm_cmse.h>
#include "mbed.h"

float Get_Temperature_NS(void);

extern "C" {
float Get_Temperature_S(void);
}

/* Non-Secure main() */
int main(void)
{
    printf("\r\n");
    printf("+---------------------------------------------+\r\n");
    printf("|    Non-secure code is running ...           |\r\n");
    printf("+---------------------------------------------+\r\n");

    printf("TERM CONF: %d/8-N-1\r\n", MBED_CONF_PLATFORM_STDIO_BAUD_RATE);

    DigitalOut led(LED1);
    int in_char;
    float (*func_ptr)(void);
    
    printf(" (a). Test secure I2C, press 1 \r\n");
    printf(" (b). Test Non-secure I2C, press 2 \r\n");
    printf(" ===> Please select your choice: ");
    printf(" %c \r\n", (in_char = getchar()));
    if (in_char == '1') {
        func_ptr = Get_Temperature_S;
    } else if (in_char == '2') {
        func_ptr = Get_Temperature_NS;
    } else {
        func_ptr = NULL;
    }
    unsigned count = 0;
    float tmp;
    while (1) {
        ThisThread::sleep_for(500);

        led = ! led;
        printf("Non-secure main thread: %d \r\n", count ++);
        if( func_ptr != NULL ) {
            tmp = func_ptr();
            printf("Func-Return = %d.%d\n", (int)tmp, ((int)(tmp*100))%100);
        }
    }
}

float Get_Temperature_NS(void)
{
    I2C i2c(I2C_SDA, I2C_SCL);
    const int addr8bit = 0x48 << 1; // 8bit I2C address, 0x90
    char cmd[2];
    cmd[0] = 0x01;
    cmd[1] = 0x00;
    i2c.write(addr8bit, cmd, 2);
    ThisThread::sleep_for(500);
    cmd[0] = 0x00;
    i2c.write(addr8bit, cmd, 1);
    i2c.read(addr8bit, cmd, 2);

    float tmp = (float((cmd[0] << 8) | cmd[1]) / 256.0);
    printf("Non-secure: Temp = %d.%d\n", (int)tmp, ((int)(tmp*100))%100);
    return tmp;
}