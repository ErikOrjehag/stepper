import PyCmdMessenger
from time import sleep
from inputs import get_gamepad
from threading import Thread
import numpy as np
import sys

arduino = PyCmdMessenger.ArduinoBoard(sys.argv[1], baud_rate=9600)

# List of command names (and formats for their associated arguments). These must
# be in the same order as in the sketch.
commands = [
    ["asd", ""],
    ["steer", "iiii"]
]

# Initialize the messenger
c = PyCmdMessenger.CmdMessenger(arduino, commands)

x = 0
y = 0
a = 0
DEADZONE = 0.1
R = 0.05
L1 = L2 = 0.16

def input_thread():
    global x, y, a
    try:
        while True:
            events = get_gamepad()
            for event in events:
                if event.code == "ABS_X":
                    x = event.state / (2**15)
                    if abs(x) < DEADZONE: x = 0
                elif event.code == "ABS_Y":
                    y = event.state / (2**15)
                    if abs(y) < DEADZONE: y = 0
                elif event.code == "ABS_RX":
                    a = event.state / (2**15)
                    if abs(a) < DEADZONE: a = 0
    except KeyboardInterrupt:
        print('interrupted!')

t = Thread(target=input_thread, args=())
t.start()

try:
    while True:
        A = np.array((
            (np.sqrt(2)/2, np.sqrt(2)/2, 1),
            (np.sqrt(2)/2, -np.sqrt(2)/2, 1),
            (-np.sqrt(2)/2, -np.sqrt(2)/2, 1),
            (-np.sqrt(2)/2, np.sqrt(2)/2, 1)
        ))
        B = np.array((
            (1, 0, 0),
            (0, 1, 0),
            (0, 0, 1)
        ))
        C = np.array((a, x, y))

        M = -0.1*(np.sqrt(2)/R)*A.dot(B.dot(C))

        S = []
        for m in M:
            S.append(int(1000*m/(2*np.pi)))

        print(S)

        c.send("steer", S[0], S[1], S[2], S[3])

        sleep(0.5)
except KeyboardInterrupt:
    print('interrupted!')
