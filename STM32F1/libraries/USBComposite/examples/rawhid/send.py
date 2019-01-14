from pywinusb import hid
from time import sleep

SIZE=300

def sample_handler(data):
    print("Raw data: {0}".format(data))
    
device = hid.HidDeviceFilter(vendor_id = 0x1EAF, product_id = 0x0004).get_devices()[0]
print(device)
device.open()
device.set_raw_data_handler(sample_handler)

n = 0
while True:
    for out_report in device.find_output_reports():
        buffer=[i for i in range(SIZE+1)]
        buffer[0]=0x0 # report id
        buffer[-1] = n
        out_report.set_raw_data(buffer)
        print("sending")
        if out_report.send():
            n = (n+1)&0xFF            
        sleep(0.1)
        #sleep(0.005)
        