#include <EEPROM.h>
#define VERSION 125
char RF_CHANNEL;
char RX_ENABLED;
char TX_CHANNEL[5];
char RX_CHANNEL_0[5];
char RX_CHANNEL_1[5];
char RX_CHANNEL_2, RX_CHANNEL_3, RX_CHANNEL_4, RX_CHANNEL_5;
long UART_SPEED;
long NEXT_TICK;
char MODE;
#define TICK_DURATION 1000
char hex2int(char c1, char c2){
  if(c1>='0'&&c1<='9')c1-='0';
  else if(c1>='a'&&c1<='f')c1-='a'-10;
  else if(c1>='A'&&c1<='F')c1-='A'-10;
  else c1 = 0;
  if(c2>='0'&&c2<='9')c2-='0';
  else if(c2>='a'&&c2<='f')c2-='a'-10;
  else if(c2>='A'&&c2<='F')c2-='A'-10;
  else c2 = 0;
  return c1 | (c2<<4);
}
void startupVars(){
  if(EEPROM.read(0)!=VERSION){
    EEPROM.write(1,2);//channel 2
    EEPROM.write(2,1);//Pipes 1 and 2 enabled
    EEPROM.write(3,'B');//EIRB0
    EEPROM.write(4,'O');
    EEPROM.write(5,'M');
    EEPROM.write(6,'B');
    EEPROM.write(7,'A');
    EEPROM.write(8,'B');//EIRB0
    EEPROM.write(9,'O');
    EEPROM.write(10,'M');
    EEPROM.write(11,'B');
    EEPROM.write(12,'A');
    EEPROM.write(13,1); //EIRB1
    EEPROM.write(14,'B');
    EEPROM.write(15,'R');
    EEPROM.write(16,'I');
    EEPROM.write(17,'E');
    EEPROM.write(18,2);
    EEPROM.write(19,3);
    EEPROM.write(20,4);
    EEPROM.write(21,5);
    EEPROM.write(22,0x80);//9600 baud
    EEPROM.write(23,0x25);
    EEPROM.write(24,0x00);
    EEPROM.write(25,0x00);
    EEPROM.write(0,VERSION);
  }
  RF_CHANNEL = EEPROM.read(1);
  RX_ENABLED = EEPROM.read(2);
  TX_CHANNEL[0] = EEPROM.read(3);
  TX_CHANNEL[1] = EEPROM.read(4);
  TX_CHANNEL[2] = EEPROM.read(5);
  TX_CHANNEL[3] = EEPROM.read(6);
  TX_CHANNEL[4] = EEPROM.read(7);
  RX_CHANNEL_0[0] = EEPROM.read(8);
  RX_CHANNEL_0[1] = EEPROM.read(9);
  RX_CHANNEL_0[2] = EEPROM.read(10);
  RX_CHANNEL_0[3] = EEPROM.read(11);
  RX_CHANNEL_0[4] = EEPROM.read(12);
  RX_CHANNEL_1[0] = EEPROM.read(13);
  RX_CHANNEL_1[1] = EEPROM.read(14);
  RX_CHANNEL_1[2] = EEPROM.read(15);
  RX_CHANNEL_1[3] = EEPROM.read(16);
  RX_CHANNEL_1[4] = EEPROM.read(17);
  RX_CHANNEL_2 = EEPROM.read(18);
  RX_CHANNEL_3 = EEPROM.read(19);
  RX_CHANNEL_4 = EEPROM.read(20);
  RX_CHANNEL_5 = EEPROM.read(21);
  UART_SPEED = (EEPROM.read(25)<<24)|(EEPROM.read(24)<<16)|(EEPROM.read(23)<<8)|EEPROM.read(22);
}
void saveVars(){
  EEPROM.write(1,RF_CHANNEL);
  EEPROM.write(2,RX_ENABLED);
  EEPROM.write(3,TX_CHANNEL[0]);
  EEPROM.write(4,TX_CHANNEL[1]);
  EEPROM.write(5,TX_CHANNEL[2]);
  EEPROM.write(6,TX_CHANNEL[3]);
  EEPROM.write(7,TX_CHANNEL[4]);
  EEPROM.write(8,RX_CHANNEL_0[0]);
  EEPROM.write(9,RX_CHANNEL_0[1]);
  EEPROM.write(10,RX_CHANNEL_0[2]);
  EEPROM.write(11,RX_CHANNEL_0[3]);
  EEPROM.write(12,RX_CHANNEL_0[4]);
  EEPROM.write(13,RX_CHANNEL_1[0]);
  EEPROM.write(14,RX_CHANNEL_1[1]);
  EEPROM.write(15,RX_CHANNEL_1[2]);
  EEPROM.write(16,RX_CHANNEL_1[3]);
  EEPROM.write(17,RX_CHANNEL_1[4]);
  EEPROM.write(18,RX_CHANNEL_2);
  EEPROM.write(19,RX_CHANNEL_3);
  EEPROM.write(20,RX_CHANNEL_4);
  EEPROM.write(21,RX_CHANNEL_5);
  EEPROM.write(22,(UART_SPEED>>0)&0xFF);
  EEPROM.write(23,(UART_SPEED>>8)&0xFF);
  EEPROM.write(24,(UART_SPEED>>16)&0xFF);
  EEPROM.write(25,(UART_SPEED>>24)&0xFF);
}
void setupFromVars(){
  Serial1.flush();
  if(UART_SPEED==0)Serial1.end();
  else Serial1.begin(UART_SPEED);
  W_REGISTER(0x05, RF_CHANNEL&0x7F);
  W_REGISTER(0x02, RX_ENABLED&0x3F);
  W_REGISTER(0x10, TX_CHANNEL, 5);
  W_REGISTER(0x0A, RX_CHANNEL_0, 5);
  W_REGISTER(0x0B, RX_CHANNEL_1, 5);
  W_REGISTER(0x0C, RX_CHANNEL_2);
  W_REGISTER(0x0D, RX_CHANNEL_3);
  W_REGISTER(0x0E, RX_CHANNEL_4);
  W_REGISTER(0x0F, RX_CHANNEL_5);
  W_REGISTER(0x1C, 0x3F);//enable dynamic payload on all pipes
  W_REGISTER(0x1D, 4);//enable dynamic payload
  FLUSH_RX();
  FLUSH_TX();
  W_REGISTER(0x07, 0x70);
}
void echoVars(){
  char buff[16];
  Serial.print("Channel: ");
  Serial.println(RF_CHANNEL, DEC);
  Serial.print("Transmiting to: ");
  sprintf(buff, "%2x %2x %2x %2x %2x",TX_CHANNEL[4]&0xFF,TX_CHANNEL[3]&0xFF,
    TX_CHANNEL[2]&0xFF,TX_CHANNEL[1]&0xFF,TX_CHANNEL[0]&0xFF);
  Serial.println(buff);
  if(RX_ENABLED&1){
    Serial.print("Pipe 0: ");
    sprintf(buff, "%2x %2x %2x %2x %2x",RX_CHANNEL_0[4]&0xFF,RX_CHANNEL_0[3]&0xFF,
      RX_CHANNEL_0[2]&0xFF,RX_CHANNEL_0[1]&0xFF,RX_CHANNEL_0[0]&0xFF);
    Serial.println(buff);
  }
  if(RX_ENABLED&2){
    Serial.print("Pipe 1: ");
    sprintf(buff, "%2x %2x %2x %2x %2x",RX_CHANNEL_1[4]&0xFF,RX_CHANNEL_1[3]&0xFF,
      RX_CHANNEL_1[2]&0xFF,RX_CHANNEL_1[1]&0xFF,RX_CHANNEL_1[0]&0xFF);
    Serial.println(buff);
  }
  if(RX_ENABLED&4){
    Serial.print("Pipe 2: ");
    sprintf(buff, "%2x %2x %2x %2x %2x",RX_CHANNEL_1[4]&0xFF,RX_CHANNEL_1[3]&0xFF,
      RX_CHANNEL_1[2]&0xFF,RX_CHANNEL_1[1]&0xFF,RX_CHANNEL_2&0xFF);
    Serial.println(buff);
  }
  if(RX_ENABLED&8){
    Serial.print("Pipe 3: ");
    sprintf(buff, "%2x %2x %2x %2x %2x",RX_CHANNEL_1[4],RX_CHANNEL_1[3],
      RX_CHANNEL_1[2]&0xFF,RX_CHANNEL_1[1]&0xFF,RX_CHANNEL_3&0xFF);
    Serial.println(buff);
  }
  if(RX_ENABLED&16){
    Serial.print("Pipe 4: ");
    sprintf(buff, "%2x %2x %2x %2x %2x",RX_CHANNEL_1[4]&0xFF,RX_CHANNEL_1[3]&0xFF,
      RX_CHANNEL_1[2]&0xFF,RX_CHANNEL_1[1]&0xFF,RX_CHANNEL_4&0xFF);
    Serial.println(buff);
  }
  if(RX_ENABLED&32){
    Serial.print("Pipe 5: ");
    sprintf(buff, "%2x %2x %2x %2x %2x",RX_CHANNEL_1[4]&0xFF,RX_CHANNEL_1[3]&0xFF,
      RX_CHANNEL_1[2]&0xFF,RX_CHANNEL_1[1]&0xFF,RX_CHANNEL_5&0xFF);
    Serial.println(buff);
  }
  Serial.print("UART speed: ");
  if(UART_SPEED==0)Serial.println("Disabled");
  else Serial.println(UART_SPEED);
}
void setup(){
  Serial.begin(115200);
  startupSPI();
  delay(11);
  //ACTIVATE();
  radioRXMode();
  startupVars();
  setupFromVars();
  NEXT_TICK = micros()+TICK_DURATION;
  MODE = 0;
}
void loop(){
  char statusR, packetSize, emmiter;
  char payload[33];
  long buff;
  char buff_str[10];
  if(MODE==1){//Edit parameters through USB
    statusR = 0;
    Serial.setTimeout(15000);
    Serial.println("EDIT MODE ON");
    while(statusR!='+'){
      echoVars();
      Serial.println("(+)QUIT  (L)OAD  (S)AVE  (B)aud  (H)abilitado  RF_(C)h  (T)x  (R)x");
      while(Serial.available()==0);
      statusR = Serial.read();
      switch(statusR){
        case '+': Serial.println("Bye.");break;
        case 'B': 
        case 'b': 
          Serial.println("Enter new baud (and press enter)");
          buff = Serial.parseInt();
          if(buff==0)UART_SPEED = 0;
          if(buff>=300 && buff<=250000)UART_SPEED = buff;
          setupFromVars();
          break;
        case 'S': 
        case 's': 
          saveVars();
          Serial.println("Variables Saved!");
          break;
        case 'L': 
        case 'l': 
          startupVars();
          Serial.println("Variables Loaded!");
          break;
        case 'C':
        case 'c'://0-125
          Serial.print("Entre com canal (0-125):");
          buff = Serial.parseInt();
          if(buff>=0 && buff<=125)RF_CHANNEL = buff;
          setupFromVars();
          Serial.print("\nCanal setado para ");
          Serial.println(RF_CHANNEL);
          break;
        case 'H':
        case 'h':
          Serial.println("Canais Habilitados:");
          if(RX_ENABLED&(1<<0))Serial.print("CH0  ");
          else                 Serial.print("     ");
          if(RX_ENABLED&(1<<1))Serial.print("CH1  ");
          else                 Serial.print("     ");
          if(RX_ENABLED&(1<<2))Serial.print("CH2  ");
          else                 Serial.print("     ");
          if(RX_ENABLED&(1<<3))Serial.print("CH3  ");
          else                 Serial.print("     ");
          if(RX_ENABLED&(1<<4))Serial.print("CH4  ");
          else                 Serial.print("     ");
          if(RX_ENABLED&(1<<5))Serial.println("CH5  ");
          else                 Serial.println("     ");
          Serial.print("Entre com canal para trocar estado (ou algum outro valor para cancelar):");
          buff = Serial.parseInt();
          if(buff==0)RX_ENABLED ^= (1<<0);
          if(buff==1)RX_ENABLED ^= (1<<1);
          if(buff==2)RX_ENABLED ^= (1<<2);
          if(buff==3)RX_ENABLED ^= (1<<3);
          if(buff==4)RX_ENABLED ^= (1<<4);
          if(buff==5)RX_ENABLED ^= (1<<5);
          Serial.println("Pronto!");
          break;
        case 'T':
        case 't':
          Serial.print("Entre com o endereço hexadecimal (10 chars):");
          Serial.readBytes(buff_str,10);
          TX_CHANNEL[0] = hex2int(buff_str[9],buff_str[8]);
          TX_CHANNEL[1] = hex2int(buff_str[7],buff_str[6]);
          TX_CHANNEL[2] = hex2int(buff_str[5],buff_str[4]);
          TX_CHANNEL[3] = hex2int(buff_str[3],buff_str[2]);
          TX_CHANNEL[4] = hex2int(buff_str[1],buff_str[0]);
          Serial.println("Endereço setado");
          break;
        case 'R':
        case 'r':
          Serial.print("Entre com o canal de recepcao (0-5):");
          buff = Serial.parseInt();
          delay(100);
          while(Serial.available())Serial.read();
          Serial.print("\n");
          if(buff==0 || buff==1){
            Serial.print("Entre com o endereço hexadecimal (10 chars):");
            Serial.readBytes(buff_str,10);
            if(buff==0){
              RX_CHANNEL_0[0] = hex2int(buff_str[9],buff_str[8]);
              RX_CHANNEL_0[1] = hex2int(buff_str[7],buff_str[6]);
              RX_CHANNEL_0[2] = hex2int(buff_str[5],buff_str[4]);
              RX_CHANNEL_0[3] = hex2int(buff_str[3],buff_str[2]);
              RX_CHANNEL_0[4] = hex2int(buff_str[1],buff_str[0]);
            }else{
              RX_CHANNEL_1[0] = hex2int(buff_str[9],buff_str[8]);
              RX_CHANNEL_1[1] = hex2int(buff_str[7],buff_str[6]);
              RX_CHANNEL_1[2] = hex2int(buff_str[5],buff_str[4]);
              RX_CHANNEL_1[3] = hex2int(buff_str[3],buff_str[2]);
              RX_CHANNEL_1[4] = hex2int(buff_str[1],buff_str[0]);
            }
          }else if(buff>=2 && buff<=5){
            Serial.print("Entre com fim de endereço hexadecimal (2 chars):");
            Serial.readBytes(buff_str,2);
                 if(buff==2)RX_CHANNEL_2 = hex2int(buff_str[1],buff_str[0]);
            else if(buff==3)RX_CHANNEL_3 = hex2int(buff_str[1],buff_str[0]);
            else if(buff==4)RX_CHANNEL_4 = hex2int(buff_str[1],buff_str[0]);
            else if(buff==5)RX_CHANNEL_5 = hex2int(buff_str[1],buff_str[0]);
          }else{
            Serial.println("Canal impróprio");
          }
          Serial.println("Fim");
          break;
      }
    }
    setupFromVars();
    MODE = 0;
  }else if(NEXT_TICK<micros()){
    NEXT_TICK = micros()+TICK_DURATION;
    
    emmiter = (NOP()>>1)&0x07;
    if(emmiter != 0x07){
      packetSize = R_RX_PL_WID();
      R_RX_PAYLOAD(payload, packetSize);
      payload[packetSize] = 0;
      Serial.print(payload);
      if(UART_SPEED>0)Serial1.print(payload);
      W_REGISTER(0x07, 0x70);
    }
    if(Serial.available()>0){
      for(packetSize=0;Serial.available()>0 && packetSize<32;packetSize++){
        payload[packetSize] = Serial.read();
        if(payload[packetSize]=='+'){
          dataTransmit(payload,packetSize-1);
          W_REGISTER(0x07, 0x70);
          payload[packetSize-1] = 0;
          if(UART_SPEED>0)Serial1.print(payload);
          payload[0] = '+';
          packetSize = 1;
          while(Serial.available()==0);
          payload[packetSize] = Serial.read();
          if(payload[packetSize]=='+'){
            packetSize++;
            while(Serial.available()==0);
            payload[packetSize] = Serial.read();
            if(payload[packetSize]=='+' && Serial.available()==0){
              packetSize++;
              NEXT_TICK = micros()+1000000;
              while(NEXT_TICK>micros() && Serial.available()==0);
              if(Serial.available()==0){
                packetSize = 50;
                MODE = 1;
              }else{
                payload[packetSize]==Serial.read();
                packetSize++;
              }
            }
          }
        }
      }
      if(packetSize>0 && packetSize<50){
        dataTransmit(payload,packetSize);
        W_REGISTER(0x07, 0x70);
        payload[packetSize] = 0;
        //Serial.print(payload);
        if(UART_SPEED>0)Serial1.print(payload);
      }
    }
    if(UART_SPEED>0 && Serial1.available()>0){
      for(packetSize=0;Serial1.available()>0 && packetSize<32;packetSize++){
        payload[packetSize] = Serial1.read();
      }
      dataTransmit(payload,packetSize);
      W_REGISTER(0x07, 0x70);
      payload[packetSize] = 0;
      Serial.print(payload);
    }
    
  }
  
  //spiStop();
}

