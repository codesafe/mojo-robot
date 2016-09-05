sudo ./usbreset /dev/bus/usb/001/004
sudo ./usbreset /dev/bus/usb/001/005
sudo ./usbreset /dev/bus/usb/001/006

sleep 2
./robotrun 192.168.1.112
  

