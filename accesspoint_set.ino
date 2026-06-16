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
#include <WiFi.h>                 // WiFi 기능 사용

const char* ssid = "ESP32-Access-Point";   // AP 이름
const char* password = "123456789";        // AP 비밀번호

WiFiServer server(80);           // HTTP 서버(포트80)

String header;                   // HTTP 요청 저장

String output41State = "off";    // GPIO41 상태
String output42State = "off";    // GPIO42 상태

const int output41 = 41;         // LED1
const int output42 = 42;         // LED2

void setup() {

  Serial.begin(115200);          // 시리얼 시작

  pinMode(output41, OUTPUT);     // GPIO41 출력 설정
  pinMode(output42, OUTPUT);     // GPIO42 출력 설정

  digitalWrite(output41, LOW);   // 초기 OFF
  digitalWrite(output42, LOW);   // 초기 OFF

  WiFi.softAP(ssid, password);   // AP 생성

  Serial.print("AP IP : ");
  Serial.println(WiFi.softAPIP()); // AP IP 출력

  server.begin();                // 웹서버 시작
}

void loop() {

  WiFiClient client = server.available();  // 접속자 확인

  if (!client) return;                     // 접속자 없으면 종료

  String currentLine = "";                 // 현재 수신 라인

  while (client.connected()) {

    if (client.available()) {

      char c = client.read();              // 문자 1개 읽기

      header += c;                         // HTTP 저장

      if (c == '\n') {

        if (currentLine.length() == 0) {

          if (header.indexOf("GET /41/on") >= 0) {

            output41State = "on";          // 상태 저장
            digitalWrite(output41, HIGH);  // GPIO41 ON

          } else if (header.indexOf("GET /41/off") >= 0) {

            output41State = "off";         // 상태 저장
            digitalWrite(output41, LOW);   // GPIO41 OFF

          } else if (header.indexOf("GET /42/on") >= 0) {

            output42State = "on";          // 상태 저장
            digitalWrite(output42, HIGH);  // GPIO42 ON

          } else if (header.indexOf("GET /42/off") >= 0) {

            output42State = "off";         // 상태 저장
            digitalWrite(output42, LOW);   // GPIO42 OFF
          }

          client.println("HTTP/1.1 200 OK");        // 응답 시작
          client.println("Content-type:text/html");
          client.println("Connection: close");
          client.println();

          client.println("<html>");                 // HTML 시작

          client.println("<h1>ESP32 Web Server</h1>");

          client.println("<p>GPIO41 : " + output41State + "</p>");

          if (output41State == "off")
            client.println("<a href=\"/41/on\">ON</a>");
          else
            client.println("<a href=\"/41/off\">OFF</a>");

          client.println("<p>GPIO42 : " + output42State + "</p>");

          if (output42State == "off")
            client.println("<a href=\"/42/on\">ON</a>");
          else
            client.println("<a href=\"/42/off\">OFF</a>");

          client.println("</html>");      // HTML 종료

          break;
        }

        currentLine = "";

      } else if (c != '\r') {

        currentLine += c;                 // 문자열 누적
      }
    }
  }

  header = "";                            // 요청 초기화

  client.stop();                          // 연결 종료
}
