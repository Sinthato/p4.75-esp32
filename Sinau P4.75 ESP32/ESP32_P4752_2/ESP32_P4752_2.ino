/**************** PIN ****************/
#define DATA_KIRI 23      // panel 1
#define DATA_KANAN 19      // panel 2 (lewat chain)
#define CLK 18
#define LAT 5
#define OE 4
#define A 15
#define B 2
#define C 16
#define DD 17

#define WIDTH 128
#define HEIGHT 16

uint8_t buffer[HEIGHT][WIDTH];

/*************** FONT TEST ***************/
const uint8_t font_A[14] = {
0xC0,0x01,
0x80,0x01,
0xBF,0x9F,
0xBF,0x9F,
0x80,0x01,
0xC0,0x01,
0xFF,0xFF
};

/*************** BUFFER *****************/
void clearBuffer(){
  for(int y=0;y<HEIGHT;y++)
    for(int x=0;x<WIDTH;x++)
      buffer[y][x]=0;
}

/*************** TAMPIL HURUF ***********/
void tampilHuruf(const uint8_t *font,int xPos){

  for(int col=0; col<7; col++){

    uint8_t atas  = font[col*2];
    uint8_t bawah = font[col*2+1];

    for(int bit=0; bit<8; bit++){

      bool pxAtas  = !(atas  & (1 << (7-bit)));
      bool pxBawah = !(bawah & (1 << (7-bit)));

      buffer[bit][xPos+col]   = pxAtas;
      buffer[bit+8][xPos+col] = pxBawah;
    }
  }
}

/*************** SCAN ORDER *************/
byte scanOrder[8] = {1,3,5,7,0,2,4,6};

void scanLayer(bool bawah){

  digitalWrite(DD,bawah);

  for(int i=0;i<8;i++){

    int row = scanOrder[i];

    // MATIKAN LED TOTAL SEBELUM GANTI ROW
    digitalWrite(OE,HIGH);
    delayMicroseconds(20);

    digitalWrite(A,row & 1);
    digitalWrite(B,(row>>1)&1);
    digitalWrite(C,(row>>2)&1);

    for(int col=0; col<128; col++){

      int realRow = row + (bawah ? 8 : 0);
      bool pixel = buffer[realRow][col];

      digitalWrite(DATA_KIRI, pixel);
      digitalWrite(DATA_KANAN, pixel);

      digitalWrite(CLK,HIGH);
      digitalWrite(CLK,LOW);
    }

    digitalWrite(LAT,HIGH);
    delayMicroseconds(5);
    digitalWrite(LAT,LOW);

    // HIDUPKAN LED SETELAH DATA STABIL
    digitalWrite(OE,LOW);

    delayMicroseconds(100);  // stabilisasi antar row
  }
}

void refreshPanel(){
  scanLayer(true);   // bawah
  scanLayer(false);  // atas
}

/*************** SETUP *****************/
void setup(){

  pinMode(DATA_KIRI,OUTPUT);
  pinMode(DATA_KANAN,OUTPUT);
  pinMode(CLK,OUTPUT);
  pinMode(LAT,OUTPUT);
  pinMode(OE,OUTPUT);
  pinMode(A,OUTPUT);
  pinMode(B,OUTPUT);
  pinMode(C,OUTPUT);
  pinMode(DD,OUTPUT);

  digitalWrite(OE,HIGH);
  delay(10);
  digitalWrite(OE,LOW);

  clearBuffer();

  // tampil huruf A di tengah
  tampilHuruf(font_A,60);
}

/*************** LOOP *****************/
void loop(){
  refreshPanel();
};