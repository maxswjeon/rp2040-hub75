#include <SPI.h>
#include <WiFiNINA.h>
#include <String.h>

#include "LittleFS.h"

#define PANEL_WIDTH 64
#define PANEL_HEIGHT 32

#include "pincfg.h"
#include "Panel.h"
#include "Neodgm.h"
#include "html.hpp"

#define MAX_SSID_LENGTH 32
#define MAX_PSK_LENGTH 32

char wifi_ssid[MAX_SSID_LENGTH + 1] = "SKKU-Jonghyun";
char wifi_psk[MAX_PSK_LENGTH + 1] = "dhwhdgus*2";

WiFiServer server(80);
WiFiSSLClient client;

#define SSID_PREFIX "SKKU-"
static_assert(strlen(SSID_PREFIX) <= 7, "SSID_PREFIX must be less than 6 characters");

static const PanelConfig configs[4] = {
    {0, LEFT},
    {0, UP},
    {180, RIGHT},
    {180, NONE},
};

Panel *panel;

uint64_t lastblink = 0;
bool blink = false;
void setup1() {

}

void loop1() {
  if (time_us_64() - lastblink > 500 * 1000)
  {
    gpio_init(6);
    gpio_set_dir(6, GPIO_OUT);
    blink = !blink;
    gpio_put(6, blink);
    lastblink = time_us_64();
  }

  panel->tick();
}

void wifi_config() {
  char ap_ssid[17] = {0, };
  char ap_psk[17] = {0,};

  uint16_t ssid_postfix = rand() % 10000;
  sprintf(ap_ssid, "%s%04d", SSID_PREFIX, ssid_postfix);
  sprintf(ap_psk, "%08d", rand() % 100000000);

  panel->printString(0, 0, "Connect to WiFi");
  panel->printString(0, 16, "to configure.");
  panel->printString(0, 32, "SSID:");
  panel->printString(40, 32, ap_ssid);
  panel->printString(0, 48, "PSK: ");
  panel->printString(40, 48, ap_psk);

  pinMode(LEDR, OUTPUT);
  digitalWrite(LEDR, HIGH);

  if (WiFi.beginAP(ap_ssid, ap_psk) != WL_AP_LISTENING) {
    panel->clear();
    panel->printString(0, 0, "Failed to start");
    panel->printString(0, 16, "WiFi to start");
    panel->printString(0, 32, "onboarding");
    panel->printString(0, 48, "Please reset");    
    while (true);
  }

  server.begin();

  while (true) {
    WiFiClient client = server.available();

    if (!client) {
      continue;
    }
  }
}

void wifi_connect() {
  if (WiFi.begin(wifi_ssid, wifi_psk) != WL_CONNECTED) {
    panel->printString(0, 0, "Connect Failed", 0b100);
    while(true);
  }
}

#define STATION_INDEX 4

const char* station_name = 0;

void setup() {
  Serial.begin();

  // Initialize the Panel
  panel = new Panel(PANEL_WIDTH, PANEL_HEIGHT, 4, configs);
  panel->clear();
  panel->setFont(Neodgm, NeodgmKorean);
  wifi_connect();

  panel->clear();
  panel->printString(0, 0, L"정류소   정보");
  panel->printString(0, 16, L"로딩중...");

  if (!client.connect("www.skku.edu", 443)) {
    Serial.println("Failed to connect");
    panel->printString(0, 0, L"정류장  이름");
    panel->printString(0, 16, L"로딩실패");
    return;
  }

  client.println("GET /skku/popup/shuttlebus_popup.do?srKey=2009");
  client.println("Host: www.skku.edu");
  client.println("User-Agent: curl/7.81.0");
  client.println("Accept: */*");
  client.println();

  std::string s;

  char last[8] = {0,};
  char end[] = "</html>";

  while(true) {
    bool should_finish = false;
    while(client.available()) {
      should_finish = true;

      char c = client.read();
      Serial.print(c);
      s += c;

      last[0] = last[1];
      last[1] = last[2];
      last[2] = last[3];
      last[3] = last[4];
      last[4] = last[5];
      last[5] = last[6];
      last[6] = c;

      for (int i = 0; i < 7; ++i) {
        if (last[i] != end[i]) {
          should_finish = false;
          break;
        }
      }
      if (should_finish) {
        break;
      }
    }
    
    if (should_finish || !client.connected()) {
      client.stop();
      break;
    }
  }

  panel->clear();

  html::parser parser;
  html::node_ptr root = parser.parse(s);

  auto content = root->select(".bot-content-wrap");
  if (content.size() == 0) {
    panel->printString(0, 0, L"정류장  이름");
    panel->printString(0, 16, L"로딩실패");
    while (true);
  }
  Serial.print(content[0]->to_html().c_str());

  auto list_items = content[0]->select("li");
  if (content.size() < STATION_INDEX) {
    panel->printString(0, 0, L"정류장  이름");
    panel->printString(0, 16, L"로딩실패");
    while (true);
  }

  station_name = list_items[STATION_INDEX]->to_text().c_str();
}

struct {
  bool bus_at_station;
  int left;
  int index;
  std::string name;
} Car;

void loop() {
  if (!client.connect("www.skku.edu", 443)) {
    delay(1000);
    return;
  }

  client.println("GET /skku/popup/shuttlebus_popup.do?srKey=2009");
  client.println("Host: www.skku.edu");
  client.println("Connection: close");
  client.println();

  std::string s;

  while(true) {
    while(client.available()) {
      s += client.read();
    }

    if (!client.connected()) {
      client.stop();
      break;
    }
  }


  html::parser parser;
  html::node_ptr root = parser.parse(s);

  auto list = root->select(".bot-content-wrap")[0]->select("li");
  for (int i = 0; i < list.size() && i < STATION_INDEX; ++i) {

  }
}