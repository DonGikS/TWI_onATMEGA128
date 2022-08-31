//address
#define MCP47x6A0       0b01100000
#define MCP47x6A1       0b01100001
#define MCP47x6A2       0b01100010
#define MCP47x6A3       0b01100011
#define MCP47x6A4       0b01100100
#define MCP47x6A5       0b01100101
#define MCP47x6A6       0b01100110
#define MCP47x6A7       0b01100111
//data
#define MCP4726_4095               0xFFF
#define MCP4726_2047               0x7FF
#define MCP4726_1023               0x3FF
#define MCP4726_0                  0x000
#define MCP4716_1023               0x3FF
#define MCP4716_511                0x1FF
#define MCP4716_255                0xFF
#define MCP4716_0                  0x00
#define MCP4706_255                0xFF
#define MCP4706_127                0x7F
#define MCP4706_63                 0x3F
#define MCP4706_0                  0x00
//PowerDown
#define NORMAL_OPERATION           0b00
#define _1kOhm_RESISTOR             0b01
#define _125kOhm_RESISTOR           0b10
#define _640kOhm_RESISTOR           0b11
//V_REF
#define _VDD                       0b00
#define _VREF_UNBUFFERED           0b10
#define _VREF_BUFFERED             0b11
//GAIN
#define _GAINx1                    0
#define _GAINx2                    1
//*******************************************************//
#define Target_Address  MCP47x6A0//use defined address on upside.
#define Target_Data     MCP4726_1023//use defined data on upside.
#define Target_PowerDown    NORMAL_OPERATION
#define Target_Vref         _VDD
#define Target_Gain         _GAINx1
//******************************************************//


#define SDA           20
#define SCL           21
#define SDA_HIGH()      digitalWrite(SDA, HIGH) // SDA pin high
#define SDA_LOW()     digitalWrite(SDA, LOW) // SDA pin low
#define SCL_HIGH()      digitalWrite(SCL, HIGH) // SCL pin high
#define SCL_LOW()      digitalWrite(SCL, LOW) // SCL pin low
#define SDA_INPUT()     pinMode(SDA, INPUT) // SDA pin input mode
#define i2c_delay()     delayMicroseconds(5)
#define i2c_start()       {SDA_HIGH();SCL_HIGH();i2c_delay();SDA_LOW();i2c_delay();SCL_LOW();i2c_delay();}
#define i2c_stop()       {SDA_LOW();SCL_HIGH();i2c_delay();SDA_HIGH();i2c_delay();}




void SDA_OUPUT(){
  pinMode(SDA, OUTPUT);
  return ;
}
unsigned char SDA_READ(){
  pinMode(SDA, INPUT);
  return digitalRead(SDA);
}
unsigned char i2c_write(unsigned char data)
{
  unsigned char i, ack;

  for(i=0; i<8; i++)
  {
    if(data & 0x80)  SDA_HIGH();  // MSB first
    else    SDA_LOW();
    i2c_delay(); 

    SCL_HIGH(); i2c_delay();
    SCL_LOW(); i2c_delay();
    data = data << 1;
  }

  // read ACK
  SDA_HIGH();   // leave SDA HI
  SDA_INPUT();    // change direction to input on SDA line

  SCL_HIGH();   // clock back up
  i2c_delay();
  ack = SDA_READ();   // get the ACK bit
  SCL_LOW();
  pinMode(SDA, OUTPUT);//SDA_OUTPUT();   // change direction back to output
  SCL_LOW();
  i2c_delay();

  return ack;
}

unsigned char i2c_read(void)
{
  unsigned char i, data;

  SDA_HIGH();   // leave SDA HI
  SDA_INPUT();    // change direction to input on SDA line

  data = 0;
  for(i=0; i<8; i++)
  {
    data = data << 1; // MSB first

    SCL_HIGH(); // clock HI
    i2c_delay();
    if(SDA_READ())  // get the Data bit
      data |= 1;
    SCL_LOW();  // clock LO
    i2c_delay();
  }

  pinMode(SDA, OUTPUT);//SDA_OUTPUT();   // change direction back to output

  // send ACK
  SDA_LOW(); i2c_delay();

  SCL_HIGH(); i2c_delay();
  SCL_LOW(); i2c_delay();
  SDA_HIGH();

  return data;
}

unsigned char i2c_read_nack(void)
{
  unsigned char i, data;

  SDA_HIGH();   // leave SDA HI
  SDA_INPUT();    // change direction to input on SDA line

  data = 0;
  for(i=0; i<8; i++)
  {
    data = data << 1; // MSB first

    SCL_HIGH(); // clock HI
    i2c_delay();
    if(SDA_READ())  // get the Data bit
      data |= 1;
    SCL_LOW();  // clock LO
    i2c_delay();
  }

  pinMode(SDA, OUTPUT);//SDA_OUTPUT();   // change direction back to output

  // send NACK
  SDA_HIGH(); i2c_delay();

  SCL_HIGH(); i2c_delay();
  SCL_LOW(); i2c_delay();
  SDA_HIGH();

  return data;
}


void setup() {
  Serial.begin(9600);
  pinMode(SDA, OUTPUT);
  pinMode(SCL, OUTPUT);
}

int temp = 0;
void loop() {
  if(Serial.available() > 0 ){
    temp = Serial.read();
  
  //Write Volatile DAC Register
  if(temp=='0'){
    unsigned char packet_1,packet_2,packet_3;
    packet_1 = (Target_Address << 1)|0x00;
    packet_2 = (0b000<<2)+Target_PowerDown;
    packet_2 = (packet_2<<4)|((Target_Data>>8)&0xF);
    packet_3 = Target_Data & 0xFF;
     
    i2c_start();
    i2c_write(packet_1);
    i2c_write(packet_2);
    i2c_write(packet_3);
    i2c_stop();
  }
  //Write Volatile Memory
  else if(temp == '1'){
    unsigned char packet_1,packet_2,packet_3,packet_4;
    packet_1 = (Target_Address << 1)|0x00;
    packet_2 = (0b010<<5)+(Target_Vref <<3) + (Target_PowerDown<<1) + Target_Gain;
    if(Target_Data == MCP4726_4095 || Target_Data == MCP4726_2047 || Target_Data == MCP4726_1023){
      packet_3 = (Target_Data >> 4) & 0xFF;
      packet_4 = (Target_Data << 4) & 0xFF;
    }else if(Target_Data == MCP4716_1023 || Target_Data == MCP4716_511 || Target_Data == MCP4716_255){
      packet_3 = (Target_Data >> 2) & 0xFF;
      packet_4 = (Target_Data << 6) & 0xFF;
    }else if(Target_Data == MCP4706_255 || Target_Data == MCP4706_127 || Target_Data == MCP4706_63){
      packet_3 = (Target_Data) & 0xFF;
      packet_4 = (Target_Data>>8) & 0xFF;
    }else{
      packet_3 = Target_Data & 0xFF;
      packet_4 = Target_Data & 0xFF;
    } 
    i2c_start();
    i2c_write(packet_1);
    i2c_write(packet_2);
    i2c_write(packet_3);
    i2c_write(packet_4);
    i2c_stop();
  }
  else if(temp == '2'){
    unsigned char packet_1,packet_2,packet_3,packet_4;
    packet_1 = (Target_Address << 1)|0x00;
    packet_2 = (0b011<<5)+(Target_Vref <<3) + (Target_PowerDown<<1) + Target_Gain;
    if(Target_Data == MCP4726_4095 || Target_Data == MCP4726_2047 || Target_Data == MCP4726_1023){
      packet_3 = (Target_Data >> 4) & 0xFF;
      packet_4 = (Target_Data << 4) & 0xFF;
    }else if(Target_Data == MCP4716_1023 || Target_Data == MCP4716_511 || Target_Data == MCP4716_255){
      packet_3 = (Target_Data >> 2) & 0xFF;
      packet_4 = (Target_Data << 6) & 0xFF;
    }else if(Target_Data == MCP4706_255 || Target_Data == MCP4706_127 || Target_Data == MCP4706_63){
      packet_3 = (Target_Data) & 0xFF;
      packet_4 = (Target_Data>>8) & 0xFF;
    }else{
      packet_3 = Target_Data & 0xFF;
      packet_4 = Target_Data & 0xFF;
    } 
    i2c_start();
    i2c_write(packet_1);
    i2c_write(packet_2);
    i2c_write(packet_3);
    i2c_write(packet_4);
    i2c_stop();
  }
  //Write Volatile Configuration bits
  else if(temp == '3'){
    unsigned char packet_1,packet_2,packet_3,packet_4;
    packet_1 = (Target_Address << 1)|0x00;
    packet_2 = (0b100<<5)+(Target_Vref <<3) + (Target_PowerDown<<1) + Target_Gain;
   
    i2c_start();
    i2c_write(packet_1);
    i2c_write(packet_2);
    i2c_stop();
  }

  //Read Command
  else if(temp == '4'){
    unsigned char packet_1,packet_2,packet_3,packet_4,packet_5,packet_6,packet_7;
    packet_1 = (Target_Address << 1)|0x01;
    
    unsigned short data;
   
    i2c_start();
    i2c_write(packet_1);
    packet_2 = i2c_read();
    packet_3 = i2c_read();
    packet_4 = i2c_read();
    packet_5 = i2c_read();
    packet_6 = i2c_read();
    packet_7 = i2c_read_nack();
    i2c_stop();
    
    Serial.println(packet_2,BIN);
    data = (packet_3<<8)+packet_4;
    Serial.println(data,HEX);
    Serial.println(packet_5,BIN);
    data = (packet_6<<8)+packet_7;
    Serial.println(data,HEX);
  }
  else if(temp == '5'){
    unsigned char packet_1,packet_2,packet_3,packet_4, packet_5;
    packet_1 = (Target_Address << 1)|0x01;

   
    i2c_start();
    i2c_write(packet_1);
    packet_2 = i2c_read();
    packet_3 = i2c_read();
    packet_4 = i2c_read();
    packet_5 = i2c_read_nack();
    i2c_stop();
    
    Serial.println(packet_2,BIN);
    Serial.println(packet_3,HEX);
    Serial.println(packet_4,BIN);
    Serial.println(packet_5,HEX);
  }
  else if(temp == '6'){
    //sendSpi(temp);  
  }
  else if(temp == '7'){
    //sendSpi(temp);
  }
  else if(temp == '8'){
    //sendSpi(temp);
  }
  else if(temp == '9'){
    //sendSpi(temp);
  }
  else if(temp == 'r'){
    //sendSpi(temp);
  }
  else if(temp == 't'){
    //sendSpi(temp);
  }
  }
}
