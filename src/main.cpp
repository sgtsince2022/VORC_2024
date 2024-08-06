#include <Arduino.h>
#include <Adafruit_PWMServoDriver.h>
#include<PS2X_lib.h>
#include <SPI.h>
#include "Adafruit_TCS34725.h"
// put function declarations here:
Adafruit_PWMServoDriver pwm = Adafruit_PWMServoDriver();
#define PS2_DAT 12 // MISO 
#define PS2_CMD 13 // MOSI 
#define PS2_SEL 15 // SS 
#define PS2_CLK 14 // SLK

//Khởi tạo class của thư viện
PS2X ps2x; // khởi tạo class PS2x
Adafruit_TCS34725 tcs = Adafruit_TCS34725(TCS34725_INTEGRATIONTIME_50MS, TCS34725_GAIN_4X);
int a = 0, check = 0;
const int limit = 8, congtac1 = 25, congtac2 = 32, congtac3 = 36, congtac4 = 39;
void setup() {
  Serial.begin(115200);
  pinMode(congtac1, INPUT);
  pinMode(congtac2, INPUT);
  pinMode(congtac3, INPUT);
  // put your setup code here, to run once:
  pwm.begin(); //khởi tạo PCA9685 
  pwm.setOscillatorFrequency(27000000); // cài đặt tần số dao động 
  pwm.setPWMFreq(60);// cài đặt tần số PWM. Tần số PWM có thể được cài đặt trong khoảng 24-1600 HZ, tần số này được cài đặt tùy thuộc vào nhu cầu xử dụng. Để điều khiển được cả servo và động cơ DC cùng nhau, tần số PWM điều khiển được cài đặt trong khoảng 50-60Hz.
  Wire.setClock(400000); // cài đặt tốc độ giao tiếp i2c ở tốc độ cao nhất(400 Mhz). Hàm này có thể bỏ qua nếu gặp lỗi hoặc không có nhu cầu tử dụng I2c tốc độ cao
  pwm.setPWM(7, 0 ,375);
  pwm.setPWM(6,0,500);
  pwm.setPWM(5, 0 ,150);
  pwm.setPWM(4, 0 , 375);
int error = -1; 
ps2_init();
if (tcs.begin()) {
    Serial.println("Cam bien mau hoat dong tot");
  } else {
    Serial.println("Chua nhan duoc tin hieu cam bien mau");
    while (1); // Halt the program
  }
}

void loop() { //KHO QUAAAAA
  ps2x.read_gamepad(0, 0); // gọi hàm để đọc tay điều khiển 
  dichuyen();
  cuonbong();
  sortbong(); //NO
  nanghathung();
  

}
void dichuyen(){ //chx xong ti nao
  int moveY = ps2x.Analog(PSS_LY)*limit, moveX = ps2x.Analog(PSS_RX)*limit;
  if (moveY >= 0){
      pwm.setPWM(8, 0, moveY + moveX); //chân số 8 set chiều dương là PWM 50%
      pwm.setPWM(9, 0, -(moveY + moveX));    //chân số 9 set chiều âm 
      pwm.setPWM(10, 0, moveY - moveX); //chân số 8 set chiều dương là PWM 50%
      pwm.setPWM(11, 0,-(moveY - moveX));    //chân số 9 set chiều âm 
      if (moveY == 0){
        pwm.setPWM(9, 0, 1200);
        pwm.setPWM(11, 0,1200);
        delay(50);
        pwm.setPWM(9, 0, 0);
        pwm.setPWM(11, 0,0);
      }
  }
  else{
    pwm.setPWM(8, 0, -(moveY - moveX)); //chân số 8 set chiều dương là PWM 50%
    pwm.setPWM(9, 0, moveY - moveX);    //chân số 9 set chiều âm 
    pwm.setPWM(10, 0, -(moveY + moveX)); //chân số 8 set chiều dương là PWM 50%
    pwm.setPWM(11, 0, moveY + moveX);    //chân số 9 set chiều âm 
  }
}
void cuonbong(){ 
  if (ps2x.ButtonPressed(PSB_START)){
    if (a == 0){
      a = 1;
      pwm.setPWM(7,0,600);
      delay(200);
      pwm.setPWM(7,0,375);
      delay(500);
    }
    else{
      a = 0;
      pwm.setPWM(7,0,150);
      delay(200);
      pwm.setPWM(7,0,375);
      delay(500);
    }
  }
  if (a == 1){
    pwm.setPWM(12, 0, 4095); //chân số 8 set chiều dương là PWM 50%
    pwm.setPWM(13, 0, 0);    //chân số 9 set chiều âm 
    }
  else{
    pwm.setPWM(12, 0, 0); //chân số 8 set chiều dương là PWM 50%
    pwm.setPWM(13, 0, 0);    //chân số 9 set chiều âm 
    pwm.setPWM(6,0,200); // xả bóng còn lại
    delay(200);
    pwm.setPWM(6,0,500); // về vị trí ban đầu
    }
    
  }
void sortbong(){ 
  uint16_t r, g, b, c;
  tcs.getRawData(&r, &g, &b, &c);

  // Define thresholds for black detection
  const int threshold = 30, clear_threshold = 50, threshold1 = 200, tolerance = 50; // Allowable difference between RGB values

  // Check if the color detected is black
  if (r < threshold && g < threshold && b < threshold && c < clear_threshold && a == 1 && check == 0) {
    pwm.setPWM(6,0,300);
    pwm.setPWM(5,0,375);
    pwm.setPWM(7,0,150);
    check == 1;
    delay(200);
    pwm.setPWM(7,0,375);
    delay(50);
    pwm.setPWM(6,0,500);
    delay(200);
    pwm.setPWM(7,0,600);
    delay(200);
    pwm.setPWM(7,0,375);
    check == 0;
}
// Check if the color detected is white
  else if (r > threshold1 && g > threshold1 && b > threshold1 && abs(r - g) < tolerance && abs(r - b) < tolerance && abs(g - b) < tolerance && a == 1 && check == 0)  {
    pwm.setPWM(6,0,300);
    pwm.setPWM(5,0,150);
    pwm.setPWM(7,0,150);
    check == 1;
    delay(200);
    pwm.setPWM(7,0,375);
    delay(50);
    pwm.setPWM(6,0,500);
    delay(200);
    pwm.setPWM(7,0,600);
    delay(200);
    pwm.setPWM(7,0,375);
    check == 0;
  }
}
void nanghathung(){ //2 nút lên hoặc xuống
  if ((ps2x.ButtonPressed(PSB_PAD_UP) and digitalRead(congtac1) == HIGH) || (ps2x.ButtonPressed(PSB_PAD_UP) and digitalRead(congtac2) == HIGH)){
    pwm.setPWM(14, 0, 4095);
    pwm.setPWM(15, 0, 0);
    if (digitalRead(congtac2) == HIGH or digitalRead(congtac3) == HIGH){
      pwm.setPWM(14, 0, 0);
      pwm.setPWM(15, 0, 0);
    }
  }
  else if ((ps2x.ButtonPressed(PSB_PAD_DOWN) and digitalRead(congtac2) == HIGH) || (ps2x.ButtonPressed(PSB_PAD_DOWN) and digitalRead(congtac3) == HIGH)){
    pwm.setPWM(14, 0, 0);
    pwm.setPWM(15, 0, 4095);
    if (digitalRead(congtac2) == HIGH or digitalRead(congtac1) == HIGH){
      pwm.setPWM(14, 0, 0);
      pwm.setPWM(15, 0, 0);
    }
  }
}
void outtake(){
  int status = 0
  if (digitalRead(congtac4) == HIGH){
    if (ps2x.ButtonPressed(PSB_GREEN)){
    }
  }
}
void ps2_init() {
    Serial.println("connecting to ps2..");

    int err = -1;
    for(int i = 0; i < 10; i++) {
        delay(1000);
        err = ps2x.config_gamepad(PS2_CLK, PS2_CMD, PS2_SEL, PS2_DAT, pressures, rumble);
        Serial.println("attempting..");
        if(!err) {Serial.println("Successfully Connected PS2 Controller!"); 
                  Serial.println("===================================="); break;}
    }
    switch (err)
    {
    case 0:
    Serial.println(" Ket noi tay cam PS2 thanh cong");
    break;
  case 1:
    Serial.println(" LOI: Khong tim thay tay cam, hay kiem tra day ket noi vơi tay cam ");
    break;
  case 2:
    Serial.println(" LOI: khong gui duoc lenh");
    break;
  case 3:
    Serial.println(" LOI: Khong vao duoc Pressures mode ");
    break;
    }
}