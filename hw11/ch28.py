import serial
import time
import matplotlib.pyplot as plt
from statistics import mean
from genref import genRef
import numpy as np


def read_plot_matrix():
    """
    Reads the number of data points from the serial port, then reads those data points 
    (reference current vs. actual current). Plots the data and displays the average error.
    """
    # Get the number of data points
    n_str = ser.read_until(b'\n').decode('utf-8')
    n_int = int(n_str)
    print('Data length = ' + str(n_int))

    # Lists to store the reference and actual current values, and times
    data = []
    times = []

    # Read n_int lines of data
    for i in range(n_int):
        dat_str = ser.read_until(b'\n').decode('utf-8')  # String of floats separated by spaces
        dat_f = list(map(float, dat_str.split()))
        data.append(dat_f)
        times.append(i * 0.2)  # 0.2 ms between samples

    # Convert data to numpy array for easier manipulation
    data = np.array(data)

    if n_int > 1:
        # Plot the reference and actual current
        plt.step(times, data[:, 0], label='Reference Current', where='mid')
        plt.step(times, data[:, 1], label='Actual Current', where='mid')
        plt.legend(loc='upper right')
    else:
        print('Only 1 sample received')
        print(data)

    # Compute and display the average error
    score = mean(abs(data[:, 0] - data[:, 1]))
    print(f'\nAverage error: {score:.1f} mA')
    plt.title(f'Average error: {score:.1f} mA')
    plt.ylabel('Current (mA)')
    plt.xlabel('Time (ms)')
    plt.show()


def read_plot_position_matrix():
    """
    Reads the number of data points from the serial port, then reads those data points
    (reference angle vs. actual angle). Plots the data once all points are collected.
    """
    # Continuously attempt to read a valid integer for number of data points
    while True:
        n_str = ser.read_until(b'\n').decode('utf-8').strip()
        if n_str:
            try:
                n_int = int(n_str)
                break
            except ValueError:
                print(f"Received invalid data for number of samples: '{n_str}'")
        else:
            print("Received empty data for number of samples")

    print('Data length = ' + str(n_int))

    # Lists to store the reference and actual angles
    refAngle = []
    actualAng = []

    data_received = 0
    while data_received < n_int:
        dat_str = ser.read_until(b'\n').decode('utf-8').strip()
        if dat_str:
            try:
                dat_f = list(map(float, dat_str.split()))
                # Ensure there are exactly two numbers in the line
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

    # Plot the reference and actual angles
    t = range(len(refAngle))
    plt.plot(t, refAngle, 'b*-', label='Reference Angle')
    plt.plot(t, actualAng, '*-', color='orange', label='Actual Angle')
    plt.ylabel('Value (mA or deg)')
    plt.xlabel('Index')
    plt.legend()
    plt.show()


# Initialize serial port
ser = serial.Serial('/dev/ttyUSB0', 230400, timeout=1)
print('Opening port:')
print(ser.name)

has_quit = False

# Menu loop
while not has_quit:
    print('PIC32 MOTOR DRIVER INTERFACE')
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

    # Read the user's choice
    selection = input('\nENTER COMMAND: ')
    selection_endline = selection + '\n'

    # Send the command to the PIC32
    ser.write(selection_endline.encode())
    ser.flush()

    # Small delay to ensure the command is processed
    time.sleep(0.1)

    if selection == 'a':  # Read current sensor (ADC counts)
        n_str = ser.read_until(b'\n').decode('utf-8')
        print(n_str)

    elif selection == 'b':  # Read current sensor (mA)
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

    elif selection == 'f':  # Set PWM (-100 to 100)
        n_str = input('Enter PWM Frequency (-100 to 100): ')
        n_int = int(n_str)
        ser.write((str(n_int) + "\n").encode())
        ser.flush()
        print('Set PWM to: ' + n_str)

    elif selection == 'g':  # Set current gains
        n1_str = input('Enter Kp: ')
        n1_flt = float(n1_str)
        ser.write((str(n1_flt) + "\n").encode())
        ser.flush()

        n2_str = input('Enter Ki: ')
        n2_flt = float(n2_str)
        ser.write((str(n2_flt) + "\n").encode())
        ser.flush()

        print(f'Set kp_mA to: {n1_flt} and ki_mA to: {n2_flt}')

    elif selection == 'h':  # Get current gains
        n_str = ser.read_until(b'\n').decode('utf-8')
        print('Received: ', n_str)

    elif selection == 'i':  # Set position gains
        n1_str = input('Enter kp_deg: ')
        n1_flt = float(n1_str)
        ser.write((str(n1_flt) + "\n").encode())
        ser.flush()

        n2_str = input('Enter ki_deg: ')
        n2_flt = float(n2_str)
        ser.write((str(n2_flt) + "\n").encode())
        ser.flush()

        n3_str = input('Enter kd_deg: ')
        n3_flt = float(n3_str)
        ser.write((str(n3_flt) + "\n").encode())
        ser.flush()

        print(f'Set kp_deg to: {n1_flt}, ki_deg to: {n2_flt}, kd_deg to: {n3_flt}')

    elif selection == 'j':  # Get position gains
        n_str = ser.read_until(b'\n').decode('utf-8')
        print('Got back kp_deg, ki_deg, kd_deg: ', n_str)

    elif selection == 'k':  # Test current control
        read_plot_matrix()

    elif selection == 'l':  # Go to angle (deg)
        n_str = input('Enter Angle: ')
        n_int = int(n_str)
        ser.write((str(n_int) + '\n').encode())
        ser.flush()
        # Optionally call read_plot_position_matrix() if desired

    elif selection == 'm':  # Load step trajectory
        ref = genRef('step')
        t = range(len(ref))
        plt.plot(t, ref, 'r*-')
        plt.ylabel('angle in degrees')
        plt.xlabel('index')
        plt.show()

        ser.write((str(len(ref)) + '\n').encode())
        ser.flush()

        for val in ref:
            ser.write((str(val) + '\n').encode())
            ser.flush()

    elif selection == 'n':  # Load cubic trajectory
        ref = genRef('cubic')
        t = range(len(ref))
        plt.plot(t, ref, 'r*-')
        plt.ylabel('angle in degrees')
        plt.xlabel('index')
        plt.show()

        ser.write((str(len(ref)) + '\n').encode())
        ser.flush()

        for val in ref:
            ser.write((str(val) + '\n').encode())
            ser.flush()

    elif selection == 'o':  # Execute trajectory
        read_plot_position_matrix()

    elif selection == 'p':  # Unpower the motor
        print('Unpowered Motor.')

    elif selection == 'q':  # Quit
        print('Exiting client')
        has_quit = True
        ser.close()

    elif selection == 'r':  # Get mode
        n_str = ser.read_until(b'\n').decode('utf-8')
        print('Got back: ', n_str.strip())

    else:
        print('Invalid Selection ' + selection_endline)
