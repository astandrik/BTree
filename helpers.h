    #include <string>
    #include <vector>
    using namespace std;

    template<class T>
    void vector_remove_at(vector<T>& v, int index) {
        for (int i = index; i < v.size() - 1; ++i) {
            v[i] = v[i+1];
        }
        v.pop_back();
    };

    template<class T>
    void vector_insert_at(vector<T>& v, int index, T value) {
        v.push_back(0);
        for (int i = v.size() - 1; i > index; --i) {
            v[i] = v[i - 1];
        }
        v[index] = value;
    };

    template<class T>
    string vector_to_string(vector<T>& v) {
        string result;
        for (int i = 0; i < v.size(); ++i) {
            result += to_string(v[i]) + " ";
        }
        return result;
    };

    template<class T>
    void array_insert_at(T* array, int index, int array_size, T value_to_insert) {
        for (int i = array_size-1; i > index ; --i) {
            array[i] = array[i-1];
        }
        array[index] = value_to_insert;
    };

    void array_insert_string(char** array, int index, int array_size, char* value_to_insert, int len) {
        for (int i = array_size-1; i > index ; --i) {
            strncpy(array[i], array[i-1], len);
        }
        strncpy(array[index],value_to_insert, len);
    };

    void array_insert_at_bytes(char* chunk, long offset, long chunk_size, char* bytes_to_insert, int num_of_bytes) {
        for (int i = chunk_size; i > offset; ++i) {
            chunk[i] = chunk[i-1];
        }
        int k = 0;
        for (int j = offset; j < num_of_bytes ; ++j) {
            chunk[j] = bytes_to_insert[k++];
        }
    };



    template<class T>
    void array_remove_at(T* array, int index, int array_size) {
        for (int i = index; i < array_size; ++i) {
            array[i] = array[i+1];
        }
    };

    template<class T>
    T* array_slice(T* array, int from_index, int to_index, int array_size) {
        T* new_array = new T[array_size];
        int j = 0;
        if(from_index < to_index) {
            for (int i = from_index; i < to_index ; ++i) {
               new_array[j] = array[i];
                j++;
            }
        }
        else {
            for (int i = to_index; i < from_index ; ++i) {
                new_array[j] = array[i];
                j++;
            }
        }
        return new_array;
    };

    char** array_slice_string(char** array, char** to_array, int from_index, int to_index, int len) {
        int j = 0;
        int size = to_index - from_index;
        char** tmp = new char*[size];
        for (int k = 0; k < size; ++k) {
            tmp[k] = new char[len];
        }
        j = 0;
        for (int i = from_index; i < to_index ; ++i) {
            strncpy(tmp[j], array[i], len);
            strncpy(to_array[j], tmp[j], len);
            delete [] tmp[j];
            j++;
        }
        delete []tmp;
    }