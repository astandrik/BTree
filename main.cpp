#include <iostream>
#include "BTree.h"
using namespace std;


int main() {
    BTree tree(3);
    for (int i = 1; i <=12; ++i) {
        tree.insert(i);
    }
    tree.del(12);
    tree.del(10);
    tree.del(2);
    tree.insert(12);
    tree.insert(13);
    tree.print();
    return 0;
}
