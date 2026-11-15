const int potPin = A2;
const int ledPin = 9;

void setup()
{
  pinMode(ledPin, OUTPUT);
  Serial.begin(115200);
}

void loop()
{
  int value = analogRead(potPin);
  int brightness = map (value, 0, 1023, 0, 255);
  
  analogWrite(ledPin, brightness);
  
  delay(10);
}
