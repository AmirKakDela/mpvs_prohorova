// Подключение библиотек
#include <SPI.h>
#include <MFRC522.h>
#include <Servo.h>

Servo microservo9g;

#define SS_PIN 10
#define RST_PIN 9
// Создаем экземпляр MFRC522
MFRC522 mfrc522(SS_PIN, RST_PIN);

// Светодиодные индикаторы, указывающие разрешен или запрещен доступ
int led_submit = 5;
int led_cancel = 6;

int row_excel = 0; // количество строк

char st[20];

void setup() {
    pinMode(led_submit, OUTPUT);
    pinMode(led_cancel, OUTPUT);
    pinMode(5, OUTPUT);
    // Подключение сервопривода на выходу 3
    microservo9g.attach(3);
    // Устанавливаем сервопривод в исходное положение
    microservo9g.write(90);
    // Инициализация Serial
    Serial.begin(9600);
    // Инициализация шини SPI
    SPI.begin();
    // Инициализация MFRC522
    mfrc522.PCD_Init();
    Serial.println("CLEARDATA");         // очистка листа excel
    Serial.println("LABEL,Time,Code,Status");   // заголовки столбцов
    Serial.println();
}

void loop() {
    row_excel++; // номер строки + 1
    // Ищем новые карты:
    if (!mfrc522.PICC_IsNewCardPresent()) {
        return;
    }
    // Выбираем одну из карт:
    if (!mfrc522.PICC_ReadCardSerial()) {
        return;
    }
    Serial.print("DATA,TIME,"); // запись в excel текущей даты и времени
    String content= "";
    byte letter;
    for (byte i = 0; i < mfrc522.uid.size; i++) {
        Serial.print(mfrc522.uid.uidByte[i] < 0x10 ? " 0" : " ");
        Serial.print(mfrc522.uid.uidByte[i], HEX);
        content.concat(String(mfrc522.uid.uidByte[i] < 0x10 ? " 0" : " "));
        content.concat(String(mfrc522.uid.uidByte[i], HEX));
    }
      content.toUpperCase();
      Serial.print(',');

    if (content.substring(1) == "BA 1D 11 B1") {
        // Поднимаем шлагбаум и включаем зеленый светодиодный индикатор
        microservo9g.write(-90);
        // Включаем светодиод
        digitalWrite(led_submit, HIGH);
        Serial.print(true);
        Serial.println();
        delay(3000);
        // Устанавливаем шлагбаум в исходное положение
        microservo9g.write(90);
        // Выключаем светодиод
        digitalWrite(led_submit, LOW);
    } else {
//        Serial.println("Карта2 - Доступ запрещен!");
        Serial.print(false);
        Serial.println();
        // Включаем мигающий красный светодиодный индикатор
        for (int i= 1; i<5 ; i++) {
            digitalWrite(led_cancel, HIGH);
            delay(200);
            digitalWrite(led_cancel, LOW);
            delay(200);
        }
    }
    delay(1000);
}