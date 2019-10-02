import serial

class SerialCommunicationHandler(object):
    """Handles communication between PC and a Serial port"""
    def __init__(self, port_name, baudrate_=9600, timeout_=0, parity_='N', bytesize_=8, stopbits_=1):
        self.ser = serial.Serial();
        self.ser.port = port_name;
        self.ser.baudrate = baudrate_;
        self.ser.timeout = timeout_;
        self.ser.parity = parity_;
        self.ser.bytesize = bytesize_;

    def CloseCommunnication(self):
        self.ser.close();

    def OpenCommunication(self):
        self.ser.open();

    def WriteMessage(self,data):
        bytes_sent = 0;
        if( self.ser.is_open ):
            bytes_sent = self.ser.write(data);
        return bytes_sent;

    def ReadMessage(self,bytes_to_read):
        bytes_read = 0;
        if( self.ser.is_open ):
            bytes_read = self.ser.read(bytes_to_read);
        return bytes_read;
