# 🕰️ JWS-P4.75-HUB08: Revitalisasi Font Legacy 2013 ke ESP32

Sistem **Jadwal Waktu Sholat (JWS)** berbasis panel LED Matrix P4.75 dengan interface HUB08. Proyek ini menghidupkan kembali font 16x16 yang dirancang tahun 2013 dengan Assembly, kini berjalan di atas framework **ESP32 modern**.

---

## 📋 Daftar Isi

- [Deskripsi Proyek](#deskripsi-proyek)
- [Fitur Teknis](#fitur-teknis)
- [Pemetaan Pin](#pemetaan-pin)
- [Struktur Font](#struktur-font)
- [Instalasi](#instalasi)
- [Penggunaan](#penggunaan)
- [Tantangan & Solusi](#tantangan--solusi)

---

## 📖 Deskripsi Proyek

Proyek ini merupakan **modernisasi sistem legacy** yang mengkonversi font binary dari era Assembly ke C++ untuk embedded system ESP32. Panel LED Matrix P4.75 dengan HUB08 interface memberikan display berkualitas tinggi untuk menampilkan jadwal waktu sholat secara real-time.

### Tujuan Utama
✅ Port font 16x16 legacy (tahun 2013) ke platform modern  
✅ Implementasi dual display (LED Matrix + 7-Segment)  
✅ Optimasi scanning rate dan refresh frequency  
✅ Integrasi dengan ESP32 untuk IoT capabilities  

---

## 🚀 Fitur Teknis

| Fitur | Deskripsi |
|-------|-----------|
| **High-Resolution Font** | Font 16x16 manual (14-byte per kolom) untuk keterbacaan maksimal |
| **Dual Display Sync** | Sinkronisasi seamless antara LED Matrix HUB08 dan 7-Segment display |
| **Legacy Font Support** | Kompatibel dengan data biner legacy dari tahun 2013 |
| **Real-time Updates** | Update jadwal waktu sholat secara dinamis |
| **Optimized Scanning** | Scanning rate 1/16 dengan A, B, C, D line selection |

---

## 🔌 Pemetaan Pin (ESP32 → HUB08)

```
┌─────────────────┬────────────────────┬──────────────────┐
│   HUB08 Pin     │    Fungsi          │   GPIO ESP32     │
├─────────────────┼────────────────────┼──────────────────┤
│  A, B, C, D     │  Line Selection    │  19, 21, 22, 3   │
│  CLK            │  Serial Clock      │  18               │
│  LAT / STB      │  Latch/Strobe      │  23               │
│  OE             │  Output Enable     │  5                │
│  GND            │  Ground            │  GND              │
│  VCC            │  Power Supply      │  5V / 3.3V*       │
└─────────────────┴────────────────────┴──────────────────┘

* Gunakan level shifter jika VCC = 5V
```

---

## 💾 Struktur Font (ASM → C++)

### Konversi Data Legacy

Font original dari Assembly telah dikonversi menjadi array PROGMEM untuk efisiensi memori:

```cpp
// Font Alfabet 26 Karakter (14-byte per karakter)
const uint8_t font_alfabet[26][14] PROGMEM = {
  // ... (A-X)
  
  // Y - Karakter Simetris Legendaris
  {0x80, 0x0F, 0x80, 0x7F, 0xFE, 0x01, 0xFE, 0x01, 
   0x80, 0x7F, 0x80, 0x0F, 0xFF, 0xFF},
  
  // Z
  {0xFE, 0xFF, 0x07, 0x1C, 0x70, 0xC0, 0x01, 0x06, 
   0x18, 0x60, 0x80, 0x01, 0xFE, 0xFF}
};
```

### Format Data
- **Ukuran Karakter**: 16x16 pixel
- **Byte per Kolom**: 14 byte (2 byte per row × 7 columns)
- **Total per Karakter**: 14 bytes
- **Storage**: PROGMEM (Flash memory)

---

## 🛠️ Instalasi

### Prerequisites
- ESP32 Development Board
- Panel LED Matrix P4.75 (HUB08 interface)
- 7-Segment Display (optional)
- Arduino IDE atau PlatformIO
- Library: `Adafruit GFX`, `HUB08 Driver`

### Setup Hardware
1. Koneksikan ESP32 ke HUB08 sesuai pemetaan pin di atas
2. Pasang power supply yang sesuai (5V untuk panel LED)
3. Tambahkan level shifter jika diperlukan

### Setup Software
```bash
# Clone repository
git clone https://github.com/Sinthato/p4.75-esp32.git
cd p4.75-esp32

# Buka di Arduino IDE
# Sesuaikan board settings untuk ESP32
# Upload sketch ke device
```

---

## 📚 Penggunaan

### Contoh Dasar
```cpp
#include "font_data.h"
#include "hub08_driver.h"

HUB08Display display;

void setup() {
  display.begin(16, 32);  // 16 rows, 32 columns
  display.setFont(font_alfabet);
}

void loop() {
  display.print("SHOLAT");
  display.display();
  delay(1000);
}
```

---

## ⚠️ Tantangan & Solusi

### 1. **Interface HUB08 Kompleksitas**
| Tantangan | Solusi |
|-----------|--------|
| Konfigurasi A, B, C, D pins | Gunakan truth table untuk line selection |
| Scanning rate tinggi | Optimize refresh timer di ISR |
| Signal timing kritis | Tambahkan delay minimal untuk stabilitas |

### 2. **Legacy Font Porting**
| Tantangan | Solusi |
|-----------|--------|
| Konversi biner 16-bit Assembly | Script Python untuk auto-convert |
| Kompatibilitas memori | Compress data menggunakan RLE encoding |
| Keterbacaan kode lama | Dokumentasi detail per karakter |

### 3. **Performance Optimization**
- Gunakan DMA untuk transfer data LED
- Implement double-buffering untuk flicker-free display
- Cache character data yang sering digunakan

---

## 📊 Perjalanan Kode

```
2013 ← Font original (Assembly)
  ↓
2026 ← Modernisasi ke C++ & ESP32 (Current)
  ↓
Future ← IoT integration, WiFi sync, Cloud updates
```

---

## 🤝 Kontribusi

Contributions sangat diterima! Silakan:
1. Fork repository ini
2. Buat feature branch (`git checkout -b feature/AmazingFeature`)
3. Commit changes (`git commit -m 'Add AmazingFeature'`)
4. Push ke branch (`git push origin feature/AmazingFeature`)
5. Buat Pull Request

---

## 📄 Lisensi

Project ini dilisensikan di bawah MIT License - lihat file `LICENSE` untuk detail.

---

## 📧 Kontak & Support

Jika ada pertanyaan atau issue, silakan buat issue di repository atau hubungi maintainer.

---

**Last Updated**: 2026-03-13 09:17:08  
**Status**: ✅ Active Development
