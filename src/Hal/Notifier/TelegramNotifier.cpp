#include "TelegramNotifier.h"
#include <WiFiClientSecure.h>

String TelegramNotifier::_botToken = "";
String TelegramNotifier::_chatID = "";

void TelegramNotifier::begin(const String& botToken, const String& chatID) {
    _botToken = botToken;
    _chatID = chatID;
}

void TelegramNotifier::sendMessage(const String& msg) {
    if (_botToken.length() == 0 || _chatID.length() == 0)
        return;

    WiFiClientSecure client;
    client.setInsecure();  

    if (!client.connect("api.telegram.org", 443)) {
        return;
    }

    String url = "/bot" + _botToken + "/sendMessage?chat_id=" + _chatID + "&text=" + msg;

    client.println("GET " + url + " HTTP/1.1");
    client.println("Host: api.telegram.org");
    client.println("Connection: close");
    client.println();
}
