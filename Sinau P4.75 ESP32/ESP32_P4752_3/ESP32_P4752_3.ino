/**************** PIN ****************/
#define DATA_KIRI 23      
#define DATA_KANAN 19     
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
  0xC0, 0x01, 0x80, 0x01, 0xBF, 0x9F, 0xBF, 0x9F, 0x80, 0x01, 0xC0, 0x01, 0xFF, 0xFF
};

/*************** BUFFER *****************/
void clearBuffer() {
  memset(buffer, 0, sizeof(buffer));
}

void tampilHuruf(const uint8_t *font, int xPos) {
  for (int col = 0; col < 7; col++) {
    uint8_t atas  = font[col * 2];
    uint8_t bawah = font[col * 2 + 1];
    for (int bit = 0; bit < 8; bit++) {
      if (xPos + col < WIDTH) {
        buffer[bit][xPos + col]   = !(atas  & (1 << (7 - bit)));
        buffer[bit + 8][xPos + col] = !(bawah & (1 << (7 - bit)));
      }
    }
  }
}

/*************** SCAN LAYER *************/
// Gunakan urutan linear jika memungkinkan untuk kestabilan sinyal
byte scanOrder[8] = {1, 3, 5, 7, 0, 2, 4, 6};

void scanLayer(bool bawah) {
  digitalWrite(DD, bawah);

  for (int i = 0; i < 8; i++) {
    int row = scanOrder[i];

    // 1. MATIKAN OUTPUT (Mencegah baris "numpuk" / Ghosting)
    digitalWrite(OE, HIGH);

    // 2. PINDAH ALAMAT BARIS
    digitalWrite(A, row & 1);
    digitalWrite(B, (row >> 1) & 1);
    digitalWrite(C, (row >> 2) & 1);

    // 3. KIRIM DATA KE REGISTER (Tanpa delay untuk Refresh Rate tinggi)
    int targetRow = row + (bawah ? 8 : 0);
    for (int col = 0; col < 128; col++) {
      digitalWrite(DATA_KIRI, buffer[targetRow][col]);
      digitalWrite(DATA_KANAN, buffer[targetRow][col]);

      // Clock super cepat (ESP32 digitalWrite sudah cukup cepat)
      digitalWrite(CLK, HIGH);
      digitalWrite(CLK, LOW);
    }

    // 4. LATCHING (Bagian krusial untuk Panel 2)
    // Beri sedikit jeda agar bit terakhir sampai ke panel ujung
    delayMicroseconds(1); 
    digitalWrite(LAT, HIGH);
    delayMicroseconds(1); 
    digitalWrite(LAT, LOW);

    // 5. NYALAKAN LED (Durasi disingkat agar refresh rate naik)
    digitalWrite(OE, LOW);
    
    // Kurangi angka ini jika panel 2 masih kedip (mencoba mode redup dulu)
    delayMicroseconds(60); 

    // 6. MATIKAN LAGI SEBELUM PINDAH
    digitalWrite(OE, HIGH);
  }
}

void refreshPanel() {
  // Scan bergantian blok bawah dan atas
  scanLayer(true);  
  scanLayer(false); 
}

/*************** SETUP *****************/
void setup() {
  pinMode(DATA_KIRI, OUTPUT);
  pinMode(DATA_KANAN, OUTPUT);
  pinMode(CLK, OUTPUT);
  pinMode(LAT, OUTPUT);
  pinMode(OE, OUTPUT);
  pinMode(A, OUTPUT);
  pinMode(B, OUTPUT);
  pinMode(C, OUTPUT);
  pinMode(DD, OUTPUT);

  digitalWrite(OE, HIGH); 
  digitalWrite(LAT, LOW);
  digitalWrite(CLK, LOW);

  clearBuffer();
  tampilHuruf(font_A, 60); // Huruf A di tengah
}

/*************** LOOP *****************/
void loop() {
  refreshPanel();
}