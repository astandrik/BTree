#include <iostream>
#include "BTree.h"
using namespace std;


int main() {
    BTree tree(2);
    tree.insert(1);
    tree.insert(5);
    tree.insert(7);
    tree.insert(9);
    tree.insert(10);
    tree.insert(11);
    tree.insert(12);
    tree.insert(13);
    tree.insert(14);
    tree.insert(15);
    tree.insert(16);
    tree.insert(17);
    tree.insert(20);
    tree.insert(18);
    tree.insert(19);
    tree.insert(21);
    tree.insert(22);
    tree.print();
    return 0;
}
