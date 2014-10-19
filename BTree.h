#include <iostream>
#include <string.h>
#include <stdio.h>
#include "helpers.h"
#include "DataBase.h"


static int factor_t;

struct NodeFileWrapper {
    long offset;
    int size;
    char** keys;
    char** values;
    long* children;
    bool leaf;
    NodeFileWrapper();
    NodeFileWrapper* operator[](int index) {
        return FileWorker::read_chunk(children[index]);
    }
    void insert_key(int index, char* key, char* value) {
        array_insert_at(keys, index, size+1, key);
        array_insert_at(values, index, size+1, value);
        if(size == -1) size = 0;
        size++;
        FileWorker::write_chunk(this);
    }

    NodeFileWrapper* get_child(int index) {
        return FileWorker::read_chunk(children[index]);
    }

    char *get(char* key) {
        int i = 0;
        for (; i < size && strcmp(key, keys[i]) > 0; ++i) {}
        if (leaf) {
            return values[i];
        }
        else {
           return get_child(i)->get(key);
        }
    }

    void insert(char *key, char* value) {
        int i = 0;
        for (; i < size && strcmp(key, keys[i]) > 0; ++i) {}

        if (leaf) {
            insert_key(i, key, value);
        } else {
            if(get_child(i)->size == 2* factor_t - 1) {
                Split(i);
                insert(key, value);
                return;
            }
            get_child(i)->insert(key, value);
        }

    }

    void print() {
        for (int j = 0; j < size; ++j) {
            cout << "Key :(" <<j << ") " << keys[j] << " Value: " << values[j] << "||";
        }
        cout << endl;
        if(!leaf) {
            cout << "Children begin: " << endl;
            for (int i = 0; i < size+1; ++i) {
                NodeFileWrapper* child = get_child(i);
                if(child->offset != 0) {
                    get_child(i)->print();
                }
            }
            cout << "Children end." << endl;
        }
    }


    void Split(int index) {
        int i = index;
        NodeFileWrapper* left = get_child(i);
        int median_index = left->size / 2;
        insert_key(i, left->keys[median_index], left->values[median_index]);
        //Теперь i - индекс нового элемента;
        int tmp_size = left->size + 1;
        for (int m = size; m > i + 2; --m) {
            children[m] = children[m - 1];
        }

        NodeFileWrapper* right = new NodeFileWrapper();
        children[i+1] = right->offset;
        right-> keys = array_slice(left->keys
                , median_index+1,
                left->size,
                2*factor_t - 1);
        right->values= array_slice(left->values
                , median_index+1,
                left->size,
                2*factor_t - 1);
        right-> size = left->size - median_index - 1;
        right->leaf = left->leaf;
        left->keys = array_slice(left->keys,
                0, median_index,
                2*factor_t - 1);
        left->values =  array_slice(left->values,
                0, median_index,
                2*factor_t - 1);
        left->size = median_index;
        int k = 0;
        for (int j = median_index + 1; j < tmp_size; ++j) {
            right->children[k] = left->children[j];
            k++;
        }
        FileWorker::write_chunk(left);
        FileWorker::write_chunk(right);
        FileWorker::write_chunk(this);
    }

};






void FileWorker::write_chunk(NodeFileWrapper* node) {
    fseek(db->db_file, node->offset, SEEK_SET);
    fwrite(&node->offset, sizeof(long), 1, db->db_file);
    fwrite(&node->size, sizeof(int), 1, db->db_file);
    for (int i = 0; i < node->size; ++i) {
        fwrite(node->keys[i], sizeof(char), db->md->key_size, db->db_file);
        fwrite(node->values[i], sizeof(char), db->md->value_size, db->db_file);
    }
    fwrite(node->children, sizeof(long), node->size + 1, db->db_file);
    fwrite(&node->leaf, sizeof(bool), 1, db->db_file);
    fflush(db->db_file);
}

NodeFileWrapper  *FileWorker::read_chunk(long _offset) {
    fseek(db->db_file, _offset, SEEK_SET);
    int size;
    char** keys;
    char** values;
    long* children;
    bool isleaf;
    long offset;

    fread(&offset, sizeof(long), 1, db->db_file);
    fread(&size, sizeof(int), 1, db->db_file);
    keys = new char*[size];
    values = new char*[size];
    children = new long[size + 1];
    for (int i = 0; i < size; ++i) {
        keys[i] = new char[db->md->key_size];
        fread(keys[i], sizeof(char), db->md->key_size, db->db_file);
        values[i] = new char[db->md->value_size];
        fread(values[i], sizeof(char), db->md->value_size, db->db_file);
    }
    fread(children, sizeof(long) , size + 1, db->db_file);
    fread(&isleaf, sizeof(bool), 1, db->db_file);

    NodeFileWrapper *node = new NodeFileWrapper();
    node->offset = offset;
    node->values = values;
    node->size = size;
    node->children = children;
    node->keys = keys;
    node->leaf = isleaf;
    return node;
}

long  FileWorker::get_free_chunk_pos() {
    fseek(db->db_file, db->begin, SEEK_SET);
    long init_pos = ftell(db->db_file);
    long tmp;
    int size;
    while(init_pos < db->end) {
        init_pos = ftell(db->db_file);
        fread(&tmp, sizeof(long), 1, db->db_file);
        fread(&size, sizeof(int), 1, db->db_file);
        if(size == 0) return init_pos;
        else {
            fseek(db->db_file, -sizeof(int), SEEK_CUR);
            fseek(db->db_file, -sizeof(long), SEEK_CUR);
            fseek(db->db_file, db->md->chunk_size, SEEK_CUR);
        }
    }
    return -1;
}




NodeFileWrapper::NodeFileWrapper() {
    DataBase* db = FileWorker::db;
    children = new long [2*db->md->t];
    keys = new char*[2*db->md->t - 1];
    values = new char*[2*db->md->t - 1];
    for (int i = 0; i < 2*db->md->t - 1; ++i) {
        keys[i] = new char[db->md->key_size];
        values[i] = new char[db->md->value_size];
    }
    size = 0;
    leaf = true;
    this->offset = FileWorker::get_free_chunk_pos();
}


struct BTree {
    NodeFileWrapper* root;
    DataBase* db;
    int height;
    BTree(int t) {
        node = new NodeFileWrapper();
        factor_t = t;
        root = NULL;
        height = 0;
        this->db = FileWorker::db;
    }

    void print() {
       // cout << "root:" << endl;
        root->print();
    }

    char* get(char * key){
        return root->get(key);
    }
/*    void del(int key) {
        root->del(key);
        if(root->size == 0) {
            root = root->children[0];
            height--;
        }
    }*/


    void insert(char* key, char* value) {
        //Если корневого узла не было, создаем
        if (root == NULL) {
            NodeFileWrapper * new_node = new NodeFileWrapper();
            root = new_node;
            root->offset = db->begin;
            FileWorker::write_chunk(root);
            insert(key, value);
            return;
        }
        //Если корень переполнен, увеличиваем высоту дерева
        if (root->size == 2*factor_t - 1) {
            NodeFileWrapper *new_node = new NodeFileWrapper();
            height++;
            new_node->leaf = false;
            new_node->children[0] = root->offset;
            new_node->size = 0;
            FileWorker::write_chunk(new_node);
            delete root;
            root = new_node;
            for (int i = 1 ; i < 2*factor_t; ++i) {
                NodeFileWrapper * new_child  = new NodeFileWrapper();
                new_child->size = -1;
                root->children[i] = new_child->offset;
                new_child->leaf =  new_node->get_child(0)->leaf;
                FileWorker::write_chunk(new_child);
            }
            insert(key, value);
            return;
        }
        else {
            int i = 0;
            for (; i < root->size && strcmp(key, root->keys[i]) > 0; ++i) {}

            if (root->leaf) {
                root->insert_key(i, key, value);
            }
            else {
                if(root->get_child(i)->size == 2* factor_t - 1) {
                    root->Split(i);
                    insert(key, value);

                    return;;
                }
                root->get_child(i)->insert(key, value);
            }
        }
    }

};

void DataBase::print() {
    tree->print();
}
int DataBase::insert(char* key, char* value) {
    try {
        tree->insert(key, value);
    } catch(...) {
        return -1;
    }
    return 0;
}

int DataBase::get(char *key, char **value) {
    try {
        char *val =(tree->get(key));
        value[0] = val;
    } catch(...) {
        return -1;
    }
    return 0;
}

void DataBase::create(char* filename, struct DBC config) {
    FileWorker::db = this;
    md = new MetaData();
    md->chunk_size = 300;
    md->t = 3;
    md->db_size = 9000;
    md->key_size = 20;
    md->value_size = 20;

    db_file = fopen(filename, "wb+");
    fwrite(md, sizeof(MetaData), 1, db_file);
    int free_chunks = md->db_size/md->chunk_size;
    begin = ftell(db_file);
    NodeFileWrapper* empty = new NodeFileWrapper();
    bool destroyer = 0;
    for (int i = 0; i < md->db_size; ++i) {
        fwrite(&destroyer, sizeof(bool), 1, db_file);
    }
    end = ftell(db_file);
    tree = new BTree(md->t);
    fflush(db_file);
}


