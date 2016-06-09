#include <SPI.h>

#define RADIO_CE   2
#define RADIO_CSN  11
#define RADIO_SCK  15
#define RADIO_MOSI 16
#define RADIO_MISO 14
#define RADIO_IRQ  3


void startupSPI(){
  pinMode     (RADIO_CE , OUTPUT);
  digitalWrite(RADIO_CE , LOW);//Enable the PA
  pinMode     (RADIO_CSN , OUTPUT);
  digitalWrite(RADIO_CSN, HIGH);
  pinMode     (RADIO_SCK, OUTPUT);
  digitalWrite(RADIO_SCK, LOW);
  pinMode     (RADIO_MOSI, OUTPUT);
  digitalWrite(RADIO_MOSI, LOW);
  pinMode     (RADIO_MISO, INPUT);
  pinMode     (RADIO_IRQ,  INPUT);
}
char spiTransfer(char a){
  char b=0b10000000;
  char d = 0;
  digitalWrite(RADIO_SCK, LOW);
  while(b!=0){
    if(b&a)digitalWrite(RADIO_MOSI, HIGH);
    else   digitalWrite(RADIO_MOSI, LOW);
    digitalWrite(RADIO_SCK, HIGH);
    if(digitalRead(RADIO_MISO))d|=b;
    b = (b>>1)&0b01111111;
    digitalWrite(RADIO_SCK, LOW);
  }
  return d;
}
void spiStart(){
  digitalWrite(RADIO_CSN, LOW);
}
void spiStop(){
  digitalWrite(RADIO_CSN, HIGH);
}
void powerUp(){
  digitalWrite(RADIO_CE , HIGH);//Enable the PA
}
void powerDown(){
  digitalWrite(RADIO_CE , LOW);//Disables the PA
} 
char IRQState(){
  return !digitalRead(RADIO_IRQ);
}
char NOP(){
  char statusR;
  spiStart();
  statusR = spiTransfer(  0xFF );
  spiStop();
  return statusR;
}
char FLUSH_TX(){
  char statusR;
  spiStart();
  statusR = spiTransfer(  0xE1 );
  spiStop();
  return statusR;
}
char FLUSH_RX(){
  char statusR;
  spiStart();
  statusR = spiTransfer(  0xE2 );
  spiStop();
  return statusR;
}
char REUSE_TX_PL(){
  char statusR;
  spiStart();
  statusR = spiTransfer(  0xE3 );
  spiStop();
  return statusR;
}
char R_REGISTER(char addr, char* ret, char count){
  char statusR;
  if(count>5 || count<1)return NOP();
  spiStart();
  statusR = spiTransfer( 0x00|(addr&0x1F) );
  while(count>0){
    *ret = spiTransfer(0);
    ret++;
    count--;
  }
  spiStop();
  return statusR;
}
char R_REGISTER(char addr){
  char ret;
  spiStart();
  spiTransfer( 0x00|(addr&0x1F) );
  ret = spiTransfer(0);
  spiStop();
  return ret;
}
char W_REGISTER(char addr, char* ret, char count){
  char statusR;
  if(count>5 || count<1)return NOP();
  spiStart();
  statusR = spiTransfer( 0x20|(addr&0x1F) );
  while(count>0){
    spiTransfer(*ret);
    ret++;
    count--;
  }
  spiStop();
  return statusR;
}
char W_REGISTER(char addr, char data){
  char statusR;
  spiStart();
  statusR = spiTransfer( 0x20|(addr&0x1F) );
  spiTransfer(data);
  spiStop();
  return statusR;
}
char R_RX_PAYLOAD(char* ret, char count){
  char statusR;
  if(count>32 || count<1)return NOP();
  spiStart();
  statusR = spiTransfer( 0x61 );
  while(count>0){
    *ret = spiTransfer(0);
    ret++;
    count--;
  }
  spiStop();
  return statusR;
}
char W_TX_PAYLOAD(char* ret, char count){
  char statusR;
  if(count>32 || count<1)return NOP();
  spiStart();
  statusR = spiTransfer( 0xA0 );
  while(count>0){
    spiTransfer(*ret);
    ret++;
    count--;
  }
  spiStop();
  return statusR;
}
char W_TX_PAYLOAD_NOACK(char* ret, char count){
  char statusR;
  if(count>32 || count<1)return NOP();
  spiStart();
  statusR = spiTransfer( 0xB0 );
  while(count>0){
    spiTransfer(*ret);
    ret++;
    count--;
  }
  spiStop();
  return statusR;
}
char R_RX_PL_WID(){
  char count;
  spiStart();
  spiTransfer( 0x60 );
  count = spiTransfer( 0 );
  spiStop();
  return count;
}
char ACTIVATE(){
  char statusR;
  spiStart();
  statusR = spiTransfer( 0x50 );
  spiTransfer(0x73);
  spiStop();
  return statusR;
}
void radioRXMode(){
  char d = R_REGISTER(0);
  powerUp();
  W_REGISTER(0,d|3);
  delay(3);
}
void dataTransmit(char* data, char Size){
  powerDown();
  W_TX_PAYLOAD(data, Size);
  char d = R_REGISTER(0);
  W_REGISTER(0, d&(~1));
  powerUp();
  delayMicroseconds(20);
  powerDown();
  W_REGISTER(0, d);
  powerUp();
}
char NRF_disponivel(){
  char a = (NOP()>>1)&0x07;
  if(a==0x07)return 0;
  return a+1;
}
char NRF_receber(char* payload){
  char packetSize;
  packetSize = R_RX_PL_WID();
  R_RX_PAYLOAD(payload, packetSize);
  W_REGISTER(0x07, 0x70);
  return packetSize;
}
void NRF_enviar(char* payload, char packetSize){
  dataTransmit(payload,packetSize);
  W_REGISTER(0x07, 0x70);
}
void NRF_begin(){
  char nRF_CHANNEL = 10;
  char nRX_ENABLED = (1<<0);
  char nTX_CHANNEL[5] = {0x11, 0x11, 0x11, 0x11, 0x11};
  char nRX_CHANNEL_0[5] = {0x11, 0x11, 0x11, 0x11, 0x11};
  //char nRX_CHANNEL_1[5];
  //char nRX_CHANNEL_2, nRX_CHANNEL_3, nRX_CHANNEL_4, nRX_CHANNEL_5;
  startupSPI();
  delay(11);
  ACTIVATE();
  radioRXMode();
  
  W_REGISTER(0x05, nRF_CHANNEL&0x7F);
  W_REGISTER(0x02, nRX_ENABLED&0x3F);
  W_REGISTER(0x10, nTX_CHANNEL, 5);
  W_REGISTER(0x0A, nRX_CHANNEL_0, 5);
  //W_REGISTER(0x0B, nRX_CHANNEL_1, 5);
  //W_REGISTER(0x0C, nRX_CHANNEL_2);
  //W_REGISTER(0x0D, nRX_CHANNEL_3);
  //W_REGISTER(0x0E, nRX_CHANNEL_4);
  //W_REGISTER(0x0F, nRX_CHANNEL_5);
  W_REGISTER(0x1C, 0x3F);//enable dynamic payload on all pipes
  W_REGISTER(0x1D, 4);//enable dynamic payload
  FLUSH_RX();
  FLUSH_TX();
  W_REGISTER(0x07, 0x70);
}
