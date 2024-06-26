int analogPin_1 = 34;  // KY-037 analog interface microphone 1
int analogPin_2 = 35;  // KY-037 analog interface microphone 2
int analogVal_1;       // Analog readings for microphone 1
int analogVal_2;       // Analog readings for microphone 2

void setup() {
  pinMode(analogPin_1, INPUT);
  pinMode(analogPin_2, INPUT);
  Serial.begin(300);
}

void loop() {
  //Read analog interface from microphones
  analogVal_1 = analogRead(analogPin_1);
  analogVal_2 = analogRead(analogPin_2);

  //Print analog value to serial of microphone 1
  Serial.print("Microphone 1: ");
  Serial.println(analogVal_1);
  if (analogVal_1 >= 105)
  Serial.println("Device is ready - Washing Machine");

  //Print analog value to serial of microphone 2
  Serial.print("Microphone 2: ");
  Serial.println(analogVal_2);
  if (analogVal_2 >= 120)
  Serial.println("Device is ready - Microwave Oven");
}