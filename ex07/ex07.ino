#include <WiFi.h>
#include <WebServer.h>

const char* ssid = "123456";
const char* password = "a1234567";
const int LED_PIN = 2;           // GPIO2

WebServer server(80);

// PWM 配置
int currentDuty = 0;             // 当前占空比 (0 ~ 255)
const int PWM_FREQ = 5000;       // 5kHz
const int PWM_RES = 8;           // 8位分辨率

String makePage() {
  // 计算当前亮度百分比
  int percent = map(currentDuty, 0, 255, 0, 100);
  String html = R"rawliteral(
<!DOCTYPE html>
<html lang="zh-CN">
<head>
  <meta charset="UTF-8">
  <meta name="viewport" content="width=device-width, initial-scale=1.0">
  <title>实验2 - 滑块控制LED亮度</title>
  <style>
    body { font-family: Arial; text-align: center; margin-top: 50px; }
    .slider-container { margin: 30px auto; width: 80%; max-width: 400px; }
    input { width: 100%; }
    .brightness { font-size: 1.5em; margin: 15px; }
    button { padding: 10px 20px; font-size: 16px; margin: 5px; cursor: pointer; }
  </style>
</head>
<body>
  <h1>第二部分：滚动条实时控制 LED 亮度</h1>
  <p>当前亮度：<b id="brightnessValue">)rawliteral" + String(percent) + R"rawliteral(%)</b></p>
  <div class="slider-container">
    <input type="range" id="brightnessSlider" min="0" max="100" value=")rawliteral" + String(percent) + R"rawliteral(" step="1">
  </div>
  <div>
    <button id="btnOn">点亮 (100%)</button>
    <button id="btnOff">熄灭 (0%)</button>
  </div>
  <script>
    const slider = document.getElementById('brightnessSlider');
    const brightnessSpan = document.getElementById('brightnessValue');
    
    function setBrightness(value) {
      fetch('/setduty?value=' + value)
        .then(response => {
          if(response.ok) brightnessSpan.innerText = value + '%';
        })
        .catch(err => console.log('Error:', err));
    }
    
    slider.addEventListener('input', function() {
      const val = this.value;
      brightnessSpan.innerText = val + '%';
      setBrightness(val);
    });
    
    document.getElementById('btnOn').addEventListener('click', function() {
      slider.value = 100;
      brightnessSpan.innerText = '100%';
      setBrightness(100);
    });
    
    document.getElementById('btnOff').addEventListener('click', function() {
      slider.value = 0;
      brightnessSpan.innerText = '0%';
      setBrightness(0);
    });
  </script>
</body>
</html>
)rawliteral";
  return html;
}

void handleRoot() {
  server.send(200, "text/html; charset=UTF-8", makePage());
}

// 处理滑块发来的占空比请求（参数 value: 0~100 百分比）
void handleSetDuty() {
  if (server.hasArg("value")) {
    int percent = server.arg("value").toInt();
    if (percent < 0) percent = 0;
    if (percent > 100) percent = 100;
    currentDuty = map(percent, 0, 100, 0, 255);
    ledcWriteChannel(0, currentDuty);   // 写入PWM占空比
    Serial.print("亮度设置为: ");
    Serial.print(percent);
    Serial.println("%");
    server.send(200, "text/plain", "OK");
  } else {
    server.send(400, "text/plain", "Missing value");
  }
}

// 保留原有的 /on 和 /off 路由（兼容旧按钮，但实际已经被新按钮替代）
void handleOn() {
  currentDuty = 255;
  ledcWriteChannel(0, currentDuty);
  server.sendHeader("Location", "/");
  server.send(303);
}

void handleOff() {
  currentDuty = 0;
  ledcWriteChannel(0, currentDuty);
  server.sendHeader("Location", "/");
  server.send(303);
}

void setup() {
  Serial.begin(115200);
  pinMode(LED_PIN, OUTPUT);
  
  // 初始化 PWM（ESP32 新版函数）
  ledcAttach(LED_PIN, PWM_FREQ, PWM_RES);
  ledcWriteChannel(0, 0);      // 初始熄灭
  currentDuty = 0;

  // 连接 WiFi
  WiFi.begin(ssid, password);
  Serial.print("连接WiFi");
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("\n连接成功");
  Serial.print("访问地址: http://");
  Serial.println(WiFi.localIP());

  server.on("/", handleRoot);
  server.on("/setduty", handleSetDuty);
  server.on("/on", handleOn);
  server.on("/off", handleOff);
  server.begin();
  Serial.println("HTTP服务器已启动，使用滑块调节亮度");
}

void loop() {
  server.handleClient();
}