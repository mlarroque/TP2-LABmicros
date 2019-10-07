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
        self.valid_message = True;
        self.last_message = [];

    def TransmitError(self):
        self.Comm.WriteMessage(bytes(ERROR,'ascii'));

    def StartCommunication(self):
        self.Comm.OpenCommunication();

    def EndCommunication(self):
        self.Comm.CloseCommunnication();

    def ReadFrame(self):
        if (self.valid_message):
            return self.Comm.ReadMessage(MAX_MSG_SIZE); #Si recibi un mensaje valido leo los proximos 6 bytes
        else:
            return self.Comm.ReadMessage(1); #Si recibo mensaje invalido, leo el proximo byte.

    def WriteFrame(self,data):
        return self.Comm.WriteMessage(data);

    def ManageLastMsg(self,msg):
        
        try:
                msg_string = msg.decode('ascii');
        except UnicodeDecodeError:
            print("Recieved string is not ascii");
            return;
        if ( len( self.last_message ) < MAX_MSG_SIZE):
            self.last_message.append( msg_string );
        else:
            self.last_message.pop(0);
            self.last_message.append( msg_string );
        if( len( self.last_message ) > 2 ):
            valid = True;
            size_value = len(self.last_message)-ID_BYTES;

            #Comienzo el parseo del mensaje recibido
            try:
              board_index = int(self.last_message[0]);
            except ValueError:
                print("Invalid Board ID recieved");
                valid = False;

            if( ( board_index < 0 ) or ( board_index>=NUMBER_OF_BOARDS ) ):
                #self.TransmitError(); #Si el id de la kinetis es invalido, envia error.
                valid = False;

            try:
              angle_value = int( str(self.last_message[2:(2+size_value)]) );
            except ValueError:
                print("Invalid angle value recieved");
                valid = False;

            if( (angle_value < ANGLE_MIN) or (angle_value > ANGLE_MAX) ):
                #self.TransmitError(); #Si recibe un valor de angulo invalido, envia error.
                valid = False;
            angle_id = self.last_message[1];
            if angle_id == 'R':
                (self.BoardList[board_index]).SetRoll(angle_value);
            elif angle_id == 'C':
                (self.BoardList[board_index]).SetHead(angle_value);
            elif angle_id == 'O':
                (self.BoardList[board_index]).SetOrientation(angle_value);
            else:
                #self.TransmitError(); #Si recibe un id de angulo invalido, envia error.
                valid = False;
            if(not valid):
                return;
            else:
                self.valid_message = True;


    def ManageMessage(self, msg, size):
        board_index =0;
        angle_value=0;
        if( size == 1):
            self.ManageLastMsg(msg);
            return;
        valid = True;
        if (size < 1):
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
            valid = False;

        if( ( board_index < 0 ) or ( board_index>=NUMBER_OF_BOARDS ) ):
            #self.TransmitError(); #Si el id de la kinetis es invalido, envia error.
            valid = False;

        try:
          angle_value = int(msg_string[2:(2+size_value)]);
        except ValueError:
            print("Invalid angle value recieved");
            valid = False;

        if( (angle_value < ANGLE_MIN) or (angle_value > ANGLE_MAX) ):
            #self.TransmitError(); #Si recibe un valor de angulo invalido, envia error.
            valid = False;
        angle_id = msg_string[1];
        if angle_id == 'R':
            (self.BoardList[board_index]).SetRoll(angle_value);
        elif angle_id == 'C':
            (self.BoardList[board_index]).SetHead(angle_value);
        elif angle_id == 'O':
            (self.BoardList[board_index]).SetOrientation(angle_value);
        else:
            #self.TransmitError(); #Si recibe un id de angulo invalido, envia error.
            valid = False;
        if(not valid):
            for i in range(size):
                if ( len( self.last_message ) < MAX_MSG_SIZE):
                    self.last_message.append( msg_string[i] );
                else:
                    self.last_message.pop(0);
                    self.last_message.append( msg_string[i] );
            self.valid_message = False;
        else:
            self.last_message.clear();
            for i in range(size):
                self.last_message.append( msg_string[i] )
            self.valid_message = True;


        #self.Comm.WriteMessage(bytes(ACK,'ascii'));

    def GetPositions(self):
        list_of_positions = [];
        for i in range(NUMBER_OF_BOARDS):
            roll_angle = self.BoardList[i].GetRoll();
            head_angle = self.BoardList[i].GetHead();
            orientation = self.BoardList[i].GetOrientation();
            list_of_positions.append( (roll_angle, head_angle, orientation) );
        return list_of_positions;
