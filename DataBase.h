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
    struct DB* database =  (struct DB*)malloc(sizeof(struct DB));
    database->dbworker = (struct DataBase*)malloc(sizeof(struct DataBase));
    database->dbworker->create(file, conf);
    return database;
};


struct DB *dbopen (char *file, struct DBC conf);
int db_close(struct DB *db);
int db_del(struct DB *, void *, size_t);
int db_get(struct DB *, void *, size_t, void **, size_t *);
int db_put(struct DB *, void *, size_t, void * , size_t );



int db_close(struct DB *db) {
    return 0;
}
int db_del(struct DB *db, void *key, size_t key_len) {
    struct DBT keyt = {
            .data = key,
            .size = key_len
    };
    return db->del(db, &keyt);
}
int db_get(struct DB *db, void *key, size_t key_len,
        void **val, size_t *val_len) {
    char** v = (char**) val;
    char* k = (char*) key;
    return db->dbworker->get(k, v);
}
int db_put(struct DB *db, void *key, size_t key_len,
        void *val, size_t val_len) {
    char* k = (char*) key;
    char * v = (char*) val;
    return db->dbworker->insert(k, v);
}

