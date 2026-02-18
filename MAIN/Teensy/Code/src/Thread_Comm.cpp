#include "AppContext.h"

#include <TeensyThreads.h>

namespace RoverApp {

void threadComm() {
  static String rxLine;
  rxLine.reserve(160);

  while (true) {
    while (Serial.available() > 0) {
      const char c = static_cast<char>(Serial.read());

      if (c == '\r') {
        continue;
      }

      if (c == '\n') {
        rxLine.trim();
        if (rxLine.length() > 0) {
          appLog(RoverConfig::LogLevel::DATA,
                 RoverConfig::LogTag::SYS,
                 String("RX,") + rxLine);

          if (rxLine.equalsIgnoreCase("HB")) {
            updateHeartbeat(millis());
            Serial.println("OK HB");
            appLog(RoverConfig::LogLevel::DATA, RoverConfig::LogTag::SYS, "TX,OK HB");
          } else if (rxLine.equalsIgnoreCase("STOP")) {
            requestEmergencyStop("STOP command");
            Serial.println("OK STOP");
            appLog(RoverConfig::LogLevel::WARN, RoverConfig::LogTag::SYS, "TX,OK STOP");
          } else {
            const auto result = gSequencer.enqueueFromLine(rxLine.c_str());
            if (result.ok) {
              Serial.print("OK Q=");
              Serial.println(result.enqueued);
              appLog(RoverConfig::LogLevel::DATA,
                     RoverConfig::LogTag::SYS,
                     String("TX,OK Q=") + result.enqueued);
            } else {
              Serial.print("ERR ");
              Serial.println(result.message ? result.message : "parse");
              appLog(RoverConfig::LogLevel::ERROR,
                     RoverConfig::LogTag::SYS,
                     String("TX,ERR,") + (result.message ? result.message : "parse"));
            }
          }
        }

        rxLine = "";
      } else {
        if (rxLine.length() < 158) {
          rxLine += c;
        }
      }
    }

    threads.delay(2);
  }
}

}  // namespace RoverApp
