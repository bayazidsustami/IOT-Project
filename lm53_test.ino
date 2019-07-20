const int analogIn = 34;
uint8_t LED_Pin = 2;

float vref = 3300;
float resolution = vref/1023;
//float resAkhir = resolution/25;
 
void setup(){
Serial.begin(9600);
pinMode(LED_Pin, OUTPUT);
}
 
void loop(){
 
  int temp = analogRead(analogIn); 
  float temperature = (float )temp*resolution; 
  temperature = temperature/10;
  Serial.print("Suhu :"); 
  Serial.print(temperature);
  Serial.print("\n");
  delay(1000);

  if (temperature == 0){
    digitalWrite(LED_Pin, HIGH);  // Turn the LED on  
    delay(50);     // Wait for a second  
    digitalWrite(LED_Pin, LOW); // Turn the LED off  
    delay(50);     // Wait for a second
  }
}
