#include <TM1637Display.h>
#define CLK 5     //segment2
#define DIO 4     //segment2
#define CLK 3     //segment1
#define DIO 2     //segment1
#define TEST_DELAY   2000

//Variable Traffic 1
#define R1 6      //LED RED 1
#define Y1 7      //LED YELLOW 1
#define G1 8      //LED GREEN 1

//Variable Traffic 2
#define R2 9      //LED RED 2
#define Y2 10     //LED YELLOW 2
#define G2 11     //LED GREEN 2

TM1637Display display(CLK, DIO);

int counter1 = 30, counter2 = 25;
int flag = 1;

void setup()
{
  //test LED
  //for( int i=6; i<12; i++) {
  //  pinMode(i, OUTPUT);
  //  digitalWrite(i, 1);
  //}

  pinMode(R1, OUTPUT);
  pinMode(Y1, OUTPUT);
  pinMode(G1, OUTPUT);
  pinMode(R2, OUTPUT);
  pinMode(Y2, OUTPUT);
  pinMode(G2, OUTPUT);
  Serial.begin(9600);
}

void loop()
{
  if (flag == 1) {
    Serial.println("flag = 1");
    digitalWrite(R1, 1);
    digitalWrite(Y1, 0);
    digitalWrite(G1, 0);

    digitalWrite(R2, 0);
    digitalWrite(Y2, 0);
    digitalWrite(G2, 1);

    if (counter2 == 0) {
      Serial.println("flag = 1 || ready next loop");
      flag = 2;
      counter2 = 5;
    }
  }
  if (flag == 2) {
    Serial.println("flag = 2");
    digitalWrite(R1, 1);
    digitalWrite(Y1, 0);
    digitalWrite(G1, 0);

    digitalWrite(R2, 0);
    digitalWrite(Y2, 1);
    digitalWrite(G2, 0);

    if ( counter1 == 0 ) {
      Serial.println("flag = 2 || ready next loop");
      counter1 = 25;
      counter2 = 30;
      flag = 3;
    }
  }
  if (flag == 3) {
    Serial.println("flag = 3");
    digitalWrite(R1, 0);
    digitalWrite(Y1, 0);
    digitalWrite(G1, 1);

    digitalWrite(R2, 1);
    digitalWrite(Y2, 0);
    digitalWrite(G2, 0);

    if (counter1 == 0) {
      Serial.println("flag = 3 || ready next loop");
      flag = 4;
      counter1 = 5;
    }
  }
  if (flag == 4) {
    Serial.println("flag = 4");
    digitalWrite(R1, 0);
    digitalWrite(Y1, 1);
    digitalWrite(G1, 0);

    digitalWrite(R2, 1);
    digitalWrite(Y2, 0);
    digitalWrite(G2, 0);
    if ( counter1 == 0 ) {
      Serial.println("flag = 4 || ready next loop");
      flag = 1;
      counter1 = 30;
      counter2 = 25;
    }
  }


  counter1--;
  counter2--;
  delay(1000);
  showSegment1();
  showSegment2();
}
void showSegment1()
{
  TM1637Display display(3, 2);
  display.setBrightness(7);
  // Selectively set different digits
  uint8_t data[] = { 0xff, 0x7f, 0xff, 0xff };
  data[0] = display.encodeDigit(0);
  data[1] = display.encodeDigit(0) | 0x80;
  data[2] = display.encodeDigit((counter1 / 10) % 10);
  data[3] = display.encodeDigit(counter1 % 10);
  display.setSegments(data);
}

void showSegment2()
{
  TM1637Display display(5, 4);
  display.setBrightness(7);
  // Selectively set different digits
  uint8_t data[] = { 0xff, 0x7f, 0xff, 0xff };
  data[0] = display.encodeDigit(0);
  data[1] = display.encodeDigit(0) | 0x80;
  data[2] = display.encodeDigit((counter2 / 10) % 10);
  data[3] = display.encodeDigit(counter2 % 10);
  display.setSegments(data);
}
