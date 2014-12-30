/*
 Crude VGA Output

 Outputs a red and white leaf to VGA. This implementation is crude and noisy,
 but a fun demo. It should run most VGA monitors at 640x480, though it does
 not follow the timing spec very carefully. Real twisted or shielded wires,
 proper grounding, and not doing this on a breadboard are recommended (but
 it seems to work ok without). In short, this example is "unsupported"; this
 surely isn't the best way to do VGA video with a Maple, but it demonstrates
 the Timer functionality and is a cool hack so here it is.

 Serial is disabled to get rid of most interrupts (which mess with timing);
 the SysTick is probably the source of the remaining flickers. This means that
 you have to use perpetual bootloader or the reset button to flash new 
 programs.

 How to wire this to a VGA port:
    D5 via ~200ohms to VGA Red     (1)
    D6 via ~200ohms to VGA Green   (2)
    D7 via ~200ohms to VGA Blue    (3)
    D11 to VGA VSync               (14)  (swapped?)
    D12 to VGA HSync               (13)  (swapped?)
    GND to VGA Ground              (5)
    GND to VGA Sync Ground         (10)

 See also:
  - notes/vga.txt for timing numbers and more caveats
  - http://www-mtl.mit.edu/Courses/6.111/labkit/vga.shtml
  - http://pinouts.ru/Video/VGA15_pinout.shtml
  - http://www.epanorama.net/documents/pc/vga_timing.html
 
 Created 20 July 2010
 By Bryan Newbold for LeafLabs
 This code is released with no strings attached.
 
 */

#define LED_PIN 13

// Pinouts
#define VGA_R 5     // STM32: B6
#define VGA_G 6     // STM32: A8
#define VGA_B 7     // STM32: A9
#define VGA_V 11    // STM32: A6
#define VGA_H 12    // STM32: A7

// These low level macros make GPIO writes much faster
#define VGA_R_HIGH (GPIOB_BASE)->BSRR = BIT(6)
#define VGA_R_LOW  (GPIOB_BASE)->BRR  = BIT(6)
#define VGA_G_HIGH (GPIOA_BASE)->BSRR = BIT(8)
#define VGA_G_LOW  (GPIOA_BASE)->BRR  = BIT(8)
#define VGA_B_HIGH (GPIOA_BASE)->BSRR = BIT(9)
#define VGA_B_LOW  (GPIOA_BASE)->BRR  = BIT(9)
#define VGA_V_HIGH (GPIOA_BASE)->BSRR = BIT(6)
#define VGA_V_LOW  (GPIOA_BASE)->BRR  = BIT(6)
#define VGA_H_HIGH (GPIOA_BASE)->BSRR = BIT(7)
#define VGA_H_LOW  (GPIOA_BASE)->BRR  = BIT(7)

void isr_porch(void);
void isr_start(void);
void isr_stop(void);
void isr_update(void);

uint8 toggle;
uint16 x = 0;       // X coordinate
uint16 y = 0;       // Y coordinate
uint8 v_active = 1; // Are we in the image?

// 1-bit!
uint8 logo[18][16] = {
    {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,},
    {0,0,0,0,0,0,1,1,1,0,0,0,0,0,0,0,},
    {0,0,0,0,0,1,0,0,0,1,0,0,0,0,0,0,},
    {0,0,0,0,1,0,0,1,0,0,1,0,0,0,0,0,},
    {0,0,0,1,0,0,0,1,0,0,0,1,0,0,0,0,},
    {0,0,1,0,0,0,0,1,0,0,0,0,1,0,0,0,},
    {0,0,1,0,0,1,0,1,0,1,0,0,1,0,0,0,},
    {0,1,0,0,0,0,1,1,1,0,0,0,0,1,0,0,},
    {0,1,0,1,0,0,0,1,0,0,0,1,0,1,0,0,},
    {1,0,0,0,1,0,0,1,0,0,1,0,0,0,1,0,},
    {1,0,0,0,0,1,0,1,0,1,0,0,0,0,1,0,},
    {1,0,0,0,0,0,1,1,1,0,0,0,0,0,1,0,},
    {0,1,0,0,0,0,0,1,0,0,0,0,0,1,0,0,},
    {0,0,1,1,0,0,0,1,0,0,0,1,1,0,0,0,},
    {0,0,0,0,1,1,1,0,1,1,1,0,0,0,0,0,},
    {0,0,0,0,0,0,1,0,1,0,0,0,0,0,0,0,},
    {0,0,0,0,0,0,1,1,1,0,0,0,0,0,0,0,},
    {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,}, };

void setup() {
	Serial.begin(115200); // Ignored by Maple. But needed by boards using hardware serial via a USB to Serial adaptor
    // Setup our pins
    pinMode(LED_PIN, OUTPUT);
    pinMode(VGA_R, OUTPUT);
    pinMode(VGA_G, OUTPUT);
    pinMode(VGA_B, OUTPUT);
    pinMode(VGA_V, OUTPUT);
    pinMode(VGA_H, OUTPUT);
    digitalWrite(VGA_R, LOW);
    digitalWrite(VGA_G, LOW);
    digitalWrite(VGA_B, LOW);
    digitalWrite(VGA_H, HIGH);
    digitalWrite(VGA_V, HIGH);

    // This gets rid of the majority of the interrupt artifacts;
    // a SysTick.end() is required as well
    Serial.end();
    

    // Configure
    Timer4.pause(); // while we configure
    Timer4.setPrescaleFactor(1);     // Full speed
    Timer4.setChannel1Mode(TIMER_OUTPUTCOMPARE);
    Timer4.setChannel2Mode(TIMER_OUTPUTCOMPARE);
    Timer4.setChannel3Mode(TIMER_OUTPUTCOMPARE);
    Timer4.setChannel4Mode(TIMER_OUTPUTCOMPARE);
    Timer4.setOverflow(2287);   // Total line time

    Timer4.setCompare1(200);
    Timer4.attachCompare1Interrupt(isr_porch);
    Timer4.setCompare2(300);
    Timer4.attachCompare2Interrupt(isr_start);
    Timer4.setCompare3(2170);
    Timer4.attachCompare3Interrupt(isr_stop);
    Timer4.setCompare4(1);      // Could be zero I guess
    Timer4.attachCompare4Interrupt(isr_update);
    
    Timer4.setCount(0);         // Ready...
    Timer4.resume();            // Go!
}

void loop() {
    toggle ^= 1;
    digitalWrite(LED_PIN, toggle);
    delay(100);

    // Everything happens in the interrupts!
}


// This ISR will end horizontal sync for most of the image and
// setup the vertical sync for higher line counts
void isr_porch(void) {
    VGA_H_HIGH;
    y++;
    // Back to the top
    if(y>=523) {
        y=1;
        v_active = 1;
        return;
    }
    // Other vsync stuff below the image
    if(y>=492) {
        VGA_V_HIGH;
        return;
    }
    if(y>=490) {
        VGA_V_LOW;
        return;
    }
    if(y>=479) { 
        v_active = 0;
        return;
    }
}

// This is the main horizontal sweep
void isr_start(void) {  
    // Skip if we're not in the image at all
    if(!v_active) { return; }

    // Start Red
    VGA_R_LOW;
    VGA_R_HIGH;

    // For each "pixel" (really 20 or so screen pixels?) go red or white
    for(x=0; x<32; x++) {
        if(logo[y/28][x/2]) {
            VGA_G_HIGH;
            VGA_B_HIGH;
        } else {
            VGA_G_LOW;
            VGA_B_LOW;
        }
    }
}

// End of the horizontal line
void isr_stop(void) {
    if(!v_active) { return; }
    VGA_R_LOW;
    VGA_G_LOW;
    VGA_B_LOW;
}

// Setup horizonal sync
void isr_update(void) {
    VGA_H_LOW;
}

