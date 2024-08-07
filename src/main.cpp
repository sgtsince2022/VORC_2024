#include "main.hpp"

Adafruit_PWMServoDriver pwm = Adafruit_PWMServoDriver();
PS2X ps2x; 
Adafruit_TCS34725 tcs = Adafruit_TCS34725(TCS34725_INTEGRATIONTIME_50MS, TCS34725_GAIN_4X);

//CÁC CƠ CẤU Ở TRẠNG THÁI BÌNH THƯỜNG
status cocau {0, 0, 0, 0, 0};

//GIÁ TRỊ DI CHUYỂN
            /*fl, fh, b_l, b_h,ll, lh, r_l, r_h, xl, ixh, yl, iyh*/
movement Move {0, 85, 170, 255, 0, 85, 170, 255, 85, 170, 85, 170};
int16_t moveY, moveX;

//Servo 180: Các giá trị PWM
//  +) 150: góc 0 độ
//  +) 375: góc 45 độ
//  +) 600: góc 90 độ
//Servo 360: Các giá trị PWM
//  +) 150: quay ngược chiều kim đồng hồ
//  +) 375: dừng quay
//  +) 600: quay cùng chiều kim đồng hồ

//NHỮNG GIÁ TRỊ TRÊN ĐỀU MANG TÍNH CHẤT THAM KHẢO, CÓ THỂ SAI LỆCH, CẦN PHẢI THỬ BOT THỰC TẾ

void setup() {
  Serial.begin(9600);
  pinMode(congtac1, INPUT);
  pinMode(congtac2, INPUT);
  pinMode(congtac3, INPUT);
  pinMode(congtac4, INPUT);
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
  pwm.setPWM(servo1, 0, neutral); //360
  pwm.setPWM(servo2, 0, hungbong); //180
  pwm.setPWM(servo3, 0, thungtren); //180
  pwm.setPWM(servo4, 0, neutral); //360
}

void dichuyen(){ 
  int LY = ps2x.Analog(PSS_LY), 
      RX = ps2x.Analog(PSS_RX); 

  //tienhoaclui = 0: đứng yên
  //tienhoaclui = 1: tiến
  //tienhoaclui = 2: lùi
  //traihoacphai = 0: không rẽ
  //traihoacphai = 1: rẽ phải
  //traihoacphai = 2: rẽ trái

  if (LY >= Move.forward_low && LY <= Move.forward_high){
    moveY = (int16_t) map(LY, Move.forward_low, Move.forward_high, lowestSpeed, limitPos);
    cocau.tienhoaclui = 1;
  }
  else if (LY >= Move.backward_low && LY <= Move.backward_high){
    moveY = (int16_t) map(LY, Move.backward_low, Move.backward_high, lowestSpeed, limitNeg);
    cocau.tienhoaclui = 2;
  }
  else if (LY >= Move.idle_y_low && LY <= Move.idle_y_high){
    moveY = 0;
  }
  if (RX >= Move.right_low && RX <= Move.right_high){
    moveX = (int16_t) map(RX, Move.right_low, Move.right_high, lowestSpeed, limitPos);
    cocau.traihoacphai = 1;
  }
  else if (RX >= Move.left_low && RX <= Move.left_high){
    moveX = (int16_t) map(RX, Move.left_low, Move.left_high, lowestSpeed, limitNeg);
    cocau.traihoacphai = 1;
  }
  else if (RX >= Move.idle_x_low && RX <= Move.idle_x_high){
    moveX = 0;
  }

  //tiến
  if (moveY > 0){ 
      pwm.setPWM(DC1DUONG, 0, moveY + moveX); 
      pwm.setPWM(DC1AM, 0, -(moveY + moveX));    
      pwm.setPWM(DC2DUONG, 0, moveY - moveX); 
      pwm.setPWM(DC2AM, 0, -(moveY - moveX));    
  }

  //phanh, có thể gây giật bot (không phải giật điện)
  else if (moveY == 0){ 
    if (cocau.tienhoaclui == 1){
      cocau.tienhoaclui = 0;
      pwm.setPWM(DC1AM, 0, brake);
      pwm.setPWM(DC2AM, 0, brake);
      //Đảm bảo chắc chắn động cơ không quay nữa
      pwm.setPWM(DC1DUONG, 0, 0);
      pwm.setPWM(DC2DUONG, 0, 0);
      delay(TimetoBrake); 
      pwm.setPWM(DC1AM, 0, 0);
      pwm.setPWM(DC2AM, 0, 0);
      pwm.setPWM(DC1DUONG, 0, 0);
      pwm.setPWM(DC2DUONG, 0, 0);
    }
    else if (cocau.tienhoaclui == 2){
      cocau.tienhoaclui = 0;
      pwm.setPWM(DC1DUONG, 0, brake);
      pwm.setPWM(DC2DUONG, 0, brake);
      //Đảm bảo chắc chắn động cơ không quay nữa
      pwm.setPWM(DC1AM, 0, 0);
      pwm.setPWM(DC2AM, 0, 0);
      delay(TimetoBrake);
      pwm.setPWM(DC1DUONG, 0, 0);
      pwm.setPWM(DC2DUONG, 0, 0);
      pwm.setPWM(DC1AM, 0, 0);
      pwm.setPWM(DC2AM, 0, 0);
    }
    //Phanh rẽ trái hoặc phải chỉ khi không tiến hoặc lùi, ưu tiên phanh theo tiến hoặc lùi để tránh sai số
    if (cocau.traihoacphai == 1 && cocau.tienhoaclui == 0){
      cocau.traihoacphai = 0;
      pwm.setPWM(DC1AM, 0, brake);
      pwm.setPWM(DC2DUONG, 0, brake);
      //Đảm bảo chắc chắn động cơ không quay nữa
      pwm.setPWM(DC1DUONG, 0, 0);
      pwm.setPWM(DC2AM, 0, 0);
      delay(TimetoBrake);
      pwm.setPWM(DC1AM, 0, 0);
      pwm.setPWM(DC2DUONG, 0, 0);
      pwm.setPWM(DC1DUONG, 0, 0);
      pwm.setPWM(DC2AM, 0, 0);
    }
    if (cocau.traihoacphai == 2 && cocau.tienhoaclui == 0){
      cocau.traihoacphai = 0;
      pwm.setPWM(DC1DUONG, 0, brake);
      pwm.setPWM(DC2AM, 0, brake);
      //Đảm bảo chắc chắn động cơ không quay nữa
      pwm.setPWM(DC1AM, 0, 0);
      pwm.setPWM(DC2DUONG, 0, 0);
      delay(TimetoBrake);
      pwm.setPWM(DC1DUONG, 0, 0);
      pwm.setPWM(DC2AM, 0, 0);
      pwm.setPWM(DC1AM, 0, 0);
      pwm.setPWM(DC2DUONG, 0, 0);
    }
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

  // battatcuonbong = 1: bật cuốn bóng, mở cửa
  // battatcuonbong = 0: tắt cuốn bóng, đóng cửa

  if (ps2x.ButtonPressed(PSB_START)){
    if (cocau.battatcuonbong == 0){ 
      //mở cửa
      cocau.battatcuonbong = 1;
      pwm.setPWM(servo1, 0, Positive_Spin);
      delay(Servo1and4OpenAndClose);
      pwm.setPWM(servo1, 0, neutral);
      delay(500);
    }
    else{
      //đóng cửa
      cocau.battatcuonbong = 0;
      pwm.setPWM(servo1, 0, Negative_Spin);
      delay(Servo1and4OpenAndClose);
      pwm.setPWM(servo1, 0, neutral);
      delay(500);
    }
  }
  if (cocau.battatcuonbong == 1){
    pwm.setPWM(DC3DUONG, 0, DC3POWER); 
    pwm.setPWM(DC3AM, 0, 0);    
    }
  else{
    pwm.setPWM(DC3DUONG, 0, 0); 
    pwm.setPWM(DC3AM, 0, 0);  
    //xả hết bóng thừa ra ngoài  
    pwm.setPWM(servo2, 0, xabong); 
    delay(200);
    pwm.setPWM(servo2, 0, hungbong); 
    }
  }

void sortbong(){ 
  //servo2 (180 độ): pwm như sau:
  //  300: chéo xuống để đưa bóng vào thùng
  //  500: chéo lên để hứng bóng
  //  200: xả bóng
  //servo3 (180 độ): pwm như sau:
  //  375: mở ra
  //  150: đóng

//NHỮNG GIÁ TRỊ TRÊN ĐỀU MANG TÍNH CHẤT THAM KHẢO, CÓ THỂ SAI LỆCH, CẦN PHẢI THỬ BOT THỰC TẾ

//tieptucsortbong = 0: được phép sort bóng tiếp theo
//tieptucsortbong = 1: không được phép sort bóng tiếp theo

  uint16_t r, g, b, c;
  tcs.getRawData(&r, &g, &b, &c);

  // Kiểm tra bóng có phải màu đen không
  if (r < thresholdblack && 
      g < thresholdblack && 
      b < thresholdblack && 
      c < clear_threshold && 
      cocau.battatcuonbong == 1 && 
      cocau.tieptucsortbong == 0) 
  {
    //cho bóng vào hộp dưới
    pwm.setPWM(servo2, 0, vaothung);
    pwm.setPWM(servo3, 0, thungduoi);
    //đóng cửa không cho bóng vào nữa
    pwm.setPWM(servo1, 0, Negative_Spin);
    cocau.tieptucsortbong == 1;
    delay(Servo1and4OpenAndClose);
    pwm.setPWM(servo1, 0, neutral);
    delay(50);
    //cho servo2 chéo lên để hứng bóng
    pwm.setPWM(servo2, 0, hungbong);
    delay(200);
    //mở lại cửa để bóng vào trong
    pwm.setPWM(servo1, 0, Positive_Spin); 
    delay(Servo1and4OpenAndClose);
    pwm.setPWM(servo1, 0, neutral);
    cocau.tieptucsortbong == 0;
  }
// Kiểm tra bóng có phải màu trắng không
  else if (r > thresholdwhite &&
           g > thresholdwhite && 
           b > thresholdwhite && 
           abs(r - g) < tolerance && 
           abs(r - b) < tolerance && 
           abs(g - b) < tolerance && 
           cocau.battatcuonbong == 1 && 
           cocau.tieptucsortbong == 0)  
  {
    //cho bóng vào hộp trên
    pwm.setPWM(servo2, 0, vaothung);
    pwm.setPWM(servo3, 0, thungtren);
    //đóng cửa không cho bóng vào nữa
    pwm.setPWM(servo1, 0, Negative_Spin);
    cocau.tieptucsortbong == 1;
    delay(Servo1and4OpenAndClose);
    pwm.setPWM(servo1, 0, neutral);
    delay(50);
    //cho servo2 chéo lên để hứng bóng
    pwm.setPWM(servo2, 0, hungbong);
    delay(200);
    //mở lại cửa để bóng vào trong
    pwm.setPWM(servo1, 0, Positive_Spin);
    delay(Servo1and4OpenAndClose);
    pwm.setPWM(servo1, 0, neutral);
    cocau.tieptucsortbong == 0;
  }
}
  //code trên để tránh tình trạng 2 bóng vào cùng 1 lúc, nhưng sẽ gây sai số nếu cửa chạm vào bóng
  //do servo 360 không điều khiển được góc vậy nên không thể xác định được góc

void nanghathung(){ 

  //Công tắc 1: Nấc dưới cùng
  //Công tắc 2: Đưa bóng đen vào lỗ
  //Công tắc 3: Cao nhất, đưa bóng trắng vào lỗ
  
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

  //outtakestatus = 0: đóng cửa cả 2 thùng
  //outtakestatus = 1: mở cửa thùng trên
  //outtakestatus = 2: mở cửa thùng dưới

  //PSB_GREEN: ấn để mở cửa trên hoặc đóng cửa
  //PSB_BLUE: ấn để mở cửa dưới hoặc đóng cửa

  if (ps2x.ButtonPressed(PSB_GREEN) && cocau.outtakestatus == 0){
    pwm.setPWM(servo4, 0, Negative_Spin);
    delay(Servo1and4OpenAndClose);
    pwm.setPWM(servo4, 0, neutral);
    cocau.outtakestatus = 1;
  }
  else if (ps2x.ButtonPressed(PSB_GREEN) && cocau.outtakestatus == 2){
    pwm.setPWM(servo4, 0, Negative_Spin);
    if (digitalRead(congtac4) == HIGH){
      pwm.setPWM(servo4, 0, neutral);
      cocau.outtakestatus = 0;
  }
  }
  else if (ps2x.ButtonPressed(PSB_BLUE) && cocau.outtakestatus == 0){
    pwm.setPWM(servo4, 0, Positive_Spin);
    delay(Servo1and4OpenAndClose);
    pwm.setPWM(servo4, 0, neutral);
    cocau.outtakestatus = 2;
  }
  else if (ps2x.ButtonPressed(PSB_BLUE) && cocau.outtakestatus == 1){
    pwm.setPWM(servo4, 0, Positive_Spin);
    if (digitalRead(congtac4) == HIGH){
      pwm.setPWM(servo4, 0, neutral);
      cocau.outtakestatus = 0;
    }
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
    Serial.println(" Cam bien mau hoat dong tot");
  } else {
    Serial.println(" LOI: Chua nhan duoc tin hieu cam bien mau");
    while (1); 
  }
}
