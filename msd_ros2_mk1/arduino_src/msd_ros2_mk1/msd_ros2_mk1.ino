// ----- ピン接続の定義 -----
// ----- Pin Connection Definition -----

// オリエンタルモーターの定義
// Oriental motor definiton
// name rules := <left or Right><PIN assigned name> <arduino pin No.>

#define LeftFWD 33 // mLeftFWD 43
#define LeftREV 31 // mLeftREV 45
#define LeftSTOP_MODE 47 // stopModeL 35
#define LeftM0 37 // m0L
#define LeftSPEED_OUT 36 // speedOutL 34
#define LeftWNG 38 // wngLN 32
#define LeftALARM_OUT 30 // alarmOutLN
#define LeftMB_FREE 53 // mbFreeL 41
#define LeftALARM_RESET 51 //alarmResetL 39
#define LeftVM 3 // mmLPin 2

#define RightFWD 33 // mRightFWD
#define RightREV 31 // mRightREV
#define RightSTOP_MODE 47 // stopModeR
#define RightM0 49 // m0R // 速度設定方法の選択 0=zero
#define RightSPEED_OUT 36 // speedOutR
#define RightWNG 38 // wngRN
#define RightALARM_OUT 30 // alarmOutRN 40
#define RightMB_FREE 53 // mbFreeR // モータ停止の電磁ブレーキの動作
#define RightALARM_RESET 51 // alarmResetR
#define RightVM 3 // mmRPin


// シリンダーの定義
// Definition of cylinder

#define LiftFWD 46 // liftFWD
#define LiftREV 48 // liftREV
#define DumpFWD 50 // dumpFWD
#define DumpREV 52 // dumpREV
#define LiftFeedBack A0 // liftFeedBack
#define DumpFeedBack A1 // dumpFeedBack


#define SwitcherOut 20 // 動力リレー信号出力

// ----- 変数定義 -----
//シリンダーフィードバック用変数

double liftData = 0;
double dumpData = 0;


// プロポスイッチ，スティックの定義
// Definition of Propo Switch, Stick

const int switcherIn = 8; // ch6 switch input
const int ch1 = 13; // 右スティック（横）// right horizontal
const int ch3 = 11; // 右スティック（縦）// right vertical
const int ch2 = 12; // 左スティック（縦）// left vertical
const int ch4 = 10; // 左スティック（横）// left horizontal
const int ch5 = 9;  // sw(B)モード切り替えスイッチ // sw(B) for convert FWD/REV direction
const int ch7 = 7;  // [VR]つまみ // VR channel

// プロポレシーバー変数
// Propo Receiver Variables

int VR = 0;
int rightX = 0;
int rightY = 0;
int leftX, leftXa , leftXb;
int leftY = 0;
int switcher = 0;
int gap;


// その他機能
// Definition of other functions

// バッテリーマネージメント
// battery manegement
int batteryPin = A2; // バッテリ電圧[V]入力
int batteryV = 0; // 初期値

// モード設定
int gMode = 1; // バケットモード:1, ブレードモード:0

//エンコーダ読み取り
unsigned long T_Left_us, T_Right_us; // 計測したパルス幅 [時間 us]
double N_Left_rpm, N_Right_rpm; // 回転数[rpm]
double v_mps; // 速度[m/s]
double v_kmph; // 速度[km/h]

// 超音波センサ
const int UssAnPinLeft = A3;
const int UssAnPinRight = A4;
long LeftDistance = 0;
long RightDistance = 0;


// main routine
void setup() {
  Serial.begin(9600);

  // pinMode(LED_BUILTIN,OUTPUT);
  // digitalWrite(LED_BUILTIN,LOW);
  
  pinMode(LeftFWD,OUTPUT);
  pinMode(LeftREV,OUTPUT);
  pinMode(LeftSTOP_MODE,OUTPUT);
  pinMode(LeftM0,OUTPUT);
  pinMode(LeftSPEED_OUT,OUTPUT);
  pinMode(LeftMB_FREE,OUTPUT);
  pinMode(LeftALARM_RESET,OUTPUT);
  pinMode(LeftVM,OUTPUT);
  pinMode(LeftALARM_OUT,INPUT);
  pinMode(LeftWNG,INPUT);


  pinMode(RightFWD,OUTPUT);
  pinMode(RightREV,OUTPUT);
  pinMode(RightSTOP_MODE,OUTPUT);
  pinMode(RightM0,OUTPUT);
  pinMode(RightSPEED_OUT,OUTPUT);
  pinMode(RightMB_FREE,OUTPUT);
  pinMode(RightALARM_RESET,OUTPUT);
  pinMode(RightVM,OUTPUT);
  pinMode(RightWNG,OUTPUT);
  pinMode(RightALARM_OUT,OUTPUT);
  
  
  pinMode(LiftFWD, OUTPUT);
  pinMode(LiftREV, OUTPUT);
  pinMode(DumpFWD, OUTPUT);
  pinMode(DumpREV, OUTPUT);

  pinMode(LiftFeedBack, INPUT);
  pinMode(DumpFeedBack, INPUT);

  
  pinMode(ch1, INPUT);
  pinMode(ch3, INPUT);
  pinMode(ch2, INPUT);
  pinMode(ch4, INPUT);
  pinMode(ch5, INPUT);
  pinMode(ch7, INPUT);
  pinMode(switcherIn, INPUT);


  pinMode(SwitcherOut, OUTPUT); // 動力リレー用リレー

  // moter encoder
  pinMode(LeftSPEED_OUT, INPUT);
  pinMode(RightSPEED_OUT, INPUT);

  // Battery Pin
  pinMode(batteryPin, INPUT);

  // UltraSonic Sensor
  

  // モータ設定
  // settings of moter

  // HIGH:時間設定 LOW:瞬時停止
  digitalWrite(LeftSTOP_MODE, LOW);
  digitalWrite(RightSTOP_MODE, LOW);
  
  // 速度指定 HIGH:外部速度設定器 LOW:内部速度設定器
  digitalWrite(LeftM0, HIGH);
  digitalWrite(RightM0, HIGH);
  
  // HIGH:ブレーキ解放 LOW:ブレーキ保持
  digitalWrite(LeftMB_FREE, HIGH);
  digitalWrite(RightMB_FREE, HIGH);
  
}

void loop() {
  // byte var;
  // var = Serial.read();

  // switch(var){
  //   case '0':
  //     // d =1000;
  //     digitalWrite(LED_BUILTIN,LOW);
  //     break;
  //   case '1':
  //     digitalWrite(LED_BUILTIN,HIGH);
  //     break;
  // }

  AttachmentModeChange(); //バケットモードとブレードモードの切り替え
  encoder();
  printer();
  switcher = pulseIn(switcherIn, HIGH);

  if (switcher > 1550) {
    digitalWrite(SwitcherOut, HIGH); //動力リレー
    VR = pulseIn(ch7, HIGH);

    if (VR > 1700) {
      leftXb = leftXa;
      rightX = pulseIn(ch1, HIGH);
      rightY = pulseIn(ch3, HIGH);
      leftX = pulseIn(ch4, HIGH); 
      leftY = pulseIn(ch2, HIGH); 
      leftXa = leftX;



      gap = abs(leftXb - leftXa);
      if (leftX < 1000 || leftX > 2000) {
        leftX = leftXb;
        leftXa = leftXb;
      } else {
        leftX = leftX;
        leftXa = leftXa;
      }

      // モーター動作
      motor(leftX, leftY);

      UltraSonicSensor();
    } else {
      motor_stop();
    }
  } else {
    digitalWrite(SwitcherOut, LOW);
    motor_stop();
  }

}

// モーター停止関数
void motor_stop() {
  //モーター速度0
  analogWrite(LeftVM, 0);
  analogWrite(RightVM, 0);

  //前進後進指示なし
  digitalWrite(LiftFWD, LOW);
  digitalWrite(LiftREV, LOW);
  digitalWrite(DumpFWD, LOW);
  digitalWrite(DumpREV, LOW);

  //HIGH:ブレーキ解放 LOW:ブレーキ保持
  digitalWrite(LeftMB_FREE, LOW);
  digitalWrite(RightMB_FREE, LOW);
}

// モーター関数
void motor(int leftX, int leftY) {
  //プロポ閾値
  int thresholdHigh = 1715;
  int thresholdMid = 1510;
  int thresholdLow = 1308;

  //移動方向フラグ
  int Right = 0;
  int Left = 0;
  int Forward = 0;
  int Reverse = 0;

  //モーター回転方向フラグ
  bool dirR = false;
  bool dirL = false;
  bool canMove = false;

  //前後左右フラグ反転
  if(gMode == 0){
    int temp = Forward;
    Forward = Reverse;
    Reverse = temp;
    temp = Right;
    Right = Left;
    Left = temp;
  }

  // 移動方向決定
  if (leftX >= thresholdHigh) {
    Right = 1;
  } else if (leftX <= thresholdLow) {
    Left = 1;
  }
  if (leftY >= thresholdHigh) {
    Forward = 1;
  } else if (leftY <= thresholdLow) {
    Reverse = 1;
  }


  //モーター動作決定
  int diffX = abs(leftX-thresholdMid);
  int diffY = abs(leftY-thresholdMid);
  int speedX = map(diffX, 0, 410, 0, 255);
    //Serial.print("speedX => ");
    //Serial.println(speedX);
  int speedY = map(diffY, 0, 410, 0, 255);
    //Serial.print("speedY => ");
    //Serial.println(speedY);
  
  if (Forward == 1) {
    canMove = true;
    dirR = dirL = false;
    if (Right == 1) {
      //右前進
      analogWrite(mmLPin, speedY);
      analogWrite(mmRPin, 0);
    } else if (Left == 1) {
      //左前進
      analogWrite(mmLPin, 0);
      analogWrite(mmRPin, speedY);
    } else {
      //前進
      analogWrite(mmLPin, speedY);
      analogWrite(mmRPin, speedY);
    }
  } else if (Reverse == 1) {
    canMove = true;
    dirR = dirL = true;
    if (Right == 1) {
      //右後進
      analogWrite(mmLPin, speedY);
      analogWrite(mmRPin, 0);
    } else if (Left == 1) {
      //左後進
      analogWrite(mmLPin, 0);
      analogWrite(mmRPin, speedY);
    } else {
      //後進
      analogWrite(mmLPin, speedY);
      analogWrite(mmRPin, speedY);
    }
  } else {
    if (Right == 1) {
      //右旋回
      canMove = true;
      if (gMode == 0) {
        dirR = true;
        dirL = false;
      } else {
        dirR = false;
        dirL = true;  
      }
      analogWrite(mmLPin, speedX);
      analogWrite(mmRPin, speedX);
    } else if (Left == 1) {
      //左旋回
      canMove = true;
      if (gMode == 0) {
        dirR = false;
        dirL = true;
      } else {
        dirR = true;
        dirL = false;  
      }
      analogWrite(mmLPin, speedX);
      analogWrite(mmRPin, speedX);
    } else {
      //停止
      canMove = false;
      analogWrite(mmLPin, 0);
      analogWrite(mmRPin, 0);
    }
  }
  digitalWrite(mRightFWD, dirR & canMove);
  digitalWrite(mRightREV, (!dirR) & canMove);
  digitalWrite(mLeftFWD, dirL & canMove);
  digitalWrite(mLeftREV, (!dirL) & canMove);
}

void propo_controllor(){
  //プロポスティック閾値
  int max = 1715;
  int midle = 1510;
  int min = 1308;


}

void encoder(){
  // Left motor
  /*LOWの0.2ms = 200 usを追加  //100rpm:598000;//4000rpm:300+200; 200rpm:300000; */
  T_Left_us = pulseIn(LeftSPEED_OUT, HIGH,900) + 200 ;

  if(T_Left_us <= 20000 && T_Left_us >= 500){
    T_Left_us = T_Left_us ;
    N_Left_rpm = 2 / (T_Left_us * 1e-6); //回転数 N[rpm]
  }
  else{
    T_Left_us = 0;
    N_Left_rpm = 0;
  }

  // Right motor
  T_Right_us = pulseIn(RightSPEED_OUT, HIGH,900) + 200 ;

  if(T_Right_us <= 20000 && T_Right_us >= 500){
    T_Right_us = T_Right_us ;
    N_Right_rpm = 2 / (T_Right_us * 1e-6); //回転数 N[rpm]
  }
  else{
    T_Right_us = 0;
    N_Right_rpm = 0;
  }

  //速度計算
  v_mps = 0.1105 / 100 * (N_Left_rpm + N_Right_rpm) / 2 * 3.14 / 30 ; //速度 [m/s]
  v_kmph = v_mps / 1000 * (60 * 60) ; //速度 [km/h]
}

void AttachmentModeChange(){
  int val=digitalRead(ch5);
   if (val == 1){
      gMode=1;
  }
   else if(val == 0){
      gMode=0;
   }
  //Serial.println(val);
}

void UltraSonicSensor(){ 
  /*  Read sensors
  Scale factor is (Vcc/1024) per 10mm. A 5V supply yields ~4.9mV/10mm
  Arduino analog pin goes from 0 to 1024, so the value has to be multiplied by 10 to get range in mm
  */
  LeftDistance = analogRead(UssAnPinLeft)*10;
  RightDistance = analogRead(UssAnPinRight)*10;

  if (LeftDistance <= 500 || RightDistance <= 500){
    motor_stop();
  }
}

void printer(){
  //UltoraSonic Sensor
  Serial.print("Left Distance");
  Serial.print(" ");
  Serial.print(LeftDistance);
  Serial.print("mm");
  Serial.print(" ");
  Serial.print("Right Distance");
  Serial.print(LeftDistance);
  Serial.print("mm");
  Serial.println();
  //encoder
  Serial.print(T_Left_us);
  Serial.print(" [us]   "); //Left Pulse width
  Serial.print(N_Left_rpm,0);
  Serial.print(" [rpm]   ");
  
  Serial.print(T_Right_us);
  Serial.print(" [us]   "); //Left Pulse width
  Serial.print(N_Right_rpm,0);
  Serial.print(" [rpm]   ");

  Serial.print(v_mps,3); //速度 [m/s]
  Serial.print(" [m/s]   ");
  Serial.print(v_kmph,3); //速度 [km/h]
  Serial.print(" [km/h]   ");

  Serial.println();//改行用
}