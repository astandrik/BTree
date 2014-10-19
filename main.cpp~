#include <iostream>
#include "BTree.h"
#include <string>
using namespace std;

int main() {
    DBC config;
    config.chunk_size = 300;
    config.db_size = 90000;
    config.t_factor = 3;


    string path = "/home/anton/MyDataBase.dat";
    DB* database = dbcreate(const_cast<char*>(path.c_str()), config);

    for(int i = 0; i < 100; i++) {
        database->dbworker->insert(i);
    }
    database->dbworker->print();
    return 0;
}
