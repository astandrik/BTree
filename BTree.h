#include <iostream>
#include "helpers.h"


static int factor_t;

struct Node {
    int * keys;
    int size;
    Node **children;
    bool leaf;

    Node() {
        keys = new int[2*factor_t - 1];
        children = new Node*[2*factor_t];
        for (int i = 0; i < 2*factor_t; ++i) {
            children[i] = NULL;
        }
        size = 0;
        leaf = true;
    }

    void del(int key) {
        int i = 0;
        for (; i < size && key > keys[i]; ++i) {}
        if(!leaf && key != keys[i]) {
            if(children[i]->size == factor_t - 1) {
                int left_ch_size = i > 0 ? children[i-1]->size : -1;
                int right_ch_size = i < size + 1 ? children[i+1]->size : -1;
                Node *current_child = children[i];
                Node *right_child = children[i+1];
                Node *left_child = children[i-1];
                if (left_ch_size > factor_t - 1) {
                    Node *child_to_insert = children[i-1]->children[children[i-1]->size - 1];
                    //Вставка в текущего ребенка
                    array_insert_at(current_child->keys, 0, 2*factor_t - 1, keys[i-1]);
                    array_insert_at(current_child->children, 0, 2*factor_t, child_to_insert);
                    current_child->size++;
                    //Удаление\вставка из узла
                    array_remove_at(keys, i-1, size);
                    array_insert_at(keys, i-1, size, left_child->keys[left_child->size-1]);
                    //Удаление из левого ребенка
                    array_remove_at(left_child->keys, left_child->size - 1, left_child->size);
                    array_remove_at(left_child->children, left_child->size, left_child->size);
                    left_child->size--;
                    del(key);
                    return;
                } else if (right_ch_size > factor_t - 1) {
                    Node *child_to_insert = children[i+1]->children[0];
                    //Вставка в текущего ребенка
                    array_insert_at(current_child->keys, current_child->size, 2*factor_t - 1, keys[i]);
                    array_insert_at(current_child->children, current_child->size + 1, 2*factor_t, child_to_insert);
                    current_child->size++;
                    //Удаление\вставка из узла
                    array_remove_at(keys, i, size);
                    array_insert_at(keys, i, size, right_child->keys[0]);
                    //Удаление из правого ребенка
                    array_remove_at(right_child->keys, 0, right_child->size);
                    array_remove_at(right_child->children, 0, right_child->size);
                    right_child->size--;
                    del(key);
                    return;
                } else if (left_ch_size == current_child->size && current_child->size == (factor_t - 1)) {
                    Node *child_to_insert;
                    array_insert_at(current_child->keys, 0, 2*factor_t - 1, keys[i-1]);
                    current_child->size++;
                    array_remove_at(keys, i-1, size);
                    array_remove_at(children, i-1, size+1);
                    size--;
                    for (int j = 0; j < left_child->size ; ++j) {
                        child_to_insert = left_child->children[j];
                        array_insert_at(current_child->keys, j, 2*factor_t - 1, left_child->keys[j]);
                        array_insert_at(current_child->children, j, 2*factor_t, child_to_insert);
                        current_child->size++;
                    }
                    array_insert_at(current_child->children, left_child->size, 2*factor_t, child_to_insert);
                    i--;
                } else if (right_ch_size == current_child->size && current_child->size == (factor_t - 1)) {
                    Node *child_to_insert;
                    array_insert_at(current_child->keys, current_child->size, 2*factor_t - 1, keys[i]);
                    current_child->size++;
                    array_remove_at(keys, i, size);
                    array_remove_at(children, i+1, size+1);
                    size--;
                    int index = current_child->size;
                    for (int j = 0; j < right_child->size ; ++j) {
                        child_to_insert = right_child->children[j];
                        array_insert_at(current_child->keys, index + j, 2*factor_t - 1, right_child->keys[j]);
                        array_insert_at(current_child->children, index + j, 2*factor_t, child_to_insert);
                        current_child->size++;
                    }
                    array_insert_at(current_child->children, index+ right_child->size, 2*factor_t, child_to_insert);
                }
            }
        }
        if(leaf && key == keys[i]) {
            array_remove_at(keys, i, size);
            array_remove_at(children, i, size+1);
            size--;
            return;
        }
        children[i]->del(key);
    }

    void insert(int key) {
        int i = 0;
        for (; i < size && key > keys[i]; ++i) {}

        if (leaf) {
            array_insert_at(keys, i, size + 1, key);
            size++;
        } else {
            if(children[i]->size == 2* factor_t - 1) {
                Split(i);
                insert(key);
                return;
            }
            children[i]->insert(key);
        }

    }

    void Split(int index) {
        int i = index;
        int median_index = children[i]->size / 2;
        array_insert_at(keys, i, size, children[i]->keys[median_index]);
        size++;
        //Теперь i - индекс нового элемента;
        int tmp_size = children[i]->size + 1;
        for (int m = size; m > i + 2; --m) {
            children[m] = children[m - 1];
        }
        if(children[i+1] == NULL) {
            children[i+1] = new Node();
        }
        int* to_delete = children[i]->keys;
        children[i+1]-> keys = array_slice(children[i]->keys
                , median_index+1,
                children[i]->size,
                2*factor_t - 1);
        children[i+1]-> size = children[i]->size - median_index - 1;

        children[i]->keys = array_slice(children[i]->keys,
                0, median_index,
                2*factor_t - 1);
        children[i]->size = median_index;
        delete [] to_delete;
        int k = 0;
        for (int j = median_index + 1; j < tmp_size; ++j) {
            children[i + 1]->children[k] = children[i]->children[j];
            k++;
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
                if(children[i] != NULL) {
                    children[i]->print();
                }
            }
            cout << "Children end." << endl;
        }

    }

};

struct BTree {
    Node* root;
    int height;
    BTree(int t) {
        factor_t = t;
        root = NULL;
        height = 0;
    }

    void print() {
        cout << "root:" << endl;
        root->print();
    }

    void del(int key) {
        root->del(key);
    }

    void insert(int key) {
        //Если корневого узла не было, создаем
        if (root == NULL) {
            Node * new_node = new Node();
            root = new_node;
            insert(key);
            return;
        }
        //Если корень переполнен, увеличиваем высоту дерева
        if (root->size == 2*factor_t - 1) {
            Node *new_node = new Node();
            height++;
            new_node->leaf = false;
            new_node->size = 0;
            new_node->children[0] = root;
            new_node->children[0]->size = root->size;
            root = new_node;
            for (int i = 1 ; i < 2*factor_t; ++i) {
                root->children[i] = new Node();
                root->children[i]->leaf =  new_node->children[0]->leaf;
            }
            insert(key);
            return;
        }
        else {
            int i = 0;
            for (; i < root->size && key > root->keys[i]; ++i) {}

            if (root->leaf) {
                array_insert_at(root->keys, i, root->size + 1, key);
                root->size++;

            }
            else {
                if(root->children[i]->size == 2* factor_t - 1) {
                    root->Split(i);
                    insert(key);
                    return;;
                }
                root->children[i]->insert(key);
            }
        }
    }

};
