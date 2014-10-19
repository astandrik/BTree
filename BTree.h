#include <iostream>
#include <string.h>
#include <stdio.h>
#include "helpers.h"
#include "DataBase.h"
#include <queue>

static int factor_t;

queue<NodeFileWrapper*> malloced_data;



struct NodeFileWrapper {
    long offset;
    int size;
    char** keys;
    char** values;
    long* children;
    bool leaf;
    NodeFileWrapper();
    NodeFileWrapper(long offset, int size, char** keys, char** values, long* children, bool leaf);
    NodeFileWrapper* operator[](int index) {
        return FileWorker::read_chunk(children[index]);
    }
    void insert_key(int index, char* key, char* value) {
        array_insert_at(keys, index, size+1, key);
        array_insert_at(values, index, size+1, value);
        if(size == -1) {
            size = 0;
        }
        size++;
        FileWorker::write_chunk(this);
    }

    NodeFileWrapper* get_child(int index) {
        NodeFileWrapper* to_return = FileWorker::read_chunk(children[index]);
        return to_return;
    }

    void free()
    {
        for (int i = 0; i < 2*FileWorker::db->md->t - 1; ++i) {
            delete [] keys[i];
            delete [] values[i];
        }
        delete[] keys;
        delete[] values;
        delete[]children;
    }

    char *get(char* key) {
        int i = 0;

        for (; i < size && strcmp(key, keys[i]) > 0; ++i) {
            cout << "Comparing: " << key << " > " << keys[i] << endl;
        }

        if(i != size && strcmp(key, keys[i]) == 0)
        {
            cout << "Hui" << endl;
            return values[i];
        }
        else {
            if (leaf) {
                return values[i];
            }
            else {

                NodeFileWrapper *child = get_child(i);
                char *result = new char[20];
                strcpy(result,  child->get(key));
                cout << result <<endl;
                return result;
            }
        }
    }

    void insert(char *key, char* value) {
        int i = 0;
        for (; i < size && strcmp(key, keys[i]) > 0; ++i) {
            cout << "Comparing " << key << " > " << keys[i] << endl;
        }

        if (leaf) {
            cout << "Inserting into child new key: " << key << endl;
            insert_key(i, key, value);
            cout << "Inserted" << endl;
        } else {
            NodeFileWrapper* child_to_insert = get_child(i);
            if(child_to_insert->size == 2* factor_t - 1) {
                cout << "Splitting child " << endl;
                Split(i);
                insert(key, value);
                return;
            }
            child_to_insert->insert(key, value);
        }

    }

    void print() {
        for (int j = 0; j < size; ++j) {
            cout << "Key :(" <<j << ") " << keys[j]  << "||";
        }
        cout << endl;
        if(!leaf) {
            cout << "Children begin: " << endl;
            for (int i = 0; i < size+1; ++i) {

                NodeFileWrapper* child = get_child(i);
                cout << "Printing child " << i << endl;
                if(child->offset != 0) {
                    child->print();
                }
                //child->free();
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
        for (int m = size; m > i + 1; --m) {
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
    cout << "size: " << size << endl;
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


    node->offset = offset;
    node->values = values;
    node->size = size;
    node->children = children;
    node->keys = keys;
    node->leaf = isleaf;
    NodeFileWrapper* node = new NodeFileWrapper(offset, size, keys, values, children, isleaf);
    return node;
}

long  FileWorker::get_free_chunk_pos() {
    //cout << "Getting free chunk" << endl;
    fseek(db->db_file, db->begin, SEEK_SET);
    long init_pos = ftell(db->db_file);
    long tmp;
    int size;
    //cout << "Looking for chunk from " << init_pos << " to " << db->end << endl;
    while(init_pos < db->end) {
        init_pos = ftell(db->db_file);
        fread(&tmp, sizeof(long), 1, db->db_file);
        fread(&size, sizeof(int), 1, db->db_file);
        if(size == 0) {
            cout << "Got new pos: " << init_pos << endl;
            return init_pos;
        }
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
    //cout << "malloc children" << endl;
    children = new long [2*db->md->t];
   // cout << "malloc keys" << endl;
    keys = new char*[2*db->md->t - 1];
    //cout << "malloc values" << endl;
    values = new char*[2*db->md->t - 1];
    //cout << "malloc keyvalues for size: " <<2*db->md->t - 1 << endl;
    for (int i = 0; i < 2*db->md->t - 1; ++i) {
        keys[i] = new char[db->md->key_size];
        values[i] = new char[db->md->value_size];
    }
    //cout << "all malloced" << endl;
    size = 0;
    leaf = true;
    this->offset = FileWorker::get_free_chunk_pos();
}

NodeFileWrapper::NodeFileWrapper(long offset, int size, char **keys, char **values, long *children, bool leaf) {
    this->offset = offset;
    this->size = size;
    this->keys = keys;
    this->values = values;
    this->children = children;
    this->leaf = leaf;
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
        cout << "root:" << endl;
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
        cout << "In root insert func" << endl;
        if (root == NULL) {
            cout << "Root is NULL now" << endl;
            NodeFileWrapper * new_node = new NodeFileWrapper();
            root = new_node;
            root->offset = db->begin;
            FileWorker::write_chunk(root);
            insert(key, value);
            return;
        }
        //Если корень переполнен, увеличиваем высоту дерева
        if (root->size == 2*factor_t - 1) {
            cout << "Increasing tree height " << endl;
            NodeFileWrapper *new_node = new NodeFileWrapper();
            height++;
            new_node->leaf = false;
            new_node->children[0] = root->offset;
            new_node->size = 0;
            FileWorker::write_chunk(new_node);
            //root->free();
            root = new_node;
            NodeFileWrapper* root_child = new_node->get_child(0);
            for (int i = 1 ; i < 2*factor_t; ++i) {
                NodeFileWrapper * new_child  = new NodeFileWrapper();
                new_child->size = -1;
                root->children[i] = new_child->offset;
                new_child->leaf =  root_child->leaf;
                FileWorker::write_chunk(new_child);
                //new_child->free();
            }
            insert(key, value);
            return;
        }
        else {
            int i = 0;
            cout << "Root size " << root->size << " is ok, insert into child" << endl;
            for (; i < root->size && strcmp(key, root->keys[i]) > 0; ++i) {
                cout << "Comparing " << key << " > " << root->keys[i] << endl;
            }

            if (root->leaf) {
                cout << "Root is leaf, insert here" << endl;
                root->insert_key(i, key, value);
            }
            else {
                cout << "Root is not leaf, insert into child number: " << i<< endl;
                NodeFileWrapper* child_to_insert = root->get_child(i);
                if(child_to_insert->size == 2* factor_t - 1) {
                    cout << "Splitting root" << endl;
                    root->Split(i);
                    insert(key, value);
                    return;;
                }
                child_to_insert->insert(key, value);
            }
        }
    }

};

void DataBase::print() {
    tree->print();
}
int DataBase::insert(char* key, char* value) {

    cout << "Inserting " << key << endl;
    try {
        tree->insert(key, value);
    } catch(...) {
        return -1;
    }
    return 0;
}

int DataBase::get(char *key, char **value) {
    cout << endl << "Attempting to get: " << key << endl;
    cout << "Tree : " << endl;
    tree->print();

    cout << "Tree printed" << endl;

    try {
        char *val =(tree->get(key));
        value[0] = val;
    } catch(...) {
        return -1;
    }
    return 0;
}

void DataBase::create(char* filename, struct DBC config) {
    cout << "Creating DB" << endl;
    FileWorker::db = this;
    md = new MetaData();
    md->chunk_size = 500;
    md->t = 5;
    md->db_size = 5000000;
    md->key_size = 20;
    md->value_size = 20;
    cout << "Read metadata" << endl;
    db_file = fopen(filename, "wb+");
    cout << "Opened file" << endl;
    fwrite(md, sizeof(MetaData), 1, db_file);
    cout << "Wrote Metadata" << endl;
    int free_chunks = md->db_size/md->chunk_size;
    begin = ftell(db_file);
    bool destroyer = 0;
    cout << "Nulling bytes of file: " << endl;
    for (int i = 0; i < md->db_size; ++i) {
        fwrite(&destroyer, sizeof(bool), 1, db_file);
    }
    cout << "Bytes successfully nulled. ended at: " << ftell(db_file) << endl;
    end = ftell(db_file);
    cout << "Creating tree: " << endl;
    tree = new BTree(md->t);
    cout << "Tree created." << endl;
    fflush(db_file);
}

