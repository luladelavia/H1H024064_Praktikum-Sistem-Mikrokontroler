# Laporan Praktikum - Jawaban Pertanyaan

* **Nama:** Lula Khaisha Delavia
* **NIM:** H1H024064
* **Shift Awal:** D
* **Shift Akhir:** D

---


# Modul 5: Multitasking dan Komunikasi Antar-Task dalam FreeRTOS
---

## 📌 Daftar Isi
1. [Percobaan 5A: Analisis Multitasking & Kendali Potensiometer](#-percobaan-5a-analisis-multitasking--kendali-potensiometer)
   - [Analisis Mekanisme Eksekusi Task](#1-analisis-mekanisme-eksekusi-task)
   - [Langkah Integrasi Task Baru (Task Keempat)](#2-langkah-integrasi-task-baru-task-keempat)
   - [Modifikasi Kode: Kontrol Kecepatan LED via Potensiometer](#3-modifikasi-kode-kontrol-kecepatan-led-via-potensiometer)
2. [Percobaan 4A: Komunikasi Data & Arsitektur ADC / DHT](#-percobaan-4a-komunikasi-data--arsitektur-adc--dht)
   - [Analisis Sinkronisasi Prosedur Kerja Task](#1-analisis-sinkronisasi-prosedur-kerja-task)
   - [Kajian Kritis Potensi Race Condition & Thread Safety](#2-kajian-kritis-potensi-race-condition--thread-safety)
   - [Modifikasi Kode: Integrasi Sensor DHT Terdistribusi dengan Queue](#3-modifikasi-kode-integrasi-sensor-dht-terdistribusi-dengan-queue)
3. [Panduan Instalasi & Pengujian](#-panduan-instalasi--pengujian)

---

## ⚡ Percobaan 5A: Analisis Multitasking & Kendali Potensiometer

### 1. Analisis Mekanisme Eksekusi Task
**Pertanyaan:** Apakah ketiga task berjalan secara bersamaan atau bergantian? Jelaskan mekanismenya!

**Penjelasan Teknis:**
Eksekusi dari ketiga task tersebut tidak terjadi secara simultan/sejajar (*true parallel execution*), melainkan secara bergantian yang ter-orchestasi dengan sangat cepat (*concurrent execution*). Hal ini disebabkan oleh keterbatasan perangkat keras mikrokontroler (seperti ATmega328P pada Arduino Uno) yang hanya memiliki satu buah inti CPU (*single-core processor*). Satu inti core hanya dapat mengeksekusi satu instruksi mesin pada satu satuan waktu terkecil.

Mekanisme penjadwalan ini diatur secara rigid oleh *Scheduler* FreeRTOS dengan menerapkan algoritma **Preemptive Scheduling Berbasis Prioritas** yang dikombinasikan dengan kebijakan **Round-Robin** untuk task berprioritas sama:
* **Mekanisme Blocking & Context Switch:** Ketika `TaskBlink1` mengeksekusi fungsi penunda `vTaskDelay(200ms)`, task tersebut secara sukarela melepaskan hak akses CPU dan statusnya berubah dari *Running* menjadi *Blocked*.
* **Pemanfaatan Idle Time:** Penjadwal (*Scheduler*) langsung mendeteksi kekosongan ini dan melakukan *Context Switching* untuk memindahkan memori eksekusi (menyimpan register ke stack task lama dan memuat stack task baru) ke task lain yang berstatus *Ready* (misalnya `TaskBlink2` atau `Taskprint`).
* **Sistem Alokasi Waktu (Time-Slicing):** Karena ketiga task dikonfigurasi dengan level prioritas yang identik (Prioritas = 1), penentuan giliran eksekusi didasarkan pada jatah waktu tertentu (*time slice*). Proses perputaran ini berjalan dalam skala milidetik sehingga menciptakan ilusi optik dan logis bagi pengguna seolah-olah seluruh program bekerja bersamaan secara paralel.

### 2. Langkah Integrasi Task Baru (Task Keempat)
**Pertanyaan:** Bagaimana cara menambahkan task keempat? Jelaskan langkahnya!

Untuk mengintegrasikan sub-rutin atau task baru ke dalam kernel FreeRTOS, diperlukan tiga tahapan prosedural yang runtut:

1. **Deklarasi Prototipe Fungsi Task (Fase Inisialisasi Atas):**
   Mendefinisikan blueprint fungsi task sebelum fungsi `setup()`. Fungsi ini wajib mengembalikan nilai `void` dan menerima satu parameter bertipe pointer *void* (`void *pvParameters`).
   ```cpp
   void TaskSensor(void *pvParameters);

```

2. **Instansiasi Instans Task pada Kernel (Fase Setup):**
Mendaftarkan task ke dalam sistem manajemen FreeRTOS menggunakan fungsi API `xTaskCreate()` di dalam cakupan fungsi `setup()`.
```cpp
xTaskCreate(
  TaskSensor,       // Nama fungsi implementasi task
  "TaskSensor4",    // String deskriptif nama task untuk debugging
  128,              // Alokasi ukuran stack (dalam satuan Words, bukan Bytes)
  NULL,             // Parameter yang dipasangkan ke task (kosong jika tidak ada)
  1,                // Level prioritas task (sesuai skala prioritas sistem)
  NULL              // Task handle untuk manipulasi eksternal (opsional)
);

```


3. **Penyusunan Definisi Tubuh Fungsi Task (Fase Implementasi):**
Contoh task ke 4
```cpp
void TaskBaru(void *pvParameters) {
  pinMode(6, OUTPUT);
  while(1) {
    digitalWrite(6, HIGH);
    vTaskDelay(700 / portTICK_PERIOD_MS);
    digitalWrite(6, LOW);
    vTaskDelay(700 / portTICK_PERIOD_MS);
  }
}

```

### 3. Modifikasi Kode: Kontrol Kecepatan LED via Potensiometer

Aplikasi ini memodifikasi sistem kerja multitaksing dengan memanfaatkan variabel global dinamis yang nilainya diperbarui oleh pembacaan ADC potensiometer pada satu task, dan digunakan sebagai durasi penundaan kedipan LED pada task lainnya.

```cpp
#include <Arduino_FreeRTOS.h>

void TaskBlink1(void *pvParameters);
void TaskBlink2(void *pvParameters);
void Taskprint(void *pvParameters);
void TaskPot(void *pvParameters);

const int ledPin1 = 8;   // LED merah (kecepatan dikontrol potensio)
const int ledPin2 = 7;   // LED kuning
const int potPin = A0;   // potensiometer

int potValue = 0;
int delayTime = 200;      // delay default (ms)

void setup() {
  Serial.begin(9600);
  
  // Membuat 4 task dengan prioritas sama (1)
  xTaskCreate(TaskBlink1, "task1", 128, NULL, 1, NULL);
  xTaskCreate(TaskBlink2, "task2", 128, NULL, 1, NULL);
  xTaskCreate(Taskprint,  "task3", 128, NULL, 1, NULL);
  xTaskCreate(TaskPot,    "task4", 128, NULL, 1, NULL);
  
  vTaskStartScheduler();   // Mulai RTOS scheduler
}

void loop() {
  // Kosong – semua eksekusi ditangani oleh scheduler
}

// Task 1: LED merah (pin 8) – kecepatan mengikuti potensiometer
void TaskBlink1(void *pvParameters) {
  pinMode(ledPin1, OUTPUT);
  while (1) {
    digitalWrite(ledPin1, HIGH);
    vTaskDelay(delayTime / portTICK_PERIOD_MS);
    digitalWrite(ledPin1, LOW);
    vTaskDelay(delayTime / portTICK_PERIOD_MS);
    Serial.print("Task1 delay: ");
    Serial.println(delayTime);
  }
}

// Task 2: LED kuning (pin 7) – kecepatan 2× lebih lambat dari LED merah
void TaskBlink2(void *pvParameters) {
  pinMode(ledPin2, OUTPUT);
  while (1) {
    digitalWrite(ledPin2, HIGH);
    vTaskDelay(2 * delayTime / portTICK_PERIOD_MS);
    digitalWrite(ledPin2, LOW);
    vTaskDelay(2 * delayTime / portTICK_PERIOD_MS);
  }
}

// Task 3: Mencetak counter setiap 500ms (untuk monitoring)
void Taskprint(void *pvParameters) {
  int counter = 0;
  while (1) {
    counter++;
    Serial.print("Counter: ");
    Serial.println(counter);
    vTaskDelay(500 / portTICK_PERIOD_MS);
  }
}

// Task 4: Membaca potensiometer dan mengupdate delayTime
void TaskPot(void *pvParameters) {
  while (1) {
    potValue = analogRead(A0);
    // Mapping nilai ADC 0-1023 ke delay 100ms – 1000ms
    delayTime = map(potValue, 0, 1023, 100, 1000);
	
    vTaskDelay(100 / portTICK_PERIOD_MS);   // update setiap 100ms
  }
}

```

---

## 📊 Percobaan 5B: Komunikasi Data & Arsitektur ADC / DHT

### 1. Analisis Sinkronisasi Prosedur Kerja Task

**Pertanyaan:** Apakah kedua task berjalan secara bersamaan atau bergantian? Jelaskan mekanismenya!

**Penjelasan Teknis:**
Kedua task (`read_data` dan `display`) beroperasi secara bergantian menggunakan konsep **Event-Driven Synchronization** dengan media perantara berupa objek *Queue* (antrean pesan). Kerja sama antar-task diatur melalui kondisi ketersediaan muatan data di dalam antrean:

* **Siklus Produsen (`read_data`):** Task ini bertugas mengambil sampel data mentah. Setelah data berhasil diakuisisi dan dikemas ke dalam struct, fungsi `xQueueSend()` dipanggil untuk mendorong data ke antrean. Segera setelah itu, task mengeksekusi `vTaskDelay(100ms)`, yang mengubah statusnya menjadi *Blocked*.
* **Siklus Konsumen (`display`):** Di sisi lain, task `display` bersiap mengambil data menggunakan macro `xQueueReceive()`. Berkat parameter `portMAX_DELAY`, jika antrean kosong, task ini tidak akan membuang siklus CPU secara sia-sia dalam putaran kosong (*busy waiting*), melainkan langsung ditidurkan (*Blocked state*) oleh kernel.
* **Mekanisme Wake-up:** Detik di mana `read_data` memasukkan data baru ke dalam antrean, kernel FreeRTOS mendeteksi perubahan state antrean, langsung membangunkan task `display` menjadi *Ready*, lalu mengeksekusinya ke status *Running* untuk membaca dan mencetak data ke Serial Monitor. Setelah data terbaca, antrean kembali kosong, dan task `display` masuk ke mode *Blocked* kembali.

### 2. Kajian Kritis Potensi Race Condition & Thread Safety

**Pertanyaan:** Apakah program ini berpotensi mengalami race condition? Jelaskan!

**Penjelasan Teknis:**
Arsitektur program ini **sepenuhnya terbebas dari resiko *Race Condition*** (kondisi balapan data). Penyebab utamanya adalah pertukaran data tidak mengandalkan variabel global telanjang, melainkan melalui perantara **Message Queue** bawaan dari framework FreeRTOS yang telah mengimplementasikan protokol *Thread-Safe*.

* **Proteksi Mutex / Critical Section Internal:** Di dalam API `xQueueSend()` dan `xQueueReceive()`, kernel FreeRTOS secara otomatis membungkus proses penulisan dan pembacaan memori dengan *Critical Section Enforcer* (seperti mematikan interupsi global untuk sementara waktu atau mengunci scheduler). Hal ini memastikan bahwa pointer memori antrean tidak dapat dialihkan atau diinterupsi di tengah jalan saat proses transfer data sedang berlangsung.
* **Sifat Penyalinan Data (Pass-by-Value):** FreeRTOS Queue bekerja dengan cara menyalin seluruh isi data (*by value*) ke dalam alokasi memori buffer milik internal queue, bukan sekadar mengirimkan alamat pointernya (*by reference*). Oleh sebab itu, meskipun variabel lokal pada task produsen langsung berubah nilainya pada siklus berikutnya, data yang tersimpan di dalam queue tetap utuh dan aman sampai diambil oleh task konsumen.

### 3. Modifikasi Kode: Integrasi Sensor DHT Terdistribusi dengan Queue

Modifikasi ini mengimplementasikan pembacaan telemetri iklim nyata menggunakan sensor DHT. Data suhu (*temperature*) dan kelembaban (*humidity*) dibaca sebagai bilangan pecahan pecahan desimal (`float`), kemudian dibungkus ke dalam satu paket struktur data untuk dikirimkan secara aman antar-task.

```cpp
#include <Arduino_FreeRTOS.h>   
#include <queue.h>              
#include <DHT.h>                

#define DHTPIN 2                // Pin data sensor DHT terhubung ke pin 2 Arduino
#define DHTTYPE DHT11           // Tipe sensor: DHT11 (bisa diganti DHT22)

DHT dht(DHTPIN, DHTTYPE);       // Buat objek DHT

struct readings {
  float temp;               // Suhu dalam derajat Celcius
  float h;         		    // Kelembaban dalam persen (%)
};

QueueHandle_t my_queue;         // Handle queue (antar task)

void read_data(void *pvParameters);   // Task membaca data dari DHT
void display(void *pvParameters);    // Task menampilkan data ke Serial Monitor

void setup() {
  Serial.begin(9600);           // Mulai komunikasi serial (untuk output)
  
  dht.begin();                  // Inisialisasi sensor DHT

  my_queue = xQueueCreate(1, sizeof(struct readings));

  // Buat task pembaca sensor
  xTaskCreate(read_data, "BacaSensor", 128, NULL, 1, NULL);
  // Buat task penampil data 
  xTaskCreate(display, "Tampilkan", 128, NULL, 1, NULL);
}

void loop() {
  // Kosong - semua eksekusi dikelola oleh scheduler FreeRTOS
}

void display(void *pvParameters){
  struct readings data;         // Variabel lokal untuk menyimpan data sementara

  for(;;){

    // Ambil data dari queue. Jika queue kosong
    if (xQueueReceive(my_queue, &data, portMAX_DELAY) == pdPASS) {

      // Jika berhasil menerima data, tampilkan ke Serial Monitor
      Serial.print("Suhu : ");
      Serial.print(data.suhu);
      Serial.println(" °C");

      Serial.print("Kelembaban : ");
      Serial.print(data.kelembaban);
      Serial.println(" %");
      
      Serial.println("------------------------");
    }
  }
}
```

---

## 🛠️ Panduan Instalasi & Pengujian

1. **Persiapan Library Arduino IDE:**
* Pastikan Anda telah memasang library **Adafruit DHT Sensor Library** dan **Adafruit Unified Sensor** melalui *Library Manager* (Ctrl+Shift+I).
* Pasang library **FreeRTOS** versi Arduino (oleh Phillip Stevens) jika belum terpasang.


2. **Skema Pengabelan Modul (DHT11):**
* Pin VCC Sensor -> Pin 5V Arduino
* Pin GND Sensor -> Pin GND Arduino
* Pin Data Sensor -> Pin Digital 2 Arduino


3. **Skema Pengabelan Potensiometer:**
* Pin Kaki Kiri -> Pin GND
* Pin Kaki Tengah (Wiper) -> Pin Analog A0
* Pin Kaki Kanan -> Pin 5V
