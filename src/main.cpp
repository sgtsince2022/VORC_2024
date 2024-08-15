#include "main.hpp"

Adafruit_PWMServoDriver pwm = Adafruit_PWMServoDriver();
PS2X ps2x; 
Adafruit_TCS34725 colorSensor = Adafruit_TCS34725(TCS34725_INTEGRATIONTIME_50MS, TCS34725_GAIN_4X);

//CÁC CƠ CẤU Ở TRẠNG THÁI BÌNH THƯỜNG
status cocau {0, 0, 0, 0, 0};

//GIÁ TRỊ DI CHUYỂN
            /*fl, fh, b_l, b_h,ll, lh, r_l, r_h, xl, ixh, yl, iyh*/
movement Move {0, 85, 170, 255, 0, 85, 170, 255, 85, 170, 85, 170};
int16_t moveY, moveX;

//Servo 180: Các giá trị PWM 
//  +) 150: 0 độ (lấy 150 làm gốc)
//  +) 375: quay 90 độ
//  +) 600: quay 180 độ
//Servo 360: Các giá trị PWM
//  +) 150: quay ngược chiều kim đồng hồ
//  +) 375: dừng quay
//  +) 600: quay cùng chiều kim đồng hồ

//NHỮNG GIÁ TRỊ TRÊN ĐỀU MANG TÍNH CHẤT THAM KHẢO, CÓ THỂ SAI LỆCH, CẦN PHẢI THỬ BOT THỰC TẾ

//Vào file header để xem các giá trị được sử dụng

void setup() {
  Serial.begin(9600);
  pinMode(congtac1, INPUT);
  pinMode(congtac2, INPUT);
  pinMode(congtac3, INPUT);
  init();
  ErrorChecking();
}

void loop() { 
  ps2x.read_gamepad(false, false); 
  dichuyen();
  cuonbong();
  sortbong(); 
  nanghathung();
  outtake();
}

// SETUP BOT
void init() {
  pwm.begin(); 
  pwm.setOscillatorFrequency(27000000);  
  pwm.setPWMFreq(60);
  Wire.setClock(400000); 

  //SET CÁC SERVO VỀ VỊ TRÍ SẴN SÀNG
  pwm.setPWM(servo1, 0, dongcua); //180
  pwm.setPWM(servo2, 0, hungbong); //180
  pwm.setPWM(servo3, 0, neutral); //360
  pwm.setPWM(servo4, 0, neutral); //360
}

void dichuyen(){ 
  byte LY = ps2x.Analog(PSS_LY), 
       RX = ps2x.Analog(PSS_RX); 

  if (LY >= Move.forward_low && LY <= Move.forward_high){
    moveY = (int16_t) map(LY, Move.forward_low, Move.forward_high, limitPos, lowestSpeed);
  }
  else if (LY >= Move.backward_low && LY <= Move.backward_high){
    moveY = (int16_t) map(LY, Move.backward_low, Move.backward_high, lowestSpeed, limitNeg);
    
  }
  else if (LY >= Move.idle_y_low && LY <= Move.idle_y_high){
    moveY = 0;
  }
  if (RX >= Move.right_low && RX <= Move.right_high){
    moveX = (int16_t) map(RX, Move.right_low, Move.right_high, lowestSpeed, limitPos);
    
  }
  else if (RX >= Move.left_low && RX <= Move.left_high){
    moveX = (int16_t) map(RX, Move.left_low, Move.left_high, limitNeg, lowestSpeed);
  
  }
  else if (RX >= Move.idle_x_low && RX <= Move.idle_x_high){
    moveX = 0;
  }

  //tiến
  if (moveY >= 0){ 
      pwm.setPWM(DC1DUONG, 0, moveY + moveX); 
      pwm.setPWM(DC1AM, 0, -(moveY + moveX));    
      pwm.setPWM(DC2DUONG, 0, moveY - moveX); 
      pwm.setPWM(DC2AM, 0, -(moveY - moveX));    
  }
  //lùi
  else{ 
    pwm.setPWM(DC1DUONG, 0, moveY - moveX); 
    pwm.setPWM(DC1AM, 0, -(moveY - moveX));    
    pwm.setPWM(DC2DUONG, 0, moveY + moveX); 
    pwm.setPWM(DC2AM, 0, -(moveY + moveX));
  } 
}

void cuonbong(){ 

  // battatcuonbong = 1: bật cuốn bóng
  // battatcuonbong = 0: tắt cuốn bóng

  if (ps2x.ButtonPressed(PSB_START)){
    if (cocau.battatcuonbong == 1){
      pwm.setPWM(DC3DUONG, 0, 0); 
      pwm.setPWM(DC3AM, 0, DC3POWER);
      cocau.battatcuonbong = 0;
    }
    else{
      pwm.setPWM(DC3DUONG, 0, 0); 
      pwm.setPWM(DC3AM, 0, 0);  
      cocau.battatcuonbong = 1;
    }
  }
}


void sortbong(){ 

//tieptucsortbong = 1: đang sort bóng (đóng cửa)
//tieptucsortbong = 0: không sort bóng (mở cửa)

//battatsortbong = 0: không sort bóng 
//battatsortbong = 1: đang sort bóng

//R, G, B phải gần tương đương nhau

  uint16_t red, green, blue, clear;
  float r,g,b;
  colorSensor.setInterrupt(false); //bật led
  delay(60); //do tốn 50ms để đọc
  colorSensor.getRawData(&red, &green, &blue, &clear);
  colorSensor.setInterrupt(true); //tắt led

  r = (red/clear)*256;
  g = (green/clear)*256;
  b = (blue/clear)*256;

  if (ps2x.ButtonPressed(PSB_PINK) && cocau.battatsortbong == 1){
    cocau.battatsortbong = 0;
  }
  else if (ps2x.ButtonPressed(PSB_PINK) && cocau.battatsortbong == 0){
    cocau.battatsortbong = 1;
  }


  // Kiểm tra bóng có phải màu đen không
  if (r < thresholdblack && 
      g < thresholdblack && 
      b < thresholdblack && 
      //r g b phải gần tương đương nhau
      abs(r - g) < tolerance && 
      abs(r - b) < tolerance && 
      abs(g - b) < tolerance && 
      cocau.battatsortbong == 1 && 
      cocau.tieptucsortbong == 1) 
  {
    pwm.setPWM(servo2, 0, thungduoi);
    delay(600);
    cocau.tieptucsortbong = 0;
  }

  // Kiểm tra bóng có phải màu trắng không
  else if (r > thresholdwhite &&
           g > thresholdwhite && 
           b > thresholdwhite &&
           //r g b phải gần tương đương nhau
           abs(r - g) < tolerance && 
           abs(r - b) < tolerance && 
           abs(g - b) < tolerance && 
           cocau.battatsortbong == 1 && 
           cocau.tieptucsortbong == 1)  
  {
    pwm.setPWM(servo2, 0, thungtren);
    delay(600);
    cocau.tieptucsortbong = 0;
  }

  //Không có bóng
  else{
    cocau.tieptucsortbong = 0;
  }

//bật sort bóng, cho bóng vào
  if (cocau.battatsortbong == 1 && cocau.tieptucsortbong == 0){
    pwm.setPWM(servo2, 0, hungbong);
    delay(50);
    pwm.setPWM(servo1, 0, mocua);
    delay(300);
    pwm.setPWM(servo1, 0, dongcua);
    cocau.tieptucsortbong = 1;
  }
//tắt sort bóng
  else if (cocau.battatsortbong == 0){
    pwm.setPWM(servo1, 0, dongcua);
    delay(150);
    pwm.setPWM(servo2, 0, hungbong);
    cocau.tieptucsortbong = 0;
  }

}

void nanghathung(){ 

  //Công tắc 1: Nấc dưới cùng
  //Công tắc 2: Đưa bóng đen vào lỗ
  //Công tắc 3: Cao nhất, đưa bóng trắng vào lỗ

  //Khi thùng được nâng, tắt và không cho sort bóng và cuốn bóng 
  if (digitalRead(congtac2) == HIGH || digitalRead(congtac3) == HIGH || ps2x.ButtonPressed(PSB_PAD_UP)){
    cocau.battatcuonbong = 0;
    cocau.battatsortbong = 0;
  }

  //Nâng/hạ
  if ((ps2x.ButtonPressed(PSB_PAD_UP) && digitalRead(congtac1) == HIGH) || 
      (ps2x.ButtonPressed(PSB_PAD_UP) && digitalRead(congtac2) == HIGH))
  {
    pwm.setPWM(DC4DUONG, 0, DC4POWER);
    pwm.setPWM(DC4AM, 0, 0);
    if (digitalRead(congtac2) == HIGH || digitalRead(congtac3) == HIGH){
      pwm.setPWM(DC4DUONG, 0, 0);
      pwm.setPWM(DC4AM, 0, 0);
    }
  }
  else if ((ps2x.ButtonPressed(PSB_PAD_DOWN) && digitalRead(congtac2) == HIGH) || 
           (ps2x.ButtonPressed(PSB_PAD_DOWN) && digitalRead(congtac3) == HIGH))
  {
    pwm.setPWM(DC4DUONG, 0, 0);
    pwm.setPWM(DC4AM, 0, DC4POWER);
    if (digitalRead(congtac2) == HIGH || digitalRead(congtac1) == HIGH){
      pwm.setPWM(DC4DUONG, 0, 0);
      pwm.setPWM(DC4AM, 0, 0);
    }
  }
}

//điều khiển cửa outtake để đưa bóng vào lỗ
void outtake(){ 

  //servo3: cửa trên
  //servo4: cửa dưới

  //outtaketren/outtakeduoi = 1: đang mở cửa, nút điều khiển tạm thời bị vô hiệu hoá
  //outtaketren/outtakeduoi = 0: đang đóng cửa

  //PSB GREEN: mở cửa trên
  //PSB BLUE: mở cửa dưới

   if (ps2x.ButtonPressed(PSB_GREEN) && cocau.outtaketren == 0){
    cocau.outtaketren = 1;
    //mở cửa trên
    pwm.setPWM(servo3, 0, Negative_Spin);
    delay(Servo3and4OpenAndClose);
    pwm.setPWM(servo3, 0, neutral);
    //chờ để bóng ra hết
    delay(OuttakeOpenTime);
    //đóng cửa trên
    pwm.setPWM(servo3, 0, Positive_Spin);
    delay(Servo3and4OpenAndClose);
    pwm.setPWM(servo3, 0, neutral);
    cocau.outtaketren = 0;
  }
   if (ps2x.ButtonPressed(PSB_BLUE) && cocau.outtakeduoi == 0){
    cocau.outtakeduoi = 1;
    //mở cửa dưới
    pwm.setPWM(servo4, 0, Positive_Spin);
    delay(Servo3and4OpenAndClose);
    pwm.setPWM(servo4, 0, neutral);
    //chờ để bóng ra hết
    delay(OuttakeOpenTime);
    //đóng cửa dưới
    pwm.setPWM(servo4, 0, Negative_Spin);
    delay(Servo3and4OpenAndClose);
    pwm.setPWM(servo4, 0, neutral);
    cocau.outtakeduoi = 0;
    }
  }


//KIỂM TRA LỖI
void ErrorChecking() {
    Serial.println("connecting to ps2..");
    // Kiểm tra PS2 đã kết nối chưa
    int err = -1;
    for(int i = 0; i < 10; i++) {
        err = ps2x.config_gamepad(PS2_CLK, PS2_CMD, PS2_SEL, PS2_DAT, pressures, rumble);
        delay(1000);
        Serial.println("attempting..");
        if(!err) {Serial.println("Successfully Connected PS2 Controller!"); 
        break;}
    }
    switch (err)
    {
  case 0:
    Serial.println(" Ket noi tay cam PS2 thanh cong");
    break;
  case 1:
    Serial.println(" LOI: Khong tim thay tay cam, hay kiem tra day ket noi voi tay cam ");
    break;
  case 2:
    Serial.println(" LOI: khong gui duoc lenh");
    break;
  case 3:
    Serial.println(" LOI: Khong vao duoc Pressures mode ");
    break;
    }
  // Kiểm tra có nhận được dữ liệu từ cảm biến màu không
  if (colorSensor.begin()) {
    Serial.println(" Cam bien mau hoat dong tot");
  } else {
    Serial.println(" LOI: Chua nhan duoc tin hieu cam bien mau");
    while (1); 
  }
}
