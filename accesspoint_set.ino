// ============================================================================
// ESP32-S3 AP 모드 GPIO41 / GPIO42 웹 제어 서버
//
// 기능
// 1. ESP32-S3가 WiFi AP(Access Point) 생성
// 2. 스마트폰 또는 PC가 ESP32에 직접 접속
// 3. 웹 브라우저에서 ON/OFF 버튼 표시
// 4. GPIO41, GPIO42 출력 제어
//
// 접속 주소
// http://192.168.4.1
// ============================================================================

#include <WiFi.h>   // WiFi 기능 사용

// ============================================================================
// AP(Access Point) 정보
// ============================================================================
const char* ssid     = "ESP32-Access-Point";
const char* password = "123456789";

// ============================================================================
// 웹서버 생성
// ============================================================================
WiFiServer server(80);

// HTTP 요청 저장 문자열
String header;

// ============================================================================
// GPIO 상태 저장 변수
// ============================================================================
String output41State = "off";
String output42State = "off";

// ============================================================================
// GPIO 번호
// ============================================================================
const int output41 = 41;
const int output42 = 42;

// ============================================================================
// SETUP
// ============================================================================
void setup() {

  // --------------------------------------------------------
  // 시리얼 모니터 시작
  // --------------------------------------------------------
  Serial.begin(115200);

  // --------------------------------------------------------
  // GPIO 출력 모드 설정
  // --------------------------------------------------------
  pinMode(output41, OUTPUT);
  pinMode(output42, OUTPUT);

  // --------------------------------------------------------
  // 초기 상태 OFF
  // --------------------------------------------------------
  digitalWrite(output41, LOW);
  digitalWrite(output42, LOW);

  // --------------------------------------------------------
  // AP 생성
  // --------------------------------------------------------
  Serial.println("Creating Access Point...");

  WiFi.softAP(ssid, password);

  // --------------------------------------------------------
  // AP IP 출력
  // 기본값 : 192.168.4.1
  // --------------------------------------------------------
  IPAddress IP = WiFi.softAPIP();

  Serial.print("AP IP Address : ");
  Serial.println(IP);

  // --------------------------------------------------------
  // 웹 서버 시작
  // --------------------------------------------------------
  server.begin();

  Serial.println("HTTP Server Started");
}

// ============================================================================
// LOOP
// ============================================================================
void loop() {

  // --------------------------------------------------------
  // 새로운 클라이언트 확인
  // --------------------------------------------------------
  WiFiClient client = server.available();

  // 접속자가 없으면 종료
  if (!client) return;

  Serial.println("New Client Connected");

  String currentLine = "";

  // --------------------------------------------------------
  // 클라이언트 연결 유지
  // --------------------------------------------------------
  while (client.connected()) {

    if (client.available()) {

      char c = client.read();

      Serial.write(c);

      header += c;

      // ----------------------------------------------------
      // 한 줄 끝
      // ----------------------------------------------------
      if (c == '\n') {

        // --------------------------------------------------
        // 빈 줄 = HTTP Header 끝
        // --------------------------------------------------
        if (currentLine.length() == 0) {

          // ==================================================
          // GPIO41 ON
          // ==================================================
          if (header.indexOf("GET /41/on") >= 0) {

            Serial.println("GPIO41 ON");

            output41State = "on";

            digitalWrite(output41, HIGH);
          }

          // ==================================================
          // GPIO41 OFF
          // ==================================================
          else if (header.indexOf("GET /41/off") >= 0) {

            Serial.println("GPIO41 OFF");

            output41State = "off";

            digitalWrite(output41, LOW);
          }

          // ==================================================
          // GPIO42 ON
          // ==================================================
          else if (header.indexOf("GET /42/on") >= 0) {

            Serial.println("GPIO42 ON");

            output42State = "on";

            digitalWrite(output42, HIGH);
          }

          // ==================================================
          // GPIO42 OFF
          // ==================================================
          else if (header.indexOf("GET /42/off") >= 0) {

            Serial.println("GPIO42 OFF");

            output42State = "off";

            digitalWrite(output42, LOW);
          }

          // ==================================================
          // HTTP 응답 시작
          // ==================================================
          client.println("HTTP/1.1 200 OK");
          client.println("Content-type:text/html");
          client.println("Connection: close");
          client.println();

          // ==================================================
          // HTML 페이지 생성
          // ==================================================
          client.println("<!DOCTYPE html>");
          client.println("<html>");

          client.println("<head>");

          client.println("<meta name=\"viewport\" content=\"width=device-width, initial-scale=1\">");

          client.println("<style>");

          client.println("html {");
          client.println("font-family: Helvetica;");
          client.println("text-align:center;");
          client.println("}");

          client.println(".button {");
          client.println("background:#4CAF50;");
          client.println("color:white;");
          client.println("padding:16px 40px;");
          client.println("font-size:30px;");
          client.println("border:none;");
          client.println("}");

          client.println(".button2 {");
          client.println("background:#555555;");
          client.println("}");

          client.println("</style>");

          client.println("</head>");

          // ==================================================
          // BODY 시작
          // ==================================================
          client.println("<body>");

          client.println("<h1>ESP32-S3 Web Server</h1>");

          // ==================================================
          // GPIO41 상태 표시
          // ==================================================
          client.println("<h2>GPIO41</h2>");

          client.println("<p>State : " + output41State + "</p>");

          if (output41State == "off") {

            client.println("<a href=\"/41/on\">");
            client.println("<button class=\"button\">ON</button>");
            client.println("</a>");

          } else {

            client.println("<a href=\"/41/off\">");
            client.println("<button class=\"button button2\">OFF</button>");
            client.println("</a>");
          }

          // ==================================================
          // 구분선
          // ==================================================
          client.println("<hr>");

          // ==================================================
          // GPIO42 상태 표시
          // ==================================================
          client.println("<h2>GPIO42</h2>");

          client.println("<p>State : " + output42State + "</p>");

          if (output42State == "off") {

            client.println("<a href=\"/42/on\">");
            client.println("<button class=\"button\">ON</button>");
            client.println("</a>");

          } else {

            client.println("<a href=\"/42/off\">");
            client.println("<button class=\"button button2\">OFF</button>");
            client.println("</a>");
          }

          client.println("</body>");
          client.println("</html>");

          // HTTP 종료
          client.println();

          break;
        }
        else {

          // 다음 줄 준비
          currentLine = "";
        }
      }
      else if (c != '\r') {

        // 현재 줄 저장
        currentLine += c;
      }
    }
  }

  // --------------------------------------------------------
  // 요청 데이터 초기화
  // --------------------------------------------------------
  header = "";

  // --------------------------------------------------------
  // 클라이언트 연결 종료
  // --------------------------------------------------------
  client.stop();

  Serial.println("Client Disconnected");
}
