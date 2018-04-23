import serial
import webbrowser
import time
import os

### INSTRUCTIONS ###
# 0. On your command prompt, run "pip install twilio"
# 1. Sign up for Twilio at https://www.twilio.com (click on Get Free API Key)
# 2. Once signed up, you will see a button to "Get your first Twilio number"
# 3. Click on it to get your Twilio number, and set the "twilioNumber" variable below to your Twilio number.
# 4. Go to https://www.twilio.com/user/account/settings and copy paste your AccountSID and AuthToken to below.
# 5. Verify your phone number here (if you have not done it already):
# https://www.twilio.com/user/account/phone-numbers/verified

SERIAL_PORT = "/dev/ttyACM0"  # Change this if it is different for you
BAUD_RATE = 9600

from twilio.rest import Client
# make sure env vars file exists
try:
    with open('.env') as f:
        pass
except FileNotFoundError:
    with open('.env', 'w') as f:
        f.write("""TWILIO_SID=
AUTH_TOKEN=
TARGET_NUMBER=
TWILIO_NUMBER=""")

# put your own credentials here
ACCOUNT_SID = os.environ.get('TWILIO_SID', "AC...put your SID here...")
AUTH_TOKEN = os.environ.get('AUTH_TOKEN', "...put your auth token here...")
# put your cell phone number here
myNumber = os.environ.get('TARGET_NUMBER', '+195100011111')
# put your twilio number here
twilioNumber = os.environ.get('TWILIO_NUMBER', '+18881112222')

client = Client(ACCOUNT_SID, AUTH_TOKEN)

# this creates a serial port object with the location of your USB-Serial adapter,
# the baud rate and optional timeout value.
sp = serial.Serial(SERIAL_PORT, BAUD_RATE, timeout=5)

# clear out the serial input and output buffers
sp.flush()

print("Listening...")
charbuffer = []
number = -1
reading_history = [number]
num_readings = 10
# loop forever
while (1):
    # read 5 characters from the serial port
    char = sp.read().decode('utf-8')
    if char == '\n':
        number = int(''.join(charbuffer))
        charbuffer = []
        if len(reading_history) >= num_readings:
            reading_history.pop(0)
        reading_history.append(number)
    else:
        charbuffer.append(char)
    # response = sp.read(5)
    smoothed_reading = sum(reading_history) / len(reading_history)
    print(number)
    if int(number) >= 30:
        print("Over threshold! Sending SMS...")
        # READ twilio documentation to figure out how to send SMS using twilio API
        # https://www.twilio.com/docs/quickstart/python/sms/sending-via-rest
    # time.sleep(1)
