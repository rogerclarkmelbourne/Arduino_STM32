
#include <Arduino.h>
#include <usb_serial.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <math.h>
/* the following is optional depending on the timing function used */
#include <time.h>
#include "whetstone.h"

/* default is double precision, define this for single precision */
#define SINGLE_PRECISION

#ifdef SINGLE_PRECISION
#define DSIN    sinf
#define DCOS    cosf
#define DATAN   atanf
#define DLOG    logf
#define DEXP    expf
#define DSQRT   sqrtf
#else
/* map the FORTRAN math functions, etc. to the C versions */
#define DSIN    sin
#define DCOS    cos
#define DATAN   atan
#define DLOG    log
#define DEXP    exp
#define DSQRT   sqrt
#endif
#define IF      if

/* function prototypes */
#ifdef SINGLE_PRECISION
void POUT(long N, long J, long K, float X1, float X2, float X3, float X4);
void PA(float E[]);
void P3(float X, float Y, float *Z);
#else
void POUT(long N, long J, long K, double X1, double X2, double X3, double X4);
void PA(double E[]);
void P3(double X, double Y, double *Z);
#endif
void P0(void);
#define USAGE   "usage: whetdc [-c] [loops]\n"

/*
    COMMON T,T1,T2,E1(4),J,K,L
*/
#ifdef SINGLE_PRECISION
float T,T1,T2,E1[5];
#else
double T,T1,T2,E1[5];
#endif
int J,K,L;

int argc=0;//Mod for nucleo. Change in code below if you want non-default loop count

/* see the note about LOOP and II below */
#define NUMBER_OF_LOOPS 10000
#define NUMBER_OF_ITERATIONS 1

int whetstone(int argc)
{
	blinks();
    Serial.print("Beginning Whetstone benchmark at ");
    if(argc==0)
        Serial.print("default speed ...\n");
    else {
    	Serial.print(argc);
        Serial.print(" MHz ...\n");
    }
    /* used in the FORTRAN version */
    long I;
    long N1, N2, N3, N4, N6, N7, N8, N9, N10, N11;
#ifdef SINGLE_PRECISION
    float X1,X2,X3,X4,X,Y,Z;
#else
    double X1,X2,X3,X4,X,Y,Z;
#endif
    long LOOP;
    int II, JJ;

    /* added for this version */
    long loopstart;
    long Begin_Time, End_Time;
    float KIPS;
    int continuous;

    loopstart = NUMBER_OF_LOOPS;       /* see the note about LOOP below */
    continuous = 0;

    II = 1;    /* start at the first arg (temp use of II here) */
 /*   while (II < argc) {
        if (strncmp(argv[II], "-c", 2) == 0 || argv[II][0] == 'c') {
            continuous = 1;
        } else if (atol(argv[II]) > 0) {
            loopstart = atol(argv[II]);
        } else {
//            fprintf(stderr, USAGE);//original code
            fprintf(stderr, USAGE);//not output toSTM32 version
            return(1);
        }
        II++;
    }*/

LCONT:
/*
C
C   Start benchmark timing at this point.
C
*/
    Begin_Time = micros();

/*
C
C   The actual benchmark starts here.
C
*/
    T  = .499975;
    T1 = 0.50025;
    T2 = 2.0;
/*
C
C   With loopcount LOOP=10, one million Whetstone instructions
C   will be executed in EACH MAJOR LOOP..A MAJOR LOOP IS EXECUTED
C   'II' TIMES TO INCREASE WALL-CLOCK TIMING ACCURACY.
C
    LOOP = 1000;
*/
    LOOP = loopstart;
    II   = NUMBER_OF_ITERATIONS;

    JJ = 1;

IILOOP:
    N1  = 0;
    N2  = 12 * LOOP;
    N3  = 14 * LOOP;
    N4  = 345 * LOOP;
    N6  = 210 * LOOP;
    N7  = 32 * LOOP;
    N8  = 899 * LOOP;
    N9  = 616 * LOOP;
    N10 = 0;
    N11 = 93 * LOOP;
/*
C
C   Module 1: Simple identifiers
C
*/
    X1  =  1.0;
    X2  = -1.0;
    X3  = -1.0;
    X4  = -1.0;

    for (I = 1; I <= N1; I++) {
        X1 = (X1 + X2 + X3 - X4) * T;
        X2 = (X1 + X2 - X3 + X4) * T;
        X3 = (X1 - X2 + X3 + X4) * T;
        X4 = (-X1+ X2 + X3 + X4) * T;
    }
#ifdef PRINTOUT
    IF (JJ==II)POUT(N1,N1,N1,X1,X2,X3,X4);
#endif

/*
C
C   Module 2: Array elements
C
*/
    E1[1] =  1.0;
    E1[2] = -1.0;
    E1[3] = -1.0;
    E1[4] = -1.0;

    for (I = 1; I <= N2; I++) {
        E1[1] = ( E1[1] + E1[2] + E1[3] - E1[4]) * T;
        E1[2] = ( E1[1] + E1[2] - E1[3] + E1[4]) * T;
        E1[3] = ( E1[1] - E1[2] + E1[3] + E1[4]) * T;
        E1[4] = (-E1[1] + E1[2] + E1[3] + E1[4]) * T;
    }

#ifdef PRINTOUT
    IF (JJ==II)POUT(N2,N3,N2,E1[1],E1[2],E1[3],E1[4]);
#endif

/*
C
C   Module 3: Array as parameter
C
*/
    for (I = 1; I <= N3; I++)
        PA(E1);

#ifdef PRINTOUT
    IF (JJ==II)POUT(N3,N2,N2,E1[1],E1[2],E1[3],E1[4]);
#endif

/*
C
C   Module 4: Conditional jumps
C
*/
    J = 1;
    for (I = 1; I <= N4; I++) {
        if (J == 1)
            J = 2;
        else
            J = 3;

        if (J > 2)
            J = 0;
        else
            J = 1;

        if (J < 1)
            J = 1;
        else
            J = 0;
    }

#ifdef PRINTOUT
    IF (JJ==II)POUT(N4,J,J,X1,X2,X3,X4);
#endif

/*
C
C   Module 5: Omitted
C   Module 6: Integer arithmetic
C
*/

    J = 1;
    K = 2;
    L = 3;

    for (I = 1; I <= N6; I++) {
        J = J * (K-J) * (L-K);
        K = L * K - (L-J) * K;
        L = (L-K) * (K+J);
        E1[L-1] = J + K + L;
        E1[K-1] = J * K * L;
    }

#ifdef PRINTOUT
    IF (JJ==II)POUT(N6,J,K,E1[1],E1[2],E1[3],E1[4]);
#endif

/*
C
C   Module 7: Trigonometric functions
C
*/
    X = 0.5;
    Y = 0.5;

    for (I = 1; I <= N7; I++) {
        X = T * DATAN(T2*DSIN(X)*DCOS(X)/(DCOS(X+Y)+DCOS(X-Y)-1.0));
        Y = T * DATAN(T2*DSIN(Y)*DCOS(Y)/(DCOS(X+Y)+DCOS(X-Y)-1.0));
    }

#ifdef PRINTOUT
    IF (JJ==II)POUT(N7,J,K,X,X,Y,Y);
#endif

/*
C
C   Module 8: Procedure calls
C
*/
    X = 1.0;
    Y = 1.0;
    Z = 1.0;

    for (I = 1; I <= N8; I++)
        P3(X,Y,&Z);

#ifdef PRINTOUT
    IF (JJ==II)POUT(N8,J,K,X,Y,Z,Z);
#endif

/*
C
C   Module 9: Array references
C
*/
    J = 1;
    K = 2;
    L = 3;
    E1[1] = 1.0;
    E1[2] = 2.0;
    E1[3] = 3.0;

    for (I = 1; I <= N9; I++)
        P0();

#ifdef PRINTOUT
    IF (JJ==II)POUT(N9,J,K,E1[1],E1[2],E1[3],E1[4]);
#endif

/*
C
C   Module 10: Integer arithmetic
C
*/
    J = 2;
    K = 3;

    for (I = 1; I <= N10; I++) {
        J = J + K;
        K = J + K;
        J = K - J;
        K = K - J - J;
    }

#ifdef PRINTOUT
    IF (JJ==II)POUT(N10,J,K,X1,X2,X3,X4);
#endif

/*
C
C   Module 11: Standard functions
C
*/
    X = 0.75;

    for (I = 1; I <= N11; I++)
        X = DSQRT(DEXP(DLOG(X)/T1));

#ifdef PRINTOUT
    IF (JJ==II)POUT(N11,J,K,X,X,X,X);
#endif

/*
C
C      THIS IS THE END OF THE MAJOR LOOP.
C
*/
    if (++JJ <= II)
        goto IILOOP;

/*
C
C      Stop benchmark timing at this point.
C
*/


    End_Time = micros();

/*
C----------------------------------------------------------------
C      Performance in Whetstone KIP's per second is given by
C
C   (100*LOOP*II)/TIME
C
C      where TIME is in seconds.
C--------------------------------------------------------------------
*/
    int usertime = End_Time - Begin_Time;

    blinks();
    Serial.println();

#define ONE_SECOND 1000000

    if (usertime < ONE_SECOND) {
        Serial.print("Insufficient duration- Increase the LOOP count\n");
        return(1);
    }

    //Serial.printf("Loops: %ld, Iterations: %d, Duration: %ld sec.\n",
    //        LOOP, II, finisec-Begin_Time);
    Serial.print("Loops:");
    Serial.print(LOOP);
    Serial.print(", Iterations:");
    Serial.print(II);
    Serial.print(", Duration:");
    float duration = 1000.0 * (float) (End_Time-Begin_Time)/(float) ONE_SECOND;
    Serial.print(duration);
    Serial.println(" millisec");

    KIPS = (100.0*LOOP*II)/duration;
    if (KIPS >= 1000.0) {
#ifdef SINGLE_PRECISION
        Serial.print("C Converted Single Precision Whetstones:");
#else
        Serial.print("C Converted Double Precision Whetstones:");
#endif
        Serial.print(KIPS/1000.0);
        Serial.print(" gflops\n");
        
    } else {
#ifdef SINGLE_PRECISION
        Serial.print("C Converted Single Precision Whetstones:");
#else
        Serial.print("C Converted Double Precision Whetstones:");
#endif
        Serial.print(KIPS);
        Serial.println(" Mflops");

    }

    if (continuous)
        goto LCONT;

    return(0);
}

#ifdef SINGLE_PRECISION
void PA(float E[])
#else
void PA(double E[])
#endif
{
    J = 0;

L10:
    E[1] = ( E[1] + E[2] + E[3] - E[4]) * T;
    E[2] = ( E[1] + E[2] - E[3] + E[4]) * T;
    E[3] = ( E[1] - E[2] + E[3] + E[4]) * T;
    E[4] = (-E[1] + E[2] + E[3] + E[4]) / T2;
    J += 1;

    if (J < 6)
        goto L10;
}

void
P0(void)
{
    E1[J] = E1[K];
    E1[K] = E1[L];
    E1[L] = E1[J];
}

#ifdef SINGLE_PRECISION
void P3(float X, float Y, float *Z)
#else
void P3(double X, double Y, double *Z)
#endif
{
#ifdef SINGLE_PRECISION
	float X1, Y1;
#else
    double X1, Y1;
#endif

    X1 = X;
    Y1 = Y;
    X1 = T * (X1 + Y1);
    Y1 = T * (X1 + Y1);
    *Z  = (X1 + Y1) / T2;
}

#ifdef PRINTOUT
#ifdef SINGLE_PRECISION
void POUT(long N, long J, long K, float X1, float X2, float X3, float X4)
#else
void POUT(long N, long J, long K, double X1, double X2, double X3, double X4)
#endif
{
    // Serial.printf("%7ld %7ld %7ld %12.4e %12.4e %12.4e %12.4e\n",
    //                     N, J, K, X1, X2, X3, X4);
    Serial.print(N);
    Serial.print(" ");
    Serial.print(J);
    Serial.print(" ");
    Serial.print(K);
    Serial.print(" ");
    Serial.print(X1);
    Serial.print(" ");
    Serial.print(X2);
    Serial.print(" ");
    Serial.print(X3);
    Serial.print(" ");
    Serial.print(X4);
    Serial.println();
    
    
}
#endif
 
