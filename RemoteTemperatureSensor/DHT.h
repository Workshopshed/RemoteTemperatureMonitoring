
//Max time to wait for results from DHT11
#define MAX_TIME 85  

bool dht11_read_val(int pin, int (*dht11_results)[5])  
{  
  byte lststate=HIGH;  
  byte  counter=0;  
  byte  j=0,i;  

  for(i=0;i<5;i++)  
     *dht11_results[i]=0;  
  pinMode(pin,OUTPUT);  
  digitalWrite(pin,LOW);  
  delay(18);  
  digitalWrite(pin,HIGH);  
  busy_wait_us_32(40);  
  pinMode(pin,INPUT);  
  for(i=0;i<MAX_TIME;i++)  
  {  
    counter=0;  
    while(digitalRead(pin)==lststate){  
      counter++;  
      busy_wait_us_32(1);  
      if(counter==255)  
        break;  
    }  
    lststate=digitalRead(pin);  
    if(counter==255)  
       break;  
    // top 3 transistions are ignored  
    if((i>=4)&&(i%2==0)){  
      *dht11_results[j/8]<<=1;  
      if(counter>60)  
        *dht11_results[j/8]|=1;  
      j++;  
    }  
  }  
  //The resulting 5 bytes in the buffer will be:
  //   0: Integral relative humidity data
  //   1: Decimal relative humidity data
  //   2: Integral temperature data
  //   3: Decimal temperature data
  //   4: Checksum

  // verify cheksum and print the verified data  
  return ((j>=40)&&(*dht11_results[4]==((*dht11_results[0]+*dht11_results[1]+*dht11_results[2]+*dht11_results[3])& 0xFF)));
}  
