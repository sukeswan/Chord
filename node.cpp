#include <stdint.h>
#include <iostream>
#include <iterator>
#include <map>
#include <cmath> 
using namespace std;
#define MBITS 8
#define INDEXOFFSET 1                           // 8 values but indexing starts at 1
#define FINGERTABLESIZE (MBITS + INDEXOFFSET)   // finger tables will 0-8 but 0 will be left empty 

class Node; // so compiler doesn't shit itself  

class FingerTable{
    public: 
        int nodeId; // ID of node hosting finger table
        int intervalEnd[FINGERTABLESIZE]; // end interval values
        int succ[FINGERTABLESIZE];  // sucessor for start nodes
        Node *nodes[FINGERTABLESIZE]; // for storing pointers to nodes

    FingerTable(){
        nodeId = -1; // default constructor for Node class
    }

    FingerTable(int aNodeID){ // constructor with node ID 
        nodeId = aNodeID; 
    }

    void print(); // print function for fingertable
    void add(int rowNum, Node* newNode); // to add a value to the finger table 
};

class Node{
    public: 
        int nodeId; 
        FingerTable table;
        Node* sucessor; // pointer to sucessor node
        Node* predecessor; // pointer to predecessor node
        map<int, int> key_vals; // map to store the keys and values at this node

    Node(){

    }

    Node(int id){ // constructor for Node
        nodeId = id; 
        table = FingerTable(nodeId);
    }

    void join(Node* node){
        return; 
    }

    void join(){ // this join is for the initial chord node joining the network 
        
        predecessor = this; // node point to itself 
        sucessor = this; 

        for(int i = 1; i < FINGERTABLESIZE; i++){
            table.add(i,this);
        }
        printFT(); 
        return; 
    }

    Node* find_succesor(int id){ // get sucessor of node with id 

        if (this->nodeId == id){ // trival case where n looks up its own sucessor
            return this->sucessor;
        }

        Node* prime = this->find_predecessor(id); 
        return prime->sucessor; 
    }

    Node* find_predecessor(int id){ // get the predecessor of id using this

        if (this->nodeId == id){ // trival case where n looks up its own predecessor 
            return this->predecessor;
        }
             
        Node* prime = this;
        int lower = prime->nodeId;
        int upper = prime->sucessor->nodeId;
        while (!((id > lower) && (id<=upper))){
            prime = prime->sucessor->closest_preceding_finger(id); 
        }
        return prime; 
    }

    Node* closest_preceding_finger(int id){

        for(int i = MBITS; i>0; i--){
            int currentId = table.nodes[i]->nodeId;
            if((currentId > nodeId) && (currentId < id)){
                return table.nodes[i]; 
            }
        }
        return this; 
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

void FingerTable::print(){
    for(int i = 1; i < FINGERTABLESIZE; i++){
        cout << "| k = " << i << " [" << nodes[i]->nodeId << ", " << intervalEnd[i] << ")   " << "succ. = " << succ[i] << " |" << endl;
    }
}

void FingerTable::add(int rowNum, Node* newNode){ // table.add(1,&node)
    nodes[rowNum] = newNode; 
    succ[rowNum] = newNode->sucessor->nodeId; 
    int dist = pow(2, rowNum-1);
    int div = pow(2, MBITS);
    intervalEnd[rowNum] = (newNode->nodeId + dist) % div; // calculate end interval for node
}

void test_fingerTablePrint(){ // test creating FingerTable and printing it 
    
    Node one(1); 
    Node two(2);
    one.sucessor = &two; 
    
    FingerTable test(0); 
   
    for(int i = 1; i < FINGERTABLESIZE; i++){
        test.add(i,&one); 
    }

    test.print();
}

void test_nodePrints(){ // test both node print functions
    
    Node testNode(8); 
    Node check(500); 
    Node next(501);
    check.sucessor = &next; 
    for(int i = 1; i < FINGERTABLESIZE; i++){
        testNode.table.add(i,&check); 
        testNode.key_vals.insert(pair<int, int>(i, i*i));

    } 
    //testNode.key_vals.insert(pair<int, int>(50, NULL));
    testNode.printFT(); 
    testNode.printMap(); 

}

void test_nodeJoin(){ // test first node joining the chord network 
    Node only(3); 
    only.join();
}

void test_CPF(){ // test closest_preceding_finger function
    Node test(3); 
    test.join();
    Node* check = test.closest_preceding_finger(4);
    check->printFT();
}

void test_trivalNeighbors(){ // test find pred and find succ for single node in chord
    Node test(3); 
    test.join();

    Node* suck = test.find_succesor(3); 
    Node* pred = test.find_predecessor(3);

    suck->printFT();
    pred->printFT();

}

int main(){ 
    test_trivalNeighbors();
    return 0;
}