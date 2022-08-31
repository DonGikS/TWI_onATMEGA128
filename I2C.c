#define STOP_STEP    4413 //ATMEGA128 Delay time (in 16Mhz Clock)
#define STOP_COUNTER  1
unsigned char I2C_start(){
  unsigned long step = 0;
  unsigned char counter = 0;
  TWCR = (1<<TWINT)|(1<<TWSTA)|(0<<TWSTO)|(1<<TWEN);  //start 조건 출력
  //TWCR = 0xA4;
  while(((TWCR&0x80)==0) ||((TWSR&0xF8) != 0x08)){
    if(step == STOP_STEP){
      //if(PORTG & 0x10) PORTG = PORTG & 0x0F;
      //else PORTG = PORTG | 0x10;
      counter += 1;
      step = 0;
    }
    if(counter == STOP_COUNTER){
      TWCR = (1<<TWINT)|(0<<TWSTA)|(1<<TWSTO)|(1<<TWEN);
      return 1;
    }
    step+=1;
  };  //slave의 수신응답
  return 0;
}
unsigned char I2C_SLA_WR_TX(unsigned char address, unsigned char readWrite){
  unsigned long step = 0;
  unsigned char counter = 0;
  //TWDR = (address<<1) | readWrite;
  TWDR = address | 0x00;
  TWCR = (1<<TWINT) | (0<<TWSTA) | (0<<TWSTO) | (1<<TWEN);
  //TWCR = 0x84;
  step = 0;
  counter = 0;
  while(((TWCR&0x80)==0) ||((TWSR&0xF8) != 0x18)){
    if(step == STOP_STEP){
      //if(PORTG & 0x10) PORTG = PORTG & 0x0F;
      //else PORTG = PORTG | 0x10;
      counter += 1;
      step = 0;
    }
    if(counter == STOP_COUNTER){
      TWCR = (1<<TWINT)|(0<<TWSTA)|(1<<TWSTO)|(1<<TWEN);
      return 1;
    }
    step+=1;
  };  //slave의 수신응답(SLA+W)
  return 0;
}
unsigned char I2C_DATA_TX(unsigned char data){
  unsigned long step = 0;
  unsigned char counter = 0;
  TWDR = data;    //slave register address 접근
  TWCR = (1<<TWINT) | (0<<TWSTA) | (0<<TWSTO) | (1<<TWEN);
  //TWCR = 0x84;
  step = 0;
  counter = 0;
  while(((TWCR&0x80)==0) ||((TWSR&0xF8) != 0x28)){
    if(step == STOP_STEP){
      //if(PORTG & 0x10) PORTG = PORTG & 0x0F;
      //else PORTG = PORTG | 0x10;
      counter += 1;
      step = 0;
    }
    if(counter == STOP_COUNTER){
      TWCR = (1<<TWINT)|(0<<TWSTA)|(1<<TWSTO)|(1<<TWEN);
      return 1;
    }
    step+=1;
  };  //slave 수신응답(ack)
  return 0;
}
unsigned char I2C_Restart_RX(){
  unsigned long step = 0;
  unsigned char counter = 0;
  TWCR = (1<<TWINT)|(1<<TWSTA)|(0<<TWSTO)|(1<<TWEN);  //repeat start 조건 출력
  //TWCR = 0xA4;
  step = 0;
  counter = 0;
  while(((TWCR&0x80)==0) ||((TWSR&0xFF) != 0x10)){
    if(step == STOP_STEP){
      //if(PORTG & 0x10) PORTG = PORTG & 0x0F;
      //else PORTG = PORTG | 0x10;
      counter += 1;
      step = 0;
    }
    if(counter == STOP_COUNTER){
      TWCR = (1<<TWINT)|(0<<TWSTA)|(1<<TWSTO)|(1<<TWEN);
      return 1;
    }
    step+=1;
  };  //조건완료시 TWSR상태코드가 0x10으로 변함
  return 0;
}
unsigned char I2C_SLA_WR_RX(unsigned char address, unsigned char readWrite){
  unsigned long step = 0;
  unsigned char counter = 0;
  //TWDR = ((address<<1)&0xFF) | 0x01;  //TWDR레지스터에 SLA(slave addr)+R 전송
  //TWDR = (address<<1) | 0x01;
  TWDR = address | 0x01;
  //TWCR = (1<<TWINT) | (0<<TWSTA) | (0<<TWSTO) | (1<<TWEN);  //마스터수신모드설정
  TWCR = 0x84;
  step = 0;
  counter = 0;
  while(((TWCR&0x80)==0) ||((TWSR&0xFF) != 0x40)){
    if(step == STOP_STEP){
      //if(PORTG & 0x10) PORTG = PORTG & 0x0F;
      //else PORTG = PORTG | 0x10;
      counter += 1;
      step = 0;
    }
    if(counter == STOP_COUNTER){
      TWCR = 0x94;
      return 1;
    }
    step+=1;
  };  //SLA+R전송이 온료 및 ack 신호를 받으면 TWSR상태가 0x40으로 변함
  return 0;
}
unsigned char I2C_DATA_RX(){
  unsigned long step = 0;
  unsigned char counter = 0;
  TWCR = (1<<TWINT)|(0<<TWEA)|(0<<TWSTA)|(0<<TWSTO)|(1<<TWEN);  //데이터 수신대기 상태
  //TWCR = 0x84;
  step = 0;
  counter = 0;
  while(((TWCR&0x80)==0) ||((TWSR&0xFF) != 0x58)){//NACK수신
    if(step == STOP_STEP){
      //if(PORTG & 0x10) PORTG = PORTG & 0x0F;
      //else PORTG = PORTG | 0x10;
      counter += 1;
      step = 0;
    }
    if(counter == STOP_COUNTER){
      TWCR = 0x94;
      return 1;
    }
    step+=1;
  };  //데이터 수신시 설정되는 값. 확인비트를 출력하지 않음
  return 0;
}
void I2C_Stop(){
  TWCR = (1<<TWINT) | (0<<TWSTA) | (1<<TWSTO) | (1<<TWEN);
  //TWCR = 0x94;
  return;
}