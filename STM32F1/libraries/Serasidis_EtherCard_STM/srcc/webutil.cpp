// Some common utilities needed for IP and web applications
// Author: Guido Socher
// Copyright: GPL V2
//
// 2010-05-20 <jc@wippler.nl>

#include "EtherCard_STM.h"

void EtherCard::copyIp (uint8_t *dst, const uint8_t *src) {
    memcpy(dst, src, 4);
}

void EtherCard::copyMac (uint8_t *dst, const uint8_t *src) {
    memcpy(dst, src, 6);
}

void EtherCard::printIp (const char* msg, const uint8_t *buf) {
    Serial.print(msg);
    EtherCard::printIp(buf);
    Serial.println();
}

void EtherCard::printIp (const __FlashStringHelper *ifsh, const uint8_t *buf) {
    Serial.print(ifsh);
    EtherCard::printIp(buf);
    Serial.println();
}

void EtherCard::printIp (const uint8_t *buf) {
    for (uint8_t i = 0; i < 4; ++i) {
        Serial.print( buf[i], DEC );
        if (i < 3)
            Serial.print('.');
    }
}

// search for a string of the form key=value in
// a string that looks like q?xyz=abc&uvw=defgh HTTP/1.1\r\n
//
// The returned value is stored in strbuf. You must allocate
// enough storage for strbuf, maxlen is the size of strbuf.
// I.e the value it is declated with: strbuf[5]-> maxlen=5
uint8_t EtherCard::findKeyVal (const char *str,char *strbuf, uint8_t maxlen,const char *key)
{
    uint8_t found=0;
    uint8_t i=0;
    const char *kp;
    kp=key;
    while(*str &&  *str!=' ' && *str!='\n' && found==0){
        if (*str == *kp){
            kp++;
            if (*kp == '\0'){
                str++;
                kp=key;
                if (*str == '='){
                    found=1;
                }
            }
        }else{
            kp=key;
        }
        str++;
    }
    if (found==1){
        // copy the value to a buffer and terminate it with '\0'
        while(*str &&  *str!=' ' && *str!='\n' && *str!='&' && i<maxlen-1){
            *strbuf=*str;
            i++;
            str++;
            strbuf++;
        }
        *strbuf='\0';
    }
    // return the length of the value
    return(i);
}

// convert a single hex digit character to its integer value
unsigned char h2int(char c)
{
    if (c >= '0' && c <='9'){
        return((unsigned char)c - '0');
    }
    if (c >= 'a' && c <='f'){
        return((unsigned char)c - 'a' + 10);
    }
    if (c >= 'A' && c <='F'){
        return((unsigned char)c - 'A' + 10);
    }
    return(0);
}

// decode a url string e.g "hello%20joe" or "hello+joe" becomes "hello joe"
void EtherCard::urlDecode (char *urlbuf)
{
    char c;
    char *dst = urlbuf;
    while ((c = *urlbuf) != 0) {
        if (c == '+') c = ' ';
        if (c == '%') {
            c = *++urlbuf;
            c = (h2int(c) << 4) | h2int(*++urlbuf);
        }
        *dst++ = c;
        urlbuf++;
    }
    *dst = '\0';
}

// convert a single character to a 2 digit hex str
// a terminating '\0' is added
void int2h(char c, char *hstr)
{
    hstr[1]=(c & 0xf)+'0';
    if ((c & 0xf) >9){
        hstr[1]=(c & 0xf) - 10 + 'a';
    }
    c=(c>>4)&0xf;
    hstr[0]=c+'0';
    if (c > 9){
        hstr[0]=c - 10 + 'a';
    }
    hstr[2]='\0';
}

// there must be enough space in urlbuf. In the worst case that is
// 3 times the length of str
void EtherCard::urlEncode (char *str,char *urlbuf)
{
    char c;
    while ((c = *str) != 0) {
        if (c == ' '||isalnum(c)){
            if (c == ' '){
                c = '+';
            }
            *urlbuf=c;
            str++;
            urlbuf++;
            continue;
        }
        *urlbuf='%';
        urlbuf++;
        int2h(c,urlbuf);
        urlbuf++;
        urlbuf++;
        str++;
    }
    *urlbuf='\0';
}

// parse a string and extract the IP to bytestr
uint8_t EtherCard::parseIp (uint8_t *bytestr,char *str)
{
    char *sptr;
    uint8_t i=0;
    sptr=NULL;
    while(i<4){
        bytestr[i]=0;
        i++;
    }
    i=0;
    while(*str && i<4){
        // if a number then start
        if (sptr==NULL && isdigit(*str)){
            sptr=str;
        }
        if (*str == '.'){
            *str ='\0';
            bytestr[i]=(atoi(sptr)&0xff);
            i++;
            sptr=NULL;
        }
        str++;
    }
    *str ='\0';
    if (i==3){
        bytestr[i]=(atoi(sptr)&0xff);
        return(0);
    }
    return(1);
}

// take a byte string and convert it to a human readable display string  (base is 10 for ip and 16 for mac addr), len is 4 for IP addr and 6 for mac.
void EtherCard::makeNetStr (char *resultstr,uint8_t *bytestr,uint8_t len,char separator,uint8_t base)
{
    uint8_t i=0;
    uint8_t j=0;
    while(i<len){
        //itoa((int)bytestr[i],&resultstr[j],base);
        // search end of str:
        while(resultstr[j]){j++;}
        resultstr[j]=separator;
        j++;
        i++;
    }
    j--;
    resultstr[j]='\0';
}

// end of webutil.c
