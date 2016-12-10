/*
리모트 콘트롤 운행 
Ver 0.1 기초 코드.
*/
#include <IRremoteModule.h>

//
// 자동자 진행 방향 정의
//
#define CAR_DIR_FW  0 // 전진.
#define CAR_DIR_BK  1 // 후진.
#define CAR_DIR_LF  2 // 좌회전.
#define CAR_DIR_RF  3 // 우회전
#define CAR_DIR_ST  4 // 정지.

// 
// 차량 운행 방향 상태 전역 변수.
int g_carDirection = CAR_DIR_ST; // 
int g_carSpeed = 150; // 최대 속도의  50 % for testing.

 
// 리모트 컨트롤러 관련 전역 변수.
const int irReceiverPin = A3;
IRrecv irrecv(irReceiverPin); //create an IRrecv object
decode_results decodedSignal; //stores results from IR sensor


void init_IRreceiverModule()
{
  pinMode(irReceiverPin, INPUT); // 
  irrecv.enableIRIn(); // Start the receiver object
}

struct IRvalueData
{
  String name;
  unsigned long value; // 
};

IRvalueData irData[21]=
{
  { "0", 0xFF6897 },
  { "1", 0xFF30CF },
  { "2", 0xFF18E7 },
  { "3", 0xFF7A85 },
  { "4", 0xFF10EF },
  { "5", 0xFF38C7 },
  { "6", 0xFF5AA5 },
  { "7", 0xFF42BD },
  { "8", 0xFF4AB5 },
  { "9", 0xFF52AD },
  { "100+", 0xFF9867 },
  { "200+", 0xFFB04F },
  { "-", 0xFFE01F },
  { "+", 0xFFA857 },
  { "EQ", 0xFF906F },
  { "<<", 0xFF22DD },
  { ">>", 0xFF02FD },
  { ">|", 0xFFC23D },
  { "CH-", 0xFFA25D },
  { "CH", 0xFF629D },
  { "CH+", 0xFFE21D }
};


// Dumps out the decode_results structure.
// Call this after IRrecv::decode()
// void * to work around compiler issue
//void dump(void *v) {
//  decode_results *results = (decode_results *)v

void dump(decode_results *results) {
  int count = results->rawlen;
  if (results->decode_type == UNKNOWN) {
    Serial.print("Unknown encoding: ");
  }
  else if (results->decode_type == NEC) {
    Serial.print("Decoded NEC: ");
  }
  else if (results->decode_type == SONY) {
    Serial.print("Decoded SONY: ");
  }
  else if (results->decode_type == RC5) {
    Serial.print("Decoded RC5: ");
  }
  else if (results->decode_type == RC6) {
    Serial.print("Decoded RC6: ");
  }
  else if (results->decode_type == PANASONIC) {
    Serial.print("Decoded PANASONIC - Address: ");
    Serial.print(results->panasonicAddress, HEX);
    Serial.print(" Value: ");
  }
  else if (results->decode_type == LG) {
    Serial.print("Decoded LG: ");
  }
  else if (results->decode_type == JVC) {
    Serial.print("Decoded JVC: ");
  }
  Serial.print(results->value, HEX);
  Serial.print(" (");
  Serial.print(results->bits, DEC);
  Serial.println(" bits)");
  Serial.print("Raw (");
  Serial.print(count, DEC);
  Serial.print("): ");

  for (int i = 0; i < count; i++) {
    if ((i % 2) == 1) {
      Serial.print(results->rawbuf[i] * USECPERTICK, DEC);
    }
    else {
      Serial.print(-(int)results->rawbuf[i] * USECPERTICK, DEC);
    }
    Serial.print(" ");
  }
  Serial.println("");
}


//
String decode_IRvalue(unsigned long irValue)
{
  for (int i = 0; i < 21;i++)
  {
    if (irData[i].value==irValue)
    { 
      return irData[i].name;
    }
  }
  Serial.println("Not Defined.");
  return String("key Value None");
}

void controllerByIRCommand(String& szIRCmd)
{

  if (szIRCmd == "2") // 전진
  {
    g_carDirection = CAR_DIR_FW;
    Serial.println("Forward");
  }
  else
  if (szIRCmd == "5")
  {
    g_carDirection = CAR_DIR_ST;
    Serial.println("Stop");
  }
  else
  if (szIRCmd == "8")
  {
    g_carDirection = CAR_DIR_BK;
    Serial.println("Backward");
  }
  else
  if (szIRCmd == "4")
  {
    g_carDirection = CAR_DIR_LF;
    Serial.println("Left");
  }
  else
  if (szIRCmd == "6")
  {
    g_carDirection = CAR_DIR_RF;
    Serial.println("Right");
  }
}


//
// IR 수신값 처리.
//
void update_IRreceiverModule()
{
  // IR 수신값이 있는지 판단.
  if (irrecv.decode(&decodedSignal) == true)
  {
    if (decodedSignal.bits > 0) // bit > 0 일 경우에만 수신값 체크.
    {
      Serial.println(decodedSignal.value);
      //dump(&decodedSignal);
      String szRecvCmd=decode_IRvalue(decodedSignal.value);
      controllerByIRCommand(szRecvCmd);
    }
    irrecv.resume(); // watch out for another message
  }
}

//
// 주의:  ENA, ENB 는 PWM 지원 포트에 연결한다.
//
#define ENA   6
#define EN1   7
#define EN2   3

#define EN3   4
#define EN4   2
#define ENB   5



void init_car_controller_board()
{
  pinMode(ENA, OUTPUT);  // ENA
  pinMode(EN1, OUTPUT);  // EN1
  pinMode(EN2, OUTPUT);  // EN2

  pinMode(ENB, OUTPUT);  // ENB
  pinMode(EN3, OUTPUT);  // EN3
  pinMode(EN4, OUTPUT);  // EN4
}

void car_backward()
{
  digitalWrite(EN1, LOW);
  digitalWrite(EN2, HIGH);
  analogWrite(ENA, g_carSpeed);

  digitalWrite(EN3, LOW);
  digitalWrite(EN4, HIGH);
  analogWrite(ENB, g_carSpeed);
}

void car_forward()//전진
{

  digitalWrite(EN1, HIGH);
  digitalWrite(EN2, LOW);
  analogWrite(ENA, g_carSpeed);

  digitalWrite(EN3, HIGH);
  digitalWrite(EN4, LOW);
  analogWrite(ENB, g_carSpeed);

}
//
void car_right()
{
  digitalWrite(EN1, HIGH);
  digitalWrite(EN2, LOW);
  analogWrite(ENA, g_carSpeed);

  digitalWrite(EN3, LOW);
  digitalWrite(EN4, HIGH);
  analogWrite(ENB, g_carSpeed);
}

//
void car_left()
{
  digitalWrite(EN1, LOW);
  digitalWrite(EN2, HIGH);
  analogWrite(ENA, g_carSpeed);

  digitalWrite(EN3, HIGH);
  digitalWrite(EN4, LOW);
  analogWrite(ENB, g_carSpeed);
}


//
void car_stop()
{
  analogWrite(ENA, 0);
  analogWrite(ENB, 0);
}

//
// 방향 전환값에 의해 차량 운행.
//
void car_update()
{
  if (g_carDirection == CAR_DIR_FW)  // 전진
  {
    car_forward();
  }
  else 
  if (g_carDirection == CAR_DIR_BK) // 후진.
  {
    car_backward();
  }
  else 
  if (g_carDirection == CAR_DIR_LF) // 좌회전
  {
    car_left();
  }
  else 
  if (g_carDirection == CAR_DIR_RF) // 우회전
  {
    car_right();
  }
  else
  if (g_carDirection == CAR_DIR_ST) // 정지.
  {
    car_stop();
  }
}


// 부팅 후 1회 실행되는 함수. 초기화 함수. Setup()
void setup()
{
  //
  Serial.begin(9600);
  //
  init_IRreceiverModule();
  init_car_controller_board();
}

// 계속 실행되는 함수. Loop()
void loop()
{
  car_update();
  update_IRreceiverModule();
}
