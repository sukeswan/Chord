#include <stdint.h>
#include <iostream>
#include <iterator>
#include <map>
#include <cmath> 
using namespace std;
#define MBITS 8
#define INDEXOFFSET 1                           // 8 values but indexing starts at 1
#define FINGERTABLESIZE (MBITS + INDEXOFFSET)   // finger tables will 0-8 but 0 will be left empty  
#define None -1

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
    
    void join(){ // this join is for the initial chord node joining the network 
        
        predecessor = this; // node point to itself 
        successor = this;

        for(int i = 1; i < FINGERTABLESIZE; i++){
            table.succNodes[i] = this;
        }
         
        //printFT(); 
        return; 
    }

    void join(Node* prime){
        init_finger_table(prime); //initialize finger table
        update_others(nodeId); // update other nodes
        migrate_keys(); // migrate keys from successor
        return; 
    }

    void leave(){
        predecessor->successor = successor; // remove pointers to this node
        successor->predecessor = predecessor; 

        successor->update_others(successor->nodeId); // update finger tables of network
        leave_migrate_keys(); // move key value pairs to successor
    }

    Node* find_successor(int id){ // get successor of node with id 

        if(nodeId == id){ // joining node will never ask for its own Succesor
            return this; // this case is used for insert 
        }

        Node* prime = find_predecessor(id); 
        return prime->successor; 
    }

    Node* find_predecessor(int id){ // get the predecessor of id using this
             
        Node* prime = this;
        int lower = prime->nodeId;
        int upper = prime->successor->nodeId;

        if (lower==upper){
            return prime; 
        }

        while (!set_check(id,lower,upper,0,1)){
            prime = prime->closest_preceding_finger(id);
            lower = prime->nodeId;
            upper = prime->successor->nodeId;
        }

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
        return this; 
    }

    void init_finger_table(Node* prime){
        int succID  = table.start[1]; // this and this+1 will have same successor
        table.succNodes[1] = prime->find_successor(succID); // ask prime to find successor of next value
        successor = table.succNodes[1]; // set

        predecessor = successor->predecessor; // fix pointers to point to this node
        successor->predecessor = this;
        predecessor->successor = this; 

        for(int row = 2; row < FINGERTABLESIZE; row++){
            if(set_check(table.start[row],nodeId,successor->nodeId,0,1)){ // case: finger has same successor as node
                table.succNodes[row] = successor; 
            }
            else{
                //cout << row << endl; 
                table.succNodes[row] = prime->find_successor(table.start[row]); // case2: finger has different successor
            }
            
        }

        return; 
    }

    void update_others(int origin){ // update other nodes of newly joined or removed node. 
        Node* current = predecessor; 

        if (current->nodeId == origin){
            return;
        }
        else{
         
            for(int row = 1; row < FINGERTABLESIZE; row++){
                current->table.succNodes[row] = find_successor(current->table.start[row]);
            }
        (*current).update_others(origin); 
        }
    }

    static bool set_check(int target, int lower, int upper, int left, int right){ // function to check if target in set 
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

	int find(int key){ // find the value of a key using node 
        Node* next = find_successor(key);

        if(next->key_vals.find(key) == next->key_vals.end()){ // key does not exist
            return -1; 
        }

        int value = next->key_vals.find(key)->second; 

        if(next->nodeId == nodeId){
            cout << "Lookup result of key " << key  << " from node " << nodeId << " with path [" << nodeId  << "] value is ";
        }
        else{
            cout << "Lookup result of key " << key << " from node " << nodeId << " with path [" << nodeId << ", " << next->nodeId << "] value is "; 
        }
        if(value==-1){ // print none if value is -1
            cout << "None" <<endl; 
        }
        else{
            cout << value <<endl; 
        }
        return value;
    }

    void migrate_keys(){ // migrate keys from successor to newly joined node
        map<int, int>::iterator itr;

        if(successor->key_vals.empty()){ // no keys to migrate
            return; 
        }
         
        for (itr = successor->key_vals.begin(); itr != successor->key_vals.end(); ++itr) {

            bool in_range = set_check(itr->first, predecessor->nodeId, nodeId, 0,1); 
            if(in_range){
                key_vals.insert(pair<int, int>(itr->first, itr->second)); // insert keys into new node 
                cout << "Migrate key " << itr->first << " from node " << successor->nodeId << " to node " << nodeId << endl;
            }
        }

        for( int i = predecessor->nodeId + 1; i <=nodeId; i++){
            successor->key_vals.erase(i); // remove keys from successor
        }

        return; 

    }

    void leave_migrate_keys(){ // add keys tro successor if node leaves
        map<int, int>::iterator itr;

        if(key_vals.empty()){ // no keys to migrate
            return; 
        }
         
        for (itr = key_vals.begin(); itr != key_vals.end(); ++itr) {
            
            successor->key_vals.insert(pair<int, int>(itr->first, itr->second)); // add key vals to successor
            cout << "Leave: Migrate key " << itr->first << " from node " << nodeId << " to node " << successor->nodeId << endl;
            
        }
        return; 
    }

	void insert(int key, int value){ // insert key value pair in correct successor 
        Node* next = find_successor(key); 
        next->key_vals.insert(pair<int, int>(key, value));
        return; 
    }

	void remove(int key){ // remove key value pair from correct successor
        Node* next = find_successor(key); 
        next->key_vals.erase(key); 
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
            if (itr->second == None){
                cout << "(" << itr->first << ": " << "None" << ") "; 
            }
            else{
                cout << "(" << itr->first << ": " << itr->second << ") "; 
            }
            
        }
        cout << "}" << endl; 
        return; 
    }

}; 

void FingerTable::print(){ // print finger tables
    for(int i = 1; i < FINGERTABLESIZE; i++){
        int dist = pow(2, i-1);
        int div = pow(2, MBITS);
        int intervalEnd = (start[i] + dist) % div;
        int succ = succNodes[i]->nodeId;
        cout << "| k = " << i << " [" << start[i] << ", " << intervalEnd << ")   " << "succ. = " << succ << " |" << endl;
    }

}

int main(){

    Node n0(0); // create nodes for Chord
    Node n1(30);
    Node n2(65); 
    Node n3(110);
    Node n4(160);
    Node n5(230); 

    n0.join(); // add nodes to the chord network 
    n1.join(&n0);
    n2.join(&n1);
    n3.join(&n2);
    n4.join(&n3);
    n5.join(&n4);

    n0.printFT(); // print the fingertables of all nodes
    n1.printFT(); // print after all joins for up to date tables
    n2.printFT();
    n3.printFT();
    n4.printFT();
    n5.printFT();

    n0.insert(3,3); // insert key,value pairs into the chord network 
    n1.insert(200,None); 
    n2.insert(123,None);
    n3.insert(45,3);
    n4.insert(99,None);
    n2.insert(60,10);
    n0.insert(50,8);
    n3.insert(100,5);
    n3.insert(101,4);
    n3.insert(102,6);
    n5.insert(240,8);
    n5.insert(250,10);

    cout << "\n"; 

    n0.printMap(); // print all key value pairs 
    n1.printMap();
    n2.printMap();
    n3.printMap();
    n4.printMap();
    n5.printMap();

    cout << "\n"; 

    Node n6(100); // join a new node with id 100 
    n6.join(&n5); 

    int check[12] = {3,200,123,45,99,60,50,100,101,102,240,250}; // print lookups for nodes 0,65,110
    cout <<"\n-------------------- Node 0 --------------------" << endl; 
    for(int i = 0; i < 12; i ++){
        n0.find(check[i]);
    }

    cout <<"\n-------------------- Node 65 --------------------" << endl; 
    for(int i = 0; i < 12; i ++){
        n2.find(check[i]);
    }

    cout <<"\n-------------------- Node 110 --------------------" << endl; 
    for(int i = 0; i < 12; i ++){
        n6.find(check[i]);
    }

    cout << "\n";
    
    cout <<"\n-------------------- LEAVE OPERATION --------------------" << endl; 

    n2.leave(); // remove n2 from the network 
    cout << "\n";

    n0.printFT(); 
    n1.printFT(); 
    cout << "\n";

    n0.printMap(); // print updated maps afret n2 leaves the network 
    n1.printMap();
    n6.printMap();
    n3.printMap();
    n4.printMap();
    n5.printMap();

    return 0;
}