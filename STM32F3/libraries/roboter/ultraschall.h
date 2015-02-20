/******************************************************************************
 * The MIT License
 *
 * Copyright (c) 2015 Frank-Michael Krause
 *
 * Permission is hereby granted, free of charge, to any person
 * obtaining a copy of this software and associated documentation
 * files (the "Software"), to deal in the Software without
 * restriction, including without limitation the rights to use, copy,
 * modify, merge, publish, distribute, sublicense, and/or sell copies
 * of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be
 * included in all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
 * EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
 * MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
 * NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS
 * BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN
 * ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN
 * CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 *****************************************************************************/

#ifndef ULTRASCHALL_H_
#define ULTRASCHALL_H_

#define US_IDLE   0
#define US_ACTIVE 1


#define US_ROUNDTRIP_CM 57      // Microseconds (uS) it takes sound to travel round-trip 1cm (2cm total), uses integer to save compiled code space.

typedef void (*usFuncPtr)(int id, int entfernung);

typedef struct USSensor_s {
	int8   id;
	int    triggerPin;
	int    echoPin;
    int8   enabled;
    int8   state;         // IDLE, ACTIVE
    int8   neuerMesswert;
    int16  maxTime;       //Zeit für maximal zu messende Entfernung in ms
    int    timerTicksPerCm;
    int16  entfernung;    // in cm
    uint32 messZeit;      // Systemzeit der Messung in ms
    int16  lastValidRange;
    uint32 lastValidTime;
//    uint32 startZeit;   // Startzeit der aktuellen Messung
    usFuncPtr  handler;
    struct USSensor_s *pNext;
    struct USSensor_s *pPrev;
} USSensor;

#define MAX_US_SENSOR 4

#define ENTFERNUNG_INVALID -1
#define ERROR_NO_SENSOR    -2
#define ERROR_SENSOR_BUSY  -3

extern USSensor USSensoren[MAX_US_SENSOR];


/*****************************************************
 * Funktionen zur Initialisierung
 ****************************************************/

/**
 * id:       Index zwischen 0 und MAX_US_SENSOR-1
 * trigger:  IO-Pin, der mit dem Trigger-Eingang des Sensors verbunden ist
 * echo:     IO-Pin, der mit dem Echo-Ausgang des Sensors verbunden ist.
 *           !!! der Echo-IO-Pin muss 5V tolerant sein oder mit einem
 *               Spannungsteiler versehen werden.
 * maxRange: Maximal zu messende Entfernung in cm; optional
 */
extern void addUSSensor(int id, int trigger, int echo, int maxRange=150);

/**
 * setzt die Zeit der Pause, die nach erfolgreicher Messung vor der nächsten
 * Messung eingefügt wird.
 * Default ist 0, d.h. keine Pause
 */
extern void UsSetMessPause(int ms);



/*****************************************************
 * Funktionen für blockierende Messung
 *   Bei Aufruf der blockierenden Einzelmessfunktion werden
 *   alle über Interrupt akktivierten Sensoren deaktiviert
 *
 *   gibt Entfernung in cm zurück
 *   bzw. die Fehlerwerte
 *      ENTFERNUNG_INVALID -1
 *      ERROR_NO_SENSOR    -2
 *      ERROR_SENSOR_BUSY  -3
 *****************************************************/
extern int16 UltraschallMessenEinzel(int id);




/*****************************************************
 * Funktionen für nicht blockierende, interrupt-basierte Messung
 *
 * Durch Zustandsvariablen gesteuert werden auf allen aktiven Sensoren
 * immer wieder abwechselnd Messungen angestossen.
 * Die Messergebnisse werden in Variablen der Sensorstruktur abgelegt
 * und können über Funktionen aus dem Hauptptogramm abgefragt werden.
 *
 * Es kann pro Sensor eine Handler-Funktion registriert werden,
 * die aufgerufen wird, sobald ein neuer Messwert vorliegt.
 *****************************************************/


extern void aktiviereUSSensor(int id);
extern void deaktiviereUSSensor(int id);

extern void UsAttachHandler(int id, usFuncPtr fkt);


/**
 * gibt das Ergebnis der letzten Messung zurück.
 * Kann ENTFERNUNG_INVALID sein.
 */
extern int16 UsAktuellerMesswert(int id);

/**
 * gibt das Alter des letzen Messergebnisses in ms an.
 */
extern int   UsAktuellerMesswertAlter(int id);

/**
 * gibt den letzten Messwert des Sensors zurück, der nicht
 * ENTFERNUNG_INVALID ist.
 */
extern int16 UsLetzterGueltigerMesswert(int id);


/**
 * gibt das Alter des letzen gültigen Messergebnisses in ms an.
 */
extern int UsLetzterGueltigerMesswertAlter(int id);



/*****************************************************
 * Debug-Funktionen
 ****************************************************/

extern void PrintUsSensorList();


#endif /* ULTRASCHALL_H_ */
