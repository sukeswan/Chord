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
        int start[FINGERTABLESIZE];
        Node *nodes[FINGERTABLESIZE]; // for storing pointers to nodes

    FingerTable(){
        nodeId = -1; // default constructor for Node class
    }

    FingerTable(int aNodeID){ // constructor with node ID 
        nodeId = aNodeID; 
    }

    void print(); // print function for fingertable
    void set(int rowNum, Node* newNode); // to set a value to the finger table 
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

    void join(Node* prime){
        init_finger_table(prime); 
        update_others();
        // move keys in (predecessor,n] from successor 
        printFT(); 
        return; 
    }

    void join(){ // this join is for the initial chord node joining the network 
        
        predecessor = this; // node point to itself 
        

        for(int i = 1; i < FINGERTABLESIZE; i++){
            table.set(i,this);
    
        }
        sucessor = table.nodes[1]; 
        printFT(); 
        return; 
    }

    Node* find_succesor(int id){ // get sucessor of node with id 

        Node* prime = this->find_predecessor(id); 
        return prime->sucessor; 
    }

    Node* find_predecessor(int id){ // get the predecessor of id using this
             
        Node* prime = this;
        int lower = prime->nodeId;
        int upper = prime->sucessor->nodeId;
        //cout << "LOWER ID UPPER: "<< lower << " " << id << " " << upper << endl;
        if (lower >= upper){
            //cout << "lower > upper" << endl; 
            return prime; 
        }

        //cout << "NOT" << endl; 
        while (id <= lower || id > upper){
            prime = prime->closest_preceding_finger(id);
            
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

    void init_finger_table(Node* prime){
        int succID  = table.start[1]; 
        Node* succ = prime->find_succesor(succID); 
        table.set(1,succ);
        sucessor = succ; 

        predecessor = sucessor->predecessor; 
        sucessor->predecessor = this;

        for(int i = 1; i < MBITS; i++){
            int currentId = table.start[i+1]; 
            if(currentId >= this->nodeId  && currentId < table.nodes[i]->nodeId){
                table.set(i+1,table.nodes[i]);
            }
            else{
                table.set(i+1,prime->find_succesor(currentId));
            }
        }

        return; 
    }

    void update_others(){
        for(int i = 1; i < FINGERTABLESIZE; i++){
            int sub = pow(2,i-1); 
            Node* p = find_predecessor(nodeId - sub);
            p->update_finger_table(this,i);  
        }
        return; 
    }

    void update_finger_table(Node* s, int i){
        if(s->nodeId >= nodeId && s->nodeId < table.nodes[i]->nodeId){
            table.set(i,s);
            if(i==1){
                sucessor = s; 
            }
            Node* p = predecessor; 
            p->update_finger_table(s,i); 
        }
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

void FingerTable::print(){
    for(int i = 1; i < FINGERTABLESIZE; i++){
        int dist = pow(2, i-1);
        int div = pow(2, MBITS);
        int intervalEnd = (start[i] + dist) % div;
        int succ = nodes[i]->sucessor->nodeId;
        cout << "| k = " << i << " [" << start[i] << ", " << intervalEnd << ")   " << "succ. = " << succ << " |" << endl;
    }
}

void FingerTable::set(int rowNum, Node* newNode){ // table.set(1,&node)
    nodes[rowNum] = newNode; 
    int dist = pow(2, rowNum-1);
    int div = pow(2, MBITS);
    start[rowNum] = (nodeId + dist) % div; // calculate end interval for node
    
}

void test_fingerTablePrint(){ // test creating FingerTable and printing it 
    
    Node one(1); 
    Node two(2);
    one.sucessor = &two; 
    
    FingerTable test(0); 
   
    for(int i = 1; i < FINGERTABLESIZE; i++){
        test.set(i,&one); 
    }

    test.print();
}

void test_nodePrints(){ // test both node print functions
    
    Node testNode(8); 
    Node check(500); 
    Node next(501);
    check.sucessor = &next; 
    for(int i = 1; i < FINGERTABLESIZE; i++){
        testNode.table.set(i,&check); 
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

void test_initFT(){ //test init_finger_table() fx
    Node three(3); 
    Node four(4); 
    three.join();

    four.init_finger_table(&three); 
    four.printFT();
}

void test_join(){
    Node n0(0);
    Node n1(30);
    Node n2(65); 
    Node n3(110);
    Node n4(160);
    Node n5(230); 

    n0.join();
    n1.join(&n0);
    n2.join(&n1);
    n3.join(&n2);
    n4.join(&n3);
    n5.join(&n4);

    n0.printFT(); 

}

int main(){ 
    test_join();
    return 0;
}