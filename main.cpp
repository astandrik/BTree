#include <iostream>
#include "BTree.h"
using namespace std;


int main() {
    BTree tree(3);
    tree.insert(1);
    tree.insert(5);
 tree.insert(7);
 tree.insert(9);
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
 tree.insert(4);
    tree.print();
    tree.del(21);
    tree.del(20);
    tree.del(11);
  tree.print();
    return 0;
}
