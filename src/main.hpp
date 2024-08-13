#ifndef _MAIN_HPP_
#define _MAIN_HPP_

#include <Arduino.h>
#include <Adafruit_PWMServoDriver.h>
#include <PS2X_lib.h>
#include <SPI.h>
#include "Adafruit_TCS34725.h"

//TRẠNG THÁI CỦA TỪNG CƠ CẤU
struct status {
    int battatcuonbong, tieptucsortbong, outtakestatus, tienhoaclui, traihoacphai;
};

//GIÁ TRỊ DI CHUYỂN
#define limitPos 2048 //giới hạn 50% năng lượng
#define limitNeg -2048
#define lowestSpeed 0
#define brake 1200

//THỜI GIAN CHỜ
#define TimetoBrake 50 //thời gian 0,05s tránh tối đa tình trạng động cơ bị dừng khi có tín hiệu di chuyển
#define Servo1and4OpenAndClose 200

//GIÁ TRỊ TỐI ĐA DI CHUYỂN
struct movement {
    int forward_low, forward_high, backward_low, backward_high, left_low, left_high, right_low, right_high;
    int idle_x_low, idle_x_high, idle_y_low, idle_y_high; 
    //Có giá trị IDLE để tránh khi nhỡ tay gạt nhẹ cần thì bot vẫn đứng yên
};

//PS2 PINS
#define PS2_DAT 12 // MISO 
#define PS2_CMD 13 // MOSI 
#define PS2_SEL 15 // SS 
#define PS2_CLK 14 // SLK

//4 CÔNG TẮC HÀNH TRÌNH
#define congtac1 25
#define congtac2 32
#define congtac3 39

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

//DC3 POWER
#define DC3POWER 4095

//DC4 POWER
#define DC4POWER 4095

//Servo1 VÀ Servo4 (360)
#define neutral 375
#define Positive_Spin 600
#define Negative_Spin 150

//Servo2 (180)
#define hungbong 420
#define vaothung 300
#define xabong 200

//Servo3 (180)
#define thungtren 150
#define thungduoi 375

//CÁC HÀM
void init();
void dichuyen();
void cuonbong();
void sortbong();
void nanghathung();
void outtake();
void ErrorChecking();

#endif
