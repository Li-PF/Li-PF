// 定义 LED 引脚（ESP32 板载 LED 通常为 GPIO 2）
const int ledPin = 2;

// 记录上一次 LED 状态切换的时间（毫秒）
unsigned long previousMillis = 0;
// 定义闪烁周期：1Hz 对应周期 1000ms，即每 500ms 切换一次状态
const long interval = 500;

// LED 当前状态
bool ledState = LOW;

void setup() {
  Serial.begin(115200);
  pinMode(ledPin, OUTPUT);
  // 初始状态为熄灭，与 previousMillis 初始值无关
  digitalWrite(ledPin, ledState);
}

void loop() {
  // 获取当前运行时间（毫秒）
  unsigned long currentMillis = millis();

  // 检查是否达到切换间隔
  if (currentMillis - previousMillis >= interval) {
    // 保存本次切换的时刻，用于下一次判断
    previousMillis = currentMillis;

    // 翻转 LED 状态
    ledState = !ledState;
    digitalWrite(ledPin, ledState);

    // 串口输出当前状态（方便观察，非必须）
    if (ledState == HIGH) {
      Serial.println("LED ON");
    } else {
      Serial.println("LED OFF");
    }
  }

  // 此处可添加其他非阻塞任务，不会影响 LED 闪烁的精确性
}