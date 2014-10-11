#include <iostream>
#include "BTree.h"
using namespace std;


int main() {
    BTree tree(3);
    for (int i = 1; i <=28; ++i) {
        tree.insert(i);
    }
    tree.del(9);
    tree.del(18);
    tree.del(17);
    tree.del(16);
    tree.del(15);
    tree.del(14);
    tree.del(13);
    tree.del(12);
    tree.print();
    return 0;
}
