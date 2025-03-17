import serial
import time
import matplotlib.pyplot as plt
from statistics import mean
from genref import genRef
import numpy as np

def read_plot_matrix():
    n_str = ser.read_until(b'\n').decode('utf-8')  # get the number of data points to receive
    n_int = int(n_str)  # turn it into an int
    print('Data length = ' + str(n_int))
    
    data = []  # to store the reference and actual current values
    times = []  # to store the time values
    
    for i in range(n_int):
        dat_str = ser.read_until(b'\n').decode('utf-8')  # get the data as a string, ints separated by spaces
        dat_f = list(map(float, dat_str.split()))  # now the data is a list of ints
        data.append(dat_f)
        times.append(i * 0.2)  # 0.2 ms between samples
    
    data = np.array(data)  # convert to numpy array for easier manipulation
    
    if n_int > 1:
        plt.step(times, data[:, 0], label='Reference Current', where='mid')
        plt.step(times, data[:, 1], label='Actual Current', where='mid')
        plt.legend(loc='upper right')
    else:
        print('Only 1 sample received')
        print(data)
    
    # compute the average error
    score = mean(abs(data[:, 0] - data[:, 1]))
    print(f'\nAverage error: {score:.1f} mA')
    plt.title(f'Average error: {score:.1f} mA')
    plt.ylabel('Current (mA)')
    plt.xlabel('Time (ms)')
    plt.show()

def read_plot_position_matrix():
    while True:
        n_str = ser.read_until(b'\n').decode('utf-8').strip()  # get the number of data points to receive
        if n_str:
            try:
                n_int = int(n_str)  # turn it into an int
                break
            except ValueError:
                print(f"Received invalid data for number of samples: '{n_str}'")
        else:
            print("Received empty data for number of samples")
    
    print('Data length = ' + str(n_int))
    # Lists to store the four values from each line
    refAngle = []
    actualAng = []
    data_received = 0
    while data_received < n_int:
        dat_str = ser.read_until(b'\n').decode('utf-8').strip()  # get the data as a string, ints separated by spaces
        if dat_str:
            try:
                dat_f = list(map(float, dat_str.split()))  # now the data is a list of floats
                # Ensure there are exactly four numbers in the line
                if len(dat_f) == 2:
                    refAngle.append(dat_f[0])
                    actualAng.append(dat_f[1])
                    data_received += 1
                else:
                    print(f"Skipping malformed line: {dat_str}")
            except ValueError:
                print(f"Received invalid data: '{dat_str}'")
        else:
            print("Received empty data for sample")
    
    # Plotting
    t = range(len(refAngle))  # index array
    plt.plot(t, refAngle, 'b*-', label='Reference Angle')  # Blue line with star markers
    plt.plot(t, actualAng, '*-', color='orange', label='Actual Angle')  # Orange line with star markers
    plt.ylabel('Value (mA or deg)')
    plt.xlabel('Index')
    plt.legend()
    plt.show()


ser = serial.Serial('/dev/ttyUSB0', 230400, timeout=1)  # Increase timeout
print('Opening port: ')
print(ser.name)

has_quit = False
# menu loop
while not has_quit:
    print('PIC32 MOTOR DRIVER INTERFACE')
    # display the menu options
    print('\tCOMMAND OPTIONS:')
    print('\tEncoder:')
    print('\t  c: Read Encoder (counts)    d: Read Encoder (deg)')
    print('\t  e: Reset Encoder')
    print('\tCurrent Sensor:')
    print('\t  b: Read Current Sensor (mA) a: Read Current Sensor (ADC counts)')
    print('\tPWM Control:')
    print('\t  f: Set PWM (-100 to 100)    p: Unpower Motor')
    print('\tGains:')
    print('\t  i: Set Position Gains       j: Get Position Gains')
    print('\t  g: Set Current Gains        h: Get Current Gains')
    print('\tControl:')
    print('\t  k: Test Current Control     l: Go to Angle (deg)')
    print('\tTrajectory:')
    print('\t  m: Load Step Trajectory     n: Load Cubic Trajectory')
    print('\t  o: Execute Trajectory')
    print('\tMisc:')
    print('\t  r: Get Mode                 q: Quit')
    # read the user's choice
    selection = input('\nENTER COMMAND: ')
    selection_endline = selection + '\n'

    # send the command to the PIC32
    ser.write(selection_endline.encode())  # .encode() turns the string into a char array
    ser.flush()  # Ensure the command is sent immediately

    # Add a small delay to ensure the command is processed
    time.sleep(0.1)

    if selection == 'a':  # read current sensor (ADC counts)
        n_str = ser.read_until(b'\n').decode('utf-8')
        print(n_str)
    elif selection == 'b':  # read current sensor (mA)
        n_str = ser.read_until(b'\n').decode('utf-8')
        print(n_str)
    elif selection == 'c':  # Get Encoder Count
        n_str = ser.read_until(b'\n').decode('utf-8')
        print(n_str)
    elif selection == 'd':  # Get Encoder Degrees
        n_str = ser.read_until(b'\n').decode('utf-8')
        print(n_str)
    elif selection == 'e':  # Reset Encoder
        n_str = ser.read_until(b'\n').decode('utf-8')
        print(n_str)
    elif selection == 'f':  # set PWM (-100 to 100)
        n_str = input('Enter PWM Frequency (-100 to 100): ')
        n_int = int(n_str)  # turn it into an int
        ser.write((str(n_int) + "\n").encode())  # send the number
        ser.flush()  # Ensure the command is sent immediately
        print('Set PWM to: ' + (n_str))
    elif selection == 'g':  # set current gains
        n1_str = input('Enter Kp: ')  # get the number to send
        n1_flt = float(n1_str)  # turn it into a float
        ser.write((str(n1_flt) + "\n").encode())  # send the number
        ser.flush()  # Ensure the command is sent immediately
        n2_str = input('Enter Ki: ')  # get the second number
        n2_flt = float(n2_str)  # convert it to a float
        ser.write((str(n2_flt) + "\n").encode())  # send the number
        ser.flush()  # Ensure the command is sent immediately
        print(f'Set kp_mA to: {n1_flt} and ki_mA to: {n2_flt}')
    elif selection == 'h':  # get current gains
        n_str = ser.read_until(b'\n').decode('utf-8')
        print('Received: ', n_str)
    elif selection == 'i':  # set position gains
        n1_str = input('Enter kp_deg: ')  # get the number to send
        n1_flt = float(n1_str)  # turn it into a float
        ser.write((str(n1_flt) + "\n").encode())  # send the number
        ser.flush()  # Ensure the command is sent immediately
        n2_str = input('Enter ki_deg: ')  # get the second number
        n2_flt = float(n2_str)  # convert it to a float
        ser.write((str(n2_flt) + "\n").encode())  # send the number
        ser.flush()  # Ensure the command is sent immediately
        n3_str = input('Enter kd_deg: ')  # get the third number
        n3_flt = float(n3_str)  # convert it to a float
        ser.write((str(n3_flt) + "\n").encode())  # send the number
        ser.flush()  # Ensure the command is sent immediately
        print(f'Set kp_deg to: {n1_flt}, ki_deg to: {n2_flt}, kd_deg to: {n3_flt}')
    elif selection == 'j':  # get position gains
        n_str = ser.read_until(b'\n').decode('utf-8')
        print('Got back kp_deg, ki_deg, kd_deg: ', n_str)
    elif selection == 'k':  # test current control
        read_plot_matrix()
    elif selection == 'l':  # go to angle (deg)
        n_str = input('Enter Angle: ')  # get the number to send
        n_int = int(n_str)  # turn it into an int
        ser.write((str(n_int) + '\n').encode())  # send the number
        ser.flush()  # Ensure the command is sent immediately
        # read_plot_position_matrix()
    elif selection == 'm':  # load step trajectory
        ref = genRef('step')
        t = range(len(ref))
        plt.plot(t, ref, 'r*-')
        plt.ylabel('angle in degrees')
        plt.xlabel('index')
        plt.show()
        ser.write((str(len(ref)) + '\n').encode())
        ser.flush()  # Ensure the command is sent immediately
        for i in ref:
            ser.write((str(i) + '\n').encode())
            ser.flush()  # Ensure the command is sent immediately
    elif selection == 'n':  # load cubic trajectory
        ref = genRef('cubic')
        t = range(len(ref))
        plt.plot(t, ref, 'r*-')
        plt.ylabel('angle in degrees')
        plt.xlabel('index')
        plt.show()
        ser.write((str(len(ref)) + '\n').encode())
        ser.flush()  # Ensure the command is sent immediately
        for i in ref:
            ser.write((str(i) + '\n').encode())
            ser.flush()  # Ensure the command is sent immediately
    elif selection == 'o':  # execute trajectory
        read_plot_position_matrix()
    elif selection == 'p':  # unpowered the motor
        print('Unpowered Motor.')
    elif selection == 'q':  # quit
        print('Exiting client')
        has_quit = True  # exit client
        ser.close()  # close the serial port
    elif selection == 'r':  # get mode
        n_str = ser.read_until(b'\n').decode('utf-8')
        print('Got back: ', n_str[0:-1])  # remove the newline character while printing
    else:
        print('Invalid Selection ' + selection_endline)