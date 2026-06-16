#include <WiFi.h>  // ESP32의 WiFi 기능을 사용하기 위한 기본 라이브러리입니다.

// ============================================================================
// AP 모드 WiFi 설정
// ============================================================================
const char* ssid     = "ESP32-Access-Point";  // ESP32가 직접 만들 WiFi 이름입니다.
const char* password = "123456789";           // 스마트폰이나 PC가 접속할 때 사용할 비밀번호입니다.

// ============================================================================
// 웹 서버 및 HTTP 요청 저장 변수
// ============================================================================
WiFiServer server(80);  // 80번 포트에서 웹 서버를 실행합니다. 브라우저 기본 HTTP 포트입니다.

String header;          // 브라우저가 보낸 HTTP 요청 내용을 저장하는 문자열입니다.

// ============================================================================
// GPIO 상태 저장 변수
// ============================================================================
String output41State = "off";  // GPIO41의 현재 상태를 웹 화면에 표시하기 위해 저장합니다.
String output42State = "off";  // GPIO42의 현재 상태를 웹 화면에 표시하기 위해 저장합니다.

// ============================================================================
// 제어할 GPIO 핀 번호
// ============================================================================
const int output41 = 41;  // 첫 번째 출력 장치가 연결된 GPIO 번호입니다.
const int output42 = 42;  // 두 번째 출력 장치가 연결된 GPIO 번호입니다.

// ============================================================================
// SETUP
// 보드가 켜질 때 한 번만 실행되는 초기 설정 영역입니다.
// ============================================================================
void setup() {

  Serial.begin(115200);  // 시리얼 모니터를 시작하여 WiFi 상태와 GPIO 제어 상태를 확인합니다.

  pinMode(output41, OUTPUT);  // GPIO41을 출력 모드로 설정하여 LED나 릴레이를 제어할 수 있게 합니다.
  pinMode(output42, OUTPUT);  // GPIO42를 출력 모드로 설정하여 두 번째 출력 장치를 제어할 수 있게 합니다.

  digitalWrite(output41, LOW);  // 부팅 직후 GPIO41을 LOW로 만들어 OFF 상태에서 시작합니다.
  digitalWrite(output42, LOW);  // 부팅 직후 GPIO42를 LOW로 만들어 OFF 상태에서 시작합니다.

  Serial.println();  // 시리얼 모니터에서 보기 좋게 한 줄을 띄웁니다.
  Serial.println("===================================="); 
  Serial.println("ESP32 AP Web Server Start");        
  Serial.println("====================================");  

  Serial.print("Setting AP : ");  // ESP32가 AP 모드를 만들기 시작했음을 출력합니다.
  Serial.println(ssid);           // 생성할 AP 이름을 시리얼 모니터에 출력합니다.

  WiFi.softAP(ssid, password);  // ESP32를 Access Point 모드로 실행하여 공유기 없이 직접 접속할 수 있게 합니다.

  IPAddress IP = WiFi.softAPIP();  // ESP32 AP 모드의 IP 주소를 가져옵니다. 일반적으로 192.168.4.1입니다.

  Serial.print("AP IP address : ");  // 브라우저에서 접속할 주소를 안내합니다.
  Serial.println(IP);                // 실제 접속 IP를 시리얼 모니터에 출력합니다.

  server.begin();  // 웹 서버를 시작하고 브라우저 접속을 기다립니다.

  Serial.println("HTTP server started.");  // 웹 서버가 정상적으로 시작되었음을 출력합니다.
  Serial.println("Connect to ESP32 WiFi and open http://192.168.4.1");  // 접속 방법을 안내합니다.
}

// ============================================================================
// LOOP
// 전원이 켜져 있는 동안 계속 반복되며, 브라우저 접속과 GPIO 제어를 처리합니다.
// ============================================================================
void loop() {

  WiFiClient client = server.available();  // 새로 접속한 웹 브라우저 클라이언트가 있는지 확인합니다.

  if (client) {  // 브라우저가 ESP32 웹서버에 접속한 경우에만 아래 코드를 실행합니다.

    Serial.println();  // 새 요청이 시작될 때 보기 좋게 한 줄을 띄웁니다.
    Serial.println("New Client Connected.");  // 새로운 사용자가 접속했음을 시리얼 모니터에 표시합니다.

    String currentLine = "";  // HTTP 요청을 한 줄씩 읽기 위해 현재 줄을 저장하는 변수입니다.

    while (client.connected()) {  // 클라이언트가 연결되어 있는 동안 HTTP 요청을 계속 읽습니다.

      if (client.available()) {  // 브라우저에서 읽을 데이터가 도착했는지 확인합니다.

        char c = client.read();  // 브라우저가 보낸 HTTP 요청에서 문자 1개를 읽습니다.

        Serial.write(c);  // 수신된 HTTP 요청 문자를 그대로 시리얼 모니터에 출력하여 디버깅합니다.

        header += c;  // 전체 HTTP 요청 내용을 header 변수에 계속 누적합니다.

        if (c == '\n') {  // 줄바꿈 문자가 들어오면 HTTP 요청의 한 줄이 끝났다는 뜻입니다.

          if (currentLine.length() == 0) {  // 빈 줄이면 HTTP 요청 헤더가 끝났다는 의미이므로 응답을 시작합니다.

            // ==================================================================
            // GPIO41 ON 요청 처리
            // ==================================================================
            if (header.indexOf("GET /41/on") >= 0) {  // 브라우저에서 /41/on 주소를 요청했는지 확인합니다.

              Serial.println();  // 디버그 메시지를 보기 좋게 한 줄 내립니다.
              Serial.println("GPIO 41 ON request received.");  // GPIO41 ON 요청이 들어왔음을 출력합니다.

              output41State = "on";  // 웹 화면에 표시할 GPIO41 상태를 ON으로 저장합니다.
              digitalWrite(output41, HIGH);  // 실제 GPIO41 핀을 HIGH로 만들어 LED 또는 릴레이를 켭니다.
            }

            // ==================================================================
            // GPIO41 OFF 요청 처리
            // ==================================================================
            else if (header.indexOf("GET /41/off") >= 0) {  // 브라우저에서 /41/off 주소를 요청했는지 확인합니다.

              Serial.println();  // 디버그 메시지를 보기 좋게 한 줄 내립니다.
              Serial.println("GPIO 41 OFF request received.");  // GPIO41 OFF 요청이 들어왔음을 출력합니다.

              output41State = "off";  // 웹 화면에 표시할 GPIO41 상태를 OFF로 저장합니다.
              digitalWrite(output41, LOW);  // 실제 GPIO41 핀을 LOW로 만들어 LED 또는 릴레이를 끕니다.
            }

            // ==================================================================
            // GPIO42 ON 요청 처리
            // ==================================================================
            else if (header.indexOf("GET /42/on") >= 0) {  // 브라우저에서 /42/on 주소를 요청했는지 확인합니다.

              Serial.println();  // 디버그 메시지를 보기 좋게 한 줄 내립니다.
              Serial.println("GPIO 42 ON request received.");  // GPIO42 ON 요청이 들어왔음을 출력합니다.

              output42State = "on";  // 웹 화면에 표시할 GPIO42 상태를 ON으로 저장합니다.
              digitalWrite(output42, HIGH);  // 실제 GPIO42 핀을 HIGH로 만들어 두 번째 출력 장치를 켭니다.
            }

            // ==================================================================
            // GPIO42 OFF 요청 처리
            // ==================================================================
            else if (header.indexOf("GET /42/off") >= 0) {  // 브라우저에서 /42/off 주소를 요청했는지 확인합니다.

              Serial.println();  // 디버그 메시지를 보기 좋게 한 줄 내립니다.
              Serial.println("GPIO 42 OFF request received.");  // GPIO42 OFF 요청이 들어왔음을 출력합니다.

              output42State = "off";  // 웹 화면에 표시할 GPIO42 상태를 OFF로 저장합니다.
              digitalWrite(output42, LOW);  // 실제 GPIO42 핀을 LOW로 만들어 두 번째 출력 장치를 끕니다.
            }

            // ==================================================================
            // HTTP 응답 헤더 전송
            // ==================================================================
            client.println("HTTP/1.1 200 OK");  // 브라우저에게 요청이 정상 처리되었음을 알려줍니다.
            client.println("Content-type:text/html");  // 응답 내용이 HTML 문서임을 알려줍니다.
            client.println("Connection: close");  // 응답 후 연결을 닫겠다고 알려줍니다.
            client.println();  // HTTP 헤더와 HTML 본문을 구분하기 위한 빈 줄입니다.

            // ==================================================================
            // HTML 웹 페이지 생성
            // ==================================================================
            client.println("<!DOCTYPE html><html>");  // HTML 문서 시작입니다.
            client.println("<head><meta name=\"viewport\" content=\"width=device-width, initial-scale=1\">");  // 모바일 화면에서도 보기 좋게 설정합니다.
            client.println("<link rel=\"icon\" href=\"data:,\">");  // favicon 요청으로 인한 불필요한 404 에러를 줄입니다.

            client.println("<style>");  // CSS 스타일 시작입니다.
            client.println("html { font-family: Helvetica; display: inline-block; margin: 0px auto; text-align: center;}");  // 전체 글꼴과 중앙 정렬을 설정합니다.
            client.println(".button { background-color: #4CAF50; border: none; color: white; padding: 16px 40px;");  // ON 버튼의 기본 색상과 크기를 설정합니다.
            client.println("text-decoration: none; font-size: 30px; margin: 2px; cursor: pointer;}");  // 버튼 글자 크기와 마우스 커서를 설정합니다.
            client.println(".button2 {background-color: #555555;}");  // OFF 버튼은 회색으로 표시되도록 설정합니다.
            client.println("</style></head>");  // CSS와 head 영역을 종료합니다.

            client.println("<body><h1>ESP32-S3 AP Web Server</h1>");  // 웹 페이지 제목을 출력합니다.

            // ==================================================================
            // GPIO41 상태와 버튼 표시
            // ==================================================================
            client.println("<p>GPIO 41 - State " + output41State + "</p>");  // 현재 GPIO41 상태를 웹 화면에 표시합니다.

            if (output41State == "off") {  // GPIO41이 꺼져 있으면 ON 버튼을 보여줍니다.
              client.println("<p><a href=\"/41/on\"><button class=\"button\">ON</button></a></p>");  // 누르면 /41/on 요청을 보내 GPIO41을 켭니다.
            } else {  // GPIO41이 켜져 있으면 OFF 버튼을 보여줍니다.
              client.println("<p><a href=\"/41/off\"><button class=\"button button2\">OFF</button></a></p>");  // 누르면 /41/off 요청을 보내 GPIO41을 끕니다.
            }

            // ==================================================================
            // GPIO42 상태와 버튼 표시
            // ==================================================================
            client.println("<p>GPIO 42 - State " + output42State + "</p>");  // 현재 GPIO42 상태를 웹 화면에 표시합니다.

            if (output42State == "off") {  // GPIO42가 꺼져 있으면 ON 버튼을 보여줍니다.
              client.println("<p><a href=\"/42/on\"><button class=\"button\">ON</button></a></p>");  // 누르면 /42/on 요청을 보내 GPIO42를 켭니다.
            } else {  // GPIO42가 켜져 있으면 OFF 버튼을 보여줍니다.
              client.println("<p><a href=\"/42/off\"><button class=\"button button2\">OFF</button></a></p>");  // 누르면 /42/off 요청을 보내 GPIO42를 끕니다.
            }

            client.println("</body></html>");  // HTML 문서를 종료합니다.

            client.println();  // HTTP 응답을 마무리하기 위한 빈 줄입니다.

            Serial.println();  // 시리얼 모니터 출력 정리를 위해 한 줄을 띄웁니다.
            Serial.println("HTML response sent to client.");  // 브라우저로 웹 페이지 응답을 보냈음을 출력합니다.

            break;  // HTTP 요청 처리가 끝났으므로 while 루프를 빠져나갑니다.
          }

          else {  // 현재 줄이 비어 있지 않다면 다음 줄을 읽기 위해 currentLine을 초기화합니다.
            currentLine = "";  // 다음 HTTP 헤더 줄을 받기 위해 현재 줄 버퍼를 비웁니다.
          }
        }

        else if (c != '\r') {  // 캐리지 리턴 문자가 아니라면 실제 문자이므로 현재 줄에 추가합니다.
          currentLine += c;  // HTTP 요청 한 줄을 완성하기 위해 문자를 계속 누적합니다.
        }
      }
    }

    header = "";  // 다음 브라우저 요청을 위해 저장된 HTTP 요청 내용을 초기화합니다.

    client.stop();  // 브라우저와의 연결을 종료합니다.

    Serial.println("Client disconnected.");  // 클라이언트 연결이 종료되었음을 출력합니다.
    Serial.println("------------------------------------");  // 다음 요청과 구분하기 위한 선을 출력합니다.
  }
}
