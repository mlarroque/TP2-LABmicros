from matplotlib.patches import Rectangle
import matplotlib.pyplot as plt
from mpl_toolkits.mplot3d import Axes3D
import mpl_toolkits.mplot3d.art3d as art3d

#Largos y Anchos
RECTANGLE_WIDTH = 6
RECTANGLE_HEIGHT = 3
#Propiedades de las graficas
X_MIN = -1
X_MAX = 6
Y_MIN = -1
Y_MAX = 6
Z_MIN = -1
Z_MAX = 6
DELAY = 0.001
GRAPH_WIDTH = 20
GRAPH_HEIGHT = 12
NUMBER_OF_KINETIS = 7


class GraphHandler(object):
    """Handles the graphical representation of objects"""
    def __init__(self):
        self.exit = False;
        #Creo los ejes de cada grafica
        self.fig = plt.figure(figsize=(GRAPH_WIDTH,GRAPH_HEIGHT), dpi=80,constrained_layout=False)
        self.axes = [];
        if NUMBER_OF_KINETIS % 2:
            if( NUMBER_OF_KINETIS > 2 ): #Si el numero de Kinetis es impar
                rows = 2;
                columns = (NUMBER_OF_KINETIS+1)/2;
            else:
                rows = 1;
                columns = 1;
        else:
            rows = 2;
            columns = NUMBER_OF_KINETIS/2
        for i in range(NUMBER_OF_KINETIS):
            self.axes.append( self.fig.add_subplot(rows, columns, i+1, projection='3d', title = "Kinetis "+str(i+1)) );
        #defino el rectangulo que representa la placa
        bottom_left_x = 1;
        bottom_left_y = 1;
        width = RECTANGLE_WIDTH;
        height = RECTANGLE_HEIGHT;
        rectangles = [];
        for i in range(NUMBER_OF_KINETIS):
            rectangles.append( Rectangle( (bottom_left_x,bottom_left_y), width, height, angle=0.0) );
            self.axes[i].add_patch( rectangles[i] );
            art3d.pathpatch_2d_to_3d( rectangles[i], z=1, zdir='y');

        #Defino los limites de las graficas
        for i in range(len(self.axes)):
            (self.axes[i]).set_xlim3d(X_MIN, X_MAX);
            (self.axes[i]).set_ylim3d(Y_MIN, Y_MAX);
            (self.axes[i]).set_zlim3d(Z_MIN, Z_MAX);
            (self.axes[i]).disable_mouse_rotation();
            (self.axes[i]).set_axis_off();
            for j in range(50):
                (self.axes[i]).set_title("Kinetis "+str(i+1) );

    def UpdateBoards(self, list_of_positions):
        for i in range(NUMBER_OF_KINETIS):
            roll_angle, head_angle, orientation = list_of_positions[i];
            (self.axes[i]).view_init(roll_angle, 90+head_angle);
            (self.axes[i]).set_title("Kinetis"+" "+str(i+1)+ "\n"+ "  R ="+ str(roll_angle) + ", C = " + str(head_angle)+", O = "+str(orientation) );
        plt.draw();
        plt.pause(DELAY);

    def on_closing(self):
        self.exit = True;