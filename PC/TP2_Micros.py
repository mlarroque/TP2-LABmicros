import GraphHandler as g
import MessageHandler as m
import numpy as np

exit = False;
baudrate=9600;
timeout=0;
port_name = "PORT1";
Gui = g.GraphHandler();
MsgHandler = m.MessageHandler("PORT1",baudrate_=baudrate, timeout_=timeout);
list_of_positions = np.empty((4,3));
for i in range(-18,18):
    list_of_positions[0]= (10*i,0,0);
    list_of_positions[1]= (10*i,20,0);
    list_of_positions[2]= (10*i,48,0);
    list_of_positions[3]= (0,10*i,0);
    Gui.UpdateBoards(list_of_positions);
