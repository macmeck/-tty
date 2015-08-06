#include <SPI.h>
#include <Adafruit_GFX.h>
#include <Adafruit_PCD8544.h>

Adafruit_PCD8544 display = Adafruit_PCD8544(4, 5, 6, 7, 8);

const int buttons[5] = {A3, A2, A1, 12, 10};

void setup()   {
    pinMode(3, OUTPUT);
    digitalWrite(3, HIGH);
    pinMode(2, OUTPUT);
    digitalWrite(2, HIGH);
    for (unsigned char button=0; button<5; ++button) {
        pinMode(buttons[button], INPUT_PULLUP);
    }

    Serial.begin(115200);

    display.begin();
    display.setContrast(18);
    display.clearDisplay();

    display.setTextSize(0);
    display.setTextWrap(true);
    display.setTextColor(BLACK, WHITE);
    display.println("Hello!");
}

void do_buttons() {
    static long last_buttons = 0;

    if (millis() - last_buttons < 300) { return; }

    for (unsigned char button=0; button<5; ++button) {
        if (!digitalRead(buttons[button])) {
            Serial.print("Button ");
            Serial.print(button);
            Serial.println();
            last_buttons = millis();
        }
    }
}

void loop() {
    do_buttons();
    while (Serial.available()) {
        display.print((char)Serial.read());
        do_buttons();
    }
    display.display();
    if (display.getCursorY() > display.height() - 5) {
        display.setCursor(0, 0);
        display.clearDisplay();
    }
    delay(10);
}
