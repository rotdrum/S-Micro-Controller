void setup() {
  // put your setup code here, to run once:
  pinMode(8,OUTPUT);
}

void loop() {
  // put your main code here, to run repeatedly:
  digitalWrite(8,0);
  delay(100);
  digitalWrite(8,1);
  delay(100);
  
}
