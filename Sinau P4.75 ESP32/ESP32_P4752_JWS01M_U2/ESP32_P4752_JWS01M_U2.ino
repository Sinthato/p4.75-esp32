#include <WiFi.h>
#include <WebServer.h>
#include <EEPROM.h>
#include <Wire.h>
#include "RTClib.h"
#include "PrayerTimes.h"

/**************** PIN CONFIGURATION ****************/
// HUB75 MATRIX
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

// 7-SEGMENT 4094
#define STR_7SEG 32  
#define DATA_7SEG 33 
#define CLK_7SEG 25  
#define Buzzer 13

/**************** VARIABLE KONTROL ****************/
RTC_DS3231 rtc;
WebServer server(80);
uint8_t buffer[HEIGHT][WIDTH];

int page = 1;
int stateAnim = 0;
int scrollX = WIDTH;
int scrollY = 0;
int wordIndex = 0;
String kataAktif = "";
unsigned long timerAnimasi = 0;
unsigned long timerTahan = 0;
unsigned long timerMenu4 = 0;
unsigned long timerMenu5 = 0;
unsigned long timerIqomah = 0; // Tambahkan baris ini

// Data JWS & Waktu
int hh, mm, ss, dd, bb, yy;
double waktu[sizeof(TimeName)/sizeof(char*)];
float Latitude, Longitude, GMT;
int hh_subuh, mm_subuh, hh_dzuhur, mm_dzuhur, hh_ashar, mm_ashar, hh_maghrib, mm_maghrib, hh_isya, mm_isya;
int iqomahMenit, iqomahDetik;
String namaSholatAktif = "";
bool isWebActive = false;
unsigned long webTimeout = 0;

/**************** FONT & DATA PESAN ****************/
String pesanRunning = "SELAMAT DATANG DI MICROBAN ELECTRONICS#";
String pesanRollingUp = "SINAU ESP32#"; 
String pesanRollingDown = "SEMANGAT BELAJAR#"; 

/**************** FONT DATA ****************/
// (Pastikan data font_alfabet dan font_angka tetap ada di sini tanpa akhiran 'h')
const uint8_t font_alfabet[26][14] = {
  {0xC0,0x01,0x80,0x01,0xBF,0x9F,0xBF,0x9F,0x80,0x01,0xC0,0x01,0xFF,0xFF}, // A
  {0x80,0x01,0x80,0x01,0xBE,0x7D,0xBE,0x7D,0x80,0x01,0xC1,0x83,0xFF,0xFF}, // B
  {0xC0,0x03,0x80,0x01,0x9F,0xF9,0xBF,0xFD,0x9F,0xF9,0xCF,0xF3,0xFF,0xFF}, // C
  {0x80,0x01,0x80,0x01,0xBF,0xFD,0xBF,0xFD,0x80,0x01,0xC0,0x03,0xFF,0xFF}, // D
  {0x80,0x01,0x80,0x01,0xBE,0x7D,0xBE,0x7D,0xBE,0x7D,0xBF,0xFD,0xFF,0xFF}, // E
  {0x80,0x01,0x80,0x01,0xBE,0x7F,0xBE,0x7F,0xBE,0x7F,0xBF,0xFF,0xFF,0xFF}, // F
  {0xC0,0x03,0x80,0x01,0x9F,0xF9,0xBE,0x7D,0xBE,0x3D,0xCE,0x03,0xFF,0xFF}, // G
  {0x80,0x01,0x80,0x01,0xFE,0x7F,0xFE,0x7F,0x80,0x01,0x80,0x01,0xFF,0xFF}, // H
  {0xBF,0xFD,0xBF,0xFD,0x80,0x01,0x80,0x01,0xBF,0xFD,0xBF,0xFD,0xFF,0xFF}, // I
  {0xFF,0xF7,0xBF,0xFB,0xBF,0xFD,0xBF,0xFD,0x80,0x01,0x80,0x03,0xFF,0xFF}, // J
  {0x80,0x01,0x80,0x01,0xF8,0x3F,0xE3,0x8F,0xCF,0xE3,0x9F,0xF9,0xFF,0xFF}, // K
  {0x80,0x01,0x80,0x01,0xFF,0xFD,0xFF,0xFD,0xFF,0xFD,0xFF,0xFD,0xFF,0xFF}, // L
  {0x80,0x01,0x9F,0xFF,0xE0,0xFF,0xE0,0xFF,0x9F,0xFF,0x80,0x01,0xFF,0xFF}, // M
  {0x80,0x01,0x80,0x01,0xF0,0x7F,0xFF,0x1F,0xFF,0x8F,0x80,0x01,0xFF,0xFF}, // N
  {0xC0,0x03,0x80,0x01,0xBF,0xFD,0xBF,0xFD,0x80,0x01,0xC0,0x03,0xFF,0xFF}, // O
  {0x80,0x01,0x80,0x01,0xBF,0xBF,0xBF,0xBF,0x80,0x3F,0x80,0x7F,0xFF,0xFF}, // P
  {0xC0,0x07,0x80,0x03,0xBF,0xFB,0xBF,0xE3,0x80,0x07,0xC0,0x09,0xFF,0xFF}, // Q
  {0x80,0x01,0x80,0x01,0xBE,0x3F,0xBE,0x3F,0x80,0x81,0xC1,0xC1,0xFF,0xFF}, // R
  {0xC1,0xF3,0x80,0xF1,0xBC,0x7D,0xBE,0x3D,0x8F,0x01,0xCF,0x83,0xFF,0xFF}, // S
  {0xBF,0xFF,0xBF,0xFF,0x80,0x01,0x80,0x01,0xBF,0xFF,0xBF,0xFF,0xFF,0xFF}, // T
  {0x80,0x03,0x80,0x01,0xFF,0xFD,0xFF,0xFD,0x80,0x01,0x80,0x03,0xFF,0xFF}, // U
  {0x80,0x0F,0x80,0x07,0xFF,0xF9,0xFF,0xF9,0x80,0x07,0x80,0x0F,0xFF,0xFF}, // V
  {0x80,0x03,0xFF,0xFD,0xFF,0x83,0xFF,0x83,0xFF,0xFD,0x80,0x03,0xFF,0xFF}, // W
  {0x83,0xC1,0x81,0x81,0xFE,0x7F,0xFE,0x7F,0x81,0x81,0x83,0xC1,0xFF,0xFF}, // X
  {0x80,0x0F,0x80,0x7F,0xFE,0x01,0xFE,0x01,0x80,0x7F,0x80,0x0F,0xFF,0xFF}, // Y
  {0xBF,0x81,0xBF,0x01,0xBE,0x7D,0xBC,0xFD,0x81,0xFD,0x81,0xFD,0xFF,0xFF}  // Z
};

const uint8_t font_angka[10][14] = {
  {0xC0,0x03,0x80,0x01,0xBF,0xFD,0xBF,0xFD,0x80,0x01,0xC0,0x03,0xFF,0xFF}, // 0
  {0xF7,0xF9,0xE7,0xF9,0x80,0x01,0x80,0x01,0xFF,0xF9,0xFF,0xF9,0xFF,0xFF}, // 1
  {0xC7,0x81,0x87,0x01,0xBE,0x39,0xBC,0x79,0x80,0xF9,0xC1,0xF9,0xFF,0xFF}, // 2
  {0xC7,0xE3,0x8E,0x71,0xBE,0x7D,0xBE,0x7D,0x88,0x11,0xC1,0x83,0xFF,0xFF}, // 3
  {0xC0,0xFF,0x80,0x7F,0xFE,0x7F,0xFE,0x7F,0x80,0x03,0xC0,0x01,0xFF,0xFF}, // 4
  {0x80,0xE3,0x80,0xF1,0x9C,0x7D,0x9E,0x7D,0x9E,0x01,0x9F,0x03,0xFF,0xFF}, // 5
  {0xC0,0x03,0x80,0x01,0x9C,0xFD,0xBC,0xFD,0xBC,0x01,0xBE,0x03,0xFF,0xFF}, // 6
  {0x9F,0xF1,0x9F,0xC1,0x9F,0x01,0x9C,0x7F,0x80,0xFF,0x83,0xFF,0xFF,0xFF}, // 7
  {0xC1,0x83,0x80,0x01,0xBE,0x7D,0xBE,0x7D,0x80,0x01,0xC1,0x83,0xFF,0xFF}, // 8
  {0xC1,0xF3,0x80,0x71,0xBE,0x7D,0xBE,0x7D,0x80,0x01,0xC0,0x03,0xFF,0xFF}  // 9
};


/**************** FUNGSI DRAWING & HELPER ****************/
void clearBuffer() { memset(buffer, 0, sizeof(buffer)); }

void drawChar(char c, int x, int y) {
  // --- LOGIKA KHUSUS TITIK DUA (:) ---
  if (c == ':') {
    // Jika panel Anda mematikan LED dengan 1, maka gunakan 0 untuk NYALA
    // Jika panel Anda mematikan LED dengan 0, maka gunakan 1 untuk NYALA
    
    int warnaNyala = 1; // GANTI KE 1 JIKA TETAP TIDAK TAMPIL

    // Titik Atas (Kotak 2x2)
    buffer[y + 4][x] = warnaNyala;     
    buffer[y + 4][x + 1] = warnaNyala; 
    buffer[y + 5][x] = warnaNyala;     
    buffer[y + 5][x + 1] = warnaNyala; 

    // Titik Bawah (Kotak 2x2)
    buffer[y + 10][x] = warnaNyala;    
    buffer[y + 10][x + 1] = warnaNyala;
    buffer[y + 11][x] = warnaNyala;    
    buffer[y + 11][x + 1] = warnaNyala;
    
    return; // Langsung keluar
  }

  const uint8_t* f = nullptr;
  if (c >= 'A' && c <= 'Z') f = font_alfabet[c - 'A'];
  else if (c >= '0' && c <= '9') f = font_angka[c - '0'];
  if (f == nullptr) return;

  for (int col = 0; col < 7; col++) {
    int cx = x + col;
    if (cx >= 0 && cx < WIDTH) {
      uint8_t a = f[col * 2];
      uint8_t b = f[col * 2 + 1];
      for (int bit = 0; bit < 8; bit++) {
        int cyA = y + bit;
        int cyB = y + bit + 8;
        if (cyA >= 0 && cyA < HEIGHT) buffer[cyA][cx] = !(a & (1 << (7 - bit)));
        if (cyB >= 0 && cyB < HEIGHT) buffer[cyB][cx] = !(b & (1 << (7 - bit)));
      }
    }
  }
}

void drawString(String t, int x, int y) {
  int currentX = x;
  for (int i = 0; i < t.length(); i++) {
    if (t[i] == '#') break;
    drawChar(t[i], currentX, y);
    
    // Jika karakter titik dua, geser sedikit saja (misal 4 pixel)
    // Jika huruf biasa, geser 8 pixel
    if (t[i] == ':') currentX += 4; 
    else currentX += 8;
  }
}

int getLengthNoHash(String t) {
  int l = 0;
  for (int i = 0; i < t.length(); i++) { if (t[i] == '#') break; l++; }
  return l;
}

String pecahKata(String sumber) {
  String hasil = "";
  while (wordIndex < sumber.length()) {
    char c = sumber[wordIndex];
    wordIndex++;
    if (c == ' ' || c == '#') {
      if (hasil.length() > 0) return hasil;
      if (c == '#') { wordIndex = 0; return "EOF"; }
      continue;
    }
    hasil += c;
  }
  wordIndex = 0; return "EOF";
}

/**************** LOGIKA JWS & 7-SEGMENT ****************/
byte angka_jam[10]={B10000001, B10110111, B11000010, B10010010, B10110100, B10011000, B10001000, B10110011, B10000000, B10010000};

void update7Segment() {
  byte buf[4];
  buf[0] = angka_jam[hh/10]; buf[1] = angka_jam[hh%10];
  buf[2] = angka_jam[mm/10]; buf[3] = angka_jam[mm%10];
  for (int i = 0; i < 4; i++) shiftOut(DATA_7SEG, CLK_7SEG, MSBFIRST, buf[i]);
  digitalWrite(STR_7SEG, HIGH); digitalWrite(STR_7SEG, LOW);
}

void updateJadwal() {
  set_calc_method(ISNA); set_asr_method(Shafii);
  get_prayer_times(yy, bb, dd, Latitude, Longitude, GMT, waktu);
  int j, m;
  get_float_time_parts(waktu[0], j, m); hh_subuh = j; mm_subuh = m + 2;
  get_float_time_parts(waktu[2], j, m); hh_dzuhur = j; mm_dzuhur = m + 2;
  get_float_time_parts(waktu[3], j, m); hh_ashar = j; mm_ashar = m + 2;
  get_float_time_parts(waktu[5], j, m); hh_maghrib = j; mm_maghrib = m + 2;
  get_float_time_parts(waktu[6], j, m); hh_isya = j; mm_isya = m + 2;
}

/**************** FUNGSI LAYOUT MANUAL JAM (MENU 4) ****************/
void layoutManualJam() {
  // Format: drawChar('Karakter', Kolom, Baris);
  
  // Menggambar "04:00" secara presisi
  drawChar('0', 30, 0);   // Angka 0 di kolom 30
  drawChar('4', 38, 0);   // Angka 4 di kolom 38 (jarak 8 pixel)
  
  drawChar(':', 46, 0);   // Titik dua di kolom 46 (jarak 8 pixel)
  
  drawChar('0', 51, 0);   // Angka 0 pertama di kolom 51 (hanya jarak 5 pixel dari :)
  drawChar('0', 59, 0);   // Angka 0 kedua di kolom 59 (jarak 8 pixel)

  // Opsional: Tambahkan keterangan teks di bawah atau di sampingnya
  // drawString("SUBUH", 75, 0); 
}

/**************** WEB SERVER ****************/
void handlePesan() {
  isWebActive = true; webTimeout = millis();
  String pesan = server.arg(0);
  // [Logika parsing pesan 'a','b','c' Anda di sini untuk simpan EEPROM]
  server.send(200, "text/plain", "SETTING BERHASIL");
}

/**************** HUB75 SCANNING ****************/
byte scanOrder[8] = {1, 3, 5, 7, 0, 2, 4, 6};
void scanLayer(bool bawah) {
  for (int i = 0; i < 8; i++) {
    int row = scanOrder[i];

    
    // 1. MATIKAN DISPLAY DULU (Sangat Penting)
    digitalWrite(OE, HIGH); 
    
    // 2. GANTI ALAMAT BARIS (A, B, C)
    digitalWrite(A, row & 1); 
    digitalWrite(B, (row >> 1) & 1); 
    digitalWrite(C, (row >> 2) & 1);
    digitalWrite(DD, bawah);

    // 3. KIRIM DATA KE SHIFT REGISTER
    for (int col = 0; col < WIDTH; col++) {
      digitalWrite(DATA_KIRI, buffer[row + (bawah ? 8 : 0)][col]);
      digitalWrite(DATA_KANAN, buffer[row + (bawah ? 8 : 0)][col]);
      digitalWrite(CLK, HIGH); 
      digitalWrite(CLK, LOW);
    }

    // 4. LATCH DATA
    digitalWrite(LAT, HIGH); 
    delayMicroseconds(2); 
    digitalWrite(LAT, LOW);

    // 5. NYALAKAN KEMBALI SEBENTAR
    digitalWrite(OE, LOW); 
    delayMicroseconds(100); // Jika terlalu lama, baris akan sangat terang
  }
  digitalWrite(OE, HIGH); // Matikan setelah selesai loop
}

/**************** MAIN SETUP & LOOP ****************/
void setup() {
  pinMode(STR_7SEG, OUTPUT); pinMode(DATA_7SEG, OUTPUT); pinMode(CLK_7SEG, OUTPUT);
  pinMode(Buzzer, OUTPUT);
  pinMode(DATA_KIRI, OUTPUT); pinMode(DATA_KANAN, OUTPUT);
  pinMode(CLK, OUTPUT); pinMode(LAT, OUTPUT); pinMode(OE, OUTPUT);
  pinMode(A, OUTPUT); pinMode(B, OUTPUT); pinMode(C, OUTPUT); pinMode(DD, OUTPUT);
  digitalWrite(OE, HIGH);

  Wire.begin(); rtc.begin(); EEPROM.begin(255);
  WiFi.softAP("JWS_ESP32", "12345678");
  server.on("/pesan", handlePesan); server.begin();

  // 3. TAMPILKAN "RESTART" STATIC (Sebelum WiFi & Animasi Jalan)
  clearBuffer();
  // Menghitung posisi center: (128 - (7 huruf * 8 pixel)) / 2 = 36
  drawString("RE:START", 36, 0); 
  
  // Lakukan scanning manual beberapa kali agar tulisan terlihat
  for(int i=0; i<500; i++) {
    scanLayer(true);
    scanLayer(false);
  }
  
  // Baca setting EEPROM (Latitude, Longitude, dll)
  EEPROM.get(0, Latitude); EEPROM.get(10, Longitude); EEPROM.get(20, GMT);
}

void loop() {
  server.handleClient();
  
  // Update Waktu & Jadwal (Setiap Detik)
  static unsigned long tDetik = 0;
  if (millis() - tDetik > 1000) {
    tDetik = millis();
    DateTime now = rtc.now();
    hh = now.hour(); mm = now.minute(); ss = now.second();
    dd = now.day(); bb = now.month(); yy = now.year();
    update7Segment();
    
    // Cek Waktu Sholat di detik 0
    if (ss == 0 && page < 10) {
      updateJadwal();
      int mIq = 0; String nSh = "";
      if (hh == hh_subuh && mm == mm_subuh) { nSh="SUBUH"; mIq=EEPROM.read(30); }
      else if (hh == hh_dzuhur && mm == mm_dzuhur) { nSh="DZUHUR"; mIq=EEPROM.read(31); }
      // ... tambahkan ashar, maghrib, isya ...
      
      if (mIq > 0) { 
        namaSholatAktif = nSh; iqomahMenit = mIq; iqomahDetik = 0;
        page = 10; timerMenu4 = millis(); digitalWrite(Buzzer, HIGH); 
      }
    }
  }

  // Jika sedang di-setting via Web, Matrix mati sejenak
  if (isWebActive) {
    digitalWrite(OE, HIGH);
    if (millis() - webTimeout > 3000) isWebActive = false;
    return;
  }

  // Animasi Matrix (Non-Blocking)
  if (millis() - timerAnimasi > 30) {
    timerAnimasi = millis();
    clearBuffer();

    // --- ANIMASI ASLI ANDA ---
    if (page == 1) {
      drawString(pesanRunning, scrollX, 0);
      scrollX--; if (scrollX < (getLengthNoHash(pesanRunning) * -8)) {
        page = 2; wordIndex = 0; kataAktif = pecahKata(pesanRollingUp); scrollY = 16; stateAnim = 0;
      }
    } 
    else if (page == 2) {
      int centerX = (WIDTH / 2) - (kataAktif.length() * 4);
      if (stateAnim == 0) {
        drawString(kataAktif, centerX, scrollY);
        scrollY--; if (scrollY <= 0) { scrollY = 0; stateAnim = 1; timerTahan = millis(); }
      } else if (stateAnim == 1) {
        drawString(kataAktif, centerX, 0);
        if (millis() - timerTahan > 1000) stateAnim = 2;
      } else if (stateAnim == 2) {
        drawString(kataAktif, centerX, scrollY);
        scrollY--; if (scrollY < -16) {
          kataAktif = pecahKata(pesanRollingUp);
          if (kataAktif == "EOF") { page = 3; wordIndex = 0; kataAktif = pecahKata(pesanRollingDown); scrollY = -16; stateAnim = 0; } 
          else { scrollY = 16; stateAnim = 0; }
        }
      }
    }
    else if (page == 3) {
      int centerX = (WIDTH / 2) - (kataAktif.length() * 4);
      if (stateAnim == 0) {
        drawString(kataAktif, centerX, scrollY);
        scrollY++; if (scrollY >= 0) { scrollY = 0; stateAnim = 1; timerTahan = millis(); }
      } else if (stateAnim == 1) {
        drawString(kataAktif, centerX, 0);
        if (millis() - timerTahan > 1000) stateAnim = 2;
      } else if (stateAnim == 2) {
        drawString(kataAktif, centerX, scrollY);
        scrollY++; if (scrollY > 16) {
          kataAktif = pecahKata(pesanRollingDown);
          if (kataAktif == "EOF") { page = 4; timerMenu4 = millis(); } 
          else { scrollY = -16; stateAnim = 0; }
        }
      }
    }
    else if (page == 4) { 
  clearBuffer();         // Bersihkan layar sebelum menggambar manual
  layoutManualJam();     // Panggil fungsi gambar per kolom tadi
  
  // Berikan durasi tampilan 5 detik sebelum kembali ke Running Text
  if (millis() - timerMenu4 > 5000) { 
    page = 5; 
    scrollX = WIDTH; 
    timerMenu5 = millis(); // RESET TIMER HANYA SEKALI DI SINI
  }
}

  else if (page == 5) {
  clearBuffer();
  
  // Ambil data jam dan menit dari RTC
  String jamS = (hh < 10 ? "0" : "") + String(hh);
  String minS = (mm < 10 ? "0" : "") + String(mm);

  // Gambar secara manual agar presisi di tengah
  drawChar(jamS[0], 42, 0);   // Puluhan Jam
  drawChar(jamS[1], 50, 0);   // Satuan Jam
  
  // Titik dua berkedip tiap detik
  if (ss % 2 == 0) {
    drawChar(':', 59, 0); 
  }
  
  drawChar(minS[0], 63, 0);   // Puluhan Menit
  drawChar(minS[1], 71, 0);   // Satuan Menit

  // Setelah 5 detik, pindah ke page berikutnya (misal page 2 atau kembali ke 1)
  if (millis() - timerMenu5 > 5000) {
    page = 1; // Kembali ke Running Text
    scrollX = WIDTH;
  }
}

    // --- STATE KHUSUS JWS (INTERRUPT) ---
    else if (page == 10) { // TAMPIL NAMA SHOLAT
      drawString("WAKTU " + namaSholatAktif, 10, 0);
      if (millis() - timerMenu4 > 2000) digitalWrite(Buzzer, LOW);
      if (millis() - timerMenu4 > 180000) { page = 11; timerIqomah = millis(); } // 3 menit
    }
    else if (page == 11) { // IQOMAH COUNTDOWN
      String disp = "IQOMAH " + String(iqomahMenit) + ":" + (iqomahDetik < 10 ? "0" : "") + String(iqomahDetik);
      drawString(disp, 10, 0);
      if (millis() - timerIqomah > 1000) {
        timerIqomah = millis();
        if (iqomahDetik == 0) {
          if (iqomahMenit > 0) { iqomahMenit--; iqomahDetik = 59; }
          else { page = 12; timerMenu4 = millis(); digitalWrite(Buzzer, HIGH); }
        } else iqomahDetik--;
      }
    }
    else if (page == 12) { // STANDBY / MATI
      clearBuffer(); // Kosongkan layar
      if (millis() - timerMenu4 > 2000) digitalWrite(Buzzer, LOW);
      int durasiMati = EEPROM.read(37); // Baca setting 'ist'
      if (millis() - timerMenu4 > (durasiMati * 60000)) { page = 1; scrollX = WIDTH; }
    }
  }

  scanLayer(true); scanLayer(false);
}