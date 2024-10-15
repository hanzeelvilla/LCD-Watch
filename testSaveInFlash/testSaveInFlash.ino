#include <Preferences.h>

Preferences preferences;

void setup() {
  Serial.begin(115200);
  /*
  true = read-only
  false = read/write
  */
  preferences.begin("student", false);
  preferences.clear();
  Serial.println("namespace cleared");

  preferences.putString("name", "Hanzeel");
  preferences.putInt("age", 20);
  
  Serial.println(preferences.getString("name", "Unknown"));
  Serial.println(preferences.getInt("age", -1));
  preferences.end();
}

void loop() {
}
