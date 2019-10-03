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
DELAY = 0.01
GRAPH_WIDTH = 20
GRAPH_HEIGHT = 12


class GraphHandler(object):
    """Handles the graphical representation of objects"""
    def __init__(self):
        self.exit = False;
        #Creo los ejes de cada grafica
        self.fig = plt.figure(figsize=(GRAPH_WIDTH,GRAPH_HEIGHT), dpi=80,constrained_layout=False)
        self.ax1 = self.fig.add_subplot(221, projection='3d', title = "Kinetis 1");
        self.ax2 = self.fig.add_subplot(222, projection='3d', title = "Kinetis 2");
        self.ax3 = self.fig.add_subplot(223, projection='3d', title = "Kinetis 3");
        self.ax4 = self.fig.add_subplot(224, projection='3d', title = "Kinetis 4");
        self.axes = [self.ax1, self.ax2, self.ax3, self.ax4];
        #defino el rectangulo que representa la placa
        bottom_left_x = 1;
        bottom_left_y = 1;
        width = RECTANGLE_WIDTH;
        height = RECTANGLE_WIDTH;
        rect1 = Rectangle( (bottom_left_x,bottom_left_y), width, height, angle=0.0);
        rect2 = Rectangle( (bottom_left_x,bottom_left_y), width, height, angle=0.0);
        rect3 = Rectangle( (bottom_left_x,bottom_left_y), width, height, angle=0.0);
        rect4 = Rectangle( (bottom_left_x,bottom_left_y), width, height, angle=0.0);

        self.ax1.add_patch(rect1);
        self.ax2.add_patch(rect2);
        self.ax3.add_patch(rect3);
        self.ax4.add_patch(rect4);
        art3d.pathpatch_2d_to_3d(rect1, z=1, zdir='y');
        art3d.pathpatch_2d_to_3d(rect2, z=1, zdir='y');
        art3d.pathpatch_2d_to_3d(rect3, z=1, zdir='y');
        art3d.pathpatch_2d_to_3d(rect4, z=1, zdir='y');
       
        #self.DrawPinoutStrips(-bottom_left_x,-bottom_left_y)
        #Defino los limites de las graficas
        for i in range(len(self.axes)):
            (self.axes[i]).set_xlim3d(X_MIN, X_MAX);
            (self.axes[i]).set_ylim3d(Y_MIN, Y_MAX);
            (self.axes[i]).set_zlim3d(Z_MIN, Z_MAX);
            (self.axes[i]).disable_mouse_rotation();
            (self.axes[i]).grid(True);
            (self.axes[i]).set_xlabel('X axis');
            (self.axes[i]).set_ylabel('Y axis');

    def UpdateBoards(self, list_of_positions):
        for i in range(len(list_of_positions) ):
            roll_angle, head_angle, orientation = list_of_positions[i];
            (self.axes[i]).view_init(roll_angle, 90+head_angle);
            (self.axes[i]).set_title("Kinetis"+" "+str(i+1)+ "\n"+ "  R ="+ str(roll_angle) + ", C = " + str(head_angle) );
        plt.draw();
        plt.pause(DELAY);

    def DrawBoardsShape(self):
        self.DrawPinoutStrips();

    def DrawPinoutStrips(self,bottom_left_x,bottom_left_y):
        top_surface = Rectangle( (bottom_left_x+1,bottom_left_y+1), RECTANGLE_WIDTH*0.667, RECTANGLE_HEIGHT*0.167, angle=0.0,color ='black');
        lateral_surface1 = Rectangle( (bottom_left_x,bottom_left_y), RECTANGLE_WIDTH*0.667, RECTANGLE_HEIGHT*0.167, angle=0.0,color ='black');
        lateral_surface2 = Rectangle( (bottom_left_x,bottom_left_y), RECTANGLE_WIDTH*0.667, RECTANGLE_HEIGHT*0.167, angle=0.0,color ='black');
        self.ax1.add_patch(top_surface);
        art3d.pathpatch_2d_to_3d(top_surface, z=1+RECTANGLE_HEIGHT*0.167, zdir='y');
        self.ax1.add_patch(lateral_surface1);
        art3d.pathpatch_2d_to_3d(lateral_surface1, z=2+RECTANGLE_HEIGHT*0.167*0.5, zdir='z');
        self.ax1.add_patch(lateral_surface2);
        art3d.pathpatch_2d_to_3d(lateral_surface2, z=2-RECTANGLE_HEIGHT*0.167*0.5, zdir='z');
    def on_closing(self):
        self.exit = True;