#include "mbed.h"
#include "ble/BLE.h"
#include "ble/services/HeartRateService.h"
#include "ble/services/BatteryService.h"
#include "ble/services/DeviceInformationService.h"
#include "rtos.h"
#include "ADXL345_I2C.h"
#include <string.h>
#include "SDFileSystem.h"
#define N 100//addressを保存する個数
#define M 6//addressとtimeを入れる
#define GET_TIME 10000
#define SEND_TIME 20000
#define AD 0xEE

DigitalOut myled3(LED3); 
Semaphore one_slot(1);
Timer timer;
BLE  ble;
char address[N][M]={};//保存する配列を初期化
const GapScanningParams scanningParams;
int wt;//threadの時間
int wt1=0;//central mode の時間
int ran;
int countfp=0;
char id[] = "/sd/e.csv"; 
char acc[] = "/sd/e_acc.csv";
 
 //加速度センサー
ADXL345_I2C accelerometer(p7, p30);
SDFileSystem sd(p25, p28, p29, p21, "sd"); // the pinout on the mbed Cool Components workshop board
FILE *fp, *kp;
int counter=0,i,b,flag;
int readings[3] = {0, 0, 0}; 
 
void onScanCallback(const Gap::AdvertisementCallbackParams_t *params){
    flag=0;//重複確認のためのフラグ
    for(i=0;i<counter;i++){
        if(address[i][0] == params->peerAddr[0]){
            flag=1;
            break;
        }
    }       
    if(flag==0){
        for(i=0; i<6; i++){
            address[counter][i]=params->peerAddr[i];
         }
        time_t seconds = time(NULL);
        
        char buff[10]={};
        sprintf(buff, "%d", seconds);
        strcat(address[counter],buff);

        for(b=0; b<6 ; b++){
                fprintf(fp,"%02x", address[counter][b]);
                //printf("%02x", address[counter][b]);
        }
        fprintf(fp, ",%d\r\n",seconds);
        //printf("    %d\r\n",seconds);
        counter++; 
      
/*        printf("DEV:");
        for(b=0; b<6 ; b++)printf("%02x ", address[counter][b]);   
        printf("%04d/%02d/%02d %02d:%02d:%02d \r\n",address[counter][6],address[counter][7],address[counter][8],address[counter][9],address[counter][10],address[counter][11]);*/

 
    }  
}      
 
void test_thread(void const *name) {    
    
    while (true) {
        //Timer timer;
        one_slot.wait();
        
        if(!strcmp((const char*)name, "1")){
            //printf("**startAdvertising thread**\n\r");
            wt=1000;
            ble.gap().stopAdvertising();
            ble.startScan(&onScanCallback);
        }
//2        
        if(!strcmp((const char*)name, "2")){
            memset(address, 0, sizeof(address));//配列の初期化
            counter = 0;
            //printf("**get**\n\r");
            myled3 = 0;
            wt=0;//central modeの際はthreadの時間は0sec
            wt1 = GET_TIME ;//centralmode の時間はこっち
            
            if(countfp == 0){
                fp = fopen( id , "w");
                fclose(fp);
                kp = fopen( acc , "w");
                fclose(kp);
                countfp = 1;
            }
            
            fp = fopen( id , "a");
            kp = fopen( acc , "a");
            
            //printf("%d seconds receive\n\r",wt1/1000);
            timer.start();
            
            while(1){ 
                myled3 = !myled3;       
                time_t seconds = time(NULL);
                accelerometer.getOutput(readings);
                //13-bit, sign extended values.
            //    printf("%f, %f, %f",(int16_t)readings[0]*(9.8/256), (int16_t)readings[1]*(9.8/256), (int16_t)readings[2]*(9.8/256));
            //    printf(":    %d\r\n",seconds);
                fprintf(kp,"%f,%f,%f",(int16_t)readings[0]*(9.8/256), (int16_t)readings[1]*(9.8/256), (int16_t)readings[2]*(9.8/256));
                fprintf(kp, ",%d\n",seconds);
                    
                ble.waitForEvent();
                if(timer.read() > wt1/1000){
                        timer.stop();
                        timer.reset();
                        ble.stopScan();
                        fclose(fp);
                        fclose(kp);
                        
                        break;
                }
            }
        
        }
        
//3        
        if(!strcmp((const char*)name, "3")){
            //printf("**send**\n\r"); 
            ran = rand() % 10;
            wt= 0;
            wt1 = SEND_TIME - ran*1000;
            //printf("%d seconds sendin\n\r",wt1/1000);
            // ble.setAdvertisingInterval(1600); /* 1000ms; in multiples of 0.625ms. */
            ble.gap().startAdvertising();//BLEの送信

            timer.start();
            while(1){ 
                myled3 = !myled3;       
                time_t seconds = time(NULL);
                accelerometer.getOutput(readings);
                //13-bit, sign extended values.
                printf("%f, %f, %f",(int16_t)readings[0]*(9.8/256), (int16_t)readings[1]*(9.8/256), (int16_t)readings[2]*(9.8/256));
                printf(":    %d\r\n",seconds);
                fprintf(kp,"%f,%f,%f",(int16_t)readings[0]*(9.8/256), (int16_t)readings[1]*(9.8/256), (int16_t)readings[2]*(9.8/256));
                fprintf(kp, ",%d\n",seconds);
        
                
                if(timer.read() > wt1/1000){
                        timer.stop();
                        timer.reset();
                        fclose(kp);
                        break;
                }
            }

        }      
        Thread::wait(wt);
        one_slot.release();
    }        
}
 
// const static char     DEVICE_NAME[]        = "BLE1";
    
void bleInitComplete(BLE::InitializationCompleteCallbackContext *params)
{
    BLE &ble= params->ble;
    ble_error_t error = params->error;
 
    if (error != BLE_ERROR_NONE) {
        return;
    }
    const uint8_t address1[] = {AD,AD,AD,AD,AD,AD};
    ble.gap().setAddress(BLEProtocol::AddressType::PUBLIC, address1);
/*  ble.gap().accumulateAdvertisingPayload(GapAdvertisingData::COMPLETE_LOCAL_NAME, (uint8_t *)DEVICE_NAME, sizeof(DEVICE_NAME));
  ble.gap().setAdvertisingType(GapAdvertisingParams::ADV_CONNECTABLE_UNDIRECTED);
  ble.gap().setAdvertisingInterval(1000);*/
}
 
 
int main (void) {
//------------------------------------------------------
//加速度センサーの初期化

    //Go into standby mode to configure the device.
    accelerometer.setPowerControl(0x00);
    //Full resolution, +/-16g, 4mg/LSB.
    accelerometer.setDataFormatControl(0x0B);
    //3.2kHz data rate.
    accelerometer.setDataRate(ADXL345_3200HZ);
    //Measurement mode.
    accelerometer.setPowerControl(0x08);
//-------------------------------------------------------
    
    ble.init(bleInitComplete);
    ble.setScanParams(GapScanningParams::SCAN_INTERVAL_MAX,GapScanningParams::SCAN_WINDOW_MAX,0);
 
    Thread t2(test_thread, (void *)"2");
    Thread t3(test_thread, (void *)"3");
    
    test_thread((void *)"1");
}