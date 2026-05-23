#include <WiFi.h>
#include <WebServer.h>

// WiFi 配置
const char* ssid = "123456";
const char* password = "a1234567";

// 硬件引脚定义
const int LED_PIN = 2;          // 报警 LED (GPIO2)
const int TOUCH_PIN = T0;       // 触摸引脚 GPIO4 (ESP32 触摸引脚 T0)

// 系统状态变量
bool armed = false;             // 是否布防
bool alarmActive = false;       // 是否正在报警闪烁
unsigned long lastToggle = 0;
const unsigned long BLINK_INTERVAL = 150;  // 狂闪间隔 150ms
bool ledState = false;

WebServer server(80);

// 触摸中断处理函数
void IRAM_ATTR onTouch() {
  // 仅在布防且未处于报警状态时触发报警
  if (armed && !alarmActive) {
    alarmActive = true;
  }
}

// 生成 HTML 页面（包含状态显示和布防/撤防按钮）
String makePage() {
  String statusText;
  String statusColor;
  if (alarmActive) {
    statusText = " 报警中！";
    statusColor = "red";
  } else if (armed) {
    statusText = "已布防 (Arm)";
    statusColor = "green";
  } else {
    statusText = "已撤防 (Disarm)";
    statusColor = "gray";
  }

  String html = R"rawliteral(
<!DOCTYPE html>
<html lang="zh-CN">
<head>
  <meta charset="UTF-8">
  <meta name="viewport" content="width=device-width, initial-scale=1.0">
  <title>ESP32 安防报警器</title>
  <style>
    body { font-family: Arial; text-align: center; margin-top: 50px; background: #f0f0f0; }
    .container { background: white; max-width: 400px; margin: auto; padding: 30px; border-radius: 20px; box-shadow: 0 4px 8px rgba(0,0,0,0.2); }
    h1 { color: #333; }
    .status { font-size: 1.5em; margin: 20px; padding: 15px; border-radius: 10px; font-weight: bold; }
    button { font-size: 1.2em; padding: 12px 25px; margin: 10px; border: none; border-radius: 30px; cursor: pointer; transition: 0.3s; }
    .arm { background-color: #2ecc71; color: white; }
    .arm:hover { background-color: #27ae60; }
    .disarm { background-color: #e74c3c; color: white; }
    .disarm:hover { background-color: #c0392b; }
    .info { margin-top: 30px; color: #555; font-size: 0.9em; }
  </style>
</head>
<body>
<div class="container">
  <h1>物联网安防报警器</h1>
  <div class="status" style="color: )rawliteral" + statusColor + R"rawliteral(;">)rawliteral" + statusText + R"rawliteral(</div>
  <a href="/arm"><button class="arm">布防 (Arm)</button></a>
  <a href="/disarm"><button class="disarm"> 撤防 (Disarm)</button></a>
  <div class="info">
    <p>布防后，触摸传感器引脚 (GPIO4) 将触发报警</p>
    <p>报警后 LED 狂闪，必须点击【撤防】才能停止</p>
  </div>
</div>
</body>
</html>
)rawliteral";
  return html;
}

void handleRoot() {
  server.send(200, "text/html; charset=UTF-8", makePage());
}

// 布防处理
void handleArm() {
  armed = true;
  alarmActive = false;       // 布防时清除之前的报警标志
  digitalWrite(LED_PIN, LOW); // 确保 LED 熄灭
  ledState = false;
  Serial.println("系统已布防 (Armed)");
  server.sendHeader("Location", "/");
  server.send(303);
}

// 撤防处理
void handleDisarm() {
  armed = false;
  alarmActive = false;       // 停止报警
  digitalWrite(LED_PIN, LOW); // 熄灭 LED
  ledState = false;
  Serial.println("系统已撤防 (Disarmed)");
  server.sendHeader("Location", "/");
  server.send(303);
}

void setup() {
  Serial.begin(115200);
  pinMode(LED_PIN, OUTPUT);
  digitalWrite(LED_PIN, LOW);

  // 配置触摸引脚中断，触发方式为触摸时产生中断
  touchAttachInterrupt(TOUCH_PIN, onTouch, 40); // 阈值 40，可根据灵敏度调整

  // 连接 WiFi
  WiFi.begin(ssid, password);
  Serial.print("正在连接 WiFi");
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("\nWiFi 连接成功");
  Serial.print("ESP32 访问地址: http://");
  Serial.println(WiFi.localIP());

  // 配置 Web 路由
  server.on("/", handleRoot);
  server.on("/arm", handleArm);
  server.on("/disarm", handleDisarm);
  server.begin();
  Serial.println("HTTP 服务器已启动");
}

void loop() {
  server.handleClient();  // 处理网页请求

  // 报警闪烁逻辑 (非阻塞)
  if (alarmActive) {
    unsigned long now = millis();
    if (now - lastToggle >= BLINK_INTERVAL) {
      lastToggle = now;
      ledState = !ledState;
      digitalWrite(LED_PIN, ledState ? HIGH : LOW);
    }
  }
}