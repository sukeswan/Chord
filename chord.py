#mock II
MBITS = 8
CIRCLESIZE = 2**MBITS
class FingerTable():

    def __init__(self, nodeId = -1):
        self.nodeId = nodeId
        ####  position 0 marked with -1 to start index at 1
        self.start = [-1]
        self.succNodes = [-1 for i in range(9)]

        for i in range(1,9):
            dist = 2**(i-1)
            self.start.append((nodeId + dist)%CIRCLESIZE)

class Node():

    def __init__(self, nodeId = -1):
        self.nodeId = nodeId
        self.table = FingerTable(nodeId)
        self.successor  = -1
        self.predecessor = -1
        key_vals = {}

    def join(self, prime = None): #if none itial join
        if prime is None:
            self.predecessor = self
            self.successor = self
            for i in range(1,9):
                self.table.succNodes[i] = self
        else:
            self.init_finger_table(prime)
            self.update_others()
            #to implement update others!

    def init_finger_table(self, prime): #prime is original, self is new node

        succID = self.table.start[1]
        succ = prime.find_successor(succID)

        #sets successor
        self.table.succNodes[1] = succ
        self.successor = self.table.succNodes[1]

        #sets predecessor
        self.predecessor = self.successor.predecessor
        self.successor.predecessor = self

        #sets predecessor's successor
        self.predecessor.successor = self
        self.predecessor.table.succNodes[1] = self

        #### call find successor for every start index
        for i in range(1,8):
            if self.table.start[i+1] in self.set_build( self.nodeId, self.table.succNodes[i].nodeId)[1:]:
                self.table.succNodes[i+1] = self.table.succNodes[i] #consider changeing to self.succesor
            else:
                self.table.succNodes[i+1] = prime.find_successor(self.table.start[i+1])


    def find_successor(self, id):

        if(self.nodeId == self.successor.nodeId):
            return self
        else:
            prime = self.find_predecessor(id)
            return prime.successor

    def find_predecessor(self, id):
        prime = self
        lower = prime.nodeId
        upper = prime.successor.nodeId
        while id not in self.set_build(lower, upper):
            prime = prime.self.closest_preceding_finger(id)
            lower = prime.nodeId
            upper = prime.successor.nodeId
        return prime

    def closest_preceding_finger(self, id):
        for i in range(8,0):
            currentId = self.table.succNodes[i].nodeId
            if id in self.set_build(currentId, self.nodeId):
                return self.table.succNodes[i]
        return self

    def set_build(self, lower, upper):

        if lower< upper:
            final_set = [i for i in range(lower,upper+1)]
            return final_set
        else:
            upper += 256
            final_set = [i%256 for i in range(lower, upper+1)]
            return final_set

    def update_others():
        pass

    def print_info(self):
        print("---------- Node ID:", self.nodeId)
        print(   "Sucessor: " ,self.successor.nodeId , "\nPredecessor: ", self.predecessor.nodeId )
        c = 0
        print("FingerTables:")
        for i, j in zip(self.table.start, self.table.succNodes):
            if c == 0:
                c+=1
                pass
            else:
                dist = pow(2, c-1)
                div = pow(2, 8)
                intervalEnd = (i + dist) % div

                print("| k = ", c, " [", i, ", ", intervalEnd,  ")   " , "succ. = " , j, " |")
                c+=1
    def __str__(self):
        return str(self.nodeId)





n0 = Node(0)
n1 = Node(250)
n2 = Node(69)
n0.join(None)
n1.join(n0)
n0.print_info()
n1.print_info()
# n0.print_info()













# ft = FingerTable(0)
# print(ft.start)
