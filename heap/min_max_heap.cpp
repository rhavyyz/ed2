#include<bits/stdc++.h>

using namespace std;


template<class T, size_t max_size>
class MinMaxHeap {
    private:
        int max_layer_size = 1;
        int curr_layer_size = 0;
        int curr_layer = 0;
        int next_insertion = 0;
        pair<int, T> data[max_size];
        function<bool(int, int)> funcs[2] = {
            [](int a, int b) {
                return a > b;
            },
            [](int a, int b) {
                return a < b;
            }
        };

        int get_parent_of(int pos) {
            if (pos <= 0)
                return -1;
            if (pos%2) return pos/2;
            return (pos-1)/2;
        }

        int get_grand_parent_of(int pos) {
            return get_parent_of(get_parent_of(pos));
        }

        inline bool is_valid_position(int pos) {
            return pos != -1 && pos < next_insertion;
        }

        inline bool is_valid_layer(int layer) {
            return layer >= 0 && layer <= curr_layer;
        }

        void swap_up(int pos, int layer, bool is_first = true) {
            if (!is_valid_position(pos) || !is_valid_layer(layer)) return;

            if (is_first) {
                auto & operation = funcs[(layer+1)%2];


                int parent = get_parent_of(pos);

                std::cout << parent << " " << pos << " " << layer << "\n";
                


                if (is_valid_position(parent) && operation(data[parent].first, data[pos].first)) {
                    std::cout << "calling "  << (layer+1)%2 << " with " <<  data[parent].first << " " << data[pos].first << "\n\n";
                    return swap(data[parent], data[pos]), swap_up(parent, layer-1, false);
                }
                
            } 

            auto & operation = funcs[layer%2];
            int grand_parent = get_grand_parent_of(pos);
            if (is_valid_position(grand_parent) && operation(data[grand_parent ].first, data[pos].first))
                return swap(data[grand_parent], data[pos]), swap_up(grand_parent , layer-2, false);
        }

        int get_child(int pos, int child_num) {
            if (!is_valid_position(pos) || child_num  > 1) return -1;
            int res = pos * 2 + 1 + child_num;
            if (!is_valid_position(res)) return -1;
            return res;
        }

        int get_best_child(int pos, const function<bool(int, int)> & operation) {
            int best_child = -1;

            for(int i = 0, child = get_child(pos, i); child != -1; child = get_child(pos, ++i)) {
                if(best_child == -1 || operation(data[child].first, data[best_child].first)) 
                    best_child = child;
            }
            
            return best_child;
        }

        int get_best_grand_child(int pos, const function<bool(int, int)> & operation) {
            int best_grand_child = -1;

            for(int i = 0, child = get_child(pos, i); child != -1; child = get_child(pos, ++i)) {
                for(int j = 0, grand_child = get_child(child, j); 
                    grand_child != -1; 
                    grand_child = get_child(child, ++j)) {

                if(best_grand_child == -1 || operation(data[grand_child].first, data[best_grand_child].first)) 
                    best_grand_child = grand_child;
                }
            }
            
            return best_grand_child;
        }

        void swap_down(int pos, int layer, bool is_first = true) {
            if (!is_valid_position(pos) || !is_valid_layer(layer)) return;

            auto & operation = funcs[layer%2];
            auto & oposite = funcs[(layer+1)%2];

            if (is_first) {
                int best = get_best_child(pos, oposite);


                std::cout << best << " " << pos << " " << layer << "\n";

                if (is_valid_position(best) &&  operation(data[pos].first, data[best].first)) {
                    std::cout << "calling "  << (layer)%2 << " with " <<  data[best].first << " " << data[pos].first << "\n\n";
                    return swap(data[pos], data[best]), swap_down(best, layer + 1, false);
                }
            }

            int best = get_best_grand_child(pos, oposite);

            if (is_valid_position(best) &&  operation(data[pos].first, data[best].first))
                return swap(data[pos], data[best]), swap_down(best, layer + 2, false);
        }

        void print_node(int node, int tab) {
            if (node >= next_insertion)
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
            if(next_insertion == max_size) return false;
            data[next_insertion] = value;
            int layer = curr_layer;

            if(++curr_layer_size == max_layer_size) {
                curr_layer_size = 0;
                max_layer_size *= 2;
                curr_layer++;
            }

            swap_up(next_insertion++, layer);
            return true;
        }

        inline int size() { return next_insertion; }

        void outheap_min() {
            if(size() == 0) throw "sem elemento para ser removidos";

            next_insertion--;
            swap(data[0], data[next_insertion]);

            if(curr_layer_size == 0) {
                max_layer_size /= 2;
                curr_layer_size = max_layer_size-1;
                curr_layer--;
            } else curr_layer_size--;

            swap_down(0, 0);
        }

        void outheap_max() {
            if(size() == 0) throw "sem elemento para ser removidos";

            int best = get_best_child(0, funcs[0]);
           
            next_insertion--;

            if(curr_layer_size == 0) {
                max_layer_size /= 2;
                curr_layer_size = max_layer_size - 1;
                curr_layer--;
            } else curr_layer_size--;

            if (!is_valid_position(best)) return;

            swap(data[best], data[next_insertion]);
            swap_down(best, 1);
        }

        pair<int, T> get_max() {
            int best = get_best_child(0, funcs[0]);
            if(is_valid_position(best))
                return data[best];
            return data[0];
        }

        void print() {
            print_node(0, 0);

            cout << std::endl;

            for(int i = 0; i < next_insertion; i++)
                cout << data[i].first << " ";
            cout << std::endl << std::endl;
        }

};

int main() {

    MinMaxHeap<int, 100> a;

    a.insert({1, 1});
    a.print(); 
        

    std::cout << "=================\n\n";

    a.insert({2, 1});
    a.print(); 

    std::cout << "=================\n\n";

    a.insert({-1, 1});
    a.print(); 


    std::cout << "=================\n\n";
    a.insert({3, 1});
    a.print(); 

    std::cout << "=================\n\n";

    a.insert({10, 1});
    a.print(); 

    std::cout << "=================\n\n";


    a.outheap_max();
    a.print(); 

    std::cout << "=================\n\n";

    a.outheap_max();
    a.print(); 

    std::cout << "=================\n\n";

    a.outheap_max();
    a.print(); 

    std::cout << "=================\n\n";

    // a.insert({1, 1});
    // a.print(); 

    // a.insert({1, 1});
    // a.print(); 

    return 0;
}