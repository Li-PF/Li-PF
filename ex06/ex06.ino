const int LED_A_PIN = 5;      // 灯A引脚
const int LED_B_PIN = 18;      // 灯B引脚
const int PWM_FREQ = 5000;     // PWM频率 5kHz
const int PWM_RESOLUTION = 8;  // 分辨率8位 (0-255)

// 呼吸控制变量
int brightness = 0;    // 当前亮度 0-255
int fadeStep = 1;      // 步进值
int delayTime = 10;     // 延迟时间(ms)

void setup() {
  Serial.begin(115200);
  
  ledcAttach(LED_A_PIN, PWM_FREQ, PWM_RESOLUTION);
  ledcAttach(LED_B_PIN, PWM_FREQ, PWM_RESOLUTION);
  
  // 初始状态
  ledcWrite(LED_A_PIN, 0);
  ledcWrite(LED_B_PIN, 0);
}

void loop() {

  brightness += fadeStep;
  if (brightness <= 0 || brightness >= 255) {
    fadeStep = -fadeStep;
  }
  
  ledcWrite(LED_A_PIN, brightness);
  ledcWrite(LED_B_PIN, 255-brightness);
   Serial.print("亮度: ");
  Serial.print(brightness);
  Serial.print("A: ");
  Serial.print(brightness);
  Serial.print("B: ");
  Serial.println(brightness);

  delay(delayTime);
}

