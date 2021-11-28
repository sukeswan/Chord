#include <stdint.h>
#include <iostream>
#include <iterator>
#include <map>
using namespace std;
#define FINGERTABLESIZE 8

class FingerTable{
    public: 
        int nodeId; // ID of node hosting finger table
        int start[FINGERTABLESIZE]; // start interval values
        int intervalEnd[FINGERTABLESIZE]; // end interval values
        int succ[FINGERTABLESIZE];  // sucessor for start nodes

    FingerTable(){
        nodeId = -1; // default constructor for Node class
    }

    FingerTable(int aNodeID){ // constructor with node ID 
        nodeId = aNodeID; 
    }
      
    void print(){ // print all values in the finger table printing: row num, interval, sucessor 
        cout << "FingerTables:" << endl;
        for(int i = 0; i < FINGERTABLESIZE; i++){
            cout << "| k = " << (i+1) << " [" << start[i] << ", " << intervalEnd[i] << ")   " << "succ. = " << succ[i] << " |" << endl;
        }
    }
};

class Node{
    public: 
        int nodeId; 
        FingerTable table;
        Node* sucessor; // pointer to sucessor node
        Node* predecessor; // pointer to predecessor node
        map<int, int> key_vals; // map to store the keys and values at this node
    
    Node(int id){ // constructor for Node
        nodeId = id; 
        table = FingerTable(nodeId);
    }

    void join(Node* node){
        return; 
    }

	//TODO: implement DHT lookup
	int find(int key){
        return -1; 
    }

	//TODO: implement DHT key insertion
	void insert(int key){
        return; 
    }

	//TODO: implement DHT key deletion
	void remove(int key){
        return; 
    }

    void printFT(){ // prints nodes id, succ, pred, and fingertable
        cout << "---------- Node ID: " << nodeId << " ----------" << endl; 
        cout << "Sucessor: " << sucessor->nodeId << "\nPredecessor: " << predecessor->nodeId << "\n" << endl; 
        table.print(); 
        cout << "-------------------------------\n*******************************" << endl;  
        return; 
    }

    void printMap(){ // print the values stored in the node

        cout << "---------- Node ID: " << nodeId << " ----------" << endl;
        cout << "{ "; 
        map<int, int>::iterator itr;
        for (itr = key_vals.begin(); itr != key_vals.end(); ++itr) {
            cout << "(" << itr->first << ": " << itr->second << ") "; 
        }
        cout << "}" << endl; 
        return; 
    }

}; 

void test_fingerTablePrint(){ // test creating FingerTable and printing it 
    
    FingerTable test(8); // initialized with node ID 8 

    for(int i = 0; i < FINGERTABLESIZE; i++){
        test.start[i] = i; 
        test.intervalEnd[i] = i+1; 
        test.succ[i] = i+2; 
    }
    
    test.print(); 
}

void test_nodePrints(){ // test both node print functions
    
    Node testNode(8); 
    for(int i = 0; i < FINGERTABLESIZE; i++){
        testNode.table.start[i] = i; 
        testNode.table.intervalEnd[i] = i; 
        testNode.table.succ[i] = i; 

        testNode.key_vals.insert(pair<int, int>(i, i*i));

    } 
    //testNode.key_vals.insert(pair<int, int>(50, NULL));
    testNode.printFT(); 
    testNode.printMap(); 

}

int main(){
    test_nodePrints();
    return 0;
}; 