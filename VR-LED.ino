
void setup() {
    pinMode(3,OUTPUT);
    pinMode(5,OUTPUT);
    pinMode(6,OUTPUT);
    pinMode(14,INPUT);
    Serial.begin(9600);
    
}

void loop() {
    int val1=255, val2=255, val3=255, val;
    int sum, res; 
    
    val = analogRead(A0);
    analogWrite(3,val);

        if(val <= 255) { // red to blue
        val1 = 0 + val;
        val3 = 255 - val;
        analogWrite(3,val1);
        analogWrite(5,val2);
        analogWrite(6,val3);

  
    }

    if(val >= 256) { // blue to green
        val3 = 0 + val;
        val2 = 255 - val;
        analogWrite(3,val1);
        analogWrite(5,val2);
        analogWrite(6,val3);
    }

    /*if(val > 511) { 
        analogWrite(3,255);
        analogWrite(5,0);
        analogWrite(6,255);
    }*/

    //red 255 - blue 255 - green 0
    if(val >= 510) {
        val3 = 255;
        res = val - 510;
        val2 = 0 + res;
        val1 = 255 - res;
        analogWrite(3,val1);
        analogWrite(5,val2);
        analogWrite(6,val3);
    }

    if(val >= 765) {
      res = val - 765 ;
      val1 = 0 +res ;
        analogWrite(3,0);
        analogWrite(5,255);
        analogWrite(6,255);
    }
  
    /*if(val >= 765) {
        val1 = 0;
        res = val - 765;
        val1 = 0 + res;
        analogWrite(3,val1);
        analogWrite(5,255);
        analogWrite(6,255);
    }

    if(val >= 1020) {
        analogWrite(3,0);
        analogWrite(5,0);
        analogWrite(6,0);
    }*/
        
    
    Serial.println(val);
    delay(10);
}
