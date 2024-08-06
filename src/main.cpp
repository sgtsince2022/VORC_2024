#include <Arduino.h>
#include <Adafruit_PWMServoDriver.h>
#include <PS2X_lib.h>
#include <SPI.h>
#include "Adafruit_TCS34725.h"

Adafruit_PWMServoDriver pwm = Adafruit_PWMServoDriver();
#define PS2_DAT 12 // MISO 
#define PS2_CMD 13 // MOSI 
#define PS2_SEL 15 // SS 
#define PS2_CLK 14 // SLK

PS2X ps2x; 
Adafruit_TCS34725 tcs = Adafruit_TCS34725(TCS34725_INTEGRATIONTIME_50MS, TCS34725_GAIN_4X);

int a = 0, check = 0; //Biến để kiểm tra
const int limit = 8, congtac1 = 25, congtac2 = 32, congtac3 = 36, congtac4 = 39; //Gioi hạn tốc độ, khai báo chân của công tắc hành trình


void dichuyen(){ //chân 8,9 -> động cơ trái ; chân 10,11 -> động cơ phải
// analog đến 255 .-. meow im so cute i want to fall in love
  int moveY = ps2x.Analog(PSS_LY)*limit, moveX = ps2x.Analog(PSS_RX)*limit;
  if (moveY >= 0){ //tận dụng pwm ko nhận giá trị âm :)))
      pwm.setPWM(8, 0, moveY + moveX); 
      pwm.setPWM(9, 0, -(moveY + moveX));    
      pwm.setPWM(10, 0, moveY - moveX); 
      pwm.setPWM(11, 0,-(moveY - moveX));    
      if (moveY == 0){ //phanh 
        pwm.setPWM(9, 0, 1200);
        pwm.setPWM(11, 0,1200);
        delay(50);
        pwm.setPWM(9, 0, 0);
        pwm.setPWM(11, 0,0);
      }
  }
  else{
    pwm.setPWM(8, 0, -(moveY - moveX)); 
    pwm.setPWM(9, 0, moveY - moveX);    
    pwm.setPWM(10, 0, -(moveY + moveX)); 
    pwm.setPWM(11, 0, moveY + moveX);    
  }
}

void cuonbong(){ 
  if (ps2x.ButtonPressed(PSB_START)){
    if (a == 0){ //mở cửa
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
    pwm.setPWM(12, 0, 4095); 
    pwm.setPWM(13, 0, 0);    
    }
  else{
    pwm.setPWM(12, 0, 0); 
    pwm.setPWM(13, 0, 0);    
    pwm.setPWM(6,0,200); //xả hết bóng thừa ra ngoài
    delay(200);
    pwm.setPWM(6,0,500); 
    }
  }

void sortbong(){ //servo6: 300 -> chéo xuống để đưa bóng, 500 -> chéo lên, 200 -> xả bóng, servo5: 375 -> mở ra, 150 -> đóng
  uint16_t r, g, b, c;
  tcs.getRawData(&r, &g, &b, &c);
  const int threshold = 30, clear_threshold = 50, threshold1 = 200, tolerance = 50; // tolerance để cho phép màu trắng vẫn ở ngưỡng ok

  // Kiểm tra bóng có phải màu đen không
  if (r < threshold && g < threshold && b < threshold && c < clear_threshold && a == 1 && check == 0) {
    //cho bóng vào hộp trên, đóng cửa ở phía trên
    pwm.setPWM(6,0,300);
    pwm.setPWM(5,0,375);
    pwm.setPWM(7,0,150);
    check == 1;
    delay(200);
    pwm.setPWM(7,0,375);
    delay(50);
    pwm.setPWM(6,0,500);
    delay(200);
    pwm.setPWM(7,0,600); //mở lại cửa để bóng vào trong
    delay(200);
    pwm.setPWM(7,0,375);
    check == 0;
    //code trên để tránh tình trạng 2 bóng vào cùng 1 lúc, nma sẽ gây sai số nếu cái cửa chạm vào bóng nên là đang xem xét chỉ để cho servo sort bóng hướng lên trên hay có thêm thao tác đóng cửa
}
// Kiểm tra bóng có phải màu trắng không
  else if (r > threshold1 && g > threshold1 && b > threshold1 && abs(r - g) < tolerance && abs(r - b) < tolerance && abs(g - b) < tolerance && a == 1 && check == 0)  {
    //cho bóng vào hộp dưới
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

void nanghathung(){ //Dùng PAD UP với PAD DOWN
  if ((ps2x.ButtonPressed(PSB_PAD_UP) && digitalRead(congtac1) == HIGH) || (ps2x.ButtonPressed(PSB_PAD_UP) && digitalRead(congtac2) == HIGH)){
    pwm.setPWM(14, 0, 4095);
    pwm.setPWM(15, 0, 0);
    if (digitalRead(congtac2) == HIGH || digitalRead(congtac3) == HIGH){
      pwm.setPWM(14, 0, 0);
      pwm.setPWM(15, 0, 0);
    }
  }
  else if ((ps2x.ButtonPressed(PSB_PAD_DOWN) && digitalRead(congtac2) == HIGH) || (ps2x.ButtonPressed(PSB_PAD_DOWN) && digitalRead(congtac3) == HIGH)){
    pwm.setPWM(14, 0, 0);
    pwm.setPWM(15, 0, 4095);
    if (digitalRead(congtac2) == HIGH || digitalRead(congtac1) == HIGH){
      pwm.setPWM(14, 0, 0);
      pwm.setPWM(15, 0, 0);
    }
  }
}

void outtake(){
  int status = 0;
  if (ps2x.ButtonPressed(PSB_GREEN) && status == 0){
    pwm.setPWM(4, 0, 150);
    delay(200);
    pwm.setPWM(4, 0, 375);
    status = 1;
  }
  else if (ps2x.ButtonPressed(PSB_GREEN) && status == 2){
    pwm.setPWM(4, 0, 150);
    if (digitalRead(congtac4) == HIGH){
      pwm.setPWM(4, 0, 375);
      status = 0;
  }
  }
  else if (ps2x.ButtonPressed(PSB_BLUE) && status == 0){
    pwm.setPWM(4, 0, 600);
    delay(200);
    pwm.setPWM(4, 0, 375);
    status = 2;
  }
  else if (ps2x.ButtonPressed(PSB_BLUE) && status == 1){
    pwm.setPWM(4, 0, 600);
    if (digitalRead(congtac4) == HIGH){
      pwm.setPWM(4, 0, 375);
      status = 0;
    }
  }
  }

void ErrorChecking() {
    Serial.println("connecting to ps2..");
    // Kiểm tra PS2 đã kết nối chưa
    int err = -1;
    for(int i = 0; i < 10; i++) {
        err = ps2x.config_gamepad(PS2_CLK, PS2_CMD, PS2_SEL, PS2_DAT, false, false);
        delay(1000);
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
  // Kiểm tra có nhận được dữ liệu từ cảm biến màu không
  if (tcs.begin()) {
    Serial.println("Cam bien mau hoat dong tot");
  } else {
    Serial.println("Chua nhan duoc tin hieu cam bien mau");
    while (1); 
  }
}
void setup() {
  Serial.begin(9600);
  pinMode(congtac1, INPUT);
  pinMode(congtac2, INPUT);
  pinMode(congtac3, INPUT);
  pwm.begin(); 
  pwm.setOscillatorFrequency(27000000);  
  pwm.setPWMFreq(60);
  Wire.setClock(400000); 
  pwm.setPWM(7, 0 ,375);
  pwm.setPWM(6,0,500);
  pwm.setPWM(5, 0 ,150);
  pwm.setPWM(4, 0 , 375);
  ErrorChecking();
}

void loop() { 
  ps2x.read_gamepad(false, false); // gọi hàm để đọc tay điều khiển 
  dichuyen();
  cuonbong();
  sortbong(); 
  nanghathung();
  outtake();
}
//Cảm ơn bác Nguyễn Phú Trọng
