#include "mbed.h"
#include "bbcar.h"
#include "mbed_rpc.h"

#define maxThreshold 85
#define minThreshold 75

Ticker servo_ticker;
PwmOut pin5(D5), pin6(D6);
BufferedSerial pc(USBTX, USBRX); //tx,rx
BufferedSerial uart(D1, D0);     //tx,rx
BBCar car(pin5, pin6, servo_ticker);

void line_detect(Arguments *in, Reply *out);
RPCFunction Line(&line_detect, "line");

// CalibTable
double pwm_table0[] = {-150, -120, -90, -60, -30, 0, 30, 60, 90, 120, 150};
double speed_table0[] = {-9.646, -9.784, -9.025, -8.445, -4.882, 0.000, 5.777, 10.364, 9.885, 9.895, 9.965};
double pwm_table1[] = {-150, -120, -90, -60, -30, 0, 30, 60, 90, 120, 150};
double speed_table1[] = {-8.530, -8.132, -8.690, -8.929, -4.824, 0.000, 4.829, 8.132, 8.371, 9.849, 9.769};

int main()
{
    // set Calib Table
    car.setCalibTable(11, pwm_table0, speed_table0, 11, pwm_table1, speed_table1);

    char buf[256], outbuf[256];
    FILE *devin = fdopen(&uart, "r");
    FILE *devout = fdopen(&uart, "w");

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
        //Call the static call method on the RPC class
        RPC::call(buf, outbuf);
        printf("%s\r\n", outbuf);
    }
}

void line_detect(Arguments *in, Reply *out)
{
    double x1 = in->getArg<double>();
    double y1 = in->getArg<double>();
    double x2 = in->getArg<double>();
    double y2 = in->getArg<double>();

    // turn left
    if (x2 > maxThreshold)
    {
        car.turn(80, 0.5);
        ThisThread::sleep_for(100ms);
        car.stop();
    }
    // turn right
    else if (x2 < minThreshold)
    {
        car.turn(80, -0.5);
        ThisThread::sleep_for(100ms);
        car.stop();
    }
    // go straight
    else
    {
        car.goStraight(150);
        ThisThread::sleep_for(100ms);
        car.stop();
    }
}