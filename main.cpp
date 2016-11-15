#include "mbed.h"
#include "ble/BLE.h"
#include "ble/services/HeartRateService.h"
#include "ble/services/BatteryService.h"
#include "ble/services/DeviceInformationService.h"
#include "rtos.h"
#include <string.h>
#include "SDFileSystem.h"
#define N 100//addressを保存する個数
#define M 12//addressとtimeを入れる
#define GET_TIME 10000
#define SEND_TIME 20000
#define AA 0xAA
#define BB 0xBB
#define CC 0xCC
#define DD 0xDD
 
DigitalOut myled3(LED3); 

Semaphore one_slot(1);
 
Timer timer;
BLE  ble;
char address[N][M]={};//保存する配列を初期化
const GapScanningParams scanningParams;
int wt;//threadの時間
int wt1=0;//central mode の時間
int ran;
 
SDFileSystem sd(p25, p28, p29, p21, "sd"); // the pinout on the mbed Cool Components workshop board
FILE *fp;
int counter=0,i,b,flag;
 
void onScanCallback(const Gap::AdvertisementCallbackParams_t *params){
    time_t seconds = time(NULL); // JST
     struct tm *t = localtime(&seconds);
     
     /*printf("%04d/%02d/%02d %02d:%02d:%02d\r\n",
      t->tm_year + 1900, t->tm_mon, t->tm_mday, t->tm_hour, t->tm_min, t->tm_sec);
    */
    flag=0;
 
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
        struct tm *t = localtime(&seconds);
        address[counter][6]= t->tm_year - 70;
        address[counter][7]= t->tm_mon;
        address[counter][8]= t->tm_mday - 1;
        address[counter][9]= t->tm_hour;
        address[counter][10]= t->tm_min;
        address[counter][11]= t->tm_sec;
    
        for(b=0; b<6 ; b++){
            fprintf(fp,"%02x", address[counter][b]);
            printf("%02x", address[counter][b]);
            if(b==5)printf(" ");
        }    
        
        fprintf(fp,",%04d",address[counter][b]);
        b++;
        
        for(; b<12; b++){
            fprintf(fp,"%02d",address[counter][b]);
            printf("%02d",address[counter][b]);
            if(b==11){
                fprintf(fp,"\n");
                printf("\r\n");
            }
        }  
        //fprintf(fp,",%04d,%02d,%02d,%02d,%02d,%02d\n",address[counter][6],address[counter][7],address[counter][8],address[counter][9],address[counter][10],address[counter][11]);
        //printf(",%04d/%02d/%02d %02d:%02d:%02d\r\n",address[counter][6],address[counter][7],address[counter][8],address[counter][9],address[counter][10],address[counter][11]);
        
        counter++;
 
/*      
        printf("DEV:");
        for(b=0; b<6 ; b++)printf("%02x ", address[counter][b]);   
        printf("%04d/%02d/%02d %02d:%02d:%02d \r\n",address[counter][6],address[counter][7],address[counter][8],address[counter][9],address[counter][10],address[counter][11]);
*/
 
/*配列の中身をすべて表示     
        for(a=0; a<counter; a++){
             for(b=0; b<6 ; b++){
                if(b==0)printf("DEV:");
                printf("%02x ", address[a][b]);
                if(b==5){
                printf("%04d/%02d/%02d %02d:%02d:%02d \r\n",address[a][6],address[a][7],address[a][8],address[a][9],address[a][10],address[a][11]);
                }
             }
             if(a==counter-1)printf("----------\n\r");
        }         
*/
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
            printf("**get**\n\r");
            myled3 = 0;
            wt=0;//central modeの際はthreadの時間は0sec
            wt1 = GET_TIME ;//centralmode の時間はこっち
            fp = fopen("/sd/test.csv", "a");
            
            printf("%d秒間の受信\n\r",wt1/1000);
            timer.start();
            while(1){ 
                ble.waitForEvent();
                if(timer.read() > wt1/1000){
                        timer.stop();
                        timer.reset();
                        ble.stopScan();
                        fclose(fp);
                        break;
                }
            }
        
        }
        
//3        
        if(!strcmp((const char*)name, "3")){
            printf("**send**\n\r"); 
            myled3 = 1;
            ran = rand() % 10;
            wt=SEND_TIME - ran*1000;
            printf("%d秒間の送信\n\r",wt/1000);
            ble.gap().startAdvertising();//BLEの送信
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
    const uint8_t address1[] = {AA,AA,AA,AA,AA,AA};
    ble.gap().setAddress(BLEProtocol::AddressType::PUBLIC, address1);
//  ble.gap().accumulateAdvertisingPayload(GapAdvertisingData::COMPLETE_LOCAL_NAME, (uint8_t *)DEVICE_NAME, sizeof(DEVICE_NAME));
//   ble.gap().setAdvertisingType(GapAdvertisingParams::ADV_CONNECTABLE_UNDIRECTED);
//   ble.gap().setAdvertisingInterval(1000); /* 1000ms. */
}
 
int main (void) {
    ble.init(bleInitComplete);
    ble.setScanParams(GapScanningParams::SCAN_INTERVAL_MAX,GapScanningParams::SCAN_WINDOW_MAX,0);
 
    Thread t2(test_thread, (void *)"2");
    Thread t3(test_thread, (void *)"3");
    
    test_thread((void *)"1");
}