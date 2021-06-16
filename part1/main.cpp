#include "mbed.h"
#include "mbed_rpc.h"
#include "bbcar.h"
#include "bbcar_rpc.h"

#define car_length 12.8
#define car_width 12.2

Thread t;
Ticker servo_ticker;
PwmOut pin5(D5), pin6(D6);
static BufferedSerial xbee(D1, D0);
static BufferedSerial pc(STDIO_UART_TX, STDIO_UART_RX);

BBCar car(pin5, pin6, servo_ticker);

void parking(Arguments *in, Reply *out);
RPCFunction Parking(&parking, "parking");

void reply_message(char *xbee_reply, char *message);
void check_addr(char *xbee_reply, char *message);

int main()
{
    pc.set_baud(9600);

    char buf[256], outbuf[256];
    FILE *devin = fdopen(&xbee, "r");
    FILE *devout = fdopen(&xbee, "w");

    while (1)
    {
        memset(buf, 0, 256);
        for (int i = 0;; i++)
        {
            char recv = fgetc(devin);
            if (recv == '\n')
            {
                printf("\r\n");
                break;
            }
            buf[i] = fputc(recv, devout);
        }
        RPC::call(buf, outbuf);
    }
}

void parking(Arguments *in, Reply *out)
{
    double d1 = in->getArg<double>();
    double d2 = in->getArg<double>();
    const char *direction = in->getArg<const char *>();

    // set CalibTable
    double pwm_table0[] = {-150, -120, -90, -60, -30, 0, 30, 60, 90, 120, 150};
    double speed_table0[] = {-9.646, -9.784, -9.025, -8.445, -4.882, 0.000, 5.777, 10.364, 9.885, 9.895, 9.965};
    double pwm_table1[] = {-150, -120, -90, -60, -30, 0, 30, 60, 90, 120, 150};
    double speed_table1[] = {-8.530, -8.132, -8.690, -8.929, -4.824, 0.000, 4.829, 8.132, 8.371, 9.849, 9.769};

    car.setCalibTable(11, pwm_table0, speed_table0, 11, pwm_table1, speed_table1);

    car.goStraightCalib(-4.8);
    ThisThread::sleep_for((d2 / 3) * 1000);
    car.stop();

    ThisThread::sleep_for(500ms);

    if (direction[0] == 'w')
        car.turn(-100, -0.2);
    else if (direction[0] == 'e')
        car.turn(-100, 0.2);
    ThisThread::sleep_for(1400);
    car.stop();

    ThisThread::sleep_for(500);
    car.goStraightCalib(-5);
    ThisThread::sleep_for((d1 / 5) * 500);
    car.stop();
    ThisThread::sleep_for(500ms);
}

void reply_message(char *xbee_reply, char *message)
{
    xbee.read(&xbee_reply[0], 1);
    xbee.read(&xbee_reply[1], 1);
    xbee.read(&xbee_reply[2], 1);
    if (xbee_reply[1] == 'O' && xbee_reply[2] == 'K')
    {
        printf("%s\r\n", message);
        xbee_reply[0] = '\0';
        xbee_reply[1] = '\0';
        xbee_reply[2] = '\0';
    }
}

void check_addr(char *xbee_reply, char message)
{
    xbee.read(&xbee_reply[0], 1);
    xbee.read(&xbee_reply[1], 1);
    xbee.read(&xbee_reply[2], 1);
    xbee.read(&xbee_reply[3], 1);
    printf("%s = %c%c%c\r\n", message, xbee_reply[1], xbee_reply[2], xbee_reply[3]);
    xbee_reply[0] = '\0';
    xbee_reply[1] = '\0';
    xbee_reply[2] = '\0';
    xbee_reply[3] = '\0';
}