#include "mbed.h"
#include "bbcar.h"
#include "mbed_rpc.h"
#include <math.h>

Ticker servo_ticker;
PwmOut pin5(D5), pin6(D6);
BufferedSerial pc(USBTX, USBRX);
BufferedSerial uart(D1, D0);

void RPC_tag(Arguments *in, Reply *out);
RPCFunction rpcTag(&RPC_tag, "tag");
DigitalInOut ping1(D10);
Timer dur;

BBCar car(pin5, pin6, servo_ticker);
double pwm_table0[] = {-150, -120, -90, -60, -30, 0, 30, 60, 90, 120, 150};
double speed_table0[] = {-9.646, -9.784, -9.025, -8.445, -4.882, 0.000, 5.777, 10.364, 9.885, 9.895, 9.965};
double pwm_table1[] = {-150, -120, -90, -60, -30, 0, 30, 60, 90, 120, 150};
double speed_table1[] = {-8.530, -8.132, -8.690, -8.929, -4.824, 0.000, 4.829, 8.132, 8.371, 9.849, 9.769};
int main()
{
   car.setCalibTable(11, pwm_table0, speed_table0, 11, pwm_table1, speed_table1);

   char buf[256], outbuf[256];
   FILE *devin = fdopen(&uart, "r");
   FILE *devout = fdopen(&uart, "w");
   while (1)
   {
      memset(buf, 0, 256);
      for (int i = 0; i < 256; i++)
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
      printf("%s\r\n", outbuf);
   }
}

void RPC_tag(Arguments *in, Reply *out)
{
   double Dx = in->getArg<double>();
   double Dy = in->getArg<double>();
   double Dz = in->getArg<double>();
   double Rx = in->getArg<double>();
   double Ry = in->getArg<double>();
   double Rz = in->getArg<double>();

   double fix_ang;
   printf("Dz = %f\n", Dz);
   if (Ry > 8 && Ry < 90)
   {
      double D = fabs(Dz) / tan(Ry * 3.14 / 180.0f);
      if (Dx < 0)
      {
         fix_ang = atan(fabs(Dx) / fabs(Dz));
         car.turn(100, -0.01);
         for (int i = 0; i < 1.5 * (90 - Ry + fix_ang); i++)
            ThisThread::sleep_for(10ms);
      }
      else
      {
         fix_ang = atan(fabs(Dx) / fabs(Dz));
         car.turn(100, -0.01);
         for (int i = 0; i < 1.5 * (90 - Ry - fix_ang); i++)
            ThisThread::sleep_for(10ms);
      }
      car.stop();
      car.goStraightCalib(10);
      for (int n = 0; n < 0.5 * D; n++)
         ThisThread::sleep_for(25ms);
      car.stop();
      car.turn(100, 0.01);
      for (int i = 0; i < 90; i++) //90 + 3 degree calibration
         ThisThread::sleep_for(12ms);
      car.stop();
   }
   else if (Ry > 270 && Ry < 352)
   {
      double D = fabs(Dz) / tan((360 - Ry) * 3.14 / 180.0f);
      if (Dx > 0)
      {
         fix_ang = atan(fabs(Dx) / fabs(Dz));
         car.turn(100, 0.01);
         for (int i = 0; i < 1.4 * (90 - (360 - Ry) + fix_ang); i++) //360 - 10 degree calibration
            ThisThread::sleep_for(10ms);
      }
      else
      {
         fix_ang = atan(fabs(Dx) / fabs(Dz));
         car.turn(100, 0.01);
         for (int i = 0; i < 1.4 * (90 - (360 - Ry) - fix_ang); i++) //360 - 10 degree calibration
            ThisThread::sleep_for(10ms);
      }
      car.stop();
      car.goStraightCalib(11);
      for (int n = 0; n < 0.5 * D; n++)
         ThisThread::sleep_for(25ms);
      car.stop();
      car.turn(100, -0.01);
      for (int i = 0; i < 98; i++) //90 + 8 degree calibration
         ThisThread::sleep_for(12ms);
      car.stop();
   }
   else
      car.goStraightCalib(8);
   car.stop();

   float val;

   ping1.output();
   ping1.write(0);
   ThisThread::sleep_for(2ms);
   ping1.write(1);
   ThisThread::sleep_for(5ms);
   ping1.write(0);

   ping1.input();
   while (ping1.read() == 0)
      ;
   dur.start();
   while (ping1.read() == 1)
      ;
   val = dur.read();
   printf("Ping = %lf\r\n", val * 17700.4f);
   dur.stop();
   dur.reset();
   ThisThread::sleep_for(1s);
   return;
}