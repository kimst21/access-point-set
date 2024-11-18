//Wi-Fi 라이브러리 로드
#include <WiFi.h>

// 네트워크 자격 증명으로 대체
const char* ssid     = "ESP32-Access-Point";
const char* password = "123456789";

// 웹 서버 포트 번호를 80으로 설정합니다
WiFiServer server(80);

// HTTP 요청을 저장할 변수
String header;

// 현재 출력 상태를 저장하는 보조 변수
String output45State = "off";
String output46State = "off";

// GPIO 핀에 출력 변수 할당
const int output41 = 41;
const int output42 = 42;

void setup() {
  Serial.begin(115200);
  // 출력 변수를 출력으로 초기화
  pinMode(output41, OUTPUT);
  pinMode(output42, OUTPUT);
  // Set outputs to LOW
  digitalWrite(output41, LOW);
  digitalWrite(output42, LOW);

  // SSID 및 암호를 사용하여 Wi-Fi 네트워크에 연결
  Serial.print("Setting AP (Access Point)…");
  // AP(Access Point)를 열려는 경우 암호 매개 변수 제거
  WiFi.softAP(ssid, password);

  IPAddress IP = WiFi.softAPIP();
  Serial.print("AP IP address: ");
  Serial.println(IP);
  
  server.begin();
}

void loop(){
  WiFiClient client = server.available();   // Listen for incoming clients

  if (client) {                             // 새 클라이언트가 연결되면,
    Serial.println("New Client.");          // 직렬 포트에서 메시지를 인쇄
    String currentLine = "";                // 클라이언트에서 들어오는 데이터를 보관할 문자열을 만듭니다
    while (client.connected()) {            // 클라이언트가 연결된 동안 루프
      if (client.available()) {             // 클라이언트에서 읽을 바이트가 있다면,
        char c = client.read();             // 그러면 바이트를 읽습니다
        Serial.write(c);                    // 직렬 모니터를 출력합니다
        header += c;
        if (c == '\n') {                    // 바이트가 줄 바꿈 문자인 경우
          // 현재 줄이 비어 있으면 두 개의 새 줄 문자가 연속으로 나타납니다.
          //클라이언트 HTTP 요청이 끝났으니 응답을 보내십시오:
          if (currentLine.length() == 0) {
            // HTTP 헤더는 항상 응답 코드(예: HTTP/1.1200 OK)로 시작합니다
            // 고객이 무엇이 올지 알 수 있도록 내용 유형을 지정한 다음 빈 줄을 표시합니다:
            client.println("HTTP/1.1 200 OK");
            client.println("Content-type:text/html");
            client.println("Connection: close");
            client.println();
            
            //GPIO 켜기/끄기
            if (header.indexOf("GET /41/on") >= 0) {
              Serial.println("GPIO 41 on");
              output41State = "on";
              digitalWrite(output41, HIGH);
            } else if (header.indexOf("GET /41/off") >= 0) {
              Serial.println("GPIO 41 off");
              output41State = "off";
              digitalWrite(output41, LOW);
            } else if (header.indexOf("GET /42/on") >= 0) {
              Serial.println("GPIO 42 on");
              output42State = "on";
              digitalWrite(output42, HIGH);
            } else if (header.indexOf("GET /42/off") >= 0) {
              Serial.println("GPIO 42 off");
              output42State = "off";
              digitalWrite(output42, LOW);
            }
            
            // HTML 웹 페이지 표시
            client.println("<!DOCTYPE html><html>");
            client.println("<head><meta name=\"viewport\" content=\"width=device-width, initial-scale=1\">");
            client.println("<link rel=\"icon\" href=\"data:,\">");
            // CSS를 사용하여 켜기/끄기 버튼 스타일 지정
            // 기본 설정에 맞게 배경색 및 글꼴 크기 속성을 자유롭게 변경할 수 있습니다
            client.println("<style>html { font-family: Helvetica; display: inline-block; margin: 0px auto; text-align: center;}");
            client.println(".button { background-color: #4CAF50; border: none; color: white; padding: 16px 40px;");
            client.println("text-decoration: none; font-size: 30px; margin: 2px; cursor: pointer;}");
            client.println(".button2 {background-color: #555555;}</style></head>");
            
            // 웹 페이지 제목
            client.println("<body><h1>ESP32 Web Server</h1>");
            
            // 현재 상태 표시 및 GPIO의 ON/OFF 버튼 45
            client.println("<p>GPIO 45 - State " + output45State + "</p>");
            //출력 45 상태가 꺼져 있으면 ON 버튼이 표시됩니다
            if (output45State=="off") {
              client.println("<p><a href=\"/45/on\"><button class=\"button\">ON</button></a></p>");
            } else {
              client.println("<p><a href=\"/45/off\"><button class=\"button button2\">OFF</button></a></p>");
            } 
               
            // 현재 상태 표시, GPIO의 ON/OFF 버튼 46
            client.println("<p>GPIO 46 - State " + output46State + "</p>");
            // If the output46State is off, it displays the ON button       
            if (output46State=="off") {
              client.println("<p><a href=\"/46/on\"><button class=\"button\">ON</button></a></p>");
            } else {
              client.println("<p><a href=\"/46/off\"><button class=\"button button2\">OFF</button></a></p>");
            }
            client.println("</body></html>");
            
            // The HTTP response ends with another blank line
            client.println();
            // Break out of the while loop
            break;
          } else { // if you got a newline, then clear currentLine
            currentLine = "";
          }
        } else if (c != '\r') {  // 캐리지 리턴 캐릭터 말고 다른 게 있다면,
          currentLine += c;      // 현재 줄 끝에 추가합니다
        }
      }
    }
    //헤더 변수 지우기
    header = "";
    // 연결을 닫습니다
    client.stop();
    Serial.println("Client disconnected.");
    Serial.println("");
  }
}

