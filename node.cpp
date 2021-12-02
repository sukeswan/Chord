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
        Node *succNodes[FINGERTABLESIZE]; // for storing pointers to succNodes

    FingerTable(){
        nodeId = -1; // default constructor for Node class
    }

    FingerTable(int aNodeID){ // constructor with node ID 
        nodeId = aNodeID; 
        int circlesize = pow(2, MBITS);
        for (int i = 1; i < FINGERTABLESIZE; i ++){
            int dist = pow(2, i-1);
            start[i] = (nodeId + dist) % circlesize; // calculate end interval for node
        }
    }

    void print(); // print function for fingertable
};

class Node{
    public: 
        int nodeId; 
        FingerTable table;
        Node* successor; // pointer to successor node
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
        printFT();
        return; // this is for testing only 
        //update_others();
        // move keys in (predecessor,n] from successor 
        printFT(); 
        return; 
    }

    void join(){ // this join is for the initial chord node joining the network 
        
        predecessor = this; // node point to itself 
        

        for(int i = 1; i < FINGERTABLESIZE; i++){
            table.succNodes[i] = this;
    
        }
        successor = table.succNodes[1]; 
        printFT(); 
        return; 
    }

    Node* find_successor(int id){ // get successor of node with id 
        cout << "node id " <<nodeId << " successor "<< successor->nodeId << " check ID " << id << endl; 
        if(nodeId == successor->nodeId){ // if this is only node, return it
            return this; 
        }

        Node* prime = find_predecessor(id); 
        cout << "prime " << prime->nodeId << "primesucc " << prime->successor->nodeId << endl; 
        return prime->successor; 
    }

    Node* find_predecessor(int id){ // get the predecessor of id using this
             
        Node* prime = this;
        int lower = prime->nodeId;
        int upper = prime->successor->nodeId;
        //cout<< "lower " << lower << " id " << id << " upper " << upper << endl; 
        if (lower>=upper){
            return prime; 
        }
        cout << "while" << endl; 
        while (!set_check(id,lower,upper,0,1)){
            prime = prime->closest_preceding_finger(id);
        }
        //cout << "HOLA" << endl;
        return prime; 
    }

    Node* closest_preceding_finger(int id){

        for(int i = MBITS; i>0; i--){
            int currentId = table.succNodes[i]->nodeId;
            bool in_range = set_check(currentId,nodeId, id, 0,0); 
            if(in_range){
                return table.succNodes[i]; 
            }
        }
        //cout << "returning id " << this->nodeId << endl; 
        return this; 
    }

    void init_finger_table(Node* prime){
        int succID  = table.start[1]; 
        //cout << "succID " << succID << endl; 
        Node* succ = prime->find_successor(succID); 
        //cout << succ->nodeId << endl; 
        table.succNodes[1] = succ;
        successor = table.succNodes[1]; 

        predecessor = successor->predecessor; 
        successor->predecessor = this;
        predecessor->successor = this; 
        predecessor->table.succNodes[1] = this; 
        //cout<< "pred " << predecessor->nodeId << endl; 
        //cout<< "succpred " << successor->predecessor->nodeId << endl; 

        for(int i = 1; i < MBITS; i++){
            int currentId = table.start[i+1]; 
            //cout << "currentId " << currentId << endl; 
            
            bool in_range = set_check(currentId, nodeId, table.succNodes[i]->nodeId, 1,0);

            if(in_range){ 
                table.succNodes[i+1] = table.succNodes[i];
                cout<< "if "<<i<<" "<< table.succNodes[i]->nodeId<<endl;

            }
            else{
                cout<< "else "<<i<<" "<< prime->find_successor(currentId)->nodeId<<endl;
                table.succNodes[i+1] = prime->find_successor(currentId);

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

        bool in_range = set_check(s->nodeId,nodeId,table.succNodes[i]->nodeId,1,0); 

        if(in_range){
            table.succNodes[i] = s; 
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

    static bool set_check(int target, int lower, int upper, int left, int right){
        // left right exclusive 0 / inclusive 1
        if (lower == upper){
            if (left ==0 || right==0){
                return false; 
            }
            if (lower == target){
                return true; 
            }
        }
        if (lower < upper){

            int range = upper-lower+1;
            int current;

            for(int i = 0; i < range; i ++){
                current = i + lower; 

                if (i==0 && left ==0){ // left exclusive
                    current = -1; 
                } 
                if (i == (range-1) && right==0){ // right exclusive 
                    current = -1; 
                }
                if (current == target){
                    return true; 
                }
            } 
        }
        else {   //if (lower > upper)
            int range = upper + 256 - lower + 1;
            int current;

            for (int i = 0; i < range; i++){
                current = (lower + i) % 256;
                
                if (i==0 && left ==0){ // left exclusive
                    current = -1; 
                } 
                if (i == (range-1) && right==0){ // right exclusive 
                    current = -1; 
                }
                if (current == target){
                    return true; 
                }
            } 
        }

        return false; 
    }

    void printFT(){ // prints succNodes id, succ, pred, and fingertable
        cout << "---------- Node ID: " << nodeId << " ----------" << endl; 
        cout << "Sucessor: " << successor->nodeId << "\nPredecessor: " << predecessor->nodeId << "\n" << endl; 
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
        int succ = succNodes[i]->nodeId;
        cout << "| k = " << i << " [" << start[i] << ", " << intervalEnd << ")   " << "succ. = " << succ << " |" << endl;
    }

}

void test_fingerTablePrint(){ // test creating FingerTable and printing it 
    
    Node* one = new Node(1); 
    Node* two = new Node(2);
    (*one).successor = two; 
    
    FingerTable test(0); 
   
    for(int i = 1; i < 8; i++){
        test.succNodes[i] = one; 
    }
    test.succNodes[8] = two;
    test.print();
}

void test_nodePrints(){ // test both node print functions
    
    Node testNode(8); 
    Node check(500); 
    Node next(501);
    check.successor = &next; 
    for(int i = 1; i < FINGERTABLESIZE; i++){
        testNode.table.succNodes[i] = &check;
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

    Node* suck = test.find_successor(3); 
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
}

int main(){
    Node n0(0);
    Node n1(30);
    Node n2(65);

    n0.join();
    n1.join(&n0);
    
    n2.join(&n1);

    cout << "____________________FINAL ____________________" << endl; 

    n0.printFT();
    n1.printFT(); 
    n2.printFT();

    return 0;
}