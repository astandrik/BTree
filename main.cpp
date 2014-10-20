#include <iostream>
#include "BTree.h"
#include <string>
#include <vector>
#include <set>
using namespace std;

/*

vector<char*> test_data;
vector<char*> inserted;
static const char alphanum[] =
        "0123456789"
                "!@#$%^&*"
                "ABCDEFGHIJKLMNOPQRSTUVWXYZ"
                "abcdefghijklmnopqrstuvwxyz";

int stringLength = 20;

char* genRandom()
{
    char* new_str = new char[stringLength];
    for (int i = 0; i < stringLength; ++i) {
        new_str[i] = alphanum[rand() % stringLength];
    }
    return new_str;
}

void generate_test(int size) {
    for (int i = 0; i < size; ++i) {
        test_data.push_back(genRandom());
    }
}

int main() {
   DBC config;
    config.chunk_size = 300;
    config.db_size = 90000;
    config.t_factor = 3;

    generate_test(100);


    string path = "/home/anton/MyDataBase.dat";
    DB* database = dbcreate(const_cast<char*>(path.c_str()), config);
    //database->dbworker->print_file();
    char** value = new char*[1];
    void* val = (char*) value;
    void **vv = &val;


    for(int i = 0; i < 20; i ++) {
        char* val =test_data[rand()% test_data.size()];
       db_put(database, val, 1, val, 1);
        inserted.push_back(val);
        db_get(database, inserted[rand()% inserted.size()], 1, vv, NULL);
        db_get(database, inserted[rand()% inserted.size()], 1, vv, NULL);
        db_get(database, inserted[rand()% inserted.size()], 1, vv, NULL);

    }

    for (int j = 0; j < test_data.size(); ++j) {
        delete [] test_data[j];
    }

   database->dbworker->print();





    fclose(database->dbworker->db_file);
    delete database->dbworker->md;
    delete database->dbworker->tree;
    free(database->dbworker);
    free(database);
    delete [] result;
    delete [] value;
    return 0;
} */