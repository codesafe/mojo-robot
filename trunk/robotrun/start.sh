#!/bin/bash


#sleep 15
ID=`lsusb|grep Future|awk -F':' '{print $1}'`
DEVICE1=`echo $ID|awk '{print $2}'`
DEVICE2=`echo $ID|awk '{print $4}'`

DEVICE3=`echo $ID|awk '{print $6}'`
DEVICE4=`echo $ID|awk '{print $8}'`

DEVICE5=`echo $ID|awk '{print $10}'`
DEVICE6=`echo $ID|awk '{print $12}'`

# Bus 001 Device 006 Bus 001 Device 005 Bus 001 Device 004
echo $ID

sudo ./usbreset /dev/bus/usb/$DEVICE1'/'$DEVICE2
sleep 1
sudo ./usbreset /dev/bus/usb/$DEVICE3'/'$DEVICE4
sleep 1
sudo ./usbreset /dev/bus/usb/$DEVICE5'/'$DEVICE6
sleep 1

./robotrun 192.168.1.112 &