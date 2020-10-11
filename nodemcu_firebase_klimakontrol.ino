//Mucit Pilot 2020
//ESP8266 ile Wifi üzerinden klima kontrolü
//Baymak Elegant Plus 24000 Klima Örneği

#include <IRremoteESP8266.h>
#include <IRsend.h>
//#ifndef UNIT_TEST
#include <Arduino.h>
//#endif

#include "FirebaseESP8266.h"
#include <ESP8266WiFi.h>

//Firebase veritabanı adresini, Token bilgisini ve ağ adresi bilgilerinizi giriniz.
#define FIREBASE_HOST "klimakontrol-37828.firebaseio.com" // http:// veya https:// olmadan yazın
#define FIREBASE_AUTH "*********************"
#define WIFI_SSID "********************"
#define WIFI_PASSWORD "******************"


//veritabanim adında bir firebase veritabanı nesnesi oluşturuyoruz
FirebaseData veritabanim;


//burada tanımladığımız değişkenler ile komut vermediğimiz durumlarda kodun işlem yapmaması için durum değişkenleri tanımladık.
int butonstatus = 0;
int butondurumu = 0;
int sicaklik = 0;
int sicaklikdurumu = 0;
int fanhizi = 0;
int fandurumu = 0;
int kapanmadurumu = 0;
int kapanma = 0;
int isitmadurumu = 0;
int isitma = 0;

const uint16_t kIrLed = D2;  // D2 pinini IR ledden çıkış için kullanacağız.
IRsend irsend(kIrLed);  // irsend adında bir IRSEND nesnesi oluşturuyoruz .

////////////////////////////////////////////////////////////////////////////////////////////

// IRrecvDumpV2.ino dosyası kullanarak seri monitörden okuduğumuz kumanda komutları
uint16_t klimaAC[211] = {9088, 4492,  584, 1674,  598, 1672,  624, 518,  624, 518,  626, 518,  624, 522,  624, 1630,  646, 1628,  648, 1644,  628, 1646,  626, 1646,
                         626, 1628,  646, 518,  626, 498,  648, 498,  648, 1628,  646, 514,  626, 516,  600, 544,  624, 520,  600, 544,  598, 1654,  648, 1628,  648, 1650,  626, 512,  600, 542,
                         600, 544,  598, 544,  624, 518,  626, 500,  646, 500,  646, 518,  626, 514,  600, 542,  626, 516,  626, 518,  626, 518,  626, 1628,  650, 1628,  646, 518,  608, 530,  626,
                         516,  600, 544,  596, 546,  624, 520,  624, 522,  626, 498,  648, 518,  604, 558,  576, 544,  598, 544,  624, 518,  624, 518,  624, 1630,  648, 500,  646, 518,  626, 512,
                         600, 542,  600, 544,  598, 546,  598, 544,  624, 522,  626, 500,  646, 518,  626, 514,  600, 542,  598, 544,  598, 544,  624, 518,  624, 522,  624, 498,  648, 518,  626, 512,
                         602, 542,  598, 544,  598, 544,  624, 518,  624, 1630,  648, 498,  648, 518,  624, 514,  600, 542,  598, 544,  598, 544,  624, 520,  624, 522,  624, 498,  648, 516,  600, 1672,
                         626, 516,  600, 1674,  626, 516,  598, 546,  624, 522,  626, 520,  626, 516,  626, 1644,  600, 1672,  600, 1674,  598, 544,  624, 1652,  626, 520,  626, 1628,  648, 1646,  598
                        };  // Klima ON 25 derece cool fan 1

uint16_t klimaKAPAT[211] = {9054, 4462,  614, 1670,  624, 1648,  600, 542,  624, 520,  622, 520,  626, 520,  626, 1630,  646, 1650,  626, 1644,  626, 1648,  624, 1648,  600, 1676,
                            624, 518,  624, 522,  624, 500,  646, 1628,  646, 516,  600, 542,  598, 544,  624, 518,  624, 518,  624, 1630,  648, 1628,  648, 1630,  646, 514,  602, 542,  600, 542,  626, 518,
                            598, 546,  624, 500,  646, 498,  648, 516,  598, 542,  600, 542,  598, 544,  598, 544,  626, 518,  624, 1632,  644, 1630,  646, 518,  598, 540,  600, 542,  600, 542,  598, 544,
                            598, 544,  624, 524,  624, 500,  648, 518,  600, 538,  600, 542,  600, 542,  598, 544,  624, 520,  598, 1654,  648, 500,  646, 518,  598, 540,  600, 542,  598, 544,  598, 544,
                            624, 520,  624, 522,  626, 500,  648, 516,  624, 514,  626, 518,  598, 544,  624, 518,  626, 518,  624, 498,  648, 520,  624, 518,  598, 540,  600, 542,  626, 516,  600, 544,
                            624, 518,  624, 522,  624, 498,  648, 518,  624, 514,  626, 516,  626, 516,  626, 518,  624, 520,  626, 520,  626, 498,  648, 516,  626, 1644,  600, 542,  600, 1674,  624, 520,
                            624, 520,  626, 520,  626, 498,  646, 518,  624, 1646,  626, 1648,  598, 1676,  624, 518,  626, 1628,  648, 1628,  646, 500,  646, 1646,  600
                           };  // Klima off 25 derece cool fan 1

uint16_t derece21_fan1_on[211] = {9072, 4492,  584, 1672,  626, 1646,  626, 518,  624, 518,  624, 520,  624, 520,  626, 1626,  648, 1628,  648, 1644,  600, 1674,  598, 1674,  600, 1656,
                                  646, 516,  624, 1630,  648, 1630,  648, 516,  628, 512,  626, 516,  600, 544,  624, 520,  624, 520,  624, 1628,  646, 1630,  648, 1628,  648, 538,  576, 542,  600, 542,  598, 546,  598,
                                  544,  624, 522,  626, 522,  624, 518,  626, 512,  626, 518,  598, 542,  626, 518,  626, 518,  626, 1628,  646, 1630,  648, 518,  626, 514,  600, 542,  598, 546,  598, 544,  624, 518,  626,
                                  520,  624, 500,  646, 516,  626, 512,  602, 540,  600, 542,  626, 518,  626, 518,  626, 1628,  648, 500,  648, 516,  626, 514,  602, 542,  624, 516,  600, 544,  626, 518,  626, 498,  646, 500,
                                  646, 518,  624, 516,  598, 542,  604, 538,  624, 518,  626, 518,  624, 522,  624, 522,  624, 518,  624, 514,  624, 518,  598, 544,  622, 520,  624, 518,  624, 1630,  646, 500,  648, 516,  626,
                                  514,  600, 566,  576, 542,  626, 518,  624, 518,  624, 522,  624, 520,  624, 518,  624, 534,  580, 542,  600, 540,  626, 518,  596, 546,  624, 520,  626, 498,  648, 518,  626, 516,  624, 1646,
                                  626, 516,  626, 518,  624, 1628,  648, 1630,  648, 498,  648, 1644,  626
                                 };  // 21 derece cool fan1

uint16_t derece21_fan2_on[211] = {9064, 4512,  588, 1648,  624, 1648,  624, 518,  624, 518,  622, 520,  624, 522,  622, 1630,  648, 1652,  622, 1648,  622, 1650,  620, 1650,  624, 1652,  622, 520,
                                  624, 1652,  624, 1652,  626, 520,  624, 518,  622, 520,  622, 518,  624, 518,  624, 520,  624, 1626,  648, 1628,  648, 1650,  624, 542,  600, 518,  624, 542,  598, 520,  624, 518,  624, 522,  624, 522,
                                  624, 518,  624, 516,  622, 518,  624, 518,  624, 520,  624, 520,  622, 520,  622, 1632,  646, 520,  622, 518,  622, 518,  624, 520,  622, 518,  624, 520,  624, 522,  624, 522,  624, 520,  622, 518,  622, 518,
                                  622, 518,  622, 520,  624, 520,  622, 1652,  624, 522,  624, 518,  624, 540,  598, 542,  598, 520,  622, 518,  624, 520,  624, 524,  622, 524,  622, 520,  624, 518,  620, 518,  624, 520,  622, 520,  622, 520,
                                  624, 522,  624, 522,  624, 518,  622, 542,  600, 558,  582, 520,  622, 520,  622, 520,  624, 1652,  624, 524,  624, 520,  622, 518,  622, 518,  622, 518,  624, 520,  622, 518,  624, 522,  624, 524,  624, 520,
                                  622, 516,  624, 518,  622, 1650,  624, 518,  624, 518,  622, 522,  624, 524,  622, 520,  622, 518,  624, 1648,  624, 1650,  624, 518,  624, 1650,  624, 522,  622, 522,  624, 1646,  624
                                 };  // 21 derece cool fan2


uint16_t derece21_fan3_on[211] = {9064, 4510,  592, 1648,  622, 1648,  624, 518,  622, 520,  624, 520,  624, 522,  624, 1628,  648, 1650,  626, 1646,  624, 1648,  624, 1650,  624, 1652,  624, 516,  626, 1628,
                                  648, 1628,  648, 520,  622, 542,  598, 542,  598, 520,  622, 518,  624, 518,  624, 1628,  648, 1628,  650, 1650,  624, 518,  620, 520,  624, 542,  598, 520,  622, 520,  624, 522,  624, 522,  626, 518,  624, 540,
                                  600, 542,  600, 518,  622, 520,  624, 522,  620, 1628,  648, 522,  624, 518,  622, 542,  598, 520,  620, 520,  624, 518,  624, 520,  624, 520,  624, 500,  648, 518,  622, 558,  584, 518,  622, 520,  622, 520,  622,
                                  522,  622, 1628,  648, 524,  624, 522,  620, 516,  624, 520,  622, 542,  598, 520,  624, 520,  624, 520,  624, 524,  624, 520,  624, 540,  598, 518,  622, 520,  622, 520,  624, 520,  624, 520,  624, 522,  624, 518,
                                  624, 516,  622, 544,  598, 518,  624, 518,  624, 520,  626, 1628,  648, 522,  624, 520,  624, 516,  622, 542,  598, 520,  622, 518,  624, 518,  624, 522,  624, 500,  646, 520,  622, 540,  600, 518,  622, 1650,  624,
                                  518,  624, 520,  622, 522,  624, 522,  624, 520,  624, 518,  624, 1648,  624, 1650,  622, 520,  622, 1650,  624, 1652,  624, 1628,  648, 518,  624
                                 };  // 21 derece cool fan3


uint16_t derece23_fan1_on[211] = {9070, 4486,  588, 1672,  600, 1672,  624, 518,  598, 544,  600, 544,  598, 548,  624, 1630,  646, 1630,  646, 1644,  600, 1674,  598, 1674,  598, 1674,
                                  624, 1630,  646, 1628,  648, 1628,  648, 518,  598, 540,  600, 542,  600, 542,  600, 544,  624, 520,  596, 1656,  648, 1630,  648, 1630,  646, 512,  602, 542,  602, 540,  598, 544,
                                  598, 544,  622, 526,  624, 500,  646, 516,  600, 540,  600, 542,  598, 544,  598, 544,  622, 520,  598, 1656,  646, 1628,  648, 518,  600, 540,  600, 542,  598, 542,  600, 544,  598,
                                  544,  624, 500,  646, 500,  646, 518,  598, 542,  600, 542,  598, 544,  598, 544,  600, 542,  624, 1630,  646, 500,  646, 518,  624, 514,  602, 540,  600, 544,  598, 546,  598, 544,
                                  624, 522,  624, 500,  646, 516,  626, 512,  602, 542,  600, 542,  598, 544,  598, 544,  626, 520,  624, 500,  646, 518,  600, 538,  600, 542,  600, 544,  598, 544,  624, 518,  624,
                                  1628,  648, 500,  648, 516,  626, 514,  600, 542,  600, 542,  598, 544,  624, 520,  626, 500,  646, 498,  646, 516,  624, 1646,  600, 542,  598, 1674,  600, 544,  622, 520,  626, 520,
                                  624, 500,  646, 516,  598, 1672,  598, 1672,  624, 1648,  598, 544,  606, 540,  624, 522,  624, 1630,  646, 1644,  600
                                 };  // 23 derece cool fan1

uint16_t derece23_fan2_on[211] = {9026, 4534,  570, 1670,  600, 1672,  598, 544,  600, 542,  600, 544,  600, 546,  598, 1654,  622, 1654,  622, 1670,  600, 1672,  600, 1674,  598, 1676,  598,
                                  1654,  622, 1652,  622, 1654,  624, 542,  600, 538,  602, 542,  598, 542,  598, 544,  600, 542,  598, 1656,  622, 1654,  624, 1674,  600, 542,  600, 544,  598, 542,  598, 544,  600, 542,  600, 524,
                                  622, 524,  622, 544,  598, 540,  600, 540,  598, 544,  598, 544,  598, 546,  598, 546,  598, 1654,  624, 544,  598, 540,  600, 542,  598, 544,  598, 544,  600, 544,  600, 524,  620, 526,  620, 544,
                                  600, 540,  600, 594,  548, 544,  598, 544,  598, 544,  600, 1654,  622, 526,  620, 544,  600, 558,  580, 594,  548, 542,  600, 544,  600, 544,  598, 546,  598, 526,  622, 544,  600, 560,  578, 542,
                                  598, 544,  598, 546,  596, 546,  598, 526,  620, 524,  622, 544,  598, 542,  600, 540,  600, 542,  600, 544,  600, 542,  600, 1654,  622, 524,  622, 544,  600, 540,  600, 542,  600, 542,  600, 544,
                                  600, 544,  598, 526,  622, 524,  622, 542,  600, 540,  600, 542,  600, 1672,  600, 544,  600, 544,  598, 548,  598, 524,  622, 542,  598, 542,  600, 1672,  598, 1674,  600, 542,  600, 544,  598, 1654,
                                  622, 524,  622, 1670,  600
                                 };  // 23 derece cool fan2

uint16_t derece23_fan3_on[211] = {9028, 4556,  544, 1672,  600, 1672,  600, 542,  598, 544,  600, 544,  598, 524,  622, 1654,  622, 1652,  648, 1644,  602, 1670,  600, 1674,  600, 1674,  600, 1676,  600, 1654,
                                  622, 1654,  624, 542,  598, 566,  574, 544,  596, 544,  598, 544,  598, 546,  598, 1656,  620, 1654,  624, 1652,  622, 538,  600, 544,  598, 570,  572, 544,  598, 544,  598, 546,  600, 524,  622, 542,  600, 540,
                                  600, 542,  600, 542,  598, 544,  600, 542,  600, 1654,  622, 524,  622, 542,  600, 540,  600, 542,  600, 544,  598, 546,  598, 544,  600, 546,  598, 526,  622, 542,  600, 564,  576, 542,  600, 542,  600, 542,  598,
                                  544,  598, 1656,  622, 524,  622, 542,  600, 548,  592, 542,  598, 544,  598, 544,  598, 544,  598, 546,  598, 526,  622, 544,  598, 540,  600, 570,  570, 544,  598, 544,  600, 544,  600, 524,  622, 524,  620, 544,
                                  598, 540,  600, 542,  598, 542,  600, 544,  598, 544,  598, 1656,  622, 524,  624, 542,  598, 542,  600, 542,  600, 542,  600, 542,  598, 544,  598, 526,  620, 524,  624, 542,  598, 562,  576, 544,  598, 1674,  598,
                                  544,  600, 544,  598, 548,  598, 526,  622, 542,  600, 544,  598, 1672,  600, 1672,  600, 542,  600, 546,  598, 524,  620, 526,  620, 1670,  600
                                 };  // 23 derece cool fan3

uint16_t derece23_fan1_off[211] = {9064, 4512,  590, 1648,  624, 1648,  624, 518,  624, 520,  624, 520,  622, 522,  624, 1630,  648, 1650,  624, 1646,  624, 1650,  622, 1650,  624, 1650,
                                   624, 1652,  624, 1652,  624, 1628,  650, 518,  624, 518,  622, 518,  622, 520,  622, 520,  624, 518,  624, 1628,  648, 1628,  648, 1650,  624, 542,  598, 520,  622, 542,  600, 520,  624,
                                   520,  622, 524,  624, 522,  624, 518,  626, 516,  622, 518,  624, 518,  624, 518,  624, 520,  624, 1630,  648, 1628,  650, 518,  624, 516,  624, 542,  600, 518,  622, 520,  622, 522,  624,
                                   522,  624, 500,  648, 518,  624, 516,  624, 542,  600, 520,  622, 520,  624, 520,  624, 1626,  648, 522,  624, 518,  624, 540,  600, 542,  600, 518,  622, 520,  624, 520,  624, 520,  624, 522,
                                   624, 518,  626, 516,  622, 518,  622, 542,  600, 520,  622, 520,  624, 522,  624, 498,  646, 520,  624, 540,  600, 518,  624, 518,  624, 518,  624, 518,  624, 498,  648, 524,  622, 518,  624, 540,
                                   600, 518,  622, 518,  624, 520,  622, 520,  626, 520,  624, 522,  624, 518,  624, 1646,  624, 518,  624, 1648,  624, 520,  622, 518,  626, 520,  624, 522,  624, 518,  624, 1648,  624, 1648,  624,
                                   1650,  622, 520,  622, 520,  624, 1628,  648, 522,  624, 1648,  624
                                  };  // 23 derece cool fan1

uint16_t derece24_fan1_on[211] = {9074, 4530,  544, 1674,  600, 1672,  624, 518,  598, 544,  624, 520,  598, 526,  646, 1628,  648, 1650,  626, 1644,  600, 1674,  598, 1674,  626, 518,  624,
                                  518,  626, 520,  624, 500,  648, 1628,  646, 514,  600, 544,  598, 544,  598, 544,  624, 518,  626, 1628,  648, 1630,  646, 1650,  624, 516,  600, 542,  598, 544,  600, 542,  624, 518,
                                  626, 520,  624, 522,  624, 518,  626, 512,  600, 542,  598, 544,  624, 518,  626, 518,  624, 1628,  648, 1628,  650, 518,  598, 540,  598, 544,  598, 544,  598, 546,  598, 544,  626, 520,
                                  626, 500,  646, 518,  626, 514,  600, 542,  624, 518,  626, 520,  598, 544,  624, 1628,  648, 500,  646, 518,  624, 536,  580, 542,  598, 546,  598, 544,  622, 520,  624, 500,  646, 522,
                                  624, 518,  598, 540,  600, 544,  598, 544,  598, 544,  626, 518,  624, 522,  624, 500,  646, 520,  624, 514,  600, 542,  604, 538,  626, 516,  626, 516,  626, 1628,  648, 500,  646, 520,
                                  624, 514,  600, 542,  598, 544,  626, 518,  624, 518,  624, 500,  646, 524,  624, 518,  624, 536,  580, 542,  624, 518,  624, 518,  626, 518,  624, 520,  626, 498,  648, 518,  624, 516,
                                  624, 1648,  620, 522,  598, 1676,  624, 520,  624, 522,  624, 1628,  650, 1644,  624
                                 };  // 24 derece cool fan1

uint16_t derece25_fan1_on[211] = {9066, 4512,  566, 1672,  598, 1676,  596, 546,  622, 520,  624, 520,  624, 522,  624, 1630,  648, 1650,  624, 1646,  598, 1672,  626, 1648,  624, 1630,  648,
                                  518,  598, 528,  644, 500,  646, 1628,  648, 514,  600, 544,  598, 544,  624, 518,  624, 518,  624, 1630,  648, 1628,  648, 1628,  646, 514,  602, 542,  598, 544,  596, 546,  598, 544,  626, 498,
                                  648, 498,  646, 518,  624, 514,  600, 542,  596, 544,  624, 520,  626, 518,  622, 1630,  648, 1630,  648, 520,  598, 544,  598, 542,  596, 544,  598, 546,  598, 544,  624, 500,  646, 498,  646, 518,
                                  624, 516,  598, 546,  596, 546,  624, 518,  622, 522,  624, 1628,  648, 500,  646, 518,  622, 542,  572, 542,  624, 518,  624, 520,  624, 518,  624, 500,  646, 500,  648, 518,  624, 516,  624, 544,
                                  572, 544,  596, 544,  624, 520,  624, 520,  624, 500,  646, 520,  622, 518,  596, 570,  572, 544,  622, 520,  624, 520,  624, 1628,  648, 500,  646, 518,  624, 518,  598, 568,  572, 570,  572, 546,
                                  622, 520,  626, 520,  624, 500,  646, 518,  626, 514,  600, 542,  622, 520,  624, 520,  622, 520,  624, 522,  624, 498,  646, 520,  624, 518,  624, 1648,  622, 520,  622, 520,  596, 1656,  646, 522,
                                  624, 1630,  648, 1646,  598
                                 };  // 25 derece cool fan1

uint16_t derece25_fan2_on[211] = {9048, 4558,  544, 1672,  600, 1672,  600, 542,  600, 544,  600, 544,  600, 546,  600, 1654,  648, 1628,  646, 1646,  600, 1674,  600, 1672,  602, 1674,  600, 542,  600,
                                  524,  620, 524,  622, 1652,  646, 530,  586, 542,  598, 542,  600, 544,  598, 544,  598, 1654,  646, 1630,  648, 1628,  646, 514,  600, 542,  598, 544,  598, 544,  598, 544,  600, 524,  622, 526,  622,
                                  542,  602, 538,  600, 542,  598, 546,  596, 542,  600, 544,  598, 546,  600, 1654,  648, 516,  600, 538,  600, 542,  600, 542,  600, 542,  600, 542,  600, 546,  600, 524,  622, 542,  602, 538,  602, 540,
                                  600, 542,  600, 544,  600, 544,  600, 1654,  622, 524,  622, 542,  602, 538,  600, 542,  600, 542,  600, 544,  600, 544,  600, 522,  622, 524,  622, 542,  600, 540,  600, 542,  598, 544,  598, 544,  600,
                                  544,  598, 524,  622, 524,  622, 542,  600, 562,  578, 542,  598, 544,  598, 544,  598, 544,  598, 1656,  622, 528,  644, 516,  600, 540,  600, 540,  600, 542,  600, 544,  600, 544,  598, 526,  620, 526,
                                  622, 542,  602, 538,  600, 540,  600, 1672,  600, 544,  598, 544,  600, 546,  598, 524,  622, 542,  600, 542,  600, 1672,  600, 1672,  598, 544,  600, 1668,  608, 1654,  648, 498,  622, 1670,  602
                                 };  // 25 derece cool fan2

uint16_t derece25_fan3_on[211] = {9048, 4534,  568, 1670,  600, 1672,  600, 542,  604, 540,  600, 544,  600, 546,  600, 1654,  648, 1650,  602, 1668,  602, 1672,  600, 1672,  598, 1654,  648, 518,  624, 500,
                                  648, 498,  648, 1628,  648, 512,  602, 568,  572, 542,  598, 542,  600, 542,  600, 1654,  648, 1630,  622, 1654,  622, 540,  602, 540,  600, 542,  598, 544,  600, 544,  598, 524,  620, 524,  622, 542,  600,
                                  538,  602, 542,  600, 540,  600, 542,  600, 544,  600, 1656,  622, 524,  624, 542,  600, 540,  600, 542,  600, 540,  600, 544,  598, 544,  600, 522,  622, 524,  622, 542,  600, 542,  600, 542,  600, 540,
                                  600, 544,  600, 542,  600, 1656,  622, 524,  646, 518,  600, 562,  578, 568,  574, 542,  598, 544,  598, 544,  600, 524,  622, 524,  648, 516,  600, 540,  600, 544,  598, 542,  602, 542,  600, 544,  600,
                                  546,  600, 524,  624, 540,  600, 540,  600, 542,  600, 542,  598, 544,  598, 544,  600, 1654,  624, 524,  624, 540,  602, 538,  602, 542,  600, 542,  600, 542,  600, 542,  600, 524,  622, 524,  646, 518,
                                  600, 538,  602, 540,  600, 1672,  600, 542,  600, 544,  600, 546,  598, 524,  622, 542,  600, 542,  600, 1674,  598, 1674,  600, 542,  602, 1652,  646, 500,  622, 524,  648, 1644,  602
                                 };  // 25 derece cool fan3

uint16_t kapanma30[211] = {9086, 4480,  596, 1672,  598, 1674,  598, 544,  598, 544,  624, 520,  624, 500,  646, 1628,  648, 1628,  648, 1644,  600, 1674,  598, 1676,  596, 1678,  624, 518,  626, 500,
                           646, 500,  646, 1628,  646, 514,  600, 544,  598, 542,  598, 544,  624, 518,  624, 1632,  646, 1630,  646, 1628,  648, 514,  598, 542,  600, 544,  598, 544,  598, 546,  598, 528,  644, 500,  646, 518,
                           624, 514,  600, 542,  598, 546,  596, 546,  594, 548,  598, 1654,  646, 1630,  646, 518,  600, 542,  600, 1672,  598, 1674,  626, 1648,  624, 1630,  648, 500,  648, 500,  646, 518,  600, 540,  600, 542,
                           600, 542,  598, 544,  624, 520,  624, 1630,  646, 502,  646, 518,  598, 540,  600, 542,  600, 542,  598, 544,  598, 544,  600, 524,  646, 500,  648, 518,  598, 540,  602, 540,  600, 544,  598, 544,  598,
                           544,  624, 500,  646, 500,  646, 518,  626, 514,  600, 542,  598, 544,  598, 544,  598, 544,  598, 1658,  644, 1630,  648, 518,  598, 542,  598, 542,  598, 544,  598, 544,  598, 544,  624, 500,  646, 500,
                           646, 518,  624, 1646,  600, 542,  598, 1674,  598, 1656,  646, 518,  598, 526,  644, 502,  646, 518,  598, 1672,  600, 544,  598, 1674,  624, 1650,  624, 1630,  646, 1630,  646, 500,  646, 514,  602
                          };  // 30dk sonra kapanma 25 derece cool

uint16_t kapanma60[211] = {9088, 4486,  588, 1672,  604, 1670,  626, 516,  626, 518,  626, 518,  626, 518,  626, 1628,  648, 1626,  648, 1642,  602, 1670,  602, 1672,  600, 1654,  648, 516,  626, 520,
                           626, 496,  648, 1650,  626, 536,  580, 542,  624, 516,  626, 516,  628, 516,  626, 1626,  650, 1628,  648, 1628,  648, 536,  578, 568,  572, 544,  598, 544,  600, 542,  600, 546,  600, 522,  648, 516,
                           602, 1668,  602, 540,  600, 542,  600, 542,  600, 544,  600, 1654,  624, 1654,  622, 542,  602, 538,  600, 542,  600, 542,  600, 542,  600, 544,  598, 546,  600, 546,  600, 542,  600, 540,  600, 540,
                           600, 542,  600, 542,  598, 542,  602, 1654,  622, 524,  646, 518,  600, 538,  602, 540,  600, 542,  600, 544,  598, 544,  600, 546,  600, 524,  622, 540,  602, 540,  600, 566,  574, 544,  598, 544,  598,
                           544,  600, 524,  622, 524,  622, 542,  600, 538,  600, 540,  600, 542,  600, 542,  600, 544,  600, 1654,  620, 1654,  624, 542,  600, 538,  604, 540,  600, 542,  600, 544,  598, 544,  600, 524,  622, 524,
                           624, 538,  602, 1670,  600, 540,  600, 1672,  600, 1674,  600, 542,  602, 522,  622, 524,  622, 542,  600, 540,  600, 564,  576, 544,  598, 544,  598, 544,  600, 1654,  622, 522,  624, 538,  602
                          };  // 60dk sonra kapanma 25 derece cool

uint16_t heat_fan2_swing[211] = {9086, 4512,  564, 1698,  582, 1664,  624, 518,  622, 520,  624, 520,  622, 522,  624, 1630,  648, 1630,  646, 516,  624, 544,  572, 544,  626, 516,  626, 518,  622, 1630,  648, 522,
                                 624, 1650,  624, 540,  600, 518,  624, 542,  598, 518,  626, 518,  624, 1628,  648, 1628,  648, 1652,  624, 516,  624, 542,  598, 518,  622, 520,  624, 520,  622, 524,  624, 522,  624, 518,  624, 542,  598,
                                 544,  598, 520,  622, 518,  624, 520,  622, 522,  624, 1628,  648, 520,  622, 540,  600, 542,  598, 520,  624, 518,  624, 520,  624, 522,  624, 522,  626, 518,  626, 540,  600, 518,  622, 522,  622, 518,
                                 624, 520,  624, 522,  624, 498,  646, 1652,  622, 516,  624, 520,  622, 518,  624, 520,  622, 522,  622, 522,  626, 498,  648, 518,  624, 516,  600, 568,  596, 520,  622, 520,  624, 518,  624, 522,  624,
                                 500,  646, 518,  624, 516,  626, 542,  584, 532,  624, 518,  624, 1630,  648, 1628,  648, 498,  646, 520,  624, 516,  598, 544,  598, 544,  624, 518,  624, 520,  624, 520,  624, 500,  646, 518,  622, 518,
                                 600, 1672,  626, 518,  598, 546,  624, 520,  624, 498,  646, 500,  648, 518,  624, 1646,  598, 542,  598, 1676,  624, 518,  624, 1628,  646, 1630,  646, 500,  646, 514,  600
                                };  // heat 28 derece fan 2 swing v


////////////////////////////////////////////////////////////////////////////////////////////



void setup()
{

  Serial.begin(115200);

  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
  Serial.print("Ağ Bağlantısı Oluşturuluyor");
  while (WiFi.status() != WL_CONNECTED)
  {
    Serial.print(".");
    delay(300);
  }
  Serial.println();
  Serial.print("IP adresine bağlanıldı: ");
  Serial.println(WiFi.localIP());
  Serial.println();


  //3. Firebase bağlantısı başlatılıyor

  Firebase.begin(FIREBASE_HOST, FIREBASE_AUTH);

  //4. Ağ bağlantısı kesilirse tekrar bağlanmasına izin veriyoruz
  Firebase.reconnectWiFi(true);
  //D2 pinini çıkış olarak tanımladık
  pinMode(D2, OUTPUT);

  //bu bölümde aşağıda tanımladığımız fonksiyonlar ile veritabanından veri çekiyoruz ve veritabanındaki son değerleri aşağıdaki durum değerlerine atamak için kullanacağız
  fangetir();
  sicaklikgetir();
  kapanmagetir();
  onoffgetir();
  isitmagetir();

  //veritabanından aldığımız ilk açılış değerlerini durum değişkenlerimize atıyoruz.
  butondurumu = butonstatus;
  sicaklikdurumu = sicaklik;
  fandurumu = fanhizi;
  kapanmadurumu = kapanma;
  isitmadurumu = isitma;

}

void loop()
{
  while (butonstatus == 0) { //Klima durumu kapalı ise hiçbir işlem yapmayacak
    onoffgetir(); //whiledan çıkış için klima onoff durumunu sürekli kontrol ediyor.
    Serial.println("Klima Kapalı");
  }

 //her döngüde verileri tekrar veritabanından çekip güncelliyoruz.
  fangetir();
  sicaklikgetir();
  kapanmagetir();
  onoffgetir();
  isitmagetir();

  if (butondurumu == 0) { //klimanın kapalı olma durumu

    if (butonstatus == 1) { //açma komutu geliyor ve işleniyor
      irsend.sendRaw(klimaAC, 211, 38);
      Serial.println("Klima Açma Komutu gönderildi");
      butondurumu = 1;
      fandurumu = 1;
      sicaklikdurumu = 25;//fan durumu ve sicaklik durumu bilgisini de olması gereken şekle atıyoruz.

    }


  }
  else {//klimanın açık olduğu durum

    if (butonstatus == 0) { //kapatma komutu gelirse kapat çık
      irsend.sendRaw(klimaKAPAT, 211, 38);
      Serial.println("Klima Kapatma Komutu gönderildi");
      butondurumu = 0;
    }
    else { //klimanın açık olduğu durumda yapılacak işlemler burada işlenecek

      if (sicaklik != sicaklikdurumu) {
        //bizim tuttuğumuz sıcaklık durum değişkeni ile veritabanından gelen veri eşit değilse demek ki durumda bir değişiklik olmuş
        //o nedenle yeni değere göre ne işlem yapılacağını belirleyeceğiz.
        //25-24 ve 23 derece için değerlendiriyoruz.
        switch (sicaklik)
        {
          case 25: // 25
            //Serial.println("Sıcaklık 25 derece");
            switch (fanhizi) {

              case 1: // fan 1
                //Serial.println("fan 1");
                irsend.sendRaw(derece25_fan1_on, 211, 38);
                Serial.println("25 derece Fan 1 Komutu gönderildi");
                fandurumu = 1;
                sicaklikdurumu = 25;
                break;

              case 2: // fan 2
                //Serial.println("fan 2");
                irsend.sendRaw(derece25_fan2_on, 211, 38);
                Serial.println("25 derece Fan 2 Komutu gönderildi");
                fandurumu = 2;
                sicaklikdurumu = 25;
                break;

              case 3: // fan 3
                //Serial.println("fan 3");
                irsend.sendRaw(derece25_fan3_on, 211, 38);
                Serial.println("25 derece Fan 2 Komutu gönderildi");
                fandurumu = 3;
                sicaklikdurumu = 25;
                break;

              default:
                break;
            }

            break;

          case 23: // 23
            //Serial.println("Sıcaklık 23 derece");
            switch (fanhizi) {

              case 1: // fan 1
                //Serial.println("fan 1");
                irsend.sendRaw(derece23_fan1_on, 211, 38);
                Serial.println("23 derece Fan 1 Komutu gönderildi");
                fandurumu = 1;
                sicaklikdurumu = 23;
                break;

              case 2: // fan 2
                //Serial.println("fan 2");
                irsend.sendRaw(derece23_fan2_on, 211, 38);
                Serial.println("23 derece Fan 2 Komutu gönderildi");
                fandurumu = 2;
                sicaklikdurumu = 23;
                break;

              case 3: // fan 3
                //Serial.println("fan 3");
                irsend.sendRaw(derece23_fan3_on, 211, 38);
                Serial.println("23 derece Fan 3 Komutu gönderildi");
                fandurumu = 3;
                sicaklikdurumu = 23;
                break;

              default:
                break;
            }

            break;
          case 21: // 21
            //Serial.println("Sıcaklık 21 derece");
            switch (fanhizi) {

              case 1: // fan 1
                //Serial.println("fan 1");
                irsend.sendRaw(derece21_fan1_on, 211, 38);
                Serial.println("21 derece Fan 1 Komutu gönderildi");
                fandurumu = 1;
                sicaklikdurumu = 21;
                break;

              case 2: // fan 2
                //Serial.println("fan 2");
                irsend.sendRaw(derece21_fan2_on, 211, 38);
                Serial.println("21 derece Fan 2 Komutu gönderildi");
                fandurumu = 2;
                sicaklikdurumu = 21;
                break;

              case 3: // fan 3
                //Serial.println("fan 3");
                irsend.sendRaw(derece21_fan3_on, 211, 38);
                Serial.println("21 derece Fan 2 Komutu gönderildi");
                fandurumu = 3;
                sicaklikdurumu = 21;
                break;

              default:
                break;
            }

            break;

          default:
            //Serial.println("Seçim dışı SICAKLIK değer");
            break;
        }//swtich (sicaklik)
      }// if sicaklik!=sicaklikdurum

      if (fanhizi != fandurumu) {
        switch (fanhizi)
        {
          case 1: // 1
            //Serial.println("fan1");
            switch (sicaklik) {

              case 25: // 25
                //Serial.println("Sıcaklık 25 derece");
                irsend.sendRaw(derece25_fan1_on, 211, 38);
                Serial.println("25 derece Fan 1 Komutu gönderildi");
                fandurumu = 1;
                sicaklikdurumu = 25;
                break;

              case 23: // 23
                //Serial.println("Sıcaklık 23 derece");
                irsend.sendRaw(derece23_fan1_on, 211, 38);
                Serial.println("23 derece Fan 1 Komutu gönderildi");
                fandurumu = 1;
                sicaklikdurumu = 23;
                break;

              case 21: // 21
                //Serial.println("Sıcaklık 21 derece");
                irsend.sendRaw(derece21_fan1_on, 211, 38);
                Serial.println("21 derece Fan 1 Komutu gönderildi");
                fandurumu = 1;
                sicaklikdurumu = 21;
                break;

              default:
                break;
            }

            break;

          case 2: // 2
            //Serial.println("fan2");
            switch (sicaklik) {

              case 25: // 25
                //Serial.println("Sıcaklık 25 derece");
                irsend.sendRaw(derece25_fan2_on, 211, 38);
                Serial.println("25 derece Fan 2 Komutu gönderildi");
                fandurumu = 2;
                sicaklikdurumu = 25;
                break;

              case 23: // 23
                //Serial.println("Sıcaklık 23 derece");
                irsend.sendRaw(derece23_fan2_on, 211, 38);
                Serial.println("23 derece Fan 2 Komutu gönderildi");
                fandurumu = 2;
                sicaklikdurumu = 23;
                break;

              case 21: // 21
                //Serial.println("Sıcaklık 21 derece");
                irsend.sendRaw(derece21_fan2_on, 211, 38);
                Serial.println("21 derece Fan 2 Komutu gönderildi");
                fandurumu = 2;
                sicaklikdurumu = 21;
                break;

              default:
                break;
            }

            break;
          case 3: // 3
            ///Serial.println("fan3");
            switch (sicaklik) {

              case 25: // 25
                //Serial.println("Sıcaklık 25 derece");
                irsend.sendRaw(derece25_fan3_on, 211, 38);
                Serial.println("25 derece Fan 3 Komutu gönderildi");
                fandurumu = 3;
                sicaklikdurumu = 25;
                break;

              case 23: // 23
                //Serial.println("Sıcaklık 23 derece");
                irsend.sendRaw(derece23_fan3_on, 211, 38);
                Serial.println("23 derece Fan 3 Komutu gönderildi");
                fandurumu = 3;
                sicaklikdurumu = 23;
                break;

              case 21: // 21
                //Serial.println("Sıcaklık 21 derece");
                irsend.sendRaw(derece21_fan3_on, 211, 38);
                Serial.println("21 derece Fan 3 Komutu gönderildi");
                fandurumu = 3;
                sicaklikdurumu = 21;
                break;

              default:
                break;
            }

            break;

          default:
            //Serial.println("Seçim dışı FAN Hızı Değeri");
            break;
        }//swtich (fanhizi)
      }// if fanhizi!=fandurumu

      if (kapanma != kapanmadurumu) {//otomatik kapanma ile ilgili komut var mı onu kontrol ediyoruz.


        switch (kapanma) {

          case 0: // yok
            //Serial.println("oto kapanma yok");
            irsend.sendRaw(derece25_fan1_on, 211, 38);
            Serial.println("Oto kapanma yok Komutu gönderildi");
            kapanmadurumu = 0;
            fandurumu = 1;
            sicaklikdurumu = 25;

            break;

          case 30: // 30dk
            //Serial.println("oto kapanma 30dk");
            irsend.sendRaw(kapanma30, 211, 38);
            Serial.println("Oto kapanma 30dk Komutu gönderildi");
            kapanmadurumu = 30;

            break;

          case 60: // 60dk
            //Serial.println("oto kapanma 60dk");
            irsend.sendRaw(kapanma60, 211, 38);
            Serial.println("Oto kapanma 60dk Komutu gönderildi");
            kapanmadurumu = 60;

            break;

          default:
            break;
        }


      }

      if (isitma != isitmadurumu) { //ısıtma modu ile ilgili komut var mı onu kontrol ediyoruz.

        if (isitma == 1) {
          //Serial.println("Isıtma açıldı");
          irsend.sendRaw(heat_fan2_swing, 211, 38);
          Serial.println("Isıtma Modunu Açma Komutu gönderildi");
          isitmadurumu = 1;

        }
        else {
          //Serial.println("Isıtma kapatıldı");
          irsend.sendRaw(derece25_fan1_on, 211, 38);
          Serial.println("Isıtma Modunu Kapatma Komutu gönderildi");
          isitmadurumu = 0;
          fandurumu = 1;
          sicaklikdurumu = 25;


        }

      }



    }//else klima açık durumu


  }




} //void loop

//kullandığımız fonksiyonlarımız

void fangetir() {

  if (Firebase.getString(veritabanim, "/fanhizi")) //Alınacak veri tipine göre getInt, getBool, getFloat, getDouble, getString olarak kullanılabilir.
  {
    //bağlantı başarılı ve veri geliyor ise
    //Serial.print("String tipinde veri alımı başarılı, veri = ");
    //Serial.println(veritabanim.stringData());
    String str = veritabanim.stringData();
    str = str.substring(2, 3);
    // Serial.print("Fanhızı veritabanından gelen değer = ");
    //Serial.println(str);

    fanhizi = str.toInt();

    //Serial.print("Fanhızı tipinde veri alımı başarılı, veri = ");
    //Serial.println(fanhizi);

  } else {
    //hata varsa hata mesajı ve nedeni yazdırılıyor

    Serial.print("veri çekilemedi, ");
    Serial.println(veritabanim.errorReason());
  }

}

void sicaklikgetir() {

  if (Firebase.getString(veritabanim, "/sicaklik")) //Alınacak veri tipine göre getInt, getBool, getFloat, getDouble, getString olarak kullanılabilir.
  {
    //bağlantı başarılı ve veri geliyor ise
    //Serial.print("String tipinde veri alımı başarılı, veri = ");
    //Serial.println(veritabanim.stringData());

    String str = veritabanim.stringData();
    str = str.substring(2, 4);
    //Serial.print("Sıcaklık veritabanından gelen değer = ");
    //Serial.println(str);
    sicaklik = str.toInt();
    //Serial.print("Sıcaklık tipinde veri alımı başarılı, veri = ");
    //Serial.println(sicaklik);

  } else {
    //hata varsa hata mesajı ve nedeni yazdırılıyor

    Serial.print("veri çekilemedi, ");
    Serial.println(veritabanim.errorReason());
  }

}
void kapanmagetir() {

  if (Firebase.getString(veritabanim, "/kapanma")) //Alınacak veri tipine göre getInt, getBool, getFloat, getDouble, getString olarak kullanılabilir.
  {
    //bağlantı başarılı ve veri geliyor ise
    //Serial.print("String tipinde veri alımı başarılı, veri = ");
    //Serial.println(veritabanim.stringData());
    String str = veritabanim.stringData();
    str = str.substring(2, 4);
    String a = str.substring(2, 3);
    if (a == "0") {
      str = "0";
    }
    //Serial.print("Kapanma veritabanından gelen değer = ");
    //Serial.println(str);
    kapanma = str.toInt();
    // Serial.print("otomatik kapanma tipinde veri alımı başarılı, veri = ");
    //Serial.println(kapanma);


  } else {
    //hata varsa hata mesajı ve nedeni yazdırılıyor

    Serial.print("veri çekilemedi, ");
    Serial.println(veritabanim.errorReason());
  }

}
void onoffgetir() {

  if (Firebase.getString(veritabanim, "/onoff")) //Alınacak veri tipine göre getInt, getBool, getFloat, getDouble, getString olarak kullanılabilir.
  {
    //bağlantı başarılı ve veri geliyor ise
    //Serial.print("String tipinde veri alımı başarılı, veri = ");
    //Serial.println(veritabanim.stringData());
    String str = veritabanim.stringData();
    // Serial.print("OnOFF veritabanından gelen değer = ");
    // Serial.println(str);
    butonstatus = str.toInt();
    //Serial.print("On OFF tipinde veri alımı başarılı, veri = ");
    //Serial.println(butonstatus);


  } else {
    //hata varsa hata mesajı ve nedeni yazdırılıyor

    Serial.print("veri çekilemedi, ");
    Serial.println(veritabanim.errorReason());
  }

}
void isitmagetir() {

  if (Firebase.getString(veritabanim, "/isitma")) //Alınacak veri tipine göre getInt, getBool, getFloat, getDouble, getString olarak kullanılabilir.
  {
    //bağlantı başarılı ve veri geliyor ise
    //Serial.print("String tipinde veri alımı başarılı, veri = ");
    //Serial.println(veritabanim.stringData());
    String str = veritabanim.stringData();
    //Serial.print("ISITMA veritabanından gelen değer = ");
    // Serial.println(str);
    isitma = str.toInt();
    //Serial.print("Sıcakmod tipinde veri alımı başarılı, veri = ");
    //Serial.println(isitma);


  } else {
    //hata varsa hata mesajı ve nedeni yazdırılıyor

    Serial.print("veri çekilemedi, ");
    Serial.println(veritabanim.errorReason());
  }

}

void varitabaniguncelle(String veriadi, int veridegeri) {
  veriadi = "/" + veriadi;
  String yazilacak = String(veridegeri);
  if (Firebase.setString(veritabanim, veriadi, yazilacak))
  {
    //bağlantı başarılı ve veri geliyor ise
    Serial.println("veri gönderimi başarılı");

  } else {
    //hata varsa hata mesajı ve nedeni yazdırılıyor

    Serial.print("veri gönderilemedi, ");
    Serial.println(veritabanim.errorReason());
  }


}
