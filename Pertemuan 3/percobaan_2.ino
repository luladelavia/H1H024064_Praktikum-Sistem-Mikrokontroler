#include <Wire.h>               // Library untuk komunikasi I2C [cite: 189]
#include <LiquidCrystal_I2C.h>  // Library untuk LCD I2C [cite: 189]
#include <Arduino.h>

// Inisialisasi LCD: Alamat 0x27, 16 kolom, 2 baris [cite: 189]
LiquidCrystal_I2C lcd(0x27, 16, 2);

const int pinPot = A0; // Pin untuk potensiometer [cite: 191]

void setup() {
  // Memulai komunikasi serial dengan baud rate 9600 [cite: 43]
  Serial.begin(9600);
  
  // Inisialisasi LCD dan lampu latar [cite: 189]
  lcd.init();
  lcd.backlight();
}

void loop() {
  // 1. Membaca nilai analog (0-1023) [cite: 189]
  int nilaiADC = analogRead(pinPot);
  
  // Jika ingin pembacaan minimal dimulai dari 1 (bukan 0)
  if (nilaiADC < 1) nilaiADC = 1;

  // 2. Menghitung nilai Volt (0.00 - 5.00V) 
  // Pastikan menggunakan 1023.0 (desimal) agar hasil tidak menjadi 0 atau berantakan
  float tegangan = (nilaiADC / 1023.0) * 5.0;
  
  // 3. Menghitung persentase (0 - 100%) [cite: 196]
  int persen = map(nilaiADC, 0, 1023, 0, 100);
  
  // 4. Menampilkan ke Serial Monitor dengan format sesuai modul [cite: 196]
  // Format: ADC: [nilai] Volt: [nilai] V Persen: [nilai]%
  Serial.print("ADC: ");
  Serial.print(nilaiADC);
  Serial.print(" Volt: ");
  Serial.print(tegangan, 2); // Menampilkan 2 angka di belakang koma
  Serial.print(" V Persen: ");
  Serial.print(persen);
  Serial.println("%");

  // 5. Menampilkan ke LCD [cite: 183]
  // Baris 1: Menampilkan teks "ADC: " dan nilainya [cite: 189]
  lcd.setCursor(0, 0);
  lcd.print("ADC:");
  lcd.print(nilaiADC);
  lcd.print(" "); // Spasi pemisah
  lcd.print(persen);
  lcd.print("%   "); // Menghapus sisa karakter

  // Baris 2: Menampilkan level dalam bentuk bar (0-16 karakter) [cite: 184]
  int panjangBar = map(nilaiADC, 0, 1023, 0, 16);
  lcd.setCursor(0, 1);
  for (int i = 0; i < 16; i++) {
    if (i < panjangBar) {
      lcd.print((char)255); // Karakter blok penuh [cite: 189]
    } else {
      lcd.print(" ");       // Karakter kosong [cite: 189]
    }
  }
  
  delay(200); // Jeda agar tampilan stabil [cite: 189]
}