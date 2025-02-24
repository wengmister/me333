# chapter 24 in python

# Professional python programmers put their project
# in a virtual environment to better manage the 
# libraries and versions

# cd into your folder and make an env like this:
    # python3 -m venv env
    # source env/bin/activate
# pip list # to see the libraries installed
# pip install matplotlib # add matplotlib
# pip install pyserial # add serial comm library

# if you are on linux you likely need to
# sudo to use serial

import serial
ser = serial.Serial('/dev/ttyUSB0',230400)
print('Opening port: ')
print(ser.name)

ser.write(b'1 0.2 1000\n') # Kp Ki eintmax

sampnum = 0
read_samples = 10
ADCval = []
ref = []
oc = []
eint = []
while read_samples > 1:
    data_read = ser.read_until(b'\n',50)
    data_text = str(data_read,'utf-8')
    data = list(map(int,data_text.split()))

    if(len(data)==3):
        read_samples = data[0]
        ADCval.append(data[1])
        ref.append(data[2])
        sampnum = sampnum + 1

    if(len(data)==5):
        read_samples = data[0]
        ADCval.append(data[1])
        ref.append(data[2])
        oc.append(data[3]*1024/2400)
        eint.append(data[4])
        sampnum = sampnum + 1

# plot it
import matplotlib.pyplot as plt 
t = range(len(ADCval)) # time array
if(len(data)==3):
    plt.plot(t,ADCval,'r*-',t,ref,'b*-')
if(len(data)==5):
    plt.plot(t,ADCval,'r*-',t,ref,'b*-',t,oc,'k*-',t,eint,'g*-')
plt.ylabel('value')
plt.xlabel('sample')
plt.show()


# be sure to close the port
ser.close()
