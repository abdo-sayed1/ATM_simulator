#ifndef TELEGRAM_NOTIFIER_H
#define TELEGRAM_NOTIFIER_H

#include <Arduino.h>

class TelegramNotifier {
public:
    static void begin(const String& botToken, const String& chatID);
    static void sendMessage(const String& msg);

private:
    static String _botToken;
    static String _chatID;
};

#endif
