#include <Arduino.h>

#define LEDON 0
#define LEDOFF 1

bool adc_overrun = false;
bool awd_triggered = false;

void overrun_handler();
void awd_handler();
void setADCprescaler();
void toggle_awd();
float temp_convert(int data);
void setup_awd(uint16_t data);

void setup() {

  //note this turns 'on' the led, as active low is assumed
  pinMode(LED_BUILTIN, OUTPUT);
  Serial.begin(); // USB serial

  while(!Serial); // wait for terminal to connect

  // setup the ADC prescaler
  setADCprescaler();
  // use a long sample time - measure temperature
  // if we assume pclk = 84 mhz, prescaler = 4, so adcclock = 84 / 4 = 21 mhz
  // from rm0009
  // Tconv = sample time + 12 cycles = 480 + 12 = 492 cycles ~ 42 k samples per sec !
  adc_set_sampling_time(ADC1, ADC_SMPR_480);
  // use single mode
  adc_clear_continuous(ADC1);

  //enable temp sensor
  adc_enable_tsvref();

  //setup the AWD to test threshold triggering
  //in this case it is using the AWD to detect high (i.e. max) temperatures
  //delay(1);
  //uint16_t data = adc_read(ADC1, 18);
  uint16_t data = 800; //set a low threshold so that awd would trigger
  setup_awd(data);
  adc_awd_enable_channel(ADC1, 18);
  Serial.print("Temp (C):");
  Serial.println(temp_convert(data));

  // attach overrun interrupt, this is for the overrun interrupt test
  adc_attach_interrupt(ADC1, ADC_OVR, overrun_handler);
  ADC1->regs->CR2 |= ADC_CR2_EOCS;

  // attach awd interrupt, this is for awd tests
  adc_attach_interrupt(ADC1, ADC_AWD, awd_handler);

  digitalWrite(LED_BUILTIN, LEDOFF);
  Serial.println("enter 'w' to toggle AWD");
  adc_awd_enable(ADC1);
}

void loop() {

  if(Serial.available()) {
    uint8_t c = Serial.read();
    if(c == 'w')
      toggle_awd();
  }

  if(adc_overrun) {
    Serial.println("ADC overrun!");
    digitalWrite(LED_BUILTIN, LEDON);
    delay(100);
    digitalWrite(LED_BUILTIN, LEDOFF);
    delay(100);
    adc_overrun = false;
  }

  // this is the call to read adc voltages.
  // uint18_t data = adc_read(ADC1, 18);
  // as both awd and adc is being used, when awd triggers
  // AWD is set but EOC may not be set, hence adc_read() is not used
  // and we'd need to do some additional handling for AWD trigger
    adc_start_single_convert(ADC1, 18);
    while (!adc_is_end_of_convert(ADC1)) {
      if(awd_triggered) {
        //if awd is triggered we disable awd and read
        //the adc again
        adc_awd_disable(ADC1);
        adc_start_single_convert(ADC1, 18);
      }
      //wait for systick, delay(1)
      asm("wfi");
    }
    uint16_t data = adc_get_data(ADC1);

  // uncomment next 3 statements to test overrun
    //ADC1->regs->CR2 |= ADC_CR2_SWSTART;
  //delay(1);
  //ADC1->regs->CR2 |= ADC_CR2_SWSTART;

  if(awd_triggered) {
    Serial.println("New High Temp");
    setup_awd(data);
    adc_awd_enable(ADC1);
    awd_triggered = false;
  }

  Serial.print("Temp (C):");
  Serial.println(temp_convert(data));

  // sleep 1-2s wait for systick, not accurate
  for(int i=0; i<2000; i++) asm("wfi");
}

void overrun_handler() {
  adc_overrun = true;
}

void awd_handler() {
  ADC1->regs->SR &= ~ ADC_SR_AWD;
  awd_triggered = true;
}

void setup_awd(uint16_t data) {
  adc_awd_set_high_limit(ADC1, data);
  adc_awd_set_low_limit(ADC1, 0);
  ADC1->regs->SR &= ~ ADC_SR_AWD;
}

void toggle_awd() {
  uint8_t awden = bb_peri_get_bit(&(ADC1->regs->CR1), ADC_CR1_AWDEN_BIT);
  if(awden) {
    adc_awd_disable(ADC1);
    Serial.println("AWD disabled");
  } else {
    adc_awd_enable(ADC1);
    Serial.println("AWD enabled");
  }

}

void setADCprescaler() {
  // in datasheet ADC max freq, e.g. F401/F411/F405/F407/F429
  const int ADC_MAX_MHZ = 36;

  // note CLOCK_SPEED_MHZ is defined in the variant board specific .h file
  //      to get a 2.4 Msps speeds for each ADC a cpu clock speeds of 144 mhz or 72 mhz needs to be used
  //      so that a pclk2 runs at 72 mhz and adc prescaler = 2 so max adc clock = 36 mhz can be achieved
  int pclk2 = CLOCK_SPEED_MHZ > 100 ? CLOCK_SPEED_MHZ / 2 : CLOCK_SPEED_MHZ;

  int prescaler_sel = 1;
  // find the pre-scaler divisor that gives adc clock frequency less than or
  // equal to ADC_MAX_MHZ
  while( pclk2 / ( prescaler_sel * 2) > ADC_MAX_MHZ )
    prescaler_sel++;

  Serial.print("Setting ADC clock to: ");
  Serial.print(pclk2 / (prescaler_sel * 2));
  Serial.println(" Mhz");

  adc_prescaler prescaler;
  switch (prescaler_sel * 2) {
  case 2:
    prescaler = ADC_PRE_PCLK2_DIV_2;
    break;
  case 4:
    prescaler = ADC_PRE_PCLK2_DIV_4;
    break;
  case 6:
    prescaler = ADC_PRE_PCLK2_DIV_6;
    break;
  case 8:
    prescaler = ADC_PRE_PCLK2_DIV_8;
    break;
  default:
    // we make an assumption pclk2 = 84mhz, so if 'all else fails'
    // we make the adc clock run at 21 mhz
    prescaler = ADC_PRE_PCLK2_DIV_4;
  }

  adc_set_prescaler(prescaler);
}


float temp_convert(int data) {

    const float averageSlope = 2.5; // mV/°C
    const float v25 = 760.0; // [mV]

    float vSense = (3300.0*data)/4096; // mV
    float temperature = ((vSense-v25)/averageSlope) + 25.0;
    return temperature;
}
