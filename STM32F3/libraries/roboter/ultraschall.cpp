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
#include <wirish/wirish.h>
#include "ultraschall.h"
#include "debug.h"

/*
 * Für jeden Ultraschallsensor gibt es eine Struktur USSensor, die im Array USSensoren
 * angelegt wird.
 * Für jeden Sensor muss einmal die Funktion addUSSensor() mit den richtigen Anschluss
 * Pins aufgerufen werden.
 *
 * Zum Messen muss ein Sensor aktiviert werden. Es können alle Sensoren aktiv sein.
 * Sie werden dann abwechselnd für Messungen getriggert. Nicht mehr benötigte Sensoren
 * sollten deaktiviert werden.
 * Im aktiven Zustand wird durch die Funktion UltraschallMessen() die Messung
 * jeweils eines aktiven Sensors gestartet, wenn keine Messung aktiv ist.
 * UltraschallMessen() koordiniert nur die Messungen (es darf immer nur ein
 * Ultraschallsensor aktiv sein) und wartet nicht, bis die Messung erfolgt ist.
 * UltraschallMessen sollte alle 25ms oder kürzer aufgerufen werden,
 * wenn Ultraschallmesswerte benötigt werden.
 * --> ggf. Aktivieren der nächsten Messung im Interrupt? --> testen
 *
 * Das Messen selbst erfolgt asynchron zur Programmausführung. Eine spezielle
 * Messroutine wird aufgerufen, wenn der Echo-Pin des Sensors seinen Zustand ändert.
 * Diese Routine trägt die gemessene Entfernung und einen Zeitstempel in die Sensor-
 * struktur ein und setzt ein Flag in der Struktur, dass ein neuer Messwert zur
 * Verfügung steht.
 *
 * Mit der Funktion UltraschallMessenFertig(id) kann abgefragt werden, ob ein neuer
 * Messwert vorliegt. Dieser kann mit UltraschallMesswert(id) abgefragt werden.
 * Mit UltraschallMesswertAlter(id) kann abgefragt werden, wie alt der ausgelesene
 * Messwert ist. Alternativ zu den Funktionsaufrufen können die Werte auch direkt aus
 * den Sensorstrukturen ausgelesen werden.
 *
 * Alternativ zum automatischen Messen im Hintergrund kann statt UltraschallMessen()
 * die Funktion UltraschallMessenEinzel(id) aufgerufen werden. Diese Funktion blockiert
 * solange bis ein Messwert vorliegt (max. 50ms falls kein Wert ermittelt werden kann)
 * und gibt diesen zurück. Eine Aktivierung des Sensors ist nicht erforderlich.
 * Eventuell laufende Messungen werden abgebrochen. Eine noch laufende Messung wird
 * jedoch zu Ende geführt.
 *
 * Zeitüberwachung:
 * Es kann eine maximale Laufzeit für das zu erwartende Echo gesetzt werden (z.B. 30ms).
 * Für diese Laufzeit wird ein Timer mit eigenem Interrupt gestartet.
 * Gestoppt wird der Timer, wenn die Echo-Interrupt-Funktion vor Ablauf des Timers ausgeführt wird
 * (der Echo-Puls ist in diesem Fall innerhalb des zulässigen Zeitfensters gekommen.
 * Läuft der Timer ab, ist die aktuelle Messung ungültig und es wird versucht eine neue Messung
 * mit dem nächsten Sensor zu starten.
 * Eine Messung kann nur gestartet werden, wenn der Echo-Pin auf LOW ist.
 * Gibt es keinen Sensor, der bei Startanforderung echo==LOW hat, so wird nichts unternommen und
 * auf die nächste fallende Flanke einer Echo-Leitung (Interrupt) gewartet. Die fallende Flanke startet
 * dann eine neue Messung per Interrupt.
 * Gibt es einen Sensor, der bereit für eine neue Messung ist, so wird bei diesem die Messung
 * gestartet.
 * Damit das funktioniert, muss für jeden Echo-Pin eine eigene Interrupt-Funktion attached werden.
 *
 * Wenn auf die Zeitüberwachung verzeichtet wird blockiert eine ungültige Messung weitere Messungen
 * anderer Sensoren um die Zeit bis zum HIGH-LOW-Übergang des Echopins
 * --> bei nur einem Sensor ist eine Zeitüberwachung nicht nötig, da kein Neustart einer Messung
 * stattfinden kann bevor der Echo-Pin wieder Low ist.
 *
 * HC-SR04: Wenn kein Echo kommt, wird das ECHO-Signal nach ca. 235ms (entspricht ca. 4m) wieder nach
 * LOW gezogen.
 *
 * Um das Suchen nach einem messbereiten Sensor zu vereinfachen werden die aktiven Sensorstrukturen in einer
 * doppelt verketteten Liste verwaltet.
 *
 * Der Aufruf von micros() im Interrupt funktioniert nicht sicher, deshalb muss die Timing-Information auf anderm Wege
 * beschafft werden:
 * Eigener Hardwaretimer für Ultraschallsensoren (ist auch schneller bei der Vewaltung)
 *
 */


#define IDLE_CHECK_TMO 25   // ms

// global state
#define US_GLOBAL_IDLE            0
#define US_GLOBAL_MESSUNG_AKTIV   1

static int state = US_GLOBAL_IDLE;

// Prototypen
static void TimerIntRangeTimeout();
static void TimerIntWaitIdle();
static inline void startMessung();
static void USInt();

// globale Variablen
USSensor USSensoren[MAX_US_SENSOR];

// static Variablen
static USSensor *pAktiverSensor = NULL;
static int SensorAktivAnzahl=0;
static int MessPause = 0;


//--------------------------------------------------------------------------
// Timer-Funktionen
//--------------------------------------------------------------------------

static HardwareTimer usTimer(15);
static int usTimerCount; // Beim Starten wird sofort ein Interrupt ausgelöst,
                         // der ausgeblendet werden muss


static void startUsTimer(int ms, voidFuncPtr handler)
{
	usTimer.pause();

	// Set up period
    usTimer.setPeriod(ms*1000); // in microseconds

    usTimerCount = 1;

	// Set up an interrupt on channel 1S
	usTimer.setChannel1Mode(TIMER_OUTPUT_COMPARE);
	usTimer.setCompare(TIMER_CH1, 1);  // Interrupt 1 count after each update
	usTimer.attachCompare1Interrupt(handler);

	// Refresh the timer's count, prescale, and overflow
	usTimer.refresh();

	// Start the timer counting
	usTimer.resume();
}


static void stopUsTimer() {
	usTimer.pause();
}

//--------------------------------------------------------------------------



/**
 * Startet die Messung beim nächsten IDLE-Sensor.
 * Wenn keine Messung aktiv ist (US_GLOBAL_IDLE) und kein Sensor gefunden wird,
 * wird der IDLE-Wait-Timer gestartet
 */
static inline void startMessung()
{
	if (state != US_GLOBAL_IDLE)
		return;  // Es  kann keine Messung gestartet werden

	int anz = SensorAktivAnzahl;

	while (anz) {
	   pAktiverSensor = pAktiverSensor->pNext; // erstmal auf den nächsten Sensor stellen
       if (pAktiverSensor->state==US_IDLE)
       {
    	   //PrintNumber(" nis", pAktiverSensor->id);
           // nur wenn EchoPin Low ist, kann die nächste Messung erfolgen
    	   if (digitalRead(pAktiverSensor->echoPin) == LOW) {
    			state = US_GLOBAL_MESSUNG_AKTIV; // globalen Zustand setzen
    			pAktiverSensor->state = US_ACTIVE;
    			//PrintNumber("sel", pAktiverSensor->id);
    			digitalWrite(pAktiverSensor->triggerPin, HIGH);
    			delayMicroseconds(10);  // FIXME: funktioniert das im Interrupt ???
    			                        // sollte gehen, ist eine Assemblerschleife
    			                        // muss aber mal gemessen werden!!
    			digitalWrite(pAktiverSensor->triggerPin, LOW);
    			attachInterrupt(pAktiverSensor->echoPin, USInt, CHANGE);
    			return;
    	   }
    	   //else
    		  //PrintNumber("ns2", pAktiverSensor->id);
       }
       //else
 	   //	  PrintNumber("ns1", pAktiverSensor->id);

       anz--;
	}
	startUsTimer(IDLE_CHECK_TMO, TimerIntWaitIdle);
}

// Interrupt-Routinen

/**
 * Interrupt-Routine für Echo-Pin
 */
static void USInt()
{
	int time = usTimer.getCount();
	int pin = digitalRead(pAktiverSensor->echoPin);
	if (pin==HIGH)
	{
		if (pAktiverSensor->state == US_ACTIVE) {
			startUsTimer(pAktiverSensor->maxTime, TimerIntRangeTimeout);
			//PrintNumber(" eh",pSensor->id);
		}
	}
	else
	{
		stopUsTimer();
		detachInterrupt(pAktiverSensor->echoPin);
		pAktiverSensor->entfernung = time/pAktiverSensor->timerTicksPerCm;
		//Print2Number("usi", pAktiverSensor->id, pAktiverSensor->entfernung);
		pAktiverSensor->messZeit = millis();
		pAktiverSensor->lastValidRange = pAktiverSensor->entfernung;
		pAktiverSensor->lastValidTime  = pAktiverSensor->messZeit;
		pAktiverSensor->neuerMesswert = 1;
		pAktiverSensor->state = US_IDLE;
		state = US_GLOBAL_IDLE;
		if (pAktiverSensor->handler)
			pAktiverSensor->handler(pAktiverSensor->id, pAktiverSensor->entfernung);
		if (MessPause)
			startUsTimer(MessPause, TimerIntWaitIdle);
		else
			startMessung();
	}
}

static void TimerIntWaitIdle() {
	if (usTimerCount)
		usTimerCount--;
	else {
  	   //debugPrint("twi");
		stopUsTimer();
        startMessung();
	}
}

static void TimerIntRangeTimeout() {
	if (usTimerCount)
	   usTimerCount--;
	else {
 	    stopUsTimer();
		if (pAktiverSensor) {
			detachInterrupt(pAktiverSensor->echoPin);
			//debugPrint(" tmo");
			if (pAktiverSensor->state == US_ACTIVE) {
				pAktiverSensor->state = US_IDLE;
				state = US_GLOBAL_IDLE; // Es kann eine Messung mit einem anderen Sensor gestartet werden
				pAktiverSensor->entfernung = ENTFERNUNG_INVALID;
				pAktiverSensor->messZeit = millis();
				if (pAktiverSensor->handler)
					pAktiverSensor->handler(pAktiverSensor->id, pAktiverSensor->entfernung);
				startMessung();
			}
			else
			{
				if (state == US_GLOBAL_IDLE)
					startMessung();
			}
		}
	}
}


//-----------------------------------------------------------------
// extern aufrufbare Funktionen
//-----------------------------------------------------------------

/**
 * Definiert und initialisiert einen Ultraschallsensor
 * maxRange in cm
 */
void addUSSensor(int id, int trigger, int echo, int maxRange)
{
	if ((id < 0) || (id >= MAX_US_SENSOR))
	   return;
	USSensor *pSensor   = &USSensoren[id];
	pSensor->triggerPin = trigger;
	pSensor->echoPin    = echo;
	pSensor->enabled    = 0;
	pSensor->state      = US_IDLE;
	pSensor->id         = id;
	pSensor->maxTime    = (maxRange*US_ROUNDTRIP_CM)/1000;

	// Timer-Wertebereich für maxRange ermitteln
    usTimer.setPeriod(pSensor->maxTime*1000); // in microseconds
    pSensor->timerTicksPerCm = usTimer.getOverflow()/maxRange;

	// pins initialisieren
	pinMode(trigger, OUTPUT);
	pinMode(echo, INPUT);
}

/**
 * aktiviert einen Sensor zum Messen im Hintergrund
 */
void aktiviereUSSensor(int id)
{
   if (USSensoren[id].enabled)  // ist bereits enabled
	   return;

   USSensoren[id].enabled = 1;

   if (pAktiverSensor == NULL) { // ist der erste Sensor
	   pAktiverSensor = USSensoren + id;
	   pAktiverSensor->pNext = pAktiverSensor; // zeigt auf sich selbst!
	   pAktiverSensor->pPrev = pAktiverSensor;
	   SensorAktivAnzahl = 1;
	   // Über den Timer wird erreicht, dass die Messung gestartet wird.
	   startUsTimer(IDLE_CHECK_TMO, TimerIntWaitIdle);
   }
   else {
       noInterrupts();
       USSensor *pSensor = USSensoren + id;
       pSensor->state = US_IDLE;

       pSensor->pNext = pAktiverSensor->pNext;
       pSensor->pPrev = pAktiverSensor;
       pAktiverSensor->pNext->pPrev = pSensor;
       pAktiverSensor->pNext = pSensor;

       SensorAktivAnzahl++;
       interrupts();
   }
}


void UsAttachHandler(int id, usFuncPtr fkt){
	USSensoren[id].handler = fkt;
}


void UsSetMessPause(int ms) {
   MessPause = ms;
}


/**
 * deaktiviert einen Sensor
 */
void deaktiviereUSSensor(int id)
{
  /*
   * Problem:
   * für den aktuellen Sensor könnte aktuell eine Messung laufen!
   */
   if (!USSensoren[id].enabled)
	   return;

   noInterrupts();
   USSensor *pSensor = USSensoren + id;
   pSensor->enabled = 0;
   pSensor->neuerMesswert = 0;

   if (SensorAktivAnzahl>1) {
      pSensor->pPrev->pNext = pSensor->pNext;
      pSensor->pNext->pPrev = pSensor->pPrev;
      if (pAktiverSensor == pSensor)
         pAktiverSensor = pSensor->pNext;
   }
   else
	  pAktiverSensor = NULL;

   SensorAktivAnzahl--;

   if (pSensor->state == US_ACTIVE) {
      detachInterrupt(pSensor->echoPin);
      pSensor->state = US_IDLE;
	  state = US_GLOBAL_IDLE;

	  if (SensorAktivAnzahl)
		   // Über den Timer wird erreicht, dass eine neue Messung gestartet wird.
		   startUsTimer(IDLE_CHECK_TMO, TimerIntWaitIdle);
   }
   interrupts();
}

/**
 * gibt 1 zurück, wenn ein neuer Messwert vorliegt.
 * Es wird nur einmal eine 1 zurückgegeben, das Flag wird beim Auslesen zurückgesetzt
 */
int UltraschallMessenFertig(int id)
{
   if (USSensoren[id].neuerMesswert)
   {
	   USSensoren[id].neuerMesswert = 0;
	   return 1;
   }
   return 0;
}


/**
 *  gibt aktuellstes Messergebnis zurück
 */
int16 UsAktuellerMesswert(int id)
{
	return USSensoren[id].entfernung;
}

/**
 * Gibt das Alter der aktuellsten Messung in ms zurück
 */
int UsAktuellerMesswertAlter(int id)
{
   return (millis() - USSensoren[id].messZeit);
}

/**
 *  gibt den letzten gueltigen Wert zurück
 */
int16 UsLetzterGueltigerMesswert(int id)
{
	return USSensoren[id].lastValidRange;
}

/**
 * Gibt das Alter der letzten Messung in ms zurück
 */
int UsLetzterGueltigerMesswertAlter(int id)
{
   return (millis() - USSensoren[id].lastValidTime);
}


/**
 * -----------------------------------------------------------
 * blockierende Messung
 * -----------------------------------------------------------
 */
int16 UltraschallMessenEinzel(int id)
{
	uint32 t1;
	uint32 t2=0;
	if ((id<0) || (id >= MAX_US_SENSOR))
		return ENTFERNUNG_INVALID;

	// alle Sensoren deaktivieren
	for (int i=0; i<MAX_US_SENSOR; i++ )
		deaktiviereUSSensor(id);
	pAktiverSensor = NULL;

	USSensor *pSensor = USSensoren+id;

	// Check dass gewünschtes EchoPin Low ist
	t1 = micros();
	while (digitalRead(pSensor->echoPin) == HIGH )
	{
		t2 = micros();
		if (t2-t1 > 100000) // Wenn 100ms lang kein LOW am Echo-Pin Fehler zurückgeben
			return ERROR_SENSOR_BUSY;
	}

	// trigger-Impuls generieren
	digitalWrite(pSensor->triggerPin, HIGH);
	delayMicroseconds(10);
	digitalWrite(pSensor->triggerPin, LOW);

	// auf HIGH am Echo-Pin warten
	t1 = micros();

	while (digitalRead(pSensor->echoPin) == LOW )
	{
		t2 = micros();
		if (t2-t1 > 1000)
			return ERROR_NO_SENSOR; // kein LOW-HIGH-Übergang innerhalb 1ms detektiert -> Fehler
	}
	t1 = micros();

	// auf LOW am Echo-Pin warten
	while (digitalRead(pSensor->echoPin) == HIGH )
	{
	     t2 = micros();
	     if ((t2-t1) > (uint32)(pSensor->maxTime*1000))
	    	 return ENTFERNUNG_INVALID; // Messentfernung größer maxRange
	}

	return (t2-t1)/US_ROUNDTRIP_CM;
}


//------------------------------------------------------------------------
void PrintUsSensorList() {
	USSensor *pSensor = pAktiverSensor;

	for (int i = 0; i < SensorAktivAnzahl; i++){
	   if (pSensor == NULL)
		   return;
       PrintNumber("Sensor", pSensor->id);
       if (pSensor->pNext == NULL)
    	   debugPrint(" next NULL");
       else
           PrintNumber(" next", pSensor->pNext->id);
       if (pSensor->pPrev == NULL)
    	   debugPrint(" prev NULL");
       else
           PrintNumber(" prev", pSensor->pPrev->id);
       pSensor = pSensor->pNext;
	}
}

#if 0
// Timer Test-Funktionen

static void testHandler() {
   debugPrint("usi");

   if (usTimerCount)
	   usTimerCount--;
   else {
       stopUsTimer();
   	  debugPrint("---");
      startUsTimer(300, testHandler);
   }
}

void TestUsTimer() {
	startUsTimer(100, testHandler);
}

int GetUsTimerCount() {
	return usTimer.getCount();
}
#endif

