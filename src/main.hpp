#ifndef _MAIN_HPP_
#define _MAIN_HPP_

#include <Arduino.h>
#include <Adafruit_PWMServoDriver.h>
#include <PS2X_lib.h>
#include <SPI.h>
#include "Adafruit_TCS34725.h"

//TRẠNG THÁI CỦA TỪNG CƠ CẤU
struct status {
    int battatcuonbong, tieptucsortbong, battatsortbong, outtaketren, outtakeduoi;
};

//GIÁ TRỊ DI CHUYỂN
#define limitPos 2500 //giới hạn xấp xỉ 62% năng lượng
#define limitNeg -2500
#define lowestSpeed 150
#define brake 1200

//THỜI GIAN CHỜ
#define Servo3and4OpenAndClose 267
// Tốc độ servo 360: 0,16s / 60* -> xấp xỉ 0,267s cho 100*
#define OuttakeOpenTime 3000

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

//servo3 và servo4: servo 360 độ, còn lại là 180 độ
#define servo1 7
#define servo2 6
#define servo3 5
#define servo4 4

//GIÁ TRỊ MÀU SẮC
#define thresholdblack 50
#define thresholdwhite 240
#define tolerance 10 //cho phép chênh lệch giá trị R G B
//đều là tham khảo, có thể sai

//GIÁ TRỊ R G B TỐI THIỂU VÀ TỐI ĐA (CHỈNH LẠI SAU KHI CALIBRATE)
#define maxR 255
#define maxG 255
#define maxB 255

#define minR 0
#define minG 0
#define minB 0

//DC3 POWER
#define DC3POWER 4095

//DC4 POWER
#define DC4POWER 4095

//Servo3 VÀ Servo4 (360)
#define neutral 375
#define Positive_Spin 600
#define Negative_Spin 150

//Servo1 (180)
#define mocua 250
#define dongcua 150 //vị trí bắt đầu

//Servo2 (180)
#define hungbong 310 //vị trí bắt đầu
#define thungtren 270
#define thungduoi 150

//CÁC HÀM
void init();
void dichuyen();
void cuonbong();
void sortbong();
void nanghathung();
void outtake();
void ErrorChecking();

#endif
