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

    generate_test(10000);


    string path = "/home/anton/MyDataBase.dat";
    DB* database = dbcreate(const_cast<char*>(path.c_str()), config);
    //database->dbworker->print_file();
    char** value = new char*[1];
    void* val = (char*) value;
    void **vv = &val;
    int keysnumber = 10;
    srand(56);
    char** keys = new char* [keysnumber];
   // for (int i = 0; i < keysnumber; ++i) {
    //    keys[i] = new char[20];
     //   string val = "HelloDolly";
     //   val += '0' + i;
      //  strcpy(keys[i], val.c_str());
   // }

    //for (int j = 0; j < keysnumber; ++j) {
     //   db_put(database,keys[j], 1, keys[j], 1);
    //}
    for(int i = 0; i < 10000; i ++) {
        char* val =test_data[rand()% test_data.size()];
        if(!vector_contains(inserted, val)) {
            db_put(database, val, 1, val, 1);
            inserted.push_back(val);
        }

    }
    database->dbworker->print();
    while(inserted.size() > 1) {
        int to_delete = rand() % inserted.size();
       // cout << "Deleting: " << inserted[to_delete] << endl;
        if(inserted[to_delete][2] == '0'){
            int a = 1;
            a += 1;
        }
     //   database->dbworker->print();
        db_del(database, inserted[to_delete], 1);
        vector_remove_at(inserted, to_delete);
    }
    database->dbworker->print();
    for (int j = 0; j < test_data.size(); ++j) {
        delete [] test_data[j];
    }

   //database->dbworker->print();
   // string to_del = "HelloDolly4";
  //  char* dest = const_cast<char*>(to_del.c_str());
//    db_del(database, dest, 1);
  //  db_del(database, key2, 1);

    database->dbworker->print();


    fclose(database->dbworker->db_file);
    delete database->dbworker->md;
    delete database->dbworker->tree;
    free(database->dbworker);
    free(database);
    delete [] result;
    delete [] value;
    return 0;
}
*/