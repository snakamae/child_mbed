/**
 * @author Jose R. Padron
 * 
 * @section LICENSE
 *
 * Copyright (c) 2010 ARM Limited
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 * THE SOFTWARE.
 *
 * @section DESCRIPTION
 *
 * ADXL345, triple axis, digital interface, accelerometer.
 *
 * Datasheet:
 *
 * http://www.analog.com/static/imported-files/data_sheets/ADXL345.pdf
 */  
 
/**
 * Includes
 */
#include "ADXL345_I2C.h"

ADXL345_I2C::ADXL345_I2C(PinName sda, 
                 PinName scl) : i2c_(sda,scl) {

     
    //100KHz, as specified by the datasheet.
    i2c_.frequency(100000);
    wait_us(500);

}

char ADXL345_I2C::getDevId(void) {

    return oneByteRead(ADXL345_DEVID_REG);

}

int ADXL345_I2C::getTapThreshold(void) {

    return oneByteRead(ADXL345_THRESH_TAP_REG);

}

void ADXL345_I2C::setTapThreshold(int threshold) {

    oneByteWrite(ADXL345_THRESH_TAP_REG, threshold);

}

int ADXL345_I2C::getOffset(int axis) {

    int address = 0;

    if (axis == ADXL345_X) {
        address = ADXL345_OFSX_REG;
    } else if (axis == ADXL345_Y) {
        address = ADXL345_OFSY_REG;
    } else if (axis == ADXL345_Z) {
        address = ADXL345_OFSZ_REG;
    }

    return oneByteRead(address);

}

void ADXL345_I2C::setOffset(int axis, char offset) {

    char address = 0;

    if (axis == ADXL345_X) {
        address = ADXL345_OFSX_REG;
    } else if (axis == ADXL345_Y) {
        address = ADXL345_OFSY_REG;
    } else if (axis == ADXL345_Z) {
        address = ADXL345_OFSZ_REG;
    }

    return oneByteWrite(address, offset);

}

int ADXL345_I2C::getTapDuration(void) {

    return oneByteRead(ADXL345_DUR_REG)*625;

}

void ADXL345_I2C::setTapDuration(int duration_us) {

    int tapDuration = duration_us / 625;

    oneByteWrite(ADXL345_DUR_REG, tapDuration);

}

float ADXL345_I2C::getTapLatency(void) {

    return oneByteRead(ADXL345_LATENT_REG)*1.25;

}

void ADXL345_I2C::setTapLatency(int latency_ms) {

    int tapLatency = latency_ms / 1.25;

    oneByteWrite(ADXL345_LATENT_REG, tapLatency);

}

float ADXL345_I2C::getWindowTime(void) {

    return oneByteRead(ADXL345_WINDOW_REG)*1.25;

}

void ADXL345_I2C::setWindowTime(int window_ms) {

    int windowTime = window_ms / 1.25;

    oneByteWrite(ADXL345_WINDOW_REG, windowTime);

}

int ADXL345_I2C::getActivityThreshold(void) {

    return oneByteRead(ADXL345_THRESH_ACT_REG);

}

void ADXL345_I2C::setActivityThreshold(int threshold) {

    oneByteWrite(ADXL345_THRESH_ACT_REG, threshold);

}

int ADXL345_I2C::getInactivityThreshold(void) {

    return oneByteRead(ADXL345_THRESH_INACT_REG);

}

void ADXL345_I2C::setInactivityThreshold(int threshold) {

    return oneByteWrite(ADXL345_THRESH_INACT_REG, threshold);

}

int ADXL345_I2C::getTimeInactivity(void) {

    return oneByteRead(ADXL345_TIME_INACT_REG);

}

void ADXL345_I2C::setTimeInactivity(int timeInactivity) {

    oneByteWrite(ADXL345_TIME_INACT_REG, timeInactivity);

}

int ADXL345_I2C::getActivityInactivityControl(void) {

    return oneByteRead(ADXL345_ACT_INACT_CTL_REG);

}

void ADXL345_I2C::setActivityInactivityControl(int settings) {

    oneByteWrite(ADXL345_ACT_INACT_CTL_REG, settings);

}

int ADXL345_I2C::getFreefallThreshold(void) {

    return oneByteRead(ADXL345_THRESH_FF_REG);

}

void ADXL345_I2C::setFreefallThreshold(int threshold) {

    oneByteWrite(ADXL345_THRESH_FF_REG, threshold);

}

int ADXL345_I2C::getFreefallTime(void) {

    return oneByteRead(ADXL345_TIME_FF_REG)*5;

}

void ADXL345_I2C::setFreefallTime(int freefallTime_ms) {

    int freefallTime = freefallTime_ms / 5;

    oneByteWrite(ADXL345_TIME_FF_REG, freefallTime);

}

int ADXL345_I2C::getTapAxisControl(void) {

    return oneByteRead(ADXL345_TAP_AXES_REG);

}

void ADXL345_I2C::setTapAxisControl(int settings) {

    oneByteWrite(ADXL345_TAP_AXES_REG, settings);

}

int ADXL345_I2C::getTapSource(void) {

    return oneByteRead(ADXL345_ACT_TAP_STATUS_REG);

}

void ADXL345_I2C::setPowerMode(char mode) {

    //Get the current register contents, so we don't clobber the rate value.
    char registerContents = oneByteRead(ADXL345_BW_RATE_REG);

    registerContents = (mode << 4) | registerContents;

    oneByteWrite(ADXL345_BW_RATE_REG, registerContents);

}

int ADXL345_I2C::getPowerControl(void) {

    return oneByteRead(ADXL345_POWER_CTL_REG);

}

void ADXL345_I2C::setPowerControl(int settings) {

    oneByteWrite(ADXL345_POWER_CTL_REG, settings);

}

int ADXL345_I2C::getInterruptEnableControl(void) {

    return oneByteRead(ADXL345_INT_ENABLE_REG);

}

void ADXL345_I2C::setInterruptEnableControl(int settings) {

    oneByteWrite(ADXL345_INT_ENABLE_REG, settings);

}

int ADXL345_I2C::getInterruptMappingControl(void) {

    return oneByteRead(ADXL345_INT_MAP_REG);

}

void ADXL345_I2C::setInterruptMappingControl(int settings) {

    oneByteWrite(ADXL345_INT_MAP_REG, settings);

}

int ADXL345_I2C::getInterruptSource(void){

    return oneByteRead(ADXL345_INT_SOURCE_REG);

}

int ADXL345_I2C::getDataFormatControl(void){

    return oneByteRead(ADXL345_DATA_FORMAT_REG);

}

void ADXL345_I2C::setDataFormatControl(int settings){

    oneByteWrite(ADXL345_DATA_FORMAT_REG, settings);

}

void ADXL345_I2C::setDataRate(int rate) {

    //Get the current register contents, so we don't clobber the power bit.
    char registerContents = oneByteRead(ADXL345_BW_RATE_REG);

    registerContents &= 0x10;
    registerContents |= rate;

    oneByteWrite(ADXL345_BW_RATE_REG, registerContents);

}

int ADXL345_I2C::getAx(){

    char buffer[2];
    
    TwoByteRead(ADXL345_DATAX0_REG, buffer);
    
    return ((int)buffer[1] << 8 | (int)buffer[0]);
}


int ADXL345_I2C::getAy(){

    char buffer[2];
    
    TwoByteRead(ADXL345_DATAY0_REG, buffer);
    
    return ((int)buffer[1] << 8 | (int)buffer[0]);
}

int ADXL345_I2C::getAz(){

    char buffer[2];
    
    TwoByteRead(ADXL345_DATAZ0_REG, buffer);
    
    return ((int)buffer[1] << 8 | (int)buffer[0]);
}



void ADXL345_I2C::getOutput(int* readings){
    
      char buffer[2];
    
    TwoByteRead(ADXL345_DATAX0_REG, buffer);
    readings[0] = (int)buffer[1] << 8 | (int)buffer[0];
    TwoByteRead(ADXL345_DATAY0_REG, buffer);
    readings[1] = (int)buffer[1] << 8 | (int)buffer[0];
    TwoByteRead(ADXL345_DATAZ0_REG, buffer);
    readings[2] = (int)buffer[1] << 8 | (int)buffer[0];

}

int ADXL345_I2C::getFifoControl(void){

    return oneByteRead(ADXL345_FIFO_CTL);

}

void ADXL345_I2C::setFifoControl(int settings){

    oneByteWrite(ADXL345_FIFO_STATUS, settings);

}

int ADXL345_I2C::getFifoStatus(void){

    return oneByteRead(ADXL345_FIFO_STATUS);

}

char ADXL345_I2C::oneByteRead(char address) {

   
   char  rx[1];
   char tx[1];
 
  // nCS_ = 1;  
   tx[0]=address;
    
       
    i2c_.write(ADXL345_I2C_WRITE, tx,1);
    i2c_.read(ADXL345_I2C_READ,rx,1);
    
  //nCS_ = 0;
    return rx[0];

}

void ADXL345_I2C::oneByteWrite(char address, char data) {
 // nCS_ = 1;
    char tx[2];
   
    tx[0]=address;
    tx[1]=data;

    i2c_.write(ADXL345_I2C_WRITE,tx,2);
   
  //nCS_ = 0;
}

void ADXL345_I2C::TwoByteRead(char startAddress, char* buffer) {


   // nCS_ = 1;
    //Send address to start reading from.
    char tx[1];
    tx[0]=startAddress;
    i2c_.write(ADXL345_I2C_WRITE,tx,1);
    i2c_.read(ADXL345_I2C_READ,buffer,2);
    
  //nCS_ = 0;
}

void ADXL345_I2C::TwoByteWrite(char startAddress, char* buffer) {

//nCS_ = 1;
    //Send address to start reading from.
    char tx[1];
    tx[0]=startAddress;
        i2c_.write(ADXL345_I2C_WRITE,tx,1);
         i2c_.write(ADXL345_I2C_WRITE,buffer,2);
    
    
 //nCS_ = 0;

}
