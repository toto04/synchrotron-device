#include <ArduinoOTA.h>
#include <ESP8266WiFi.h>
#include <WebSocketsClient.h>
#include <WiFiUdp.h>

#include "light_helpers.h"

WiFiUDP udp;
WebSocketsClient ws;

void broadcast() {
    IPAddress broadcast_address = WiFi.localIP();
    IPAddress subnet = WiFi.subnetMask();
    // get the boardcast address as local IP AND NOT subnet mask
    for (uint8_t i = 0; i < 4; i++)
        broadcast_address[i] = broadcast_address[i] | ~subnet[i];
    //broadcast the packet to the synch port
    udp.beginPacket(broadcast_address, SYNCHROTRON_PORT);
    udp.write("SYNCHROTRON CONNECT");
    udp.endPacket();
    Serial.print("Broadcasted discovery packet");
    Serial.println(broadcast_address);
}

void handle_packet() {
    Serial.println("Received response");
    // recieves a packet with 2 bytes representing the websocket port
    char packet[2];
    udp.read(packet, 2);
    uint16_t synch_port = (packet[0] << 8) + packet[1];

    Serial.print("Connecting to ws at:   ");
    Serial.print(udp.remoteIP());
    Serial.print(":");
    Serial.print(synch_port);
    Serial.println("/");
    // connects to the sending IP and the given port
    ws.begin(udp.remoteIP(), synch_port, "/", NAME);
}

void listener(WStype_t type, uint8_t* payload, size_t length) {
    switch (type) {
        case WStype_BIN:
            // Websocket packet contains the bytes in RGB order for each pixel
            for (size_t i = 0; i < TOTAL_LENGTH && i < length / 3; i++) {
                leds[i].r = payload[i * 3 + 0];
                leds[i].g = payload[i * 3 + 1];
                leds[i].b = payload[i * 3 + 2];
            }
            // Show when all the payload is cloned in the pixel array
            FastLED.show();
            break;

        case WStype_CONNECTED:
            Serial.println("WS Connected!");
            break;

        case WStype_DISCONNECTED:
            Serial.println("WS Disconnected!");
            break;

        default:
            break;
    }
}

void setup() {
    Serial.begin(9600);
    Serial.print("\n\nSynchrotron device  -  ");
    Serial.println(NAME);
    initializeLEDStrips();

    WiFi.persistent(false);
    WiFi.setAutoConnect(true);
    WiFi.setAutoReconnect(true);
    WiFi.mode(WIFI_STA);
    WiFi.begin(SSID, PASS);

    while (WiFi.status() != WL_CONNECTED) flash(139, 205, 255, 1);
    Serial.print("WiFi connected!   ");
    Serial.println(WiFi.localIP());

    ArduinoOTA.setHostname(NAME);
    ArduinoOTA.onStart([]() { FastLED.clear(); });
    ArduinoOTA.onProgress([](unsigned int progress, unsigned int total) {
        showCompletion((double)progress / (double)total);
    });
    ArduinoOTA.onEnd([]() { flash(139, 205, 150, TOTAL_LENGTH); });
    ArduinoOTA.onError([](ota_error_t error) { flash(0, 255, 150, TOTAL_LENGTH); });
    ArduinoOTA.begin();

    ws.onEvent(listener);
    ws.setReconnectInterval(5000);
    ws.enableHeartbeat(15000, 3000, 2);
    udp.begin(SYNCHROTRON_PORT);
    broadcast();
}

unsigned long upd_broadcast_elapsed = 0;
void loop() {
    ArduinoOTA.handle();
    ws.loop();
    if (!ws.isConnected()) {
        if (millis() - upd_broadcast_elapsed > 30 * 1000) {
            broadcast();
            upd_broadcast_elapsed = millis();
        }

        int packet_size = udp.parsePacket();
        if (packet_size) handle_packet();
    }
}
