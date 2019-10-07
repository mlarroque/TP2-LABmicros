import CircuitBoard as c
import SerialCommunicationHandler as s
#Numero de Kinetis utilizadas
NUMBER_OF_BOARDS = 7
ANGLE_MAX = 180
ANGLE_MIN = -179
ACK = 'A'
ERROR = 'E'
ID_BYTES = 2
MAX_MSG_SIZE = 6
class MessageHandler(object):
    """Handles messages recieved from the Kinetis"""
    def __init__(self, port_name, baudrate_=9600, timeout_=0, parity_='N', bytesize_=8, stopbits_=1):
        self.BoardList = [];
        for i in range(NUMBER_OF_BOARDS):
            self.BoardList.append( c.CircuitBoard() );
        self.Comm = s.SerialCommunicationHandler(port_name, baudrate_, timeout_, parity_, bytesize_, stopbits_);

    def TransmitError(self):
        self.Comm.WriteMessage(bytes(ERROR,'ascii'));

    def StartCommunication(self):
        self.Comm.OpenCommunication();

    def EndCommunication(self):
        self.Comm.CloseCommunnication();

    def ReadFrame(self):
        return self.Comm.ReadMessage(MAX_MSG_SIZE);

    def WriteFrame(self,data):
        return self.Comm.WriteMessage(data);

    def ManageMessage(self, msg, size):
        if (size < 3):
            print("Paquete invalido");
            return;
        try:
                msg_string = msg.decode('ascii');
        except UnicodeDecodeError:
            print("Recieved string is not ascii");
            return;
        size_value = size-ID_BYTES;

        #Comienzo el parseo del mensaje recibido
        try:
          board_index = int(msg_string[0]);
        except ValueError:
            print("Invalid Board ID recieved");
            return;

        if( ( board_index < 0 ) or ( board_index>=NUMBER_OF_BOARDS ) ):
            self.TransmitError(); #Si el id de la kinetis es invalido, envia error.
            return;

        try:
          angle_value = int(msg_string[2:(2+size_value)]);
        except ValueError:
            print("Invalid angle value recieved");
            return;

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
        #self.Comm.WriteMessage(bytes(ACK,'ascii'));

    def GetPositions(self):
        list_of_positions = [];
        for i in range(NUMBER_OF_BOARDS):
            roll_angle = self.BoardList[i].GetRoll();
            head_angle = self.BoardList[i].GetHead();
            orientation = self.BoardList[i].GetOrientation();
            list_of_positions.append( (roll_angle, head_angle, orientation) );
        return list_of_positions;
