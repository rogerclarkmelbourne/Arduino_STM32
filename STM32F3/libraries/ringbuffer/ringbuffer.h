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

#ifndef RINGBUFFER_H_
#define RINGBUFFER_H_

class RingBuffer {
public:
	int  size;     // muss ZweierPotenz sein
	int  *values;  // Pointer auf Speicher für Werte
    unsigned long *timeStamps; // optional Pointer auf Speicher für ms timestamps
    int rdIdx;    // Leseindex - Benutzung optional
    int wrIdx;    // Schreibindex
    int mask;

    int anz;      // Aktuelle Anzahl an Werten im Puffer
    int lastRead; // wird auf eins gesetz, wenn letzter vorhandener Wert gelesen wurde


    RingBuffer(int _size, int *_values, unsigned long *_timeStamps);

    /*
     * fügt einen Wert zum Ringpuffer hinzu und erzeugt einen zugehörigen Timestamp
     * wenn timeStamps != NULL ist
     */
    void addValue(int val);

    /*
     * setzt rdIdx auf distance Werte vor den letzten Wert (wrPtr-1)
     * Mit getValue können die letzten Werte bis zum letzten wert abgefragt werden
     *
     * gibt 1 zurück, wenn genügend Werte im Puffer sind
     * gibt 0 zurück, wenn noch nicht genügend Werte im Puffer sind oder distance
     *                größer als die Pufferlänge ist
     */
    int setReadIdx(int distance);

    /*
     * Setzt rdIdx auf den ersten Wert, für den der zugehörige timeStamp mindestens <ms> zurückliegt,
     * Es wird das Alter des betreffenden Wertes in ms zurückgegeben
     * = wird zurückgegeben, wenn kein Wert mindestens so alt wie gefordert ist oder keine timestamps vorhanden sind
     */
    int setReadIdxDt(int ms);

    /*
     * gibt Wert für rdIdx zurück und incrementiert rdIdx, solange rdIdx < wrIdx
     * ACHTUNG: wird wrPtr erreicht, so wird immer wieder der letzte Wert zurückgegeben!
     */
    int getValue();

    /*
     * gibt Wert für rdIdx zurück, trägt den zugehörigen timestamp
     * in time ein und incrementiert rdIdx, solange rdIdx < wrIdx
     * ACHTUNG: wird wrPtr erreicht, so wird immer wieder der letzte Wert zurückgegeben!
     */
    int getValue(unsigned long *time);

    /*
     * gibt 1 zurück, wenn rdPtr != wrPtr
     * gibt 0 zurück, wenn rdPtr == wrPtr
     *
     * --> funktioniert nur, wenn zuvor SetReadIdx() aufgerufen wurde
     */
    int valueAvailable();

    /*
     * Setzt den Ringbuffer zurück
     */
    void reset();

    /*
    `* decrementiert rdPtr und gibt den zugehörigen Wert zurück
     */
    int getPreviousValue();
    int getPreviousValue(unsigned long *time);

    /*
    `* decrementiert rdPtr und gibt den zugehörigen Wert zurück
     * schreibt Zeiitdifferenz zur aktuellen Zeit in dt
     */
    int getPreviousValueDt(unsigned long *dt);


    /*
     *  gibt eins zurück, sobald der älteste Wert mit einer PreviousValue-Funktion
     *  gelesen wurde
     *  wird durch jeden Aufruf einer SetReadIdx-Funktion zurückgesetzt
     */
     int lastValueRead();

     /*
      *  Diese Funktion erlaubt den Zugriff auf den Ringbuffer wie auf ein Array.
      *  Der älteste Wert hat immer den Index 0, der aktuellste Wert den Index size-1
      *  Der rdIdx des Puffers wird nicht verändert.
      */ 
     int getValueAt(int idx);
     int getValueAt(int idx, unsigned long *time);

     /*
      *  Diese Funktion erlaubt den Zugriff auf den Ringbuffer wie auf ein Array.
      *  Der neueste Wert hat immer den Index 0, der älteste Wert den Index size-1
      *  Der rdIdx des Puffers wird nicht verändert.
      */ 
     int getPreviousValueAt(int idx);
     int getPreviousValueAt(int idx, unsigned long *time);


};



class RingBufferAverage : public RingBuffer 
{
public:
   int averageSize;
   int averageSum;

   RingBufferAverage(int _size, int *_values, unsigned long *_timeStamps, int _averageSize); 

   void setAverageLen(int len);

   void addValue(int val);
   void reset();

   int getAverage();
   int getAverageSum();


};


#endif /* RINGBUFFER_H_ */
