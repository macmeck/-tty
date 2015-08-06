#include <SPI.h>
#include <Adafruit_GFX.h>
#include <Adafruit_PCD8544.h>

Adafruit_PCD8544 display = Adafruit_PCD8544(4, 5, 6, 7, 8);

const int MAX_BUTTONS = 6;
const int buttons[MAX_BUTTONS] = {A3, A2, A1, 12, 11, 10};

const int MAX_BUFFER = 64;
const int WIDTH = 14;
char buffer[MAX_BUFFER][WIDTH] = {};
int buffer_tail = 0;
int buffer_cursor = 0;
int scroll_position = 6;
bool freeze = false;


void setup()   {
    pinMode(3, OUTPUT);
    digitalWrite(3, HIGH);
    pinMode(2, OUTPUT);
    digitalWrite(2, HIGH);
    for (unsigned char button=0; button < MAX_BUTTONS; ++button) {
        pinMode(buttons[button], INPUT_PULLUP);
    }

    Serial.begin(115200);

    display.begin();
    display.setContrast(18);
    display.clearDisplay();

    display.setTextSize(0);
    display.setTextWrap(false);
    display.setTextColor(BLACK, WHITE);
}

void add_character(char c) {
    if (c == '\n' || buffer_cursor >= WIDTH) {
        buffer_cursor = 0;
        buffer_tail = (buffer_tail + 1) % MAX_BUFFER;
        for (int i=0; i < WIDTH; ++i) {
            buffer[buffer_tail][i] = '\0';
        }
    }
    if (c == '\n' || c == '\r') { return; };
    buffer[buffer_tail][buffer_cursor] = c;
    ++buffer_cursor;
}

void do_buttons() {
    static long last_buttons = 0;

    if (millis() - last_buttons < 300) { return; }

    for (unsigned char button=0; button<MAX_BUTTONS; ++button) {
        if (!digitalRead(buttons[button])) {
            switch (button) {
                case 0:
                    scroll_position = max(scroll_position - 1, 6);
                    break;
                case 1:
                    scroll_position = max(scroll_position - 6, 6);
                    break;
                case 2:
                    freeze = true;
                    break;
                case 3:
                    freeze = false;
                    break;
                case 4:
                    scroll_position = min(scroll_position + 6, MAX_BUFFER - 1);
                    break;
                case 5:
                    scroll_position = min(scroll_position + 1, MAX_BUFFER - 1);
                    break;
            }
            Serial.println(scroll_position);
            last_buttons = millis();
        }
    }
}

void display_buffer() {
    int line = (buffer_tail - scroll_position) % MAX_BUFFER;
    while (line < 0) { line += MAX_BUFFER; }

    display.setCursor(0, 0);
    for (int j=0; j < 6; ++j) {
        for (int i=0; i < WIDTH; ++i) {
            if (buffer[line][i]) {
                display.print(buffer[line][i]);
            } else {
                display.print(' ');
            }
        }
        display.println();
        line = (line + 1) % MAX_BUFFER;
    }
    display.display();
}

void loop() {
    do_buttons();
    if (!freeze) {
        while (Serial.available()) {
            add_character(Serial.read());
            do_buttons();
        }
    }
    display_buffer();
    delay(10);
}
