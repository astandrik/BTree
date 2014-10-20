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
    char* new_str = new char[20];
    for (int i = 0; i < 20; ++i) {
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
    char** value = new char*[1];
    void* val = (char*) value;
    void **vv = &val;


    for(int i = 0; i < 50; i ++) {
        char* val =test_data[rand()% test_data.size()];
       db_put(database, val, 1, val1, 1);
        inserted.push_back(val);
        db_get(database, inserted[rand()% inserted.size()], 1, vv, NULL);
        db_get(database, inserted[rand()% inserted.size()], 1, vv, NULL);
        db_get(database, inserted[rand()% inserted.size()], 1, vv, NULL);

    }
    db_put(database, key1, 1, val1, 1);
    for (int j = 0; j < test_data.size(); ++j) {
        delete test_data[j];
    }

  // database->dbworker->print();

   // database->dbworker->print();




    fclose(database->dbworker->db_file);
    delete database->dbworker->md;
    delete database->dbworker->tree;
    free(database->dbworker);
    free(database);

    return 0;
}*/