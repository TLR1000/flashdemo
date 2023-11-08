#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 32
#define OLED_RESET -1  // Reset pin not used with I2C

Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

const int potPin = A0;    // Connect potentiometer pin to A0 on NodeMCU
const int trigPin = D4;   // Connect HC-SR04 trig pin to D4 on NodeMCU
const int echoPin = D5;   // Connect HC-SR04 echo pin to D5 on NodeMCU
const int flashPin = D6;  // Connect Tomar power supply trigger pin to D6 on NodeMCU

int flashTriggerPulseDuration = 50;  // Initial flash trigger pulse duration in milliseconds. Tomer requires min 25 ms.
int flashTriggerValue = 50;          // Initial flash trigger value in cm

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
  pinMode(trigPin, OUTPUT);
  pinMode(echoPin, INPUT);

  // Configure the flash pin as an output and set it low to deactivate the relay by default
  pinMode(flashPin, OUTPUT);
  digitalWrite(flashPin, HIGH);

  display.clearDisplay();
  display.display();
}

void loop() {
  // Read the potentiometer value to set the flash trigger value
  int potValue = analogRead(potPin);
  flashTriggerValue = map(potValue, 0, 1023, 50, 400);

  // Measure distance using HC-SR04
  long duration;
  float distance;
  int roundedDistance;

  digitalWrite(trigPin, LOW);
  delayMicroseconds(2);
  digitalWrite(trigPin, HIGH);
  delayMicroseconds(10);
  digitalWrite(trigPin, LOW);
  duration = pulseIn(echoPin, HIGH);
  distance = (duration / 2.0) / 29.1;  // Calculate distance in centimeters
  roundedDistance = round(distance);

  // Print both potentiometer and distance values to the OLED screen
  display.clearDisplay();
  display.setCursor(0, 0);
  display.print("Trigger: ");
  display.print(potValue);
  display.println(" cm");
  display.print("Distance: ");
  display.print(roundedDistance);
  display.println(" cm");
  display.display();

  // Print both potentiometer and distance values to the serial monitor
  Serial.print("Trigger: ");
  Serial.println(potValue);
    Serial.println(" cm");
  Serial.print("Distance: ");
  Serial.print(roundedDistance);
  Serial.println(" cm");

  // Check if the measured distance is less than or equal to the flash trigger value
  if (roundedDistance <= flashTriggerValue) {
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

  // Set the flash pin high to activate the relay
  digitalWrite(flashPin, LOW);

  delay(flashTriggerPulseDuration);  // Delay for the specified pulse duration

  // Set the flash pin back to low to deactivate the relay
  digitalWrite(flashPin, HIGH);

  display.clearDisplay();
  display.display();
}