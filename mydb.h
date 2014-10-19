#include <stddef.h>
#include <stdlib.h>

/* check `man dbopen` */
struct DBT {
    void *data;
    size_t size;
};
struct DB {
/* Public API */
/* Returns 0 on OK, -1 on Error */
    int (*close)(struct DB *db);
    int (*del)(struct DB *db, struct DBT *key);
/* * * * * * * * * * * * * *
* Returns malloc'ed data into 'struct DBT *data'.
* Caller must free data->data. 'struct DBT *data' must be alloced in
* caller.
* * * * * * * * * * * * * */
    int (*get)(struct DB *db, struct DBT *key, struct DBT *data);
    int (*put)(struct DB *db, struct DBT *key, struct DBT *data);
/* For future uses - sync cached pages with disk
* int (*sync)(const struct DB *db)
* */
/* Private API */
/* ... */
struct DataBase* dbworker;
}; /* Need for supporting multiple backends (HASH/BTREE) */
struct DBC {
/* Maximum on-disk file size
* 512MB by default
* */
    size_t db_size;

    //t - tree factor
    int t_factor;

    /* Maximum chunk (node/data chunk) size
* 4KB by default
* */

     size_t chunk_size;
/* For future uses - maximum cached memory size
* 16MB by default
* size_t mem_size; */
};

/* For future uses - sync cached pages with disk
* int db_sync(const struct DB *db);
* */
extern "C" {

struct DB *dbcreate(char *file, struct DBC conf);

struct DB *dbopen(char *file, struct DBC conf);
int db_close(struct DB *db);
int db_del(struct DB *, void *, size_t);
int db_get(struct DB *, void *, size_t, void **, size_t *);
int db_put(struct DB *, void *, size_t, void *, size_t);
}
