#include "mydb.h"
#include <stdio.h>

using namespace std;

struct BTree;
struct NodeFileWrapper;
NodeFileWrapper* node;

struct FileWorker {
    static DataBase* db;
    static long get_free_chunk_pos();
    static NodeFileWrapper * read_chunk(long offset);
    static void write_chunk(NodeFileWrapper* node);
};

DataBase* FileWorker::db = NULL;

struct DataBase {

    struct MetaData {
        int t;
        int chunk_size;
        int key_size;
        int value_size;
        long db_size;
        long root_offset;
    };

public:
    FILE* db_file;
    BTree* tree;
    MetaData* md;
    long begin;
    long end;

    void create(char* filename, struct DBC config);
    int insert(char* key, char* value);
    int get(char* key, char **value);
    void print();
    void print_file() {
        bool byte;

        fseek(db_file, 0, SEEK_SET);

    }
};





struct DB *dbcreate(char *file, struct DBC conf) {
    cout << "In dbcreate" << endl;
    cout << "path: " << file << endl;
    struct DB *database = (struct DB *) malloc(sizeof(struct DB));
    cout << "Malloced DB" << endl;
    database->dbworker = (struct DataBase *) malloc(sizeof(struct DataBase));
    cout << "Malloced DBworker" << endl;
    database->dbworker->create(file, conf);
    cout<< "Created file, conf DBWorker" << endl;
    return database;
} ;

int db_close(struct DB *db) {
    return 0;
}
int db_del(struct DB *db, void *key, size_t key_len) {
    return 0;
}
int db_get(struct DB *db, void *key, size_t key_len,
        void **val, size_t *val_len) {
    char **v = (char **) val;
    char *k = (char *) key;
    *val_len = 20;
    return db->dbworker->get(k, v);
}
int db_put(struct DB *db, void *key, size_t key_len,
        void *val, size_t val_len) {
    char *k = (char *) key;
    char *v = (char *) val;
    db->dbworker->insert(k, v);
    //db->dbworker->print();
    return  1;
}
