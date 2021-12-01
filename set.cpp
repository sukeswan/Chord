#include <stdint.h>
#include <iostream>
using namespace std;

bool set_build(int target, int lower, int upper, int left, int right){
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

int main(){
    cout << "test1" << endl;
    bool test1 = set_build(5,5,5,0,1);  // 0
    cout << (test1==false) << endl; 

    cout << "test2" << endl;
    bool test2 = set_build(5,5,5,1,1);  // 1
    cout << (test2==true) << endl; 

    cout << "test3" << endl;
    bool test3 = set_build(2,1,5,1,1);  // 1
    cout << (test3==true) << endl; 

    cout << "test4" << endl;
    bool test4 = set_build(1,1,5,0,1);  // 0
    cout << (test4==false) << endl; 

    cout << "test5" << endl;
    bool test5 = set_build(5,1,5,1,0);  // 0
    cout << (test5==false) << endl; 

    cout << "test6" << endl;
    bool test6 = set_build(0,250,3,1,0);  // 1
    cout << (test6==true) << endl; 

    cout << "test7" << endl;
    bool test7 = set_build(250,250,3,0,1);  // 0
    cout << (test7==false) << endl; 

    cout << "test8" << endl;
    bool test8 = set_build(3,250,3,1,0);  // 0
    cout << (test8==false) << endl; 

    return 0;
}