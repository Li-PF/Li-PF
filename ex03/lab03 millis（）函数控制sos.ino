const int ledPin = 4;

// 时间记录变量
unsigned long currentTime;
unsigned long previousTime = 0;

// 状态机状态
int state = 0;

// 定时时间
const int shortLight = 200;    // 短亮
const int longLight = 600;     // 长亮
const int gap = 200;           // 单次间隔
const int charGap = 500;       // S-O 间隔
const int wordGap = 2000;      // 整组SOS结束间隔

void setup() {
  Serial.begin(115200);
  pinMode(ledPin, OUTPUT);
  digitalWrite(ledPin, LOW);
}

void loop() {
  currentTime = millis();
  
  switch(state)
  {
    // S 3次短闪
    case 0:
      SOS_Short(3);
      previousTime = currentTime;
      state = 1;
      break;

    // 字符间隔
    case 1:
      if(currentTime - previousTime >= charGap)
      {
        state = 2;
      }
      break;

    // O 3次长闪
    case 2:
      SOS_Long(3);
      previousTime = currentTime;
      state = 3;
      break;

    // 字符间隔
    case 3:
      if(currentTime - previousTime >= charGap)
      {
        state = 4;
      }
      break;

    // S 3次短闪
    case 4:
      SOS_Short(3);
      previousTime = currentTime;
      state = 5;
      break;

    // 整组结束延时，循环重来
    case 5:
      if(currentTime - previousTime >= wordGap)
      {
        state = 0;
      }
      break;
  }
}

// 短闪函数
void SOS_Short(int num)
{
  for(int i=0;i<num;i++)
  {
    digitalWrite(ledPin,HIGH);
    delay(shortLight);
    digitalWrite(ledPin,LOW);
    delay(gap);
  }
}

// 长闪函数
void SOS_Long(int num)
{
  for(int i=0;i<num;i++)
  {
    digitalWrite(ledPin,HIGH);
    delay(longLight);
    digitalWrite(ledPin,LOW);
    delay(gap);
  }
}
