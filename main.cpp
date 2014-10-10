#include <iostream>
#include "BTree.h"
using namespace std;


int main() {
    BTree tree(3);
    for (int i = 1; i <=28; ++i) {
        tree.insert(i);
    }
    tree.del(9);
    tree.print();
    return 0;
}
