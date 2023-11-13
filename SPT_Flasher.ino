#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 32
#define OLED_RESET -1  // Reset pin not used with I2C

Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

const int potPin = A0;  // Connect potentiometer pin to A0 on NodeMCU
const int us_trigPin = D6;  // Connect HC-SR04 trig pin / Red
const int us_echoPin = D5;  // Connect HC-SR04 echo pin / Green
const int flash_Pin = D4;   // Connect Tomar power supply trigger pin / Green

int flash_TriggerPulseDuration = 30;  // Initial flash trigger pulse duration in milliseconds. Tomer requires min 25 ms.
int flash_TriggerValue = 10;          // Initial flash trigger value in cm
int flash_ChargePumpDelay = 500;      // ms needed for recharging the lamp

void setup() {
  // Initialize the OLED screen
  if (!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) {
    Serial.println(F("SSD1306 allocation failed"));
    for (;;)
      ;  // Don't proceed, loop forever
  }
  display.setTextSize(1);
  display.setTextColor(SSD1306_WHITE);
  display.clearDisplay();
  display.display();
  display.println(F("Distance: "));
  display.display();
  display.setTextSize(1);

  // Initialize serial communication
  Serial.begin(9600);

  // Initialize the HC-SR04 pins
  pinMode(us_trigPin, OUTPUT);
  pinMode(us_echoPin, INPUT);

  // Configure the flash pin as an output and set it low to deactivate the relay by default
  pinMode(flash_Pin, OUTPUT);
  digitalWrite(flash_Pin, LOW);  //pin is high by default

  display.clearDisplay();
  display.display();
}

void loop() {
  // Read the potentiometer value to set the flash trigger value
  int potTriggerValue = analogRead(potPin);
  flash_TriggerValue = map(potTriggerValue, 0, 1023, 50, 400);

  // Measure distance using HC-SR04
  long us_duration;
  float us_distance;
  int roundedDistance;

  digitalWrite(us_trigPin, LOW);
  delayMicroseconds(2);
  digitalWrite(us_trigPin, HIGH);
  delayMicroseconds(10);
  digitalWrite(us_trigPin, LOW);
  us_duration = pulseIn(us_echoPin, HIGH);
  us_distance = (us_duration / 2.0) / 29.1;  // Calculate distance in centimeters
  roundedDistance = round(us_distance);

  // Print both potentiometer and distance values to the OLED screen
  display.clearDisplay();
  display.setCursor(0, 0);
  display.print("Trigger: ");
  display.print(potTriggerValue);
  display.println(" cm");
  display.print("Distance: ");
  display.print(roundedDistance);
  display.println(" cm");
  display.display();

  // Print both potentiometer and distance values to the serial monitor
  Serial.print("Trigger: ");
  Serial.println(potTriggerValue);
  Serial.println(" cm");
  Serial.print("Distance: ");
  Serial.print(roundedDistance);
  Serial.println(" cm");

  // Check if the measured distance is less than or equal to the flash trigger value
  if (roundedDistance <= flash_TriggerValue) {
    flash();  // Call the flash function
  }

  delay(100);  // Add a delay to control the update rate
}

void flash() {
  Serial.println("FLASH");  // Print 'FLASH' to the serial monitor
  display.clearDisplay();
  display.setCursor(0, 0);
  display.println("FLASH");  // Print 'FLASH' to the OLED screen
  display.display();

  //Trigger the flash
  digitalWrite(flash_Pin, HIGH);      // Set the flash pin
  delay(flash_TriggerPulseDuration);  // Delay for the specified pulse duration
  digitalWrite(flash_Pin, LOW);       // Set the flash pin

  delay(flash_ChargePumpDelay);  // Delay for recharge and return

  display.clearDisplay();
  display.display();
}
