from PyQt5.QtWidgets import *
from PyQt5.QtCore import *
from PyQt5.QtGui import *
import sys
import numpy as np
from enum import Enum
import socket
import threading

class Direction(Enum):
    Left = 0
    Right = 1
    Up = 2
    Down = 3

class Joystick(QWidget):
    def __init__(self, parent=None):
        super(Joystick, self).__init__(parent)
        self.setMinimumSize(200, 200) # Updated from 100, 100
        self.movingOffset = QPointF(0, 0)
        self.grabCenter = False
        self.__maxDistance = 100 # updated from 50
        self.sock = None
        self.setStyleSheet("background-color: #242424;")

    def setSocket(self, sock, UDP_IP, UDP_PORT):
        self.sock = sock
        self.UDP_IP = UDP_IP
        self.UDP_PORT = UDP_PORT
        self.timer = QTimer()
        self.timer.timeout.connect(self.send_joystick_position)
        self.timer.start(100)

    def send_joystick_position(self):
        if self.sock is None or not self.grabCenter:
            return
        
        # Calculate the distance from the center
        normVector = QLineF(self._center(), self.movingOffset)
        currentDistance = normVector.length()
        distance = min(currentDistance / self.__maxDistance, 1.0)
        
        # Calculate the angle in radians
        angle = normVector.angle() * 3.14159 / 180  # convert from degrees to radians
        # Qt's angles are clockwise, we need to convert it to anti-clockwise
        angle = -angle + 3.14159 / 2
        
        # Calculate the x, y coordinates
        x = int(500 + distance * 499 * np.cos(angle))
        y = int(500 + distance * 499 * np.sin(angle))
        
        # Clamp the values between 0 and 999
        x = max(0, min(x, 999))
        y = max(0, min(y, 999))

        message = "A{:03d}{:03d}".format(x, y)
        self.sock.sendto(bytes(message, "utf-8"), (self.UDP_IP, self.UDP_PORT))

    def paintEvent(self, event):
        painter = QPainter(self)
        bounds = QRectF(-self.__maxDistance, -self.__maxDistance, self.__maxDistance * 2, self.__maxDistance * 2).translated(self._center())
        painter.setBrush(Qt.gray)
        painter.drawEllipse(bounds)
        painter.setBrush(Qt.black)
        painter.drawEllipse(self._centerEllipse())

    def _centerEllipse(self):
        if self.grabCenter:
            return QRectF(-20, -20, 40, 40).translated(self.movingOffset)
        return QRectF(-20, -20, 40, 40).translated(self._center())

    def _center(self):
        return QPointF(self.width()/2, self.height()/2)

    def _boundJoystick(self, point):
        limitLine = QLineF(self._center(), point)
        if (limitLine.length() > self.__maxDistance):
            limitLine.setLength(self.__maxDistance)
        return limitLine.p2()

    def joystickDirection(self):
        if not self.grabCenter:
            return (None, 0)
        normVector = QLineF(self._center(), self.movingOffset)
        currentDistance = normVector.length()
        angle = normVector.angle()

        distance = min(currentDistance / self.__maxDistance, 1.0)
        if 45 <= angle < 135:
            return (Direction.Up, distance)
        elif 135 <= angle < 225:
            return (Direction.Left, distance)
        elif 225 <= angle < 315:
            return (Direction.Down, distance)
        return (Direction.Right, distance)

    def mousePressEvent(self, ev):
        if self.sock is None:
            return
        self.grabCenter = self._centerEllipse().contains(ev.pos())

    def mouseReleaseEvent(self, event):
        self.grabCenter = False
        self.movingOffset = QPointF(0, 0)
        self.update()

        # Send a final packet with coordinates 500,500 when the joystick is released
        if self.sock is not None:
            message = "A500500"
            self.sock.sendto(bytes(message, "utf-8"), (self.UDP_IP, self.UDP_PORT))

    def mouseMoveEvent(self, event):
        if self.sock is None:
            return
        if self.grabCenter:
            self.movingOffset = self._boundJoystick(event.pos())
            self.update()

class MyApp(QWidget):
    def __init__(self):
        super().__init__()
        self.setStyleSheet("background-color: #2C2C2C; color: white")
        layout = QVBoxLayout()

        self.joystick = Joystick()
        layout.addWidget(self.joystick)

        # Create the buttons
        buttonLayout = QHBoxLayout()
        self.buttonB = QPushButton("Read Name")
        self.buttonB.clicked.connect(self.send_B)
        self.buttonB.setEnabled(False)
        self.buttonB.setStyleSheet("background-color: #242424; color: white")
        buttonLayout.addWidget(self.buttonB)

        self.buttonC = QPushButton("Read Age")
        self.buttonC.clicked.connect(self.send_C)
        self.buttonC.setEnabled(False)
        self.buttonC.setStyleSheet("background-color: #242424; color: white")
        buttonLayout.addWidget(self.buttonC)

        self.buttonD = QPushButton("Calibrate Magnetometer")
        self.buttonD.clicked.connect(self.send_D)
        self.buttonD.setEnabled(False)
        self.buttonD.setStyleSheet("background-color: #242424; color: white")
        buttonLayout.addWidget(self.buttonD)

        self.buttonE = QPushButton("Read Polarity")
        self.buttonE.clicked.connect(self.send_E)
        self.buttonE.setEnabled(False)
        self.buttonE.setStyleSheet("background-color: #242424; color: white")
        buttonLayout.addWidget(self.buttonE)

        self.buttonSTOP = QPushButton("STOP")
        self.buttonSTOP.clicked.connect(self.send_STOP)
        self.buttonSTOP.setEnabled(False)
        self.buttonSTOP.setStyleSheet("background-color: #242424; color: white")
        buttonLayout.addWidget(self.buttonSTOP)

        layout.addLayout(buttonLayout)

        # Create the response text fields
        responseLayout = QHBoxLayout()
        self.responseB = QLineEdit()
        self.responseB.setEnabled(False)
        responseLayout.addWidget(self.responseB)

        self.responseC = QLineEdit()
        self.responseC.setEnabled(False)
        responseLayout.addWidget(self.responseC)

        self.responseD = QLineEdit()
        self.responseD.setEnabled(False)
        responseLayout.addWidget(self.responseD)

        self.responseE = QLineEdit()
        self.responseE.setEnabled(False)
        responseLayout.addWidget(self.responseE)

        layout.addLayout(responseLayout)

        # Create the input fields
        self.ipInput = QLineEdit()
        self.ipInput.setPlaceholderText("Enter IP")
        layout.addWidget(self.ipInput)

        self.portInput = QLineEdit()
        self.portInput.setPlaceholderText("Enter Port")
        layout.addWidget(self.portInput)

        self.connectButton = QPushButton("Connect")
        self.connectButton.clicked.connect(self.connectSocket)
        self.connectButton.setStyleSheet("background-color: #242424; color: white")
        layout.addWidget(self.connectButton)

        self.setLayout(layout)

    def connectSocket(self):
        self.UDP_IP = self.ipInput.text()
        self.UDP_PORT = int(self.portInput.text())
        self.sock = socket.socket(socket.AF_INET, # Internet
                                  socket.SOCK_DGRAM) # UDP
        self.sock.settimeout(5) # Set timeout for the socket
        self.joystick.setSocket(self.sock, self.UDP_IP, self.UDP_PORT)
        self.buttonB.setEnabled(True)
        self.buttonC.setEnabled(True)
        self.buttonD.setEnabled(True)
        self.buttonE.setEnabled(True)

    def send_B(self):
        self.sock.sendto(bytes("B", "utf-8"), (self.UDP_IP, self.UDP_PORT))
        self.buttonB.setEnabled(False)
        self.buttonC.setEnabled(False)
        self.buttonD.setEnabled(False)
        self.buttonE.setEnabled(False)
        self.joystick.sock = None
        threading.Thread(target=self.receive_response, args=(self.responseB,)).start()

    def send_C(self):
        self.sock.sendto(bytes("C", "utf-8"), (self.UDP_IP, self.UDP_PORT))
        self.buttonB.setEnabled(False)
        self.buttonC.setEnabled(False)
        self.buttonD.setEnabled(False)
        self.buttonE.setEnabled(False)
        self.joystick.sock = None
        threading.Thread(target=self.receive_response, args=(self.responseC,)).start()

    def send_D(self):
        self.sock.sendto(bytes("D", "utf-8"), (self.UDP_IP, self.UDP_PORT))
        self.buttonB.setEnabled(False)
        self.buttonC.setEnabled(False)
        self.buttonD.setEnabled(False)
        self.buttonE.setEnabled(False)
        self.joystick.sock = None
        threading.Thread(target=self.receive_response, args=(self.responseD,)).start()

    def send_E(self):
        self.sock.sendto(bytes("E", "utf-8"), (self.UDP_IP, self.UDP_PORT))
        self.buttonB.setEnabled(False)
        self.buttonC.setEnabled(False)
        self.buttonD.setEnabled(False)
        self.buttonE.setEnabled(False)
        self.joystick.sock = None
        threading.Thread(target=self.receive_response, args=(self.responseE,)).start()

    def send_STOP(self):
        self.sock.sendto(bytes("E500500"), (self.UDP_IP, self.UDP_PORT))

    def receive_response(self, textbox):
        try:
            data, addr = self.sock.recvfrom(1024) # buffer size is 1024 bytes
            textbox.setText(data.decode("utf-8"))  # Here, we decode the bytes to a string.
        except socket.timeout:
            print('No response')
        finally:
            self.buttonB.setEnabled(True)
            self.buttonC.setEnabled(True)
            self.buttonD.setEnabled(True)
            self.buttonE.setEnabled(True)
            self.joystick.sock = self.sock

if __name__ == '__main__':
    app = QApplication([])
    mw = MyApp()
    mw.setWindowTitle('Vortron App')
    mw.show()
    sys.exit(app.exec_())
