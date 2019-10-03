class CircuitBoard(object):
    """Class that represents the physical orientation in a 3D space of a circuit board"""
    def __init__(self):
        self.roll_angle = 0;
        self.head_angle = 0;
        self.orientation_angle = 0;

    #Setters
    def SetRoll(self,angle):
        self.roll_angle = angle;

    def SetHead(self,angle):
        self.head_angle = angle;

    def SetOrientation(self,angle):
        self.orientation_angle = angle;

    #Getters
    def GetRoll(self):
        return self.roll_angle;

    def GetHead(self):
        return self.head_angle;

    def GetOrientation(self):
        return self.orientation_angle;


