// LED 引脚定义（ESP32 板载 LED 通常为 GPIO 2）
const int ledPin = 2;

// 定义步骤结构：LED 状态与持续时间（毫秒）
struct Step {
  bool ledState;
  unsigned long duration;
};

// SOS 闪烁序列（短闪 S：亮 200ms 灭 200ms，重复 3 次；长闪 O：亮 600ms 灭 200ms，重复 3 次）
// 字母间隔 500ms（灭），单词间隔 2000ms（灭）
Step steps[] = {
  // 字母 S（短闪 3 次）
  {HIGH, 200}, {LOW, 200},
  {HIGH, 200}, {LOW, 200},
  {HIGH, 200}, {LOW, 200},
  // 字母间隔
  {LOW, 500},
  // 字母 O（长闪 3 次）
  {HIGH, 600}, {LOW, 200},
  {HIGH, 600}, {LOW, 200},
  {HIGH, 600}, {LOW, 200},
  // 字母间隔
  {LOW, 500},
  // 字母 S（短闪 3 次）
  {HIGH, 200}, {LOW, 200},
  {HIGH, 200}, {LOW, 200},
  {HIGH, 200}, {LOW, 200},
  // 单词间隔（结束一组 SOS 后的停顿）
  {LOW, 2000}
};

const int stepCount = sizeof(steps) / sizeof(steps[0]);
int currentStep = 0;               // 当前步骤索引
unsigned long previousMillis = 0;  // 上一次步骤切换的时间

void setup() {
  Serial.begin(115200);
  pinMode(ledPin, OUTPUT);
  // 初始 LED 状态设置为第一步的状态
  digitalWrite(ledPin, steps[0].ledState);
  previousMillis = millis();
}

void loop() {
  unsigned long currentMillis = millis();

  // 检查当前步骤是否已执行完毕
  if (currentMillis - previousMillis >= steps[currentStep].duration) {
    // 切换到下一步
    currentStep++;
    if (currentStep >= stepCount) {
      currentStep = 0;  // 循环播放 SOS
    }

    // 更新 LED 状态
    digitalWrite(ledPin, steps[currentStep].ledState);
    previousMillis = currentMillis;

    // 可选：在串口监视器输出状态（方便调试）
    if (steps[currentStep].ledState == HIGH) {
      Serial.println("LED ON");
    } else {
      Serial.println("LED OFF");
    }
  }

  // 此处可添加其他不阻塞的任务（例如读取传感器、通信等）
}