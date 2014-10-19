#include <iostream>
#include "BTree.h"
#include <string>
using namespace std;

int main() {
 /*  DBC config;
    config.chunk_size = 300;
    config.db_size = 90000;
    config.t_factor = 3;


    string path = "/home/anton/MyDataBase.dat";
    DB* database = dbcreate(const_cast<char*>(path.c_str()), config);
    char* key1 = "helloDolly1";
    char* key2 = "helloDolly2";
    char* key3 = "helloDolly3";
    char* key4 = "helloDolly4";
    char* key5 = "helloDolly5";
    char* key6 = "helloDolly6";
    char* key7 = "helloDolly7";
    char* key8 = "helloDolly8";
    char* key9 = "helloDolly9";
    char* val1 = "Value 1";
    char* val2 = "Value 2";
    char* val4 = "Value 4";

    for(int i = 0; i < 50; i ++) {
        db_put(database, key1, 1, val1, 1);
    }
    db_put(database, key8, 1, val4, 1);
    database->dbworker->print();
    char** value = new char*[1];
    void* val = (char*) value;
    void **vv = &val;
    for (int j = 0; j < 1000000; ++j) {
        db_get(database, key8, 1, vv, NULL);

    }
    cout << (char*)*vv;
    return 0;*/
}