from pywinusb import hid
from time import sleep

REPORT_ID = 20
HID_REPORT_FEATURE = 3

device = hid.HidDeviceFilter(vendor_id = 0x1EAF).get_devices()[0] # , product_id = 0x0024
print(device)
device.open()

"""
    uint8_t reportID;
    uint32_t buttons;
    unsigned hat:4;
    unsigned x:10;
    unsigned y:10;
    unsigned rx:10;
    unsigned ry:10;
    unsigned sliderLeft:10;
    unsigned sliderRight:10;
"""

def toBits(n,bits):
    return tuple((n>>i)&1 for i in range(bits))
    
def getByteFromBits(bits,n):
    out = 0
    for i in range(8):
        out += bits[8*n+i] << i
    return out

def joystickData(reportID=REPORT_ID, buttons=0, hat=15, x=512, y=512, rx=512, ry=512, sliderLeft=512, sliderRight=512):
    joyData = ( toBits(reportID,8) + toBits(buttons,32) + toBits(hat,4) + toBits(x,10) + toBits(y,10) + 
                toBits(rx,10) + toBits(ry,10) + toBits(sliderLeft,10) + toBits(sliderRight,10) )
    out = [getByteFromBits(joyData,n) for n in range(13)]
    print(out)
    return out
    
myReport = None    
    
for report in device.find_feature_reports():
    if report.report_id == REPORT_ID and report.report_type == "Feature":
        myReport = report
        break
if myReport is None:        
    for report in device.find_output_reports():
        if report.report_id == REPORT_ID and report.report_type == "Output":
            myReport = report
            break
        
assert myReport is not None        

while True:
    myReport.set_raw_data(joystickData(buttons=7,x=0,y=0))
    
    myReport.send()
    sleep(0.5)
    myReport.set_raw_data(joystickData(buttons=0,x=1023,y=1023))
    myReport.send()
    sleep(0.5)
