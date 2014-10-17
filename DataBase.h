#include "mydb.h"

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
    void insert(int key);
    void print();
    void print_file() {
        bool byte;

        fseek(db_file, 0, SEEK_SET);
        while(fread(&byte, sizeof(bool), 1, db_file)) {
            cout << byte << " ";
        }
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
    db->close(db);
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
    struct DBT keyt = {
            .data = key,
            .size = key_len
    };
    struct DBT valt = {0, 0};
    int rc = db->get(db, &keyt, &valt);
    *val = valt.data;
    *val_len = valt.size;
    return rc;
}
int db_put(struct DB *db, void *key, size_t key_len,
        void *val, size_t val_len) {
    struct DBT keyt = {
            .data = key,
            .size = key_len
    };
    struct DBT valt = {
            .data = val,
            .size = val_len
    };
    return db->put(db, &keyt, &valt);
}

