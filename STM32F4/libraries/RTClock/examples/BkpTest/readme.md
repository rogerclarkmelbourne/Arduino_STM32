# STM32 F4 backup domain registers and backup domain sram

STM32F4xx tend to have 20 backup domain registers. BPK sram is normally only available on the 'larger' 
stm32f4xx mcus, normally 4KB. mcus like stm32f401, stm32f411 has only backup registers.

## Enable writes before writing BKP registers and BKP sram

Before writing to BKP registers or BKP sram  it is necessary to enable writes, 
or it will hardfault / freeze e.g.

```
// BKP register
bkp_enable_writes();
bkp_write(1, 10);
bkp_disable_writes();

// BKP sram
bkp_enable_writes();
bkp_sramwrite8(1, 100);
bkp_disable_writes();

```
Backup registers index starts at 1 ... 20 (normally).

While BKP sram api offset start from zero, offset 0 is beginning of BKP sram.


## Initialization - dependency on RTClock

Using the bkp registers requires RTClock to be initialized. e.g.

```
RTClock rt;
void setup() {
    bkp_init();
    rt.begin();
    
    bkp_enable_writes();
    bkp_write(1, 100); //write 100 in bkp register 1
    bkp_disable_writes();

    int32_t regval = bkp_read(1); // read register 1
}
```

To use bkp sram, it requires separate call to bkp_initsram() to enable it.
passing true enable the backup power regulator, runs on VBAT e.g. coin cell.
passing false in bkp_initsram(false), BKPSRAM is lost if VDD is lost,
but preserves across a reset. e.g.

```
void setup() {
     bkp_init();
     bkp_initsram(true);
     rt.begin();
     ...
}
```