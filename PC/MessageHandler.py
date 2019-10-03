import CircuitBoard as c
import SerialCommunicationHandler as s
#Numero de Kinetis utilizadas
NUMBER_OF_BOARDS = 4
ANGLE_MAX = 180
ANGLE_MIN = -179
ACK = 'A'
ERROR = 'E'
ID_BYTES = 2
MAX_MSG_SIZE = 6
class MessageHandler(object):
    """Handles messages recieved from the Kinetis"""
    def __init__(self, port_name, baudrate_=9600, timeout_=0, parity_='N', bytesize_=8, stopbits_=1):
        self.BoardList = [c.CircuitBoard(), c.CircuitBoard(), c.CircuitBoard(), c.CircuitBoard()];
        self.Comm = s.SerialCommunicationHandler(port_name, baudrate_, timeout_, parity_, bytesize_, stopbits_);

    def TransmitError(self):
        self.Comm.WriteMessage(bytes(ERROR,'ascii'));

    def StartCommunication(self):
        self.Comm.OpenCommunication();

    def EndCommunication(self):
        self.Comm.CloseCommunnication();

    def ReadFrame(self):
        return self.Comm.ReadMessage(MAX_MSG_SIZE);

    def ManageMessage(self, msg, size):
        msg_string = msg.decode('ascii');
        size_value = size-ID_BYTES;
        #Comienzo el parseo del mensaje recibido
        board_index = int(msg_string[0]);
        if( ( board_index < 0 ) or ( board_index>=NUMBER_OF_BOARDS ) ):
            self.TransmitError(); #Si el id de la kinetis es invalido, envia error.
            return;
        angle_value = int(msg_string[2:(2+size_value)]);
        if( (angle_value < ANGLE_MIN) or (angle_value > ANGLE_MAX) ):
            self.TransmitError(); #Si recibe un valor de angulo invalido, envia error.
            return;
        angle_id = msg_string[1];
        if angle_id == 'R':
            (self.BoardList[board_index]).SetRoll(angle_value);
        elif angle_id == 'C':
            (self.BoardList[board_index]).SetHead(angle_value);
        elif angle_id == 'O':
            (self.BoardList[board_index]).SetOrientation(angle_value);
        else:
            self.TransmitError(); #Si recibe un id de angulo invalido, envia error.
            return;
        self.Comm.WriteMessage(bytes(ACK,'ascii'));

    def GetPositions(self):
        position_list = [];
        for i in range( len(self.BoardList) ):
            roll = (self.BoardList[i]).GetRoll();
            head = (self.BoardList[i]).GetHead();
            orientation = (self.BoardList[i]).GetOrientation();
            position_list.append( (roll, head, orientation) );
        return position_list;
