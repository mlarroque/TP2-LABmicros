import GraphHandler as g
import MessageHandler as m

exit = False;
baudrate=10000;
timeout=None;
port_name = "COM4";
Gui = g.GraphHandler();
MsgHandler = m.MessageHandler(port_name,baudrate_=baudrate, timeout_=timeout);
MsgHandler.StartCommunication();
while(1):
    msg = MsgHandler.ReadFrame();
    print(msg);
list_of_positions = MsgHandler.GetPositions();
Gui.UpdateBoards(list_of_positions);

