#ifndef _MAIN_HPP_
#define _MAIN_HPP_

#include <Arduino.h>
#include <Adafruit_PWMServoDriver.h>
#include <PS2X_lib.h>
#include <SPI.h>
#include "Adafruit_TCS34725.h"

//TRẠNG THÁI CỦA TỪNG CƠ CẤU
struct status {
    int battatcuonbong, tieptucsortbong, outtakestatus;
};

//PS2 PINS
#define PS2_DAT 12 // MISO 
#define PS2_CMD 13 // MOSI 
#define PS2_SEL 15 // SS 
#define PS2_CLK 14 // SLK

//GIỚI HẠN TỐC ĐỘ
#define limit 8 //Analog tối đa là 255 -> 255 * 8 = 2040 (gần bằng 50% pwm)

//4 CÔNG TẮC HÀNH TRÌNH
#define congtac1 25
#define congtac2 32
#define congtac3 36
#define congtac4 39

//PRESSURE VÀ RUMBLE CỦA PS2 
#define pressures false
#define rumble false //Tránh hết pin nhanh

//ĐỘNG CƠ VÀ SERVO
//DƯƠNG: quay cùng chiều kim đồng hồ, ÂM: quay ngược chiều kim đồng hồ
#define DC1DUONG 8
#define DC1AM 9
#define DC2DUONG 10
#define DC2AM 11
#define DC3DUONG 12
#define DC3AM 13
#define DC4DUONG 14
#define DC4AM 15

//servo1 và servo4: servo 360 độ, còn lại là 180 độ
#define servo1 7
#define servo2 6
#define servo3 5
#define servo4 4

//GIÁ TRỊ MÀU SẮC
#define thresholdblack 30
#define clear_threshold 50
#define thresholdwhite 200
#define tolerance 50 //cho phép sai số của màu trắng

//CÁC HÀM
void init();
void dichuyen();
void cuonbong();
void sortbong();
void nanghathung();
void outtake();
void ErrorChecking();

#endif