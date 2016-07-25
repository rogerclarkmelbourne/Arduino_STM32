/*
  Twitter.cpp - Arduino library to Post messages to Twitter using OAuth.
  Copyright (c) NeoCat 2010-2011. All right reserved.
  
  This library is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. 
 */

// ver1.2 - Use <string.h>
// ver1.3 - Support IDE 1.0

#include <string.h>
#include "Twitter.h"

#define LIB_DOMAIN "arduino-tweet.appspot.com"

#if defined(ARDUINO) && ARDUINO < 100
static uint8_t server[] = {0,0,0,0}; // IP address of LIB_DOMAIN
Twitter::Twitter(const char *token) : client(server, 80), token(token)
{
}
#else
Twitter::Twitter(const char *token) : token(token)
{
}
#endif

bool Twitter::post(const char *msg)
{
#if defined(ARDUINO) && ARDUINO < 100
	DNSError err = EthernetDNS.resolveHostName(LIB_DOMAIN, server);
	if (err != DNSSuccess) {
		return false;
	}
#endif
	parseStatus = 0;
	statusCode = 0;
#if defined(ARDUINO) && ARDUINO < 100
	if (client.connect()) {
#else
	if (client.connect(LIB_DOMAIN, 80)) {
#endif
		client.println("POST http://" LIB_DOMAIN "/update HTTP/1.0");
		client.print("Content-Length: ");
		client.println(strlen(msg)+strlen(token)+14);
		client.println();
		client.print("token=");
		client.print(token);
		client.print("&status=");
		client.println(msg);
	} else {
		return false;
	}
	return true;
}

bool Twitter::checkStatus(Print *debug)
{
	if (!client.connected()) {
		if (debug)
			while(client.available())
				debug->print((char)client.read());
		client.flush();
		client.stop();
		return false;
	}
	if (!client.available())
		return true;
	char c = client.read();
	if (debug)
		debug->print(c);
	switch(parseStatus) {
	case 0:
		if (c == ' ') parseStatus++; break;  // skip "HTTP/1.1 "
	case 1:
		if (c >= '0' && c <= '9') {
			statusCode *= 10;
			statusCode += c - '0';
		} else {
			parseStatus++;
		}
	}
	return true;
}

int Twitter::wait(Print *debug)
{
	while (checkStatus(debug));
	return statusCode;
}
