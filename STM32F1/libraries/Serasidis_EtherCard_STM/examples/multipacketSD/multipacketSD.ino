// works only with tinyfat library
// with SD library packets lost will occurs
// don't know why ! 
// tested with arduino mega 1280 and uno
// send 240 Megabyte file without packet loss
// at 100 kbyte/s
// tinyfat read a block of 512 bytes on SD card , 
// so the buffer must be 512 + 60 bytes
// on the arduino mega with bigger buffer you can adjust 
// if (cur>=512) { // 512 to 1024 with 1100 bytes buffer

#include <EtherCard_STM.h>
#include <tinyFAT.h>
#include <avr/pgmspace.h>
#define TCP_FLAGS_FIN_V 1 //as declared in net.h
#define TCP_FLAGS_ACK_V 16 //as declared in net.h
static byte myip[] = { 192,168,0,66 };
static byte gwip[] = { 192,168,0,250 };
static byte mymac[] = { 0x74,0x69,0x69,0x2D,0x30,0x39 };
byte Ethernet::buffer[700]; // tcp/ip send and receive buffer
unsigned long cur;
unsigned long pos;
byte res;

void setup() {
  // Initialize serial communication at 115200 baud
  Serial.begin(115200);
  // Initialize tinyFAT 
  // You might need to select a lower speed than the default SPISPEED_HIGH
    file.setSSpin(4);
  res=file.initFAT(0); 
  if (res==NO_ERROR)    Serial.println("SD started");
  ether.begin(sizeof Ethernet::buffer, mymac , 10); //53 on mega ethernet shield 10 on others
  ether.staticSetup(myip, gwip);
  Serial.println("ETH started");
}  

void loop() 
{
   wait:
    pos = ether.packetLoop(ether.packetReceive());// check if valid tcp data is received
    if (pos) {
        char* data = (char *) Ethernet::buffer + pos;
        cur=0;
        if (strncmp("GET / ", data, 6) == 0) {// nothing specified
          sendfiles("index.htm");
          goto wait;
          }
        if (strncmp("GET /", data, 5) == 0) { // serve anything on sd card 
            int i =0;  
            char temp[15]=""; // here will be the name of requested file
            while (data[i+5]!=32) {temp[i]=data[i+5];i++;}//search the end
            sendfiles((char*) temp);
            goto wait;
          }
       not_found();
       
  }
  
}

void not_found() { //content not found 
  cur=0;
  streamfile ("404.hea",TCP_FLAGS_FIN_V); 
  // Serial.println("not found");
}

byte streamfile (char* name , byte lastflag) { //send a file to the buffer 
  if (!file.exists(name)) {return 0;}
  res=file.openFile(name, FILEMODE_BINARY);
  int  car=512;
  while (car==512) {
    car=file.readBinary();
    for(int i=0;i<car;i++) {
    cur++;
    Ethernet::buffer[cur+53]=file.buffer[i];
    }
if (cur>=512) {
      ether.httpServerReply_with_flags(cur,TCP_FLAGS_ACK_V);
      cur=0;
    }  else {
 
  if (lastflag==TCP_FLAGS_FIN_V) {
    ether.httpServerReply_with_flags(cur,TCP_FLAGS_ACK_V+TCP_FLAGS_FIN_V);
  }
    }
}
  file.closeFile();
  return 1;
}

byte sendfiles(char* name) { // function to find the correct header and send a file 
  ether.httpServerReplyAck();
  int i =0;  
  char dtype[13]=""; 
  while (name[i]!=0) {
    i++;
  }//search the end
  int b=i-1;
  while ((name[b]!=46)&&(b>0)) {
    b--;
  }//search the point
  int a=b+1;
  while (a<i) {
    dtype[a-b-1]=name[a];
    a++;
  }
  dtype[a-b-1]='.';
  dtype[a-b]='h';
  dtype[a-b+1]='e';
  dtype[a-b+2]='a';        
  //Serial.println(dtype); // print the requested header file
  if (streamfile ((char *)dtype,0)==0) {
    streamfile ("txt.hea",0);
  }
    //Serial.println(name); // print the requested file
  if (streamfile ((char *)name,TCP_FLAGS_FIN_V)==0) {
    cur=0;
    not_found();
  }
  /*  uncomment this if you want to have printed the ip of the target browser
  Serial.print("content send to ");
  for(int i=30; i<34; i++) {
Serial.print(Ethernet::buffer[i]);
if (i<33) Serial.print(".");
  }
  Serial.println(" ");
  */
}
