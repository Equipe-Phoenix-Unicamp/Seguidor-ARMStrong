#include "mbed.h"
#include "MMA8451Q.h"
#include "test_env.h"

#ifdef TARGET_KL05Z
#define SDA     PTB4
#define SCL     PTB3
#else
#define SDA     PTE25
#define SCL     PTE24
#endif

namespace {
    const int MMA8451_I2C_ADDRESS = 0x1D << 1;          // I2C bus address
    const float MMA8451_DIGITAL_SENSITIVITY = 4096.0;   // Counts/g
}

float calc_3d_vector_len(float x, float y, float z) {
    return sqrt(x*x + y*y + z*z);
}

#define TEST_ITERATIONS                 25
#define TEST_ITERATIONS_SKIP            5
#define MEASURE_DEVIATION_TOLERANCE     0.025   // 2.5%

int main(void) {
    DigitalOut led(LED_GREEN);
    MMA8451Q acc(SDA, SCL, MMA8451_I2C_ADDRESS);
    bool result = true;
    printf("WHO AM I: 0x%2X\r\n\n", acc.getWhoAmI());

    for (int i = 0; i < TEST_ITERATIONS; i++) {
        if (i < TEST_ITERATIONS_SKIP) {
            // Skip first 5 measurements
            continue;
        }
        const float g_vect_len = calc_3d_vector_len(acc.getAccX(), acc.getAccY(), acc.getAccZ()) / MMA8451_DIGITAL_SENSITIVITY;
        const float deviation = fabs(g_vect_len - 1.0);
        const char *succes_str = deviation <= MEASURE_DEVIATION_TOLERANCE ? "[OK]" : "[FAIL]";
        result = result && (deviation <= MEASURE_DEVIATION_TOLERANCE);
        printf("X:% 6d Y:% 6d Z:% 5d GF:%0.3fg, dev:%0.3f ... %s\r\n", acc.getAccX(), acc.getAccY(), acc.getAccZ(), g_vect_len, deviation, succes_str);
        wait(0.5);
        led = !led;
    }
    notify_completion(result);
}
