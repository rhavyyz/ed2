#include<iostream>
#include<vector>

using std::cout, std::pair, std::vector;


template<class T, size_t d, size_t max_size>
class HeapPriorityQueue {
    private:
        pair<int, T> data[max_size];
        int next_inclusion = 0;

        int get_child(int pos, int child_num) {
            if(child_num >= d)
                return -1;
                
            pos = pos*d + 1 + child_num;

            if (is_empty_index(pos)) return -1;

            return pos;
        }

        int get_smallest_child_of(int pos) {
            int smallest_child = -1, smallest_value;

            int child = -1, i = 0;
            while((child = get_child(pos, i++))!= -1) {
                if(smallest_child == -1 ||data[child].first  < smallest_value) {
                    smallest_child = child;
                    smallest_value = data[child].first;
                } 
            }
            return smallest_child;
        }

        int parent_of(int index) {
            if (index % d == 0) return (index - 1)/d;
            return index/d;
        }

        void swap_up(int pos) {
            if(pos == 0 ) return;
            int parent = this->parent_of(pos);
            if(this->data[pos].first < this->data[parent].first) {
                std::swap(data[pos], data[parent]);
                swap_up(parent);
            }
        }

        void swap_down(int pos) {
            int child = get_smallest_child_of(pos);

            if(child == -1)
                return;

            if(data[child].first < data[pos].first) {
                std::swap(data[pos], data[child]);
                swap_down(child);
            }
        }

        bool is_empty_index(int i) {
            return i >= next_inclusion;
        }

        void print_node(int node, int tab) {
            if (node >= next_inclusion)
                return;
            for(int i = 0; i < tab; i++) {
                cout << " ";
            }            

            if (tab) {
                cout  << "└─ ";
            }

            cout << data[node].first << std::endl;

            int child = -1, i = 0;
            while((child = get_child(node, i++))!= -1) {
                print_node(child, tab+2);
            }
        }

    public:
        bool insert(const pair<int, T> & value) {
            if (next_inclusion == max_size)
                return false;

            data[next_inclusion] = value;
            swap_up(next_inclusion++);                        
            return true;
        }

        size_t size() {
            return next_inclusion;
        }

        pair<int, T> top() {
            if (this->size() == 0) {
                throw "no element at the top";
            }

            return data[0];
        }

        void pop() {
            if (this->size() == 0) {
                throw "no element at the top";
            }

            next_inclusion--;
            std::swap(data[next_inclusion], data[0]);
            swap_down(0);
        }

        void print() {
            print_node(0, 0);

            cout << std::endl;

            for(int i = 0; i < next_inclusion; i++)
                cout << data[i].first << " ";
            cout << std::endl;
        }


};

void q1() {

}


void q2() {

}

void q3() {
    vector<pair<int, int>> items = {{18,39}, {41,17}, {8,57}, {32, 3}, {27,14}, {19,22}, {17,18}, {7,14}, {11,57}, {72,16}, {31,16}, {2,14}, {21,41},
    {12,7}, {9, 1}, {3, 51}, {41,11}, {18,6}};

    HeapPriorityQueue<int, 4, 50> queue;

    for(auto [item, priority] : items) {

        std::cout << "adicionando " << item << " , " << priority << "\n";
        queue.insert({priority, item});
        queue.print();
    }

    
    while(queue.size()) {
        cout << "\n\nremovendo " << queue.top().second << " , " << queue.top().second << std::endl;
        queue.pop();
        queue.print();
    }



}


void q4() {
}


void q5() {
}


void q6() {
}


void q7() {
}

int main() {


    HeapPriorityQueue<int, 2, 50> h;


    std::vector<int> to_add_value = {1, -1, 15, 25, 10, 3, -3, 5, 55, 15, 20};

    for(auto a : to_add_value) {

        cout << "\n\nadd " << a << std::endl;
        h.insert({a, a});
        h.print();
    }

    while(h.size()) {
        cout << "\n\nremove " << h.top().first << std::endl;
        h.pop();
        h.print();
    }



    return 0;
}