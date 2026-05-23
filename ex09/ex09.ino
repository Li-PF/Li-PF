#include <WiFi.h>
#include <WebServer.h>

// WiFi 配置
const char* ssid = "123456";
const char* password = "a1234567";

// 触摸引脚 GPIO4 (T0)
const int TOUCH_PIN = T0;

WebServer server(80);

// 读取触摸数值
int readTouchValue() {
  return touchRead(TOUCH_PIN);
}

// 生成简化版 HTML 页面
String makeHTML() {
  String html = R"rawliteral(
<!DOCTYPE html>
<html>
<head>
  <meta charset="UTF-8">
  <meta name="viewport" content="width=device-width, initial-scale=1.0">
  <title>触摸传感器仪表盘</title>
  <style>
    body { font-family: Arial, sans-serif; text-align: center; margin-top: 50px; }
    .value { font-size: 48px; font-weight: bold; margin: 20px; padding: 20px; border: 1px solid #ccc; display: inline-block; min-width: 150px; }
    .progress { width: 80%; margin: 20px auto; background: #ddd; height: 20px; }
    .fill { width: 0%; height: 100%; background: #4caf50; }
    .status { margin: 20px; font-size: 18px; }
  </style>
</head>
<body>
  <h2>实时触摸传感器数值</h2>
  <div class="value" id="touchValue">---</div>
  <div class="progress"><div class="fill" id="progressFill"></div></div>
  <div class="status" id="statusText">等待数据...</div>
  <p>手指靠近 GPIO4 (T0) 数值会变小</p>
  <script>
    function fetchData() {
      fetch('/data')
        .then(response => response.json())
        .then(data => {
          document.getElementById('touchValue').innerText = data.touchValue;
          document.getElementById('progressFill').style.width = data.percent + '%';
          if (data.touchValue < 30) document.getElementById('statusText').innerText = '状态：触摸强烈';
          else if (data.touchValue < 70) document.getElementById('statusText').innerText = '状态：轻微接近';
          else document.getElementById('statusText').innerText = '状态：无触摸';
        })
        .catch(err => console.error(err));
    }
    setInterval(fetchData, 100);
    fetchData();
  </script>
</body>
</html>
)rawliteral";
  return html;
}

void handleRoot() {
  server.send(200, "text/html; charset=UTF-8", makeHTML());
}

void handleData() {
  int raw = touchRead(TOUCH_PIN);
  // 映射百分比：无触摸时约100~150，触摸最低接近0，这里将数值反转：值越小百分比越大
  int maxRef = 120;
  int percent = constrain(map(raw, 0, maxRef, 100, 0), 0, 100);
  if (raw > maxRef) percent = 0;
  String json = "{\"touchValue\":" + String(raw) + ",\"percent\":" + String(percent) + "}";
  server.send(200, "application/json", json);
}

void setup() {
  Serial.begin(115200);
  WiFi.begin(ssid, password);
  Serial.print("Connecting WiFi");
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("\nWiFi connected");
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());

  server.on("/", handleRoot);
  server.on("/data", handleData);
  server.begin();
}

void loop() {
  server.handleClient();
}