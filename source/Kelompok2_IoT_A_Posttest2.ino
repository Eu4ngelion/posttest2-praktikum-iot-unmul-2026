#include <WiFi.h>
#include <WiFiClientSecure.h>
#include <UniversalTelegramBot.h>
#include <DHT.h>

// == WIFI ==
#define WIFI_SSID "_ENTER_WIFI_SSID"
#define WIFI_PASSWORD "ENTER_WIFI_PASSWORD"

// == Telegram (@BotFather) ==
#define BOT_TOKEN "ENTER_BOT_TOKEN"

// == Telegram (@myidbot) ==
#define ID_ANGGOTA_A "ENTER_ID_ANGGOTA" // Arya
#define ID_ANGGOTA_B "ENTER_ID_ANGGOTA" // Aiman
#define ID_ANGGOTA_C "ENTER_ID_ANGGOTA" // Injil
#define ID_ANGGOTA_D "ENTER_ID_ANGGOTA" // Ocha
#define ID_GRUP "ENTER_ID_GRUP"

// == Konfig Hardware ==
#define PIN_LED_A     5
#define PIN_LED_B     6
#define PIN_LED_C     7
#define PIN_LED_D     8
#define PIN_LED_UTAMA 9
#define PIN_DHT       3 
#define PIN_MQ2       4

#define TIPE_DHT DHT11
#define BATAS_GAS 2500

WiFiClientSecure secured_client;
UniversalTelegramBot bot(BOT_TOKEN, secured_client);
DHT dht(PIN_DHT, TIPE_DHT);

// == Interval ==
const unsigned long INTERVAL = 1000;
unsigned long waktu_bot_terakhir = 0;

// == Variabel Status ==
bool status_led_a = false;
bool status_led_b = false;
bool status_led_c = false;
bool status_led_d = false;
bool status_led_utama = false;
bool notifikasi_gas_terkirim = false;


// == Fungsi ==
bool isAnggotaTerdaftar(String from_id) {
  return (from_id == ID_ANGGOTA_A || from_id == ID_ANGGOTA_B ||
          from_id == ID_ANGGOTA_C || from_id == ID_ANGGOTA_D);
}

String getNamaAnggota(String from_id) {
  if (from_id == ID_ANGGOTA_A) return "Arya";
  if (from_id == ID_ANGGOTA_B) return "Aiman";
  if (from_id == ID_ANGGOTA_C) return "Injil";
  if (from_id == ID_ANGGOTA_D) return "OCha";
  return "Tamu";
}

void setLED(int pin_led, bool &status_led, bool nyala) {
  digitalWrite(pin_led, nyala ? HIGH : LOW);
  status_led = nyala;
}


// == Fungsi Handler Tiap Command ==
void handleStart(String chat_id, String nama) {
  String pesan = "🌟Halo " + nama + "! Selamat datang di Kelompok 2 Smart Home.🌟\n\n";
  pesan += "DAFTAR PERINTAH:\n\n";
  pesan += "/ledon  <A/B/C/D/utama> - Nyalakan LED\n";
  pesan += "/ledoff <A/B/C/D/utama> - Matikan LED\n";
  pesan += "/cekled - Lihat status semua LED\n";
  pesan += "/ceksuhu - Menunjukkan Temperatur\n";
  pesan += "/cekkelembapan - Menunjukkan Kelembapan\n";
  pesan += "/cekgas - Menunjukkan Kadar gas\n";
  bot.sendMessage(chat_id, pesan, "");
}
 
void handlePerintahLED(String chat_id, String from_id, String nama, String arg, bool nyala) {
  String aksi     = nyala ? "menyalakan" : "mematikan";
  String stateStr = nyala ? "ON🟢" : "OFF🔴";

  if (arg == "utama") {
    setLED(PIN_LED_UTAMA, status_led_utama, nyala);
    bot.sendMessage(chat_id, "[LED Utama - Ruang Tamu] Status: " + stateStr, "");
    return;
  }

  if (arg == "A") {
    if (from_id != ID_ANGGOTA_A) {
      bot.sendMessage(chat_id, "AKSES DITOLAK!\n" + nama + " mencoba " + aksi + " LED milik Arya.", "");
      return;
    }
    setLED(PIN_LED_A, status_led_a, nyala);
    bot.sendMessage(chat_id, "[LED A - Arya] Status: " + stateStr, "");
    return;
  }

  if (arg == "B") {
    if (from_id != ID_ANGGOTA_B) {
      bot.sendMessage(chat_id, "AKSES DITOLAK!\n" + nama + " mencoba " + aksi + " LED milik Aiman.", "");
      return;
    }
    setLED(PIN_LED_B, status_led_b, nyala);
    bot.sendMessage(chat_id, "[LED B - Aiman] Status: " + stateStr, "");
    return;
  }

  if (arg == "C") {
    if (from_id != ID_ANGGOTA_C) {
      bot.sendMessage(chat_id, "AKSES DITOLAK!\n" + nama + " mencoba " + aksi + " LED milik Injil.", "");
      return;
    }
    setLED(PIN_LED_C, status_led_c, nyala);
    bot.sendMessage(chat_id, "[LED C - Injil] Status: " + stateStr, "");
    return;
  }

  if (arg == "D") {
    if (from_id != ID_ANGGOTA_D) {
      bot.sendMessage(chat_id, "AKSES DITOLAK!\n" + nama + " mencoba " + aksi + " LED milik Ocha.", "");
      return;
    }
    setLED(PIN_LED_D, status_led_d, nyala);
    bot.sendMessage(chat_id, "[LED D - Ocha] Status: " + stateStr, "");
    return;
  }

  bot.sendMessage(chat_id,
    "⚠️Perintah tidak sesuai.⚠️\n"
    "Contoh penggunaan:\n"
    "/ledon A  atau  /ledon utama\n"
    "/ledoff B  atau  /ledoff utama", "");
}

void handleCekLED(String chat_id) {
  String pesan = "🖥️STATUS SEMUA LAMPU:\n\n";
  pesan += "LED A              : ";  pesan += status_led_a     ? "ON🟢\n" : "OFF🔴\n";
  pesan += "LED B              : ";  pesan += status_led_b     ? "ON🟢\n" : "OFF🔴\n";
  pesan += "LED C              : ";  pesan += status_led_c     ? "ON🟢\n" : "OFF🔴\n";
  pesan += "LED D              : ";  pesan += status_led_d     ? "ON🟢\n" : "OFF🔴\n";
  pesan += "LED Utama (R.Tamu) : ";  pesan += status_led_utama ? "ON🟢\n" : "OFF🔴\n";
  bot.sendMessage(chat_id, pesan, "");
}

void handleCekSuhu(String chat_id) {
  float suhu = dht.readTemperature();
  if (isnan(suhu)) {
    bot.sendMessage(chat_id, "❌Gagal membaca sensor DHT11.", "");
    return;
  }
  String pesan = "🌡️DATA SUHU RUANGAN:\n\n";
  pesan += "Suhu : " + String(suhu, 1) + " C";
  bot.sendMessage(chat_id, pesan, "");
}

void handleCekKelembapan(String chat_id) {
  float kelembapan = dht.readHumidity();
  if (isnan(kelembapan)) {
    bot.sendMessage(chat_id, "Gagal membaca sensor DHT11.", "");
    return;
  }
  String pesan = "🌦️DATA KELEMBAPAN RUANGAN:\n\n";
  pesan += "Kelembapan : " + String(kelembapan, 1) + " %\n";
  bot.sendMessage(chat_id, pesan, "");
}

void handleCekGas(String chat_id) {
  int nilai_gas = analogRead(PIN_MQ2);
  String status_gas = (nilai_gas > BATAS_GAS)
    ? "BAHAYA - Gas Berlebihan terdeteksi!."
    : "AMAN - Tidak ada kebocoran gas.";

  String pesan = "🤖DATA SENSOR MQ-2:\n\n";
  pesan += "Kadar Gas  : " + String(nilai_gas) + " / 4095\n";
  pesan += "Batas Aman : " + String(BATAS_GAS) + "\n";
  pesan += "Status     : " + status_gas;
  bot.sendMessage(chat_id, pesan, "");
}

void handleTidakDikenali(String chat_id, String teks) {
  bot.sendMessage(chat_id,
    "Perintah tidak dikenali: " + teks + "\n"
    "Ketik /start untuk melihat daftar perintah.", "");
}


// == Handle New Messages ==

void handleNewMessages(int jumlah_pesan) {
  for (int i = 0; i < jumlah_pesan; i++) {
    String chat_id = bot.messages[i].chat_id;
    String from_id = bot.messages[i].from_id;
    String teks    = bot.messages[i].text;

    if (!isAnggotaTerdaftar(from_id)) {
      bot.sendMessage(chat_id, "AKSES DITOLAK!\nKamu tidak terdaftar sebagai anggota.", "");
      continue;
    }

    String nama = getNamaAnggota(from_id);

    int posisi_spasi = teks.indexOf(' ');
    String command, arg;
    if (posisi_spasi == -1) {
      command = teks;
      arg = "";
    } else {
      command = teks.substring(0, posisi_spasi);
      arg = teks.substring(posisi_spasi + 1);
      arg.trim();
    }

    int perintah = 0;
    if (command == "/start")         perintah = 1;
    if (command == "/ledon")         perintah = 2;
    if (command == "/ledoff")        perintah = 3;
    if (command == "/cekled")        perintah = 4;
    if (command == "/ceksuhu")       perintah = 5;
    if (command == "/cekkelembapan") perintah = 6;
    if (command == "/cekgas")        perintah = 7;

    switch (perintah) {
      case 1: handleStart(chat_id, nama); break;
      case 2: handlePerintahLED(chat_id, from_id, nama, arg, true); break;
      case 3: handlePerintahLED(chat_id, from_id, nama, arg, false); break;
      case 4: handleCekLED(chat_id); break;
      case 5: handleCekSuhu(chat_id); break;
      case 6: handleCekKelembapan(chat_id); break;
      case 7: handleCekGas(chat_id); break;
      default: handleTidakDikenali(chat_id, teks); break;
    }
  }
}


// == Cek Gas Otomatis (Push Notification) ==

void cekGasOtomatis() {
  int nilai_gas = analogRead(PIN_MQ2);

  if (nilai_gas > BATAS_GAS && !notifikasi_gas_terkirim) {
    String peringatan = "🚨PERINGATAN DARURAT - KEBOCORAN GAS!\n\n";
    peringatan += "Kadar Gas : " + String(nilai_gas) + " / 4095";
    bot.sendMessage(ID_GRUP, peringatan, "");
    notifikasi_gas_terkirim = true;
  }

  if (nilai_gas <= BATAS_GAS && notifikasi_gas_terkirim) {
    String aman = "🫡INFO: Kadar gas kembali normal.\n";
    aman += "Kadar Gas : " + String(nilai_gas) + " / 4095";
    bot.sendMessage(ID_GRUP, aman, "");
    notifikasi_gas_terkirim = false;
  }
}


// == Setup & Loop ==

void setup() {
  Serial.begin(115200);

  pinMode(PIN_LED_A, OUTPUT); digitalWrite(PIN_LED_A, LOW);
  pinMode(PIN_LED_B, OUTPUT); digitalWrite(PIN_LED_B, LOW);
  pinMode(PIN_LED_C, OUTPUT); digitalWrite(PIN_LED_C, LOW);
  pinMode(PIN_LED_D, OUTPUT); digitalWrite(PIN_LED_D, LOW);
  pinMode(PIN_LED_UTAMA,OUTPUT); digitalWrite(PIN_LED_UTAMA,LOW);

  dht.begin();

  configTime(0, 0, "pool.ntp.org");
  secured_client.setInsecure();

  Serial.print("Menghubungkan ke WiFi");
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
  while (WiFi.status() != WL_CONNECTED) {
    Serial.print(".");
    delay(500);
  }
  Serial.println("\nTerhubung. IP: " + WiFi.localIP().toString());

  Serial.print("Sinkronisasi NTP");
  time_t now = time(nullptr);
  while (now < 24 * 3600) {
    Serial.print(".");
    delay(100);
    now = time(nullptr);
  }
  Serial.println(" OK");
}

void loop() {

  if (millis() - waktu_bot_terakhir > INTERVAL) {
    int jumlah_pesan = bot.getUpdates(bot.last_message_received + 1);
    if (jumlah_pesan > 0) handleNewMessages(jumlah_pesan);
    cekGasOtomatis();
    waktu_bot_terakhir = millis();
  }
}
