🕰️ JWS-P4.75-HUB08-Legacy: Revitalisasi Font 2013 ke ESP32Proyek ini adalah sistem Jadwal Waktu Sholat (JWS) yang menggunakan panel LED Matrix P4.75 dengan interface HUB08. 
Fokus utama proyek ini adalah implementasi ulang Font 16x16 yang didesain pada tahun 2013 menggunakan Assembly, kini dijalankan di atas framework ESP32.📜 Latar BelakangBerbeda dengan panel P10 (HUB12) yang umum di pasaran, panel P4.75 (HUB08) menawarkan kerapatan pixel yang lebih tinggi untuk penggunaan indoor. 
Tantangan utama dalam proyek ini adalah:Interface HUB08: Memerlukan konfigurasi pin A, B, C, D untuk scanning 1/16.Legacy Porting: Mengonversi data biner font 16-bit tinggi dari era Assembly ke dalam struktur data yang kompatibel dengan library DMD ESP32.
🚀 Fitur TeknisHigh-Resolution Font: Menggunakan font 16x16 manual (14-byte per kolom) untuk keterbacaan maksimal.Dual Display Sync: Sinkronisasi antara panel LED Matrix HUB08 dan unit 7-Segment melalui Shift Register 74HC595.Optimasi Buffer: Penggunaan array buf_jadwal[36] untuk memetakan data kalender dan hari secara real-time.Ahad Mode: Tampilan nama hari "AHAD" (bukan Minggu) sesuai permintaan user.Double Column M: Implementasi huruf 'M' yang menggunakan 2 segmen (M1 & M2) agar proporsional pada display 7-segment.
🔌 Pemetaan Pin (ESP32 ke HUB08)Karena menggunakan standar HUB08, pemetaan pin SPI harus disesuaikan:HUB08 PinFungsiPin ESP32A, B, C, DLine SelectionGPIO 19, 21, 22, 3CLKClockGPIO 18LAT / STBLatchGPIO 5OEOutput EnableGPIO 15R1DataGPIO 23
💾 Struktur Font (ASM to C++)Data font tahun 2013 yang awalnya berupa instruksi db (Assembly) telah dikonversi menjadi array PROGMEM:C++// Contoh karakter 'Y' yang legendaris (Simetris 14-data)
const uint8_t font_alfabet[26][14] PROGMEM = {
  // ...
  {0x80,0x0F,0x80,0x7F,0xFE,0x01,0xFE,0x01,0x80,0x7F,0x80,0x0F,0xFF,0xFF}, // Y
  // ...
};
📝 Review Perjalanan Kode"Dari baris-baris Assembly tahun 2013 hingga menjadi sistem modern di tahun 2026. Transisi ke HUB08 P4.75 memberikan tantangan baru dalam hal scanning rate, namun keindahan font 16-bit asli tetap terjaga dengan tajam."— Sindhu Hari Mukti🛠️ Cara KontribusiJika Anda menemukan bug pada scanning HUB08 atau ingin mengoptimalkan penggunaan memori pada ESP32, silakan ajukan Pull Request.
