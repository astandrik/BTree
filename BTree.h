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
        if(key != keys[i]) {
            children[i]->del(key);
        }
        if(leaf && key == keys[i]) {
            array_remove_at(keys, i, size);
            array_remove_at(children, i, size+1);
            size--;
        }
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
        children[i+1]-> keys = array_slice(children[i]->keys
                , median_index+1,
                children[i]->size,
                children[i]->size);
        children[i+1]-> size = children[i]->size - median_index - 1;

        children[i]->keys = array_slice(children[i]->keys,
                0, median_index,
                children[i]->size);
        children[i]->size = median_index;

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
