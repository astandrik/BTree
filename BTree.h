#include <iostream>
#include <string.h>
#include <stdio.h>
#include "helpers.h"
#include "DataBase.h"
#include <queue>
#include <unistd.h>

static int factor_t;
static char* result = new char[20];

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
        array_insert_string(keys, index, size+1, key, FileWorker::db->md->key_size);
        array_insert_string(values, index, size+1, value, FileWorker::db->md->key_size);
        size++;
        FileWorker::write_chunk(this);
    }

    NodeFileWrapper* get_child(int index) {
        if(index >= 0 && index <= size) {
            NodeFileWrapper *to_return = FileWorker::read_chunk(children[index]);
            return to_return;
        } else {
            return NULL;
        }

    }

    void free()
    {
        for (int i = 0; i < 2*FileWorker::db->md->t - 1; ++i) {
            delete [] keys[i];
            delete [] values[i];
        }
        delete[] keys;
        delete[] values;
        delete[] children;
        delete this;
    }

    char *get(char* key) {
        int i = 0;
        for (; i < size && strncmp(key, keys[i], FileWorker::db->md->key_size) > 0; ++i) {}
		
	if(i != size && strncmp(key, keys[i], FileWorker::db->md->key_size) == 0)
        {
            return values[i];
        }
        else {
            if (leaf) {
                return values[i];
            }
            else {

                NodeFileWrapper *child = get_child(i);
                strncpy(result,  child->get(key), FileWorker::db->md->value_size);
                child->free();
                return result;
            }
        }
    }




    char* get_biggest() {
        while(!leaf) {
            NodeFileWrapper* child = get_child(size);
            strncpy(result, child->get_biggest(), FileWorker::db->md->key_size);
            child->free();
            return result;
        }
        return keys[size-1];
    }

    char* get_smallest() {
        while(!leaf) {
            NodeFileWrapper* child = get_child(0);
            strncpy(result, child->get_smallest(), FileWorker::db->md->key_size);
            child->free();
            return result;
        }
        return keys[0];
    }

    void del(char* key);





    void insert(char *key, char* value) {
        int i = 0;
        for (; i < size && strncmp(key, keys[i], FileWorker::db->md->key_size) > 0; ++i) { }
	
	if(i != size && strncmp(key, keys[i], FileWorker::db->md->key_size) == 0) {
		cout << " Key: " << key << " oldvalue: " << values[i] << " newvalue: " << value << endl;
		strncpy(values[i], value, FileWorker::db->md->value_size);
		FileWorker::write_chunk(this);
		return;
	}
		

        if (leaf) {
            if(i == size || strncmp(keys[i], key, FileWorker::db->md->key_size) != 0) {
                insert_key(i, key, value);
            }
        } else {
            NodeFileWrapper* child_to_insert = get_child(i);
            if(child_to_insert->size == 2* factor_t - 1) {
                Split(i);
                insert(key, value);
                child_to_insert->free();
                return;
            }
            child_to_insert->insert(key, value);
            child_to_insert->free();
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
                cout << "Printing child " << i << endl;
                if(child->offset != 0) {
                    child->print();
                }
                child->free();
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
        array_slice_string(left->keys, right->keys, median_index + 1, left->size, FileWorker::db->md->key_size);
        array_slice_string(left->values, right->values, median_index + 1, left->size, FileWorker::db->md->value_size);
        right-> size = left->size - median_index - 1;
        right->leaf = left->leaf;

        array_slice_string(left->keys, left->keys, 0, median_index,  FileWorker::db->md->key_size);
        array_slice_string(left->values, left->values,0, median_index,  FileWorker::db->md->value_size);
        left->size = median_index;

        int k = 0;
        for (int j = median_index + 1; j < tmp_size; ++j) {
            right->children[k] = left->children[j];
            k++;
        }
        FileWorker::write_chunk(left);
        FileWorker::write_chunk(right);
        FileWorker::write_chunk(this);
        left->free();
        right->free();
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
    fwrite(&node->leaf, sizeof(bool), 1, db->db_file);
    if(!node->leaf) {
        fwrite(node->children, sizeof(long), node->size + 1, db->db_file);
    }
    fflush(db->db_file);
}

NodeFileWrapper  *FileWorker::read_chunk(long _offset) {
    fseek(db->db_file, _offset, SEEK_SET);
    int size = 0;
    char** keys;
    char** values;
    long* children;
    bool isleaf = true;
    long offset = 0;
    int keyvalue_size = 2*db->md->t - 1;
    fread(&offset, sizeof(long), 1, db->db_file);
    fread(&size, sizeof(int), 1, db->db_file);
    keys = new char*[keyvalue_size];
    values = new char*[keyvalue_size];
    children = new long[keyvalue_size + 1];
    bzero(keys, keyvalue_size);
    bzero(values, keyvalue_size);
    bzero(children, keyvalue_size + 1);
    for (int i = 0; i < keyvalue_size; ++i) {
        keys[i] = new char[db->md->key_size];
        values[i] = new char[db->md->value_size+1];
        bzero(keys[i], db->md->key_size);
        bzero(values[i], db->md->value_size);
    }
    for (int j = 0; j < size; ++j) {
        fread(keys[j], sizeof(char), db->md->key_size, db->db_file);
        fread(values[j], sizeof(char), db->md->value_size, db->db_file);
    }
    fread(&isleaf, sizeof(bool), 1, db->db_file);
    if(!isleaf) {
        fread(children, sizeof(long), size + 1, db->db_file);
    }


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
//            cout << "Got new pos: " << init_pos << endl;
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
    int t_2 = 2*db->md->t;
    children = new long [t_2];
    bzero(children, t_2);
    keys = new char*[t_2 - 1];
    bzero(keys, t_2 - 1);
    values = new char*[t_2- 1];
    bzero(values, t_2 - 1);
    for (int i = 0; i < t_2 - 1; ++i) {
        keys[i] = new char[db->md->key_size];
        values[i] = new char[db->md->value_size];
        bzero(keys[i], db->md->key_size);
        bzero(values[i], db->md->value_size);
    }
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
        factor_t = t;
        root = NULL;
        height = 0;
        this->db = FileWorker::db;
    }

    void print() {
        cout << "root:" << endl;
        root = FileWorker::read_chunk(db->root_offset);
        root->print();
        root->free();
    }

    char* get(char * key){
        root = FileWorker::read_chunk(db->root_offset);
        return root->get(key);
    }

    void del(char* key) {
        root = FileWorker::read_chunk(db->root_offset);
        root->del(key);
        if(root->size == 0) {
            NodeFileWrapper* child = root->get_child(0);
            db->root_offset = child->offset;
            child->free();
        }
        root->free();
    }

    void insert(char* key, char* value) {
        //Если корневого узла не было, создаем
        if (db->root_offset == -1) {
            NodeFileWrapper * new_node = new NodeFileWrapper();
            root = new_node;
            root->offset = db->begin;
            FileWorker::write_chunk(root);
            db->root_offset = root->offset;
            root->free();
            insert(key, value);
            return;
        }
        root = FileWorker::read_chunk(db->root_offset);
        //Если корень переполнен, увеличиваем высоту дерева
        if (root->size == 2*factor_t - 1) {
          //  cout << "Increasing tree height " << endl;
            NodeFileWrapper *new_node = new NodeFileWrapper();
            height++;
            new_node->leaf = false;
            new_node->children[0] = root->offset;
            new_node->size = 0;
            root->free();
            root = new_node;
            db->root_offset = root->offset;
            NodeFileWrapper* root_child = new_node->get_child(0);
            FileWorker::write_chunk(root);
            root->free();
            root_child->free();
            insert(key, value);
            return;
        }
        else {
            int i = 0;
            //cout << "Root size " << root->size << " is ok" << endl;
            for (; i < root->size && strncmp(key, root->keys[i], db->md->key_size) > 0; ++i) {}

            if (root->leaf) {
             //   cout << "Root is leaf, insert here" << endl;
                if(i == root->size || strncmp(root->keys[i], key, db->md->key_size) != 0) {
                    root->insert_key(i, key, value);
                }
                root->free();
            }
            else {
             //   cout << "Root is not leaf, insert into child number: " << i<< endl;
                NodeFileWrapper* child_to_insert = root->get_child(i);
                if(child_to_insert->size == 2* factor_t - 1) {
                 //   cout << "Splitting root" << endl;
                    root->Split(i);
                    root->free();
                    insert(key, value);
                    child_to_insert->free();
                    return;;
                }
               // cout << "Catching errors here nvm" << endl;
                child_to_insert->insert(key, value);
                child_to_insert->free();
                root->free();
            }
        }
    }
};

void DataBase::print() {
    tree->print();
}
int DataBase::insert(char* key, char* value) {
  //  cout << "In Insert function" << endl;
    try {
        tree->insert(key, value);
    } catch(...) {
        return -1;
    }
   // cout << "Quitting Insert function" << endl;
    return 0;
}

int DataBase::del(char *key) {
   // cout << "In Delete function" << endl;
    try {
        tree->del(key);
    } catch(...) {
        return -1;
    }
}

int DataBase::get(char *key, char **value) {
    try {
        char *val =(tree->get(key));
	int i = 0;
        *value = new char[20];	
        strcpy(*value,val);
        tree->root->free();
    } catch(...) {
        return -1;
    }
    return 0;
}

void DataBase::create(char* filename, struct DBC config) {
    FileWorker::db = this;
    md = new MetaData();
    md->chunk_size = 500;
    md->t = 3;
    md->db_size = 50000000;
    md->key_size = 20;
    md->value_size = 20;
    db_file = fopen(filename, "wb+");
    fwrite(md, sizeof(MetaData), 1, db_file);
    begin = ftell(db_file);
    bool* destroyer = new bool[md->db_size];
    bzero(destroyer, md->db_size);
    fwrite(destroyer, sizeof(bool), md->db_size, db_file);
    delete [] destroyer;
    end = ftell(db_file);
    tree = new BTree(md->t);
    root_offset= -1;
    fflush(db_file);
}

void NodeFileWrapper::del(char *key)  {
    int i = 0;

    for (; i < size && strncmp(key, keys[i], FileWorker::db->md->key_size) > 0; ++i) {}
   // FileWorker::db->tree->root->print();
    char* compared_key = i == size ? keys[i-1] : keys[i];
    if(!leaf) {
        NodeFileWrapper *current_child = get_child(i);
        NodeFileWrapper *right_child = get_child(i+1);
        NodeFileWrapper *left_child = get_child(i-1);
        int left_ch_size = (!leaf && i > 0) ? left_child->size : -1;
        int right_ch_size = !leaf && (i < size) ? right_child->size : -1;
        int curr_ch_size = !leaf ? current_child->size : 0 ;

        if (strncmp(key, compared_key, FileWorker::db->md->key_size) == 0) {
            if(curr_ch_size>= factor_t) {
                char* biggest_key = current_child->get_biggest();
                char* biggest_value = get(biggest_key);
                array_remove_at_string(keys, i, size,FileWorker::db->md->key_size);
                array_remove_at_string(values, i, size,FileWorker::db->md->value_size);
                array_insert_string(keys, i, size, biggest_key, FileWorker::db->md->key_size);
                array_insert_string(values, i, size, biggest_value, FileWorker::db->md->key_size);
                FileWorker::write_chunk(this);
                current_child->del(biggest_key);
                return;
            } else if (right_ch_size >= factor_t) {
                char* smallest_key = right_child->get_smallest();
                char* smallest_value = get(smallest_key);
                array_remove_at_string(keys, i, size,FileWorker::db->md->key_size);
                array_remove_at_string(values, i, size, FileWorker::db->md->value_size);
                array_insert_string(keys, i, size, smallest_key, FileWorker::db->md->key_size);
                array_insert_string(values, i, size, smallest_value, FileWorker::db->md->key_size);
                FileWorker::write_chunk(this);
                right_child->del(smallest_key);
                return;
            } else if (curr_ch_size == right_ch_size && right_ch_size == factor_t - 1) {
                array_insert_string(current_child->keys, current_child->size, current_child->size, compared_key, FileWorker::db->md->key_size);
                array_insert_string(current_child->values, current_child->size, current_child->size, get(compared_key), FileWorker::db->md->key_size);
                current_child->size++;
                int j = 0;
                for (; j < right_child->size; ++j) {
                    array_insert_string(current_child->keys, current_child->size, current_child->size, right_child->keys[j], FileWorker::db->md->key_size);
                    array_insert_string(current_child->values, current_child->size, current_child->size, right_child->values[j], FileWorker::db->md->value_size);
                    array_insert_at(current_child->children, current_child->size, current_child->size + 1, right_child->children[j]);
                    current_child->size++;
                }
                if(!right_child->leaf) {
                    NodeFileWrapper *child = right_child->get_child(j);
                    array_insert_at(current_child->children, current_child->size, current_child->size, child->offset);
                    child->free();
                }
                FileWorker::write_chunk(current_child);
                array_remove_at_string(keys, i, size, FileWorker::db->md->key_size);
                array_remove_at_string(values, i, size, FileWorker::db->md->value_size);
                array_remove_at(children, i+1, size + 1);
                size--;
                FileWorker::write_chunk(this);
                current_child->del(key);
                return;
            }
        }
        if(strncmp(key, compared_key, FileWorker::db->md->key_size) != 0) {
            if(current_child->size == factor_t - 1) {
                if (left_ch_size > factor_t - 1) {

                    NodeFileWrapper *child_to_insert;
                    if(!left_child->leaf) {
                        child_to_insert = left_child->get_child(left_child->size);
                    }
                    //Вставка в текущего ребенка
                    array_insert_string(current_child->keys, 0, 2*factor_t - 1, keys[i-1], FileWorker::db->md->key_size);
                    array_insert_string(current_child->values, 0, 2*factor_t - 1, values[i-1], FileWorker::db->md->key_size);
                    if(!left_child->leaf) {
                        array_insert_at(current_child->children, 0, 2 * factor_t, child_to_insert->offset);
                    }
                    current_child->size++;
                    //Удаление\вставка из узла
                    array_remove_at_string(keys, i-1, size,FileWorker::db->md->key_size);
                    array_remove_at_string(values, i-1, size,FileWorker::db->md->value_size);
                    array_insert_string(keys, i-1, size, left_child->keys[left_child->size-1], FileWorker::db->md->key_size);
                    array_insert_string(values, i-1, size, left_child->values[left_child->size-1], FileWorker::db->md->key_size);
                    //Удаление из левого ребенка
                    array_remove_at_string(left_child->keys, left_child->size - 1, left_child->size, FileWorker::db->md->key_size);
                    array_remove_at_string(left_child->values, left_child->size - 1, left_child->size, FileWorker::db->md->key_size);
                    array_remove_at(left_child->children, left_child->size, left_child->size);
                    left_child->size--;
                    FileWorker::write_chunk(left_child);
                    FileWorker::write_chunk(current_child);
                    FileWorker::write_chunk(this);
                    del(key);
                    return;
                } else if (right_ch_size > factor_t - 1) {
                    NodeFileWrapper *child_to_insert;
                    if(!right_child->leaf) {
                        child_to_insert = right_child->get_child(0);
                    }
                    //Вставка в текущего ребенка
                    array_insert_string(current_child->keys, current_child->size, 2*factor_t - 1, compared_key, FileWorker::db->md->key_size);
                    array_insert_string(current_child->values, current_child->size, 2*factor_t - 1, values[i], FileWorker::db->md->value_size);

                    if(!right_child->leaf) {
                        array_insert_at(current_child->children, current_child->size + 1, 2 * factor_t, child_to_insert->offset);
                        child_to_insert->free();
                    }
                    current_child->size++;
                    //Удаление\вставка из узла
                    array_remove_at_string(keys, i, size, FileWorker::db->md->key_size);
                    array_remove_at_string(values, i, size, FileWorker::db->md->value_size);
                    array_insert_string(keys, i, size, right_child->keys[0], FileWorker::db->md->key_size);
                    array_insert_string(values, i, size, right_child->values[0], FileWorker::db->md->value_size);
                    //Удаление из правого ребенка
                    array_remove_at_string(right_child->keys, 0, right_child->size, FileWorker::db->md->key_size);
                    array_remove_at_string(right_child->values, 0, right_child->size, FileWorker::db->md->value_size);
                    array_remove_at(right_child->children, 0, right_child->size);
                    right_child->size--;
                    FileWorker::write_chunk(right_child);
                    FileWorker::write_chunk(current_child);
                    FileWorker::write_chunk(this);
                    del(key);
                    return;
                } else if (left_ch_size == current_child->size && current_child->size == (factor_t - 1)) {
                    NodeFileWrapper *child_to_insert;
                    array_insert_string(current_child->keys, 0, 2*factor_t - 1, keys[i-1], FileWorker::db->md->key_size);
                    array_insert_string(current_child->values, 0, 2*factor_t - 1, values[i-1], FileWorker::db->md->value_size);
                    current_child->size++;
                    array_remove_at_string(keys, i-1, size, FileWorker::db->md->key_size);
                    array_remove_at_string(values, i-1, size, FileWorker::db->md->value_size);
                    array_remove_at(children, i-1, size+1);
                    size--;
                    for (int j = 0; j < left_child->size ; ++j) {
                        array_insert_string(current_child->keys, j, 2*factor_t - 1, left_child->keys[j], FileWorker::db->md->key_size);
                        array_insert_string(current_child->values, j, 2*factor_t - 1, left_child->values[j], FileWorker::db->md->value_size);
                        if(!left_child->leaf) {
                            child_to_insert = left_child->get_child(j);
                            array_insert_at(current_child->children, j, 2 * factor_t, child_to_insert->offset);
                            child_to_insert->free();
                        }
                        current_child->size++;
                    }
                    if(!left_child->leaf) {
                        child_to_insert = left_child->get_child(left_child->size);
                        array_insert_at(current_child->children, left_child->size, 2 * factor_t, child_to_insert->offset);
                        child_to_insert->free();
                    }
                    FileWorker::write_chunk(current_child);
                    FileWorker::write_chunk(this);
                } else if (right_ch_size == current_child->size && current_child->size == (factor_t - 1)) {
                    NodeFileWrapper *child_to_insert;
                    array_insert_string(current_child->keys, current_child->size, 2*factor_t - 1, compared_key, FileWorker::db->md->key_size);
                    array_insert_string(current_child->values, current_child->size, 2*factor_t - 1, values[i], FileWorker::db->md->value_size);
                    current_child->size++;
                    array_remove_at_string(keys, i, size, FileWorker::db->md->key_size);
                    array_remove_at_string(values, i, size, FileWorker::db->md->value_size);
                    array_remove_at(children, i+1, size+1);
                    size--;
                    int index = current_child->size;
                    for (int j = 0; j < right_child->size ; ++j) {
                        array_insert_string(current_child->keys, index + j, 2*factor_t - 1, right_child->keys[j], FileWorker::db->md->key_size);
                        array_insert_string(current_child->values, index + j, 2*factor_t - 1, right_child->values[j], FileWorker::db->md->value_size);
                        if(!right_child->leaf) {
                            child_to_insert = right_child->get_child(j);
                            array_insert_at(current_child->children, index + j, 2 * factor_t, child_to_insert->offset);
                            child_to_insert->free();
                        }
                        current_child->size++;
                    }
                    if(!right_child->leaf) {
                        child_to_insert = right_child->get_child(right_child->size);
                        array_insert_at(current_child->children, index + right_child->size, 2 * factor_t, child_to_insert->offset);
                        child_to_insert->free();
                    }
                    FileWorker::write_chunk(current_child);
                    FileWorker::write_chunk(this);
                }
            }
        }
        current_child->del(key);
    }
    if(leaf && strncmp(key,compared_key, FileWorker::db->md->key_size) == 0) {
        array_remove_at_string(keys, i, size,FileWorker::db->md->key_size);
        array_remove_at_string(values, i, size, FileWorker::db->md->value_size);
        size--;
        FileWorker::write_chunk(this);
        return;
    }
}
