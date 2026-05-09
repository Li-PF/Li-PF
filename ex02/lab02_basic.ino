// 定义LED引脚
const int ledPin = 2;

// 时间变量
unsigned long preTime = 0;
// 闪烁间隔 1000ms = 1秒
const long interval = 1000;
// LED状态
bool ledState = LOW;

void setup() {
  Serial.begin(115200);
  pinMode(ledPin, OUTPUT);
}

void loop() {
  // 获取当前系统毫秒时间
  unsigned long nowTime = millis();

  // 判断时间是否达到间隔
  if (nowTime - preTime >= interval)
  {
    preTime = nowTime;       // 更新记录时间
    ledState = !ledState;    // 翻转LED状态
    digitalWrite(ledPin, ledState);

    // 串口打印状态
    if(ledState)
      Serial.println("LED ON");
    else
      Serial.println("LED OFF");
  }
}
