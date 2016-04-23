#include <cstdlib>
#include <iostream>
#include <sstream>
#include <string>
#include <unistd.h>
#include <RF24/RF24.h>

#define GPIO_CE_PIN 25
#define GPIO_IRQ_PIN 0

#define RADIO_CHANNEL 0x4c

#define PAYLOAD_SIZE 10
#define LOOP_DELAY 1000
#define RETRIES_DELAY 15
#define RETRIES_COUNT 15
#define AUTO_ACK 0

using namespace std;

RF24 radio(GPIO_CE_PIN, GPIO_IRQ_PIN);

const uint64_t pipes[] = { 0x4152443031LL, 0x5250493031LL };

void setup() {
  radio.begin();
  radio.setRetries(RETRIES_DELAY, RETRIES_COUNT);
  radio.setChannel(RADIO_CHANNEL);
  radio.setAutoAck(AUTO_ACK);  
  radio.setDataRate(RF24_1MBPS);
  radio.openWritingPipe(pipes[0]);
  radio.openReadingPipe(1, pipes[1]);
  radio.startListening();  
  radio.printDetails();
}

int loop() {
  int error = 0;
  bool status; 
  char request[PAYLOAD_SIZE];
  char response[PAYLOAD_SIZE];
  
  // Richiesta
  if (radio.available()) {
    radio.read(request, PAYLOAD_SIZE);
    cout << "Ricezione OK" << " - Richiesta:   " << request << endl;            
  }  
    
  // Riposta
  radio.stopListening();  
  strcpy(response, "RES000001");  
  status = radio.write(response, PAYLOAD_SIZE);
  if (status) {    
    cout << "Trasmissione OK" << " - Risposta: " << response << endl;    
  } else {
    cout << "*** Errore di trasmissione ***" << endl;
  }
  radio.startListening();
  
  delay(LOOP_DELAY);
  
  cout << "---------------------------------------------------------------" << endl;
  
  return error;
}

int main(int argc, char** argv){    
  int error = 0;
  setup();
  while (!error) {
    error = loop();
  }
}