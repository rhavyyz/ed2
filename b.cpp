#include <cstdint>
#include <vector>
#include <iostream>
#include <type_traits>
#include <algorithm>

template <class T>
concept Comparable = requires(T a, T b) {
  	{ a == b } -> std::convertible_to<bool>;
};

template<Comparable T>
void remove_from_vec(std::vector<T> &v, T val) {
	std::erase(v, val);
}

template <class T>
concept ComparableAndPrintable = requires(T a, T b, std::ostream &os) {
  	{ a == b } -> std::convertible_to<bool>;            // Check for operator==
  	{ a < b } -> std::convertible_to<bool>;             // Check for operator<
  	{ a > b } -> std::convertible_to<bool>;             // Check for operator>
  	{ os << a } -> std::convertible_to<std::ostream &>; // Check for printing
};

template <ComparableAndPrintable T, int o> 
class BTree {
private:

  	struct Node {
		bool is_leaf = true;
		std::vector<T> keys;
		std::vector<Node*> next;

		Node* left_neighbor = nullptr, * right_neighbor = nullptr;

		inline bool needs_split() {
			return keys.size() == o * 2 + 1;
		}

		void add(T k, Node* next_add = nullptr) {
			keys.push_back(k);


			if (next_add != nullptr) {
				next.push_back(next_add);
			}

			int ptr = keys.size();
			while(ptr--, ptr >= 1 && keys[ptr] < keys[ptr-1]) {
				std::swap(keys[ptr], keys[ptr-1]);



				if (!is_leaf)
				std::swap(next[ptr+1], next[ptr]);
			}
		}

		Node(const std::vector<T> keys, const std::vector<Node*> next = {}) {
			this->keys = keys;
			this->next = next;
			is_leaf = next.size() == 0;
		}

		std::pair<T,Node*> split() {
			std::vector<T> neighbour_keys;
			while(keys.size() != o + 1) {
				neighbour_keys.push_back(keys.back());
				keys.pop_back();
			}
			std::reverse(neighbour_keys.begin(), neighbour_keys.end());

			std::vector<Node*> neighbour_next;

			while(next.size() > o + 1) {
				neighbour_next.push_back(next.back());
				next.pop_back();
			}
			std::reverse(neighbour_next.begin(), neighbour_next.end());
			if(neighbour_next.size()) {
				neighbour_next[0]->left_neighbor = nullptr;
				neighbour_next[ neighbour_next.size()-1 ]->right_neighbor = nullptr;
			}


			if(next.size()) {
				next[0]->left_neighbor = nullptr;
				next[ next.size()-1 ]->right_neighbor = nullptr;
			}

			Node * neighbour = new Node(neighbour_keys, neighbour_next); 

			neighbour->left_neighbor = this;

			neighbour->right_neighbor = this->right_neighbor;

			if (this->right_neighbor != nullptr) {
				this->right_neighbor->left_neighbor = neighbour;
			}

			this->right_neighbor = neighbour;

			std::pair<T, Node*> response = {keys.back(), neighbour};
			keys.pop_back();
			return response;
		}

		int find_next(T key) {
			return std::upper_bound(keys.begin(), keys.end(), key) - keys.begin() ;
		}
		
		int find_contained(T key) {
			return std::lower_bound(keys.begin(), keys.end(), key) - keys.begin();
		}
	
		int contains(T key) {

			if (keys.size() == 0) {

				return false;
			}


			auto index = std::lower_bound(keys.begin(), keys.end(), key) - keys.begin();
			return index != keys.size() && keys[index] == key ;
		}
	
	};

	Node* root = nullptr;

	struct insert_rec_res {
		bool inserted;
		bool need_append_parent;
		T key;
		Node* next;
	};

	insert_rec_res insert_rec(Node* node, T key) {

		if (node->contains(key)) {
			return {false, false};
		}


		if(node->is_leaf){

			node->add(key);


			if (node->needs_split()) {

				auto [key, neighbour] = node->split();

				return {true, true, key, neighbour};
			}
			return {true, false};
		}

		int next_index = node->find_next(key);

		auto [inserted, need_append, to_append, to_append_next] = insert_rec(node->next[next_index], key);

		if(!inserted)
			return {false, false};

		if (need_append) {

			node->add(to_append, to_append_next);

			if (node->needs_split()) {
				auto [key, neighbour] = node->split();
				return {true, true, key, neighbour};
			}
		}

		return {true, false};
	}


	void print_rec(Node* node, int depth = 0) {

		std::cout << std::string(depth * 3, ' ');
		if (depth > 0) std::cout << "└─";

		std::cout << "[";
		for (size_t i = 0; i < node->keys.size(); ++i) {
			std::cout << node->keys[i];
			if (i + 1 < node->keys.size()) std::cout << "|";
		}
		std::cout << "]" << std::endl;


		for (auto child : node->next)
			print_rec(child, depth + 1);

	}


	enum direction{
		left, none, right
	};

	struct delete_rec_res {
		bool deleted;
		direction swap = direction::none;
		T swap_parent_for;

		direction merge = direction::none;
	};

	// get_max_ptr

	void merge(Node*node, int pos) {

		T middle = node->keys[pos];
		remove_from_vec<T>(node->keys, middle);

		auto child = node->next[pos];
		auto right = child->right_neighbor;
		
		child->keys.push_back(middle);

		for(auto el : right->keys) {
			child->keys.push_back(el);
		}

		// Se um tiver next o outro obrigatoriamente tem tambem.
		if(child->next.size() && right->next.size()) {
			child->next[child->next.size()-1]->right_neighbor = right->next[0];
			right->next[0]->left_neighbor = child->next[child->next.size()-1];
		}


		for(auto el : right->next) {
			child->next.push_back(el);
		}

		child->right_neighbor = right->right_neighbor;
		if (child->right_neighbor != nullptr)
			child->right_neighbor->left_neighbor = child;


		remove_from_vec<Node*>(node->next, right);

		delete right;
	}

	Node* get_max_node_of(Node* node) {
		if (node->is_leaf) {
			return node;				
		}

		return get_max_node_of(node->next.back());
	}

	delete_rec_res delete_rec(Node* node, T key, bool p=false) {

		if (p) {

			for(int i = 0; i < node->keys.size(); i++)
				std:: cout <<node->keys[i] << "|";
			std::cout << " --> ";

			std::cout << key << std::endl;
		}

		int next_node = -1;

		if (node->contains(key)) {
			if(node->is_leaf) {

				remove_from_vec<T>(node->keys, key);
				// Caso trivial
				if(node->keys.size()>= o) {
					return {true};
				}

				// Tenta pegar um elemento da esquerda
				if(node->left_neighbor != nullptr && node->left_neighbor->keys.size() > o) {
					auto to_swap = node->left_neighbor->keys.back();
					node->left_neighbor->keys.pop_back();

					return {true, left, to_swap};
				}

				// Tenta pegar um elemento da direita
				if(node->right_neighbor != nullptr && node->right_neighbor->keys.size() > o) {
					auto to_swap = node->right_neighbor->keys.front();
					remove_from_vec<T>(node->right_neighbor->keys ,node->right_neighbor->keys.front());

					return {true, right, to_swap};
				}

				// Requisita merge para desta pagina com a da esquerda
				if(node->left_neighbor != nullptr) {

					return {true, none, key, left};
				}

				// Requisita merge para desta pagina com a da direita
				if(node->right_neighbor != nullptr) {

					return {true, none, key, right};
				}

				return {true};
			}
				
			int pos = node->find_contained(key);	

			auto end_node = get_max_node_of(node->next[pos]);

			node->keys[pos] = end_node->keys.back();

			key = end_node->keys.back();
			next_node = pos;
		} 

		if (node->is_leaf)
			return {false};
		
		if (next_node  == -1)
			next_node = node->find_next(key);


		auto [deleted, needs_parent_swap, swap_for, merge_type] = delete_rec( node->next[next_node], key, p);

		if(p)
		std::cout << deleted << " " << needs_parent_swap << " " << swap_for << " " << merge_type << std::endl;

		if (!deleted) return {false};


		if (needs_parent_swap != none) {
			int key_node= next_node;
			if (needs_parent_swap == left) key_node--;

			node->next[next_node]->add(node->keys[key_node]);
			node->keys[key_node] = swap_for;

			return {true, direction::none, key, direction::none};
		}

		if (merge_type != none) {

			int key_node= next_node;
			if (needs_parent_swap == left) key_node--;

			merge(node, key_node);

			if (node->keys.size() < o) {
				if(node->left_neighbor != nullptr) {
					return {true, direction::none, key, left};
				}

				if(node->right_neighbor != nullptr) {
					return {true, direction::none, key, right};
				}
			}
		}

		return {true};
	}

	void clear_rec(Node* node) {
		if (node->is_leaf) {
			delete node;
			return;
		}
		for(auto a : node->next) {
			clear_rec(a);
		}
		delete node;
	}

	bool find_rec(Node* node, T key) {
		if( node->contains(key) )
			return true;
		if(node->is_leaf)
			return false;

		return find_rec(node->next[node->find_next(key)], key);
	}


public:
	bool find(T key) {
		return find_rec(root, key);
	}

	BTree() {
		root = new Node({});
	}
    
	void clear() {
		clear_rec(root);
		root = new Node({});
	}

	~BTree() {
		clear_rec(root);
	}

	bool insert(const T &key) {
		auto res = insert_rec(root, key);

		auto [inserted, need_append, to_append, to_append_next] = res;

		if(need_append) {
			root = new Node({to_append}, {root, to_append_next});
		}

		return inserted;
	}


	bool del(const T&key) {

		auto res = delete_rec(root, key);		

		if (root->keys.size() == 0 && root->next.size() == 1) {
			root = 	root->next.back();
		}

		return res.deleted;
	}

	void print() {

		print_rec(root);

	}
};


template<ComparableAndPrintable T, int o>
void insert_many(BTree<T, o> & b, std::vector<T> v) {

	b.print();
	for(auto el : v) {
		std::cout << "add " << el << std::endl; 

		b.insert(el);
		b.print();

		std::cout << std::endl << std::endl;
	}
}

template<ComparableAndPrintable T, int o>
void delete_many(BTree<T, o> & b, std::vector<T> v) {

	b.print();
	for(auto el : v) {
		std::cout << "delete " << el << std::endl; 

		b.del(el);
		b.print();

		std::cout << std::endl << std::endl;
	}
}


template<ComparableAndPrintable T, int o>
void find_many(BTree<T, o> & b, std::vector<T> v) {
	for(auto el : v) {
		std::cout << "find " << el << " : " << (b.find(el) ? "true" : "false") << std::endl; 
	}
}


int main() { 

	BTree<int, 2> b;

	std::vector<int> elements = {11, 59 , 25, 28, 73, 79, 61, 25 , 57, 41, 38 , 2 , 4 , 12, 44 , 34, 45, 32 , 1 , 72, 19 , 72, 17, 61, 33, 512, 17, 43, 54, 44, 6, 11, 17, 28, 35};

	std::vector<int>del_elements = {45, 72, 1, 41, 12, 33, 79};

	insert_many(b, elements);
	find_many(b, elements);
	delete_many(b, del_elements);
	find_many(b, del_elements);


  	return 0; 
}
