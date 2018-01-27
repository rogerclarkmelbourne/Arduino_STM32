/**
 * Based on data from RM0008 Reference manual:
 *  STM32F101xx, STM32F102xx, STM32F103xx, STM32F105xx and STM32F107xx advanced ARM ® -based 32-bit MCUs
 */
#include <Arduino.h>
#include <HardwareTimer.h>

namespace timerdiagnostics_stm {

/**
 * a Stream.print(,BIN) function that does not hide preceding '0'
 * TODO:This only works for integer data
 *
 * TODO:upToBit is not accuratly named, maybe size would be better.
 */
template< typename T >
static String numeric2BinString(const T val, uint16_t fromBit=0, uint16_t upToBit=sizeof(T)*8) {
  String rv;

  for (int16_t i=upToBit-1; i>=fromBit; i--) {
    rv += ((val>>i)&1)?'1':'0';
  }
  return rv;
}

/**
 */
template< typename T >
static String numeric2HexString( const T &t ){
  uint8_t *ptr = (uint8_t*) &t;
  static const char hexb[] = {'0','1','2','3','4','5','6','7','8','9','A','B','C','D','E','F'};
  String rv("0x");
  for( size_t count = sizeof(T); count; count-- ) {
    rv += hexb[((*(ptr+count-1))>>4)&0x0f];
    rv += hexb[(*(ptr+count-1))&0x0f];
  }
  return rv;
}

template<typename T, uint8_t POS, uint8_t SIZE>
struct BitField {
  T * const _data;
  BitField() = delete;
  BitField(T *data) : _data(data) {
  }

  T get() const {
    return ((*_data) >> POS) & ((1ULL << SIZE)-1);
  }

  void set(uint32_t x) {
    T mask( ((1ULL << SIZE)-1) << POS );
    *_data = (*_data & ~mask) | ((x << POS) & mask);
  }

  void set() {
    T mask( ((1ULL << SIZE)-1) << POS );
    *_data = (*_data | mask);
  }

  void clear() {
    T mask( ((1ULL << SIZE)-1) << POS );
    *_data = (*_data & ~mask);
  }

  String operator()(void) {
    return numeric2BinString(get(), 0, SIZE);
  }
};

template <typename T>
struct TIMx{
  TIMx() = delete;
  TIMx(T *p) : _p(p){
  }

  T *_p;
};

template <typename T>
struct TIMxCR1 : protected TIMx<T> {
  TIMxCR1(T *p):TIMx<T>(p),CEN(p),UDIS(p),URS(p),OPM(p),DIR(p),CMS(p),ARPE(p),CKD(p),reserved(p) {
  }

  BitField<T, 0, 1> CEN;
  BitField<T, 1, 1> UDIS;
  BitField<T, 2, 1> URS;
  BitField<T, 3, 1> OPM;
  BitField<T, 4, 1> DIR;
  BitField<T, 5, 2> CMS;
  BitField<T, 7, 1> ARPE;
  BitField<T, 8, 2> CKD;
  BitField<T,10, 6> reserved;

  String toString() {
    String rv("CR1");

    rv += "{reserved:" + reserved();
    rv += ", CKD:" + CKD();
    rv += ",ARPE:" + ARPE();
    rv += ",CMS:" + CMS();
    rv += ",DIR:" + DIR();
    rv += ",OPM:" + OPM();
    rv += ",URS:" + URS();
    rv += ",UDIS:" + UDIS();
    rv += ",CEN:" + CEN();
    rv += "} " + numeric2HexString(*CEN._data);
    return rv;
  }
};

template <typename T>
struct TIMxCR2 : protected TIMx<T> {
  TIMxCR2(T *p):TIMx<T>(p), CCPC(p),reserved1(p),CCUS(p),CCDS(p),MMS(p),TI1S(p),
                OIS1(p),OIS1N(p),OIS2(p),OIS2N(p),
                OIS3(p),OIS3N(p),OIS4(p),
                reserved2(p){
  }

  BitField<T, 0, 1> CCPC;
  BitField<T, 1, 1> reserved1;
  BitField<T, 2, 1> CCUS;
  BitField<T, 3, 1> CCDS;
  BitField<T, 4, 3> MMS;
  BitField<T, 7, 1> TI1S;
  BitField<T, 8, 1> OIS1;
  BitField<T, 9, 1> OIS1N;
  BitField<T,10, 1> OIS2;
  BitField<T,11, 1> OIS2N;
  BitField<T,12, 1> OIS3;
  BitField<T,13, 1> OIS3N;
  BitField<T,14, 1> OIS4;
  BitField<T,15, 1> reserved2;

  String toString() {
    String rv("CR2");
    rv += "{reserved:" + reserved2();
    rv += ",OIS4:" + OIS4();
    rv += ",OIS3N:" + OIS3N();
    rv += ",OIS3:" + OIS3();
    rv += ",OIS2N:" + OIS2N();
    rv += ",OIS2:" + OIS2();
    rv += ",OIS1N:" + OIS1N();
    rv += ",OIS1:" + OIS1();
    rv += ",TI1S:" + TI1S();
    rv += ",MMS:" + MMS();
    rv += ",CCDS:" + CCDS();
    rv += ",CCUS:" + CCUS();
    rv += ",reserved:" + reserved1();
    rv += ",CCPC:" + CCPC();
    rv += "} " + numeric2HexString(*this->_p);
    return rv;
  }
};

template <typename T>
struct TIMxSMCR : protected TIMx<T> {
  TIMxSMCR(T *p):TIMx<T>(p), SMS(p), reserved1(p), TS(p), MSM(p), ETF(p), ETPS(p), ECE(p), ETP(p) {
  }

  BitField<T,  0, 3> SMS;
  BitField<T,  3, 1> reserved1;
  BitField<T,  4, 3> TS;
  BitField<T,  7, 1> MSM;
  BitField<T,  8, 4> ETF;
  BitField<T, 12, 2> ETPS;
  BitField<T, 14, 1> ECE;
  BitField<T, 15, 1> ETP;

  String toString() {
    String rv("SMCR");

    rv += "{ETP:" + ETP();
    rv += ",ECE:" + ECE();
    rv += ",ETPS:" + ETPS();
    rv += ",ETF:" + ETF();
    rv += ",MSM:" + MSM();
    rv += ",TS:" + TS();
    rv += ",reserved:" + reserved1();
    rv += ",SMS:" + SMS();
    rv += "} " + numeric2HexString(*this->_p);
    return rv;
  }
};

template <typename T>
struct TIMxDIER : protected TIMx<T> {
  TIMxDIER(T *p):TIMx<T>(p),UIE(p),CC1IE(p),CC2IE(p),CC3IE(p),CC4IE(p),COMIE(p),
                 TIE(p),BIE(p),UDE(p),CC1DE(p),CC2DE(p),CC3DE(p),CC4DE(p),
                 CCDE(p),TDE(p),reserved(p) {
  }

  BitField<T, 0, 1> UIE;
  BitField<T, 1, 1> CC1IE;
  BitField<T, 2, 1> CC2IE;
  BitField<T, 3, 1> CC3IE;
  BitField<T, 4, 1> CC4IE;
  BitField<T, 5, 1> COMIE;
  BitField<T, 6, 1> TIE;
  BitField<T, 7, 1> BIE;
  BitField<T, 8, 1> UDE;
  BitField<T, 9, 1> CC1DE;
  BitField<T,10, 1> CC2DE;
  BitField<T,11, 1> CC3DE;
  BitField<T,12, 1> CC4DE;
  BitField<T,13, 1> CCDE;
  BitField<T,12, 1> TDE;
  BitField<T, 15, 1> reserved;

  String toString() {
    String rv("DIER");

    rv += "{reserved:" + reserved();
    rv += ",TDE:" + TDE();
    rv += ",CCDE:" + CCDE();
    rv += ",CC4DE:" + CC4DE();
    rv += ",CC3DE:" + CC3DE();
    rv += ",CC2DE:" + CC2DE();
    rv += ",CC1DE:" + CC1DE();
    rv += ",UDE:" + UDE();
    rv += ",BIE:" + BIE();
    rv += ",TIE:" + TIE();
    rv += ",COMIE:" + COMIE();
    rv += ",CC4IE:" + CC4IE();
    rv += ",CC3IE:" + CC3IE();
    rv += ",CC2IE:" + CC2IE();
    rv += ",CC1IE:" + CC1IE();
    rv += ",UIE:" + UIE();
    rv += "} " + numeric2HexString(*this->_p);
    return rv;
  }
};

template <typename T>
struct TIMxSR : protected TIMx<T> {
  TIMxSR(T *p):TIMx<T>(p),UIF(p),CC1IF(p),CC2IF(p),CC3IF(p),CC4IF(p),
               COMIF(p),TIF(p), BIF(p), reserved1(p),
               CC1OF(p),CC2OF(p),CC3OF(p),CC4OF(p),reserved2(p){
  }

  BitField<T, 0, 1> UIF;
  BitField<T, 1, 1> CC1IF;
  BitField<T, 2, 1> CC2IF;
  BitField<T, 3, 1> CC3IF;
  BitField<T, 4, 1> CC4IF;
  BitField<T, 5, 1> COMIF;
  BitField<T, 6, 1> TIF;
  BitField<T, 7, 1> BIF;
  BitField<T, 8, 1> reserved1;
  BitField<T, 9, 1> CC1OF;
  BitField<T,10, 1> CC2OF;
  BitField<T,11, 1> CC3OF;
  BitField<T,12, 1> CC4OF;
  BitField<T,13, 3> reserved2;

  String toString() {
    String rv("SR");

    rv += "{reserved:" + reserved2();
    rv += ",CC4OF:" + CC4OF();
    rv += ",CC3OF:" + CC3OF();
    rv += ",CC2OF:" + CC2OF();
    rv += ",CC1OF:" + CC1OF();
    rv += ",reserved:" + reserved1();
    rv += ",BIF:" + BIF();
    rv += ",TIF:" + TIF();
    rv += ",COMIF:" + COMIF();
    rv += ",CC4IF:" + CC4IF();
    rv += ",CC3IF:" + CC3IF();
    rv += ",CC2IF:" + CC2IF();
    rv += ",CC1IF:" + CC1IF();
    rv += ",UIF:" + UIF();
    rv += "} " + numeric2HexString(*this->_p);
    return rv;
  }
};


template <typename T>
struct TIMxEGR : protected TIMx<T> {
  TIMxEGR(T *p):TIMx<T>(p),UG(p),CC1G(p),CC2G(p),CC3G(p),CC4G(p),COMG(p),TG(p),BG(p),reserved(p) {
  }

  BitField<T, 0, 1> UG;
  BitField<T, 1, 1> CC1G;
  BitField<T, 2, 1> CC2G;
  BitField<T, 3, 1> CC3G;
  BitField<T, 4, 1> CC4G;
  BitField<T, 5, 1> COMG;
  BitField<T, 6, 1> TG;
  BitField<T, 7, 1> BG;
  BitField<T, 8, 8> reserved;

  String toString() {
    String rv("EGR");

    rv += "{reserved:" + reserved();
    rv += ", BG:" + BG();
    rv += ",TG:" + TG();
    rv += ",COMG:" + COMG();
    rv += ",CC4G:" + CC4G();
    rv += ",CC3G:" + CC3G();
    rv += ",CC2G:" + CC2G();
    rv += ",CC1G:" + CC1G();
    rv += ",UG:" + UG();
    rv += "} " + numeric2HexString(*this->_p);
    return rv;
  }
};

template <typename T>
struct TIMxCCMR1 : protected TIMx<T> {
  TIMxCCMR1(T *p):TIMx<T>(p),
                  CC1S(p),OC1FE(p),OC1PE(p),OC1M(p),OC1CE(p),
                  CC2S(p),OC2FE(p),OC2PE(p),OC2M(p),OC2CE(p),
                  IC1PSC(p),IC1F(p),IC2PSC(p),IC2F(p){
  }

  BitField<T,  0, 2> CC1S;
  BitField<T,  2, 1> OC1FE;
  BitField<T,  3, 1> OC1PE;
  BitField<T,  4, 3> OC1M;
  BitField<T,  7, 1> OC1CE;

  BitField<T,  8, 2> CC2S;
  BitField<T, 10, 1> OC2FE;
  BitField<T, 11, 1> OC2PE;
  BitField<T, 12, 3> OC2M;
  BitField<T, 15, 1> OC2CE;

  BitField<T, 2, 2> IC1PSC;
  BitField<T, 4, 4> IC1F;
  BitField<T, 10, 2> IC2PSC;
  BitField<T, 12, 4> IC2F;

  String toString() {
    String rv("CCMR1{");
    if (CC2S.get()==0) {
      // output mode
      rv += "OC2CE:" + OC2CE();
      rv += ",OC2M:" + OC2M();
      rv += ",OC2PE:" + OC2PE();
      rv += ",OC2FE:" + OC2FE();
    } else {
      // input mode
      rv += "IC2F:" + IC2F();
      rv += ",IC2PSC:" + IC2PSC();
    }
    rv += ",CC2S:" + CC2S();

    if (CC1S.get()==0) {
      // output mode
      rv += ", OC1CE:" + OC1CE();
      rv += ",OC1M:" + OC1M();
      rv += ",OC1PE:" + OC1PE();
      rv += ",OC1FE:" + OC1FE();
    } else {
      // input mode
      rv += ", IC1F:" + IC1F();
      rv += ",IC1PSC:" + IC1PSC();
    }
    rv += ",CC1S:" + CC1S();
    rv += "} " + numeric2HexString(*this->_p);

    return rv;
  }
};

template <typename T>
struct TIMxCCMR2 : protected TIMx<T> {
  TIMxCCMR2(T *p):TIMx<T>(p),
                  CC3S(p),OC3FE(p),OC3PE(p),OC3M(p),OC3CE(p),
                  CC4S(p),OC4FE(p),OC4PE(p),OC4M(p),OC4CE(p),
                  IC3PSC(p),IC3F(p),IC4PSC(p),IC4F(p){
  }

  BitField<T,  0, 2> CC3S;
  BitField<T,  2, 1> OC3FE;
  BitField<T,  3, 1> OC3PE;
  BitField<T,  4, 3> OC3M;
  BitField<T,  7, 1> OC3CE;

  BitField<T,  8, 2> CC4S;
  BitField<T, 10, 1> OC4FE;
  BitField<T, 11, 1> OC4PE;
  BitField<T, 12, 3> OC4M;
  BitField<T, 15, 1> OC4CE;

  BitField<T, 2, 2> IC3PSC;
  BitField<T, 4, 4> IC3F;
  BitField<T, 10, 2> IC4PSC;
  BitField<T, 12, 4> IC4F;

  String toString() {
    String rv("CCMR2{");
    if (CC4S.get()==0) {
      // output mode
      rv += "OC4CE:" + OC4CE();
      rv += ",OC4M:" + OC4M();
      rv += ",OC4PE:" + OC4PE();
      rv += ",OC4FE:" + OC4FE();
    } else {
      // input mode
      rv += "IC4F:" + IC4F();
      rv += ",IC4PSC:" + IC4PSC();
    }
    rv += ",CC4S:" + CC4S();

    if (CC3S.get()==0) {
      // output mode
      rv += ", OC3CE:" + OC3CE();
      rv += ",OC3M:" + OC3M();
      rv += ",OC3PE:" + OC3PE();
      rv += ",OC3FE:" + OC3FE();
    } else {
      // input mode
      rv += ", IC3F:" + IC3F();
      rv += ",IC3PSC:" + IC3PSC();
    }
    rv += ",CC3S:" + CC3S();
    rv += "} " + numeric2HexString(*this->_p);

    return rv;
  }
};

template <typename T>
struct TIMxCCER : protected TIMx<T> {
  TIMxCCER(T *p):TIMx<T>(p),CC1E(p), CC1P(p), CC1NE(p), CC1NP(p),
                   CC2E(p), CC2P(p), CC2NE(p), CC2NP(p),
                   CC3E(p), CC3P(p), CC3NE(p), CC3NP(p),
                   CC4E(p), CC4P(p), reserved(p) {
  }

  BitField<T, 0, 1> CC1E;
  BitField<T, 1, 1> CC1P;
  BitField<T, 2, 1> CC1NE;
  BitField<T, 3, 1> CC1NP;

  BitField<T, 4, 1> CC2E;
  BitField<T, 5, 1> CC2P;
  BitField<T, 6, 1> CC2NE;
  BitField<T, 7, 1> CC2NP;

  BitField<T, 8, 1> CC3E;
  BitField<T, 9, 1> CC3P;
  BitField<T,10, 1> CC3NE;
  BitField<T,11, 1> CC3NP;

  BitField<T,12, 1> CC4E;
  BitField<T,13, 1> CC4P;

  BitField<T,14, 2> reserved;

  String toString() {
    String rv("CCER");

    rv += "{reserved:" + reserved();

    rv += ", CC4P:" + CC4P();
    rv += ",CC4E:" + CC4E();

    rv += ", CC3NP:" + CC3NP();
    rv += ",CC3NE:" + CC3NE();
    rv += ",CC3P:" + CC3P();
    rv += ",CC3E:" + CC3E();

    rv += ", CC2NP:" + CC2NP();
    rv += ",CC2NE:" + CC2NE();
    rv += ",CC2P:" + CC2P();
    rv += ",CC2E:" + CC2E();

    rv += ", CC1NP:" + CC1NP();
    rv += ",CC1NE:" + CC1NE();
    rv += ",CC1P:" + CC1P();
    rv += ",CC1E:" + CC1E();
    rv += "} " + numeric2HexString(*this->_p);
    return rv;
  }
};

template <typename T>
struct TIMxCNT : protected TIMx<T> {
  TIMxCNT(T *p):TIMx<T>(p),CNT(p) {
  }

  BitField<T, 0, 16> CNT;

  String toString() {
    String rv("CNT{");
    rv += "CNT:" + CNT();
    rv += "} " + numeric2HexString(*this->_p);
    return rv;
  }
};

template <typename T>
struct TIMxPSC : protected TIMx<T> {
  TIMxPSC(T *p):TIMx<T>(p),PSC(p) {
  }

  BitField<T, 0, 16> PSC;

  String toString() {
    String rv("PSC{");
    rv += "PSC:" + PSC();
    rv += "} " + numeric2HexString(*this->_p);
    return rv;
  }
};

template <typename T>
struct TIMxARR : protected TIMx<T>{
  TIMxARR(T *p):TIMx<T>(p), ARR(p) {
  }

  BitField<T, 0, 16> ARR;

  String toString() {
    String rv("ARR{");
    rv += "ARR:" + ARR();
    rv += "} " + numeric2HexString(*this->_p);
    return rv;
  }
};

template <typename T>
struct TIMxRCR : protected TIMx<T> {
  TIMxRCR(T *p):TIMx<T>(p), REP(p), reserved(p) {
  }

  BitField<T, 0, 8> REP;
  BitField<T, 8, 8> reserved;

  String toString() {
    String rv("RCR");

    rv += "{reserved:" + reserved();

    rv += ", REP:" + REP();
    rv += "} " + numeric2HexString(*this->_p);
    return rv;
  }
};

template <typename T>
struct TIMxCCR1 : protected TIMx<T> {
  TIMxCCR1(T *p):TIMx<T>(p),CCR1(p) {
  }

  BitField<T, 0, 16> CCR1;

  String toString() {
    String rv("CCR1");

    rv += "{CCR1:" + CCR1();
    rv += "} " + numeric2HexString(*this->_p);
    return rv;
  }
};

template <typename T>
struct TIMxCCR2 : protected TIMx<T>{
  TIMxCCR2(T *p):TIMx<T>(p),CCR2(p) {
  }

  BitField<T, 0, 16> CCR2;

  String toString() {
    String rv("CCR2");

    rv += "{CCR2:" + CCR2();
    rv += "} " + numeric2HexString(*this->_p);
    return rv;
  }
};

template <typename T>
struct TIMxCCR3 : protected TIMx<T> {
  TIMxCCR3(T *p):TIMx<T>(p),CCR3(p) {
  }

  BitField<T, 0, 16> CCR3;

  String toString() {
    String rv("CCR3");

    rv += "{CCR3:" + CCR3();
    rv += "} " + numeric2HexString(*this->_p);
    return rv;
  }
};

template <typename T>
struct TIMxCCR4 : protected TIMx<T> {
  TIMxCCR4(T *p):TIMx<T>(p),CCR4(p) {
  }

  BitField<T, 0, 16> CCR4;

  String toString() {
    String rv("CCR4");

    rv += "{CCR4:" + CCR4();
    rv += "} " + numeric2HexString(*this->_p);
    return rv;
  }
};

template <typename T>
struct TIMxBDTR : protected TIMx<T> {
  TIMxBDTR(T *p):TIMx<T>(p),DGT(p),LOCK(p),OSSI(p),OSSR(p),BKE(p),BKP(p),AOE(p),MOE(p){
  }

  BitField<T, 0, 8> DGT;
  BitField<T, 8, 2> LOCK;
  BitField<T,10, 1> OSSI;
  BitField<T,11, 1> OSSR;
  BitField<T,12, 1> BKE;
  BitField<T,13, 1> BKP;
  BitField<T,14, 1> AOE;
  BitField<T,15, 1> MOE;

  String toString() {
    String rv("BDTR");

    rv += "{MOE:" + MOE();
    rv += ",AOE:" + AOE();
    rv += ",BKP:" + BKP();
    rv += ",BKE:" + BKE();
    rv += ",OSSR:" + OSSR();
    rv += ",OSSI:" + OSSI();
    rv += ",LOCK:" + LOCK();
    rv += ",DGT:" + DGT();
    rv += "} " + numeric2HexString(*this->_p);
    return rv;
  }
};

template <typename T>
struct TIMxDCR : protected TIMx<T> {
  TIMxDCR(T *p):TIMx<T>(p),DBA(p),reserved1(p),DBL(p),reserved2(p) {
  }

  BitField<T, 0, 5> DBA;
  BitField<T, 3, 3> reserved1;
  BitField<T, 8, 5> DBL;
  BitField<T,13, 3> reserved2;

  String toString() {
    String rv("DRC");

    rv += "{reserved:" + reserved2();
    rv += " DBL:" + DBL();
    rv += ",reserved:" + reserved1();
    rv += " DBA:" + DBA();
    rv += "} " + numeric2HexString(*this->_p);
    return rv;
  }
};

template <typename T>
struct TIMxDMAR : protected TIMx<T> {
  TIMxDMAR(T *p):TIMx<T>(p),DMAR(p) {
  }

  BitField<T, 0, 32> DMAR;

  String toString() {
    String rv("DMAR");
    rv += "{DMAR:" + DMAR();
    rv += "} " + numeric2HexString(*this->_p);
    return rv;
  }
};

struct BasicTimer {
  BasicTimer() = delete;
  BasicTimer(timer_gen_reg_map* p):
    CR1(&p->CR1), CR2(&p->CR2), DIER(&p->DIER),
    SR(&p->SR), EGR(&p->EGR), CNT(&p->CNT), PSC(&p->PSC), ARR(&p->ARR) {
  }

  TIMxCR1<volatile uint32_t>     CR1; /**< Control register 1 */
  TIMxCR2<volatile uint32_t>     CR2; /**< Control register 2 */
  TIMxDIER<volatile uint32_t>   DIER; /**< DMA/interrupt enable register */
  TIMxSR<volatile uint32_t>       SR; /**< Status register */
  TIMxEGR<volatile uint32_t>     EGR; /**< Event generation register  */
  TIMxCNT<volatile uint32_t>     CNT; /**< Counter */
  TIMxPSC<volatile uint32_t>     PSC; /**< Prescaler */
  TIMxARR<volatile uint32_t>     ARR; /**< Auto-reload register */


  void println(Stream& stream) {
    stream.println(CR1.toString());
    stream.println(CR2.toString());
    stream.println(DIER.toString());
    stream.println(SR.toString());
    stream.println(EGR.toString());
    stream.println(CNT.toString());
    stream.println(PSC.toString());
    stream.println(ARR.toString());
  }
};

struct  GeneralTimer{
  GeneralTimer() = delete;
  GeneralTimer(timer_gen_reg_map* p):
    CR1(&p->CR1), CR2(&p->CR2), SMCR(&p->SMCR), DIER(&p->DIER),
    SR(&p->SR), EGR(&p->EGR), CCMR1(&p->CCMR1), CCMR2(&p->CCMR2),
    CCER(&p->CCER), CNT(&p->CNT), PSC(&p->PSC), ARR(&p->ARR),
    CCR1(&p->CCR1), CCR2(&p->CCR2), CCR3(&p->CCR3),
    CCR4(&p->CCR4), DCR(&p->DCR), DMAR(&p->DMAR){
  }

  TIMxCR1<volatile uint32_t>     CR1; /**< Control register 1 */
  TIMxCR2<volatile uint32_t>     CR2; /**< Control register 2 */
  TIMxSMCR<volatile uint32_t>   SMCR; /**< Slave mode control register */
  TIMxDIER<volatile uint32_t>   DIER; /**< DMA/interrupt enable register */
  TIMxSR<volatile uint32_t>       SR; /**< Status register */
  TIMxEGR<volatile uint32_t>     EGR; /**< Event generation register  */
  TIMxCCMR1<volatile uint32_t> CCMR1; /**< Capture/compare mode register 1 */
  TIMxCCMR2<volatile uint32_t> CCMR2; /**< Capture/compare mode register 2 */
  TIMxCCER<volatile uint32_t>   CCER; /**< Capture/compare enable register */
  TIMxCNT<volatile uint32_t>     CNT; /**< Counter */
  TIMxPSC<volatile uint32_t>     PSC; /**< Prescaler */
  TIMxARR<volatile uint32_t>     ARR; /**< Auto-reload register */
  TIMxCCR1<volatile uint32_t>   CCR1; /**< Capture/compare register 1 */
  TIMxCCR2<volatile uint32_t>   CCR2; /**< Capture/compare register 2 */
  TIMxCCR3<volatile uint32_t>   CCR3; /**< Capture/compare register 3 */
  TIMxCCR4<volatile uint32_t>   CCR4; /**< Capture/compare register 4 */
  TIMxDCR<volatile uint32_t>     DCR; /**< DMA control register */
  TIMxDMAR<volatile uint32_t>   DMAR; /**< DMA address for full transfer */

  void println(Stream& stream) {
    stream.println(CR1.toString());
    stream.println(CR2.toString());
    stream.println(SMCR.toString());
    stream.println(DIER.toString());
    stream.println(SR.toString());
    stream.println(EGR.toString());
    stream.println(CCMR1.toString());
    stream.println(CCMR2.toString());
    stream.println(CCER.toString());
    stream.println(CNT.toString());
    stream.println(PSC.toString());
    stream.println(ARR.toString());
    stream.println(CCR1.toString());
    stream.println(CCR2.toString());
    stream.println(CCR3.toString());
    stream.println(CCR4.toString());
    stream.println(DCR.toString());
    stream.println(DMAR.toString());
  }
};

struct  AdvancedTimer{
  AdvancedTimer() = delete;
  AdvancedTimer(timer_adv_reg_map* p):
    CR1(&p->CR1), CR2(&p->CR2), SMCR(&p->SMCR), DIER(&p->DIER),
    SR(&p->SR), EGR(&p->EGR), CCMR1(&p->CCMR1), CCMR2(&p->CCMR2),
    CCER(&p->CCER), CNT(&p->CNT), PSC(&p->PSC), ARR(&p->ARR),
    RCR(&p->RCR), CCR1(&p->CCR1), CCR2(&p->CCR2), CCR3(&p->CCR3),
    CCR4(&p->CCR4), BDTR(&p->BDTR), DCR(&p->DCR), DMAR(&p->DMAR){
  }

  TIMxCR1<volatile uint32_t>     CR1; /**< Control register 1 */
  TIMxCR2<volatile uint32_t>     CR2; /**< Control register 2 */
  TIMxSMCR<volatile uint32_t>   SMCR; /**< Slave mode control register */
  TIMxDIER<volatile uint32_t>   DIER; /**< DMA/interrupt enable register */
  TIMxSR<volatile uint32_t>       SR; /**< Status register */
  TIMxEGR<volatile uint32_t>     EGR; /**< Event generation register  */
  TIMxCCMR1<volatile uint32_t> CCMR1; /**< Capture/compare mode register 1 */
  TIMxCCMR2<volatile uint32_t> CCMR2; /**< Capture/compare mode register 2 */
  TIMxCCER<volatile uint32_t>   CCER; /**< Capture/compare enable register */
  TIMxCNT<volatile uint32_t>     CNT; /**< Counter */
  TIMxPSC<volatile uint32_t>     PSC; /**< Prescaler */
  TIMxARR<volatile uint32_t>     ARR; /**< Auto-reload register */
  TIMxRCR<volatile uint32_t>     RCR; /**< Repetition counter register */
  TIMxCCR1<volatile uint32_t>   CCR1; /**< Capture/compare register 1 */
  TIMxCCR2<volatile uint32_t>   CCR2; /**< Capture/compare register 2 */
  TIMxCCR3<volatile uint32_t>   CCR3; /**< Capture/compare register 3 */
  TIMxCCR4<volatile uint32_t>   CCR4; /**< Capture/compare register 4 */
  TIMxBDTR<volatile uint32_t>   BDTR; /**< Break and dead-time register */
  TIMxDCR<volatile uint32_t>     DCR; /**< DMA control register */
  TIMxDMAR<volatile uint32_t>   DMAR; /**< DMA address for full transfer */

  void println(Stream& stream) {
    stream.println(CR1.toString());
    stream.println(CR2.toString());
    stream.println(SMCR.toString());
    stream.println(DIER.toString());
    stream.println(SR.toString());
    stream.println(EGR.toString());
    stream.println(CCMR1.toString());
    stream.println(CCMR2.toString());
    stream.println(CCER.toString());
    stream.println(CNT.toString());
    stream.println(PSC.toString());
    stream.println(ARR.toString());
    stream.println(RCR.toString());
    stream.println(CCR1.toString());
    stream.println(CCR2.toString());
    stream.println(CCR3.toString());
    stream.println(CCR4.toString());
    stream.println(BDTR.toString());
    stream.println(DCR.toString());
    stream.println(DMAR.toString());
  }
};
} // namespace timerdiagnostics_stm

extern timerdiagnostics_stm::AdvancedTimer timer1Diagnostics;
extern timerdiagnostics_stm::GeneralTimer timer2Diagnostics;
extern timerdiagnostics_stm::GeneralTimer timer3Diagnostics;
extern timerdiagnostics_stm::GeneralTimer timer4Diagnostics;
#ifdef STM32_HIGH_DENSITY
extern timerdiagnostics_stm::GeneralTimer timer5Diagnostics;
extern timerdiagnostics_stm::BasicTimer timer6Diagnostics;
extern timerdiagnostics_stm::BasicTimer timer7Diagnostics;
extern timerdiagnostics_stm::AdvancedTimer timer8Diagnostics;
#endif //STM32_HIGH_DENSITY
