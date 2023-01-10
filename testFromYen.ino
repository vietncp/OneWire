#define DS18B20_PIN 2

void  SetMode(unsigned char mode){//set tín hiệu I/O cho pin
  if (mode==0){
    pinMode(DS18B20_PIN, OUTPUT);
    digitalWrite(DS18B20_PIN,LOW);
  }
  else
    pinMode(DS18B20_PIN, INPUT);
}
unsigned char DS18B20_Reset()     //khởi tạo cảm biến 
{
  unsigned char result;
  SetMode(0);
  
  delayMicroseconds(480);
  SetMode(1);//release bus
  delayMicroseconds(70);  //550-480
  result = digitalRead(DS18B20_PIN);// Read Presence pulse
  delayMicroseconds(410); //960-550

  return result;
}

void DS18B20_Write1()//viết bit 1
{
  SetMode(0);
  delayMicroseconds(6);
  SetMode(1);//release bus
  delayMicroseconds(64);//54+10
}

void DS18B20_Write0()// viết bit 0
{
  SetMode(0);  
  delayMicroseconds(60);
  SetMode(1);//release bus
  delayMicroseconds(10);  
}

void DS18B20_Write_Bit(unsigned char b)//viết bit
{
  if(b == 1)
  {
    DS18B20_Write1();
  }
  else
  {
    DS18B20_Write0();
  }
}

unsigned char DS18B20_Read_Bit()//đọc bit
{
  unsigned char result;
  SetMode(0);  
  delayMicroseconds(6);
  SetMode(1);
  delayMicroseconds(4);
  result = digitalRead(DS18B20_PIN);
  delayMicroseconds(60);
  return result;
}
unsigned int DS18B20_Read_Byte(void) { //đọc 1 byte
   unsigned char result=0;
   unsigned char i;
   for(i=0;i<8;i++){                        //cho vòng lặp chạy 8 lần
    result>>=1;                             //dịch result sang phải 1
    result=result | DS18B20_Read_Bit()<< 7; //lưu bit đọc được vào bit cuối cùng của result
    }
   return result;
}


void DS18B20_Write_Byte(unsigned char b)//viết 1 byte
{
  unsigned char i ;

  for(i=0;i<8;i++)               //cho vòng lặp chạy 8 lần
  {
    DS18B20_Write_Bit(b & 0x01);//lấy ra bit thấp nhất của bite b và gửi đi 
    b >>= 1;                    //dịch bit tiếp theo xuống thành bit thấp nhất 
  }
}
void setup(){
  Serial.begin(9600);
  DS18B20_Reset();
  delay(1000);
  // Write Scratch pad
  DS18B20_Write_Byte(0xCC);               //skip ROM
  delay(1000);
  
  // cai dat do phan giai cho ADC
  DS18B20_Reset();
  DS18B20_Write_Byte(0xCC);               //skip ROM
  DS18B20_Write_Byte(0x4E);               //Write cratchpad
  
  DS18B20_Write_Byte(0x00);               //Write first byte(value 0) to byte 2
  DS18B20_Write_Byte(0x00);               //Write second byte(value 0) to byte 3
  DS18B20_Write_Byte(0x5F);               //Write third byte(value 5F) to  CONFIG register(byte 4)
  // 1F-> 9 bit resolution; 3F-> 10 bit resolution; 5F -> 11 bit resolution;7F->12 bit resolution;
  
 
}
void loop(){
  unsigned char j;
  unsigned char lsB, msB;  
  unsigned char data[8];
  float t;
  
  DS18B20_Reset();
  DS18B20_Write_Byte(0xCC);               //skip ROM
  DS18B20_Write_Byte(0x44);               //convert_T
  delay(1000);
  DS18B20_Reset();
  DS18B20_Write_Byte(0xCC);                 //skip ROM
  DS18B20_Write_Byte(0xBE);                 //read Scratchpad
  Serial.print("Data= ");
  
  for(j=0;j<8;j++){
    data[j]=DS18B20_Read_Byte();              //đọc từng byte trong scratchpad và lưu vào biến data
    
  Serial.print(data[j],HEX);
  Serial.print(".");
  }
  
  //lsB=DS18B20_Read_Byte();              // đọc LS Byte
  //msB=DS18B20_Read_Byte();              // đọc MS Byte

  Serial.println(" ");
  Serial.println("Nhiet do: ");
  t= ((float)(data[0]&0x0F))/16
  t=((data[0]&0xF0)>>4|(data[1]&0x0F)<<4)+t;  //byte thứ nhất lấy 4 bit cuối>>4 +byte thứ 2 lấy 4 bit đầu<<4+phần thập phân ở trên
  Serial.println(t,4);                //in ra giá trị nhiệt độ, 4 vị trí cho giá trị thập phân
  
  Serial.println("ADC resolution: ");
  Serial.println(data[4],HEX);      //in ra gia tri o dinh dang so HEX
  Serial.println(data[4],BIN);      //in ra gia tri o dinh dang so BIN
           
  
  
  
  //t= ((float)(lsB&0x0F))/16;              //byte thứ nhất lấy 4 bit đầu (phần thập phân)
  //t=((lsB&0xF0)>>4|(msB&0x0F)<<4)+t;  //byte thứ nhất lấy 4 bit cuối>>4 +byte thứ 2 lấy 4 bit đầu<<4+phần thập phân ở trên
  //Serial.println(t,4);                //in ra giá trị nhiệt độ, 4 vị trí cho giá trị thập phân

}
