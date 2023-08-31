import serial
import time

# Open the serial port
ser = serial.Serial('COM5', 9600)  # Replace 'COMX' with your Arduino's serial port

# Open a file for writing
file = open('data_chamber1_40micro_final2.txt', 'w')

# Set the duration in seconds (e.g., 20 minutes = 20 * 60 seconds)
duration = 15 * 60

# Get the starting time
start_time = time.time()

while time.time() - start_time < duration:
    # Read data from the serial port
    data = ser.readline().decode().strip()

    # Save the data to the file
    file.write(data + '\n')
    file.flush()

    # Print the received value
    print(data)

# Close the file and serial port
file.close()
ser.close()

