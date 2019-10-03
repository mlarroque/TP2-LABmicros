import GraphHandler as g
import MessageHandler as m

exit = False;
baudrate=9600;
timeout=0.1;
port_name = "PORT1";
Gui = g.GraphHandler();
MsgHandler = m.MessageHandler("PORT1",baudrate_=baudrate, timeout_=timeout);
MsgHandler.StartCommunication();
while(not Gui.exit):
    msg = MsgHandler.ReadFrame();
    MsgHandler.ManageMessage( msg, len(msg) );
    list_of_positions = MsgHandler.GetPositions();
    Gui.UpdateBoards(list_of_positions);

MsgHandler.EndCommunication();
