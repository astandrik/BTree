#include <iostream>
#include "helpers"
#include "DataBase.h"


static int factor_t;

struct NodeFileWrapper {
    long offset;
    int size;
    int* keys;
    long* children;
    bool leaf;
    NodeFileWrapper();
    NodeFileWrapper* operator[](int index) {
        return FileWorker::read_chunk(children[index]);
    }
    void insert_key(int index, int key) {
        array_insert_at(keys, index, size + 1, key);
        size++;
        FileWorker::write_chunk(this);
    }

    NodeFileWrapper* get_child(int index) {
        return FileWorker::read_chunk(children[index]);
    }

    void insert(int key) {
        int i = 0;
        for (; i < size && key > keys[i]; ++i) {}

        if (leaf) {
            insert_key(i, key);
        } else {
            if(get_child(i)->size == 2* factor_t - 1) {
                //Split(i);
               // insert(key);
                return;
            }
            //children[i]->insert(key);
        }

    }

    void print() {
        for (int j = 0; j < size; ++j) {
            cout << "Key :(" <<j << ") " << keys[j] << "||";
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
        int median_index = get_child(i)->size / 2;
        insert_key(i, get_child(i)->keys[median_index]);
        //Теперь i - индекс нового элемента;
        int tmp_size = get_child(i)->size + 1;
        for (int m = size; m > i + 2; --m) {
            children[m] = children[m - 1];
        }

        NodeFileWrapper* right = new NodeFileWrapper();
        NodeFileWrapper* left = get_child(i);
        children[i+1] = right->offset;
        right-> keys = array_slice(left->keys
                , median_index+1,
                left->size,
                2*factor_t - 1);
        right-> size = left->size - median_index - 1;

        left->keys = array_slice(left->keys,
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
    fwrite(node->keys, sizeof(int), node->size, db->db_file);
    fwrite(node->children, sizeof(long), node->size + 1, db->db_file);
    fwrite(&node->leaf, sizeof(bool), 1, db->db_file);
    fflush(db->db_file);
}

NodeFileWrapper  *FileWorker::read_chunk(long _offset) {
    fseek(db->db_file, _offset, SEEK_SET);
    int size;
    int* keys;
    long* children;
    bool isleaf;
    long offset;

    fread(&offset, sizeof(long), 1, db->db_file);
    fread(&size, sizeof(int), 1, db->db_file);
    keys = new int[size];
    children = new long[size + 1];
    fread(keys, sizeof(int), size, db->db_file);
    fread(children, sizeof(long) , size + 1, db->db_file);
    fread(&isleaf, sizeof(bool), 1, db->db_file);

    NodeFileWrapper *node = new NodeFileWrapper();
    node->offset = offset;
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
    keys = new int[2*db->md->t - 1];
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
        cout << "root:" << endl;
        root->print();
    }

/*    void del(int key) {
        root->del(key);
        if(root->size == 0) {
            root = root->children[0];
            height--;
        }
    }*/

    void insert(int key) {
        //Если корневого узла не было, создаем
        if (root == NULL) {
            NodeFileWrapper * new_node = new NodeFileWrapper();
            root = new_node;
            root->offset = db->begin;
            FileWorker::write_chunk(root);
            insert(key);
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
            /*for (int i = 1 ; i < 2*factor_t; ++i) {
                root->children[i] = new Node();
                root->children[i]->leaf =  new_node->children[0]->leaf;
            }*/
            insert(key);
            return;
        }
        else {
            int i = 0;
            for (; i < root->size && key > root->keys[i]; ++i) {}

            if (root->leaf) {
                root->insert_key(i, key);
            }
            else {
                if(root->get_child(i)->size == 2* factor_t - 1) {
                    root->Split(i);
                    insert(key);
                    return;;
                }
                root->get_child(i)->insert(key);
            }
        }
    }

};

void DataBase::print() {
    tree->print();
}
void DataBase::insert(int key) {
    tree->insert(key);
}

void DataBase::create(char* filename, struct DBC config) {
    FileWorker::db = this;
    md = new MetaData();
    md->chunk_size = config.chunk_size;
    md->t = config.t_factor;
    md->db_size = config.db_size;

    db_file = fopen(filename, "wb+");
    fwrite(md, sizeof(MetaData), 1, db_file);
    //fseek(db_file, 0, SEEK_SET);
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


