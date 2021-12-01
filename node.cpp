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
    void set(int rowNum, Node* newNode); // to set a value to the finger table 
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
        //return; // this is for testing only 
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
        successor = this; 
        printFT(); 
        return; 
    }

    Node* find_successor(int id){ // get successor of node with id 
        Node* prime = find_predecessor(id); 
        return prime->successor; 
    }

    Node* find_predecessor(int id){ // get the predecessor of id using this
             
        Node* prime = this;
        int lower = prime->nodeId;
        int upper = prime->successor->nodeId;
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
            int currentId = table.succNodes[i]->nodeId;
            if((currentId > nodeId) && (currentId < id)){
                return table.succNodes[i]; 
            }
        }
        return this; 
    }

    void init_finger_table(Node* prime){
        int succID  = table.start[1]; 
        cout << "succID " << succID << endl; 
        Node* succ = prime->find_successor(succID); 
        cout << succ->nodeId << endl; 
        table.set(1,succ);
        successor = table.succNodes[1]; 

        predecessor = successor->predecessor; 
        successor->predecessor = this;
        cout<< "pred " << predecessor->nodeId << endl; 
        cout<< "succpred " << successor->predecessor->nodeId << endl; 

        for(int i = 1; i < MBITS; i++){
            int currentId = table.start[i+1]; 
            cout << "251 node" << table.succNodes[i]->nodeId << endl; 
            int* set = set_build(this->nodeId , table.succNodes[i]->nodeId, 1, 1);
            bool set_check = false;
            while (set != NULL) {
                cout<< "while" << endl;
                if ((*set) == currentId){
                    // table.set(i+1,table.succNodes[i]);
                    set_check = true;
                } 
                set++;
            }
            if(set_check){ //&& currentId < table.succNodes[i]->nodeId
                table.set(i+1,table.succNodes[i]);
                cout<< "if"<<i<<" "<< table.succNodes[i]->nodeId<<endl;

            }
            else{
                //prime->find_successor(currentId)->table.succNodes[1]
                cout<< "else"<<i<<" "<< prime->find_successor(currentId)->nodeId<<endl;
                table.set(i+1,prime->find_successor(currentId));
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
        if(s->nodeId >= nodeId && s->nodeId < table.succNodes[i]->nodeId){
            table.set(i,s);
            if(i==1){
                successor = s; 
            }
            Node* p = predecessor; 
            p->update_finger_table(s,i); 
        }
        return;
    }
    int* inclusion(int* set, int left, int right){
        if (left == 1 && right ==1){
            return set;
        }
        else if (left == 0 && right ==0){
            set[0] = -1;
            set[(sizeof(set)/sizeof(*set))-1] = -1;
            return set;
        }
        else{
            return set;
        }
    }
    int* set_build(int lower, int upper, int left, int right){
        // left right exclusive 0 / inclusive 1
        // int* finalset;
        if (lower == upper){
            int finalset[1] = {lower};
            return finalset;  

        }
        else if (lower < upper){
            int range = upper-lower+1;
            int finalset[range];

            for(int i = 0; i < range; i ++){
                finalset[i] = i + lower; 
            } 
            return inclusion(finalset, left,right);
        }

        else {   //if (lower > upper)
            int range = upper + 256 - lower + 1;
            int finalset[range];
            for (int i = 0; i < range; i++){
                finalset[i] = (lower + i) % 256;
            } 
               return inclusion(finalset, left,right);

        }
        // //inclusion exclusion
        // if (left == 1 and right == 1){
        //     return finalset;
        // }
        // else if (left == 0 and right == 1){
        //     return finalset[];
        // }()

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

void FingerTable::set(int rowNum, Node* newNode){ // table.set(1,&node)

    succNodes[rowNum] = newNode; 
}

void test_fingerTablePrint(){ // test creating FingerTable and printing it 
    
    Node* one = new Node(1); 
    Node* two = new Node(2);
    (*one).successor = two; 
    
    FingerTable test(0); 
   
    for(int i = 1; i < 8; i++){
        test.set(i,one); 
    }
    test.set(8,two); 
    test.print();
}

void test_nodePrints(){ // test both node print functions
    
    Node testNode(8); 
    Node check(500); 
    Node next(501);
    check.successor = &next; 
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
    Node n1(250);
    Node n2(65); 
    Node n3(110);
    Node n4(160);
    Node n5(230); 

    n0.join();
    n1.join(&n0);
    n0.printFT();
    n1.printFT(); 
    // n2.join(&n1);
    // n3.join(&n2);
    // n4.join(&n3);
    // n5.join(&n4);

    //n0.printFT(); 

}

int main(){
    //test_fingerTablePrint(); 
    test_join();
    return 0;
}