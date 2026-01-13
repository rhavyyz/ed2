#include <iostream>
#include <vector>
#include <functional>
#include <algorithm>
#include <chrono>
#include <cmath>
#include <random>
#include <iomanip>

using std::cout, std::pair, std::vector;

// Implementação de AVL Tree para indexar prioridades
class AVLNode
{
public:
    int priority;
    std::vector<int> indices; // Usado quando permite duplicatas
    int single_index;         // Usado quando não permite duplicatas
    AVLNode *left;
    AVLNode *right;
    int height;

    AVLNode(int p, int idx, bool allow_duplicates) : priority(p), single_index(idx), left(nullptr), right(nullptr), height(1)
    {
        if (allow_duplicates)
        {
            indices.push_back(idx);
        }
    }
};

class AVLTree
{
private:
    AVLNode *root;
    bool allow_duplicates;

    int height(AVLNode *node)
    {
        return node ? node->height : 0;
    }

    int balance_factor(AVLNode *node)
    {
        return node ? height(node->left) - height(node->right) : 0;
    }

    void update_height(AVLNode *node)
    {
        if (node)
        {
            node->height = 1 + std::max(height(node->left), height(node->right));
        }
    }

    AVLNode *rotate_right(AVLNode *y)
    {
        AVLNode *x = y->left;
        AVLNode *T2 = x->right;

        x->right = y;
        y->left = T2;

        update_height(y);
        update_height(x);

        return x;
    }

    AVLNode *rotate_left(AVLNode *x)
    {
        AVLNode *y = x->right;
        AVLNode *T2 = y->left;

        y->left = x;
        x->right = T2;

        update_height(x);
        update_height(y);

        return y;
    }

    AVLNode *balance(AVLNode *node)
    {
        update_height(node);
        int bf = balance_factor(node);

        // Left heavy
        if (bf > 1)
        {
            if (balance_factor(node->left) < 0)
            {
                node->left = rotate_left(node->left);
            }
            return rotate_right(node);
        }

        // Right heavy
        if (bf < -1)
        {
            if (balance_factor(node->right) > 0)
            {
                node->right = rotate_right(node->right);
            }
            return rotate_left(node);
        }

        return node;
    }

    AVLNode *insert_helper(AVLNode *node, int priority, int index)
    {
        if (!node)
        {
            return new AVLNode(priority, index, allow_duplicates);
        }

        if (priority < node->priority)
        {
            node->left = insert_helper(node->left, priority, index);
        }
        else if (priority > node->priority)
        {
            node->right = insert_helper(node->right, priority, index);
        }
        else
        {
            // Prioridade já existe
            if (allow_duplicates)
            {
                node->indices.push_back(index);
            }
            // Se não permite duplicatas, não faz nada (ignora)
            return node;
        }

        return balance(node);
    }

    AVLNode *find_min(AVLNode *node)
    {
        while (node->left)
        {
            node = node->left;
        }
        return node;
    }

    AVLNode *remove_helper(AVLNode *node, int priority)
    {
        if (!node)
            return nullptr;

        if (priority < node->priority)
        {
            node->left = remove_helper(node->left, priority);
        }
        else if (priority > node->priority)
        {
            node->right = remove_helper(node->right, priority);
        }
        else
        {
            // Node found
            if (!node->left || !node->right)
            {
                AVLNode *temp = node->left ? node->left : node->right;
                delete node;
                return temp;
            }
            else
            {
                AVLNode *temp = find_min(node->right);
                node->priority = temp->priority;
                node->indices = temp->indices;
                node->right = remove_helper(node->right, temp->priority);
            }
        }

        return balance(node);
    }

    AVLNode *search_helper(AVLNode *node, int priority)
    {
        if (!node)
            return nullptr;

        if (priority < node->priority)
        {
            return search_helper(node->left, priority);
        }
        else if (priority > node->priority)
        {
            return search_helper(node->right, priority);
        }

        return node;
    }

    void destroy(AVLNode *node)
    {
        if (node)
        {
            destroy(node->left);
            destroy(node->right);
            delete node;
        }
    }

    void print_helper(AVLNode *node, const std::string &prefix, bool isLeft)
    {
        if (!node)
            return;

        std::cout << prefix;
        std::cout << (isLeft ? "├──" : "└──");

        std::cout << "[" << node->priority << "] (h=" << node->height
                  << ", bf=" << balance_factor(node) << ") → ";

        if (allow_duplicates)
        {
            std::cout << "índices: ";
            for (size_t i = 0; i < node->indices.size(); i++)
            {
                std::cout << node->indices[i];
                if (i < node->indices.size() - 1)
                    std::cout << ",";
            }
        }
        else
        {
            std::cout << "índice: " << node->single_index;
        }
        std::cout << "\n";

        if (node->left || node->right)
        {
            if (node->left)
            {
                print_helper(node->left, prefix + (isLeft ? "│   " : "    "), true);
            }
            else
            {
                std::cout << prefix << (isLeft ? "│   " : "    ") << "├──[null]\n";
            }

            if (node->right)
            {
                print_helper(node->right, prefix + (isLeft ? "│   " : "    "), false);
            }
            else
            {
                std::cout << prefix << (isLeft ? "│   " : "    ") << "└──[null]\n";
            }
        }
    }

    void in_order_helper(AVLNode *node)
    {
        if (!node)
            return;
        in_order_helper(node->left);
        std::cout << node->priority << " ";
        in_order_helper(node->right);
    }

    int count_nodes(AVLNode *node)
    {
        if (!node)
            return 0;
        return 1 + count_nodes(node->left) + count_nodes(node->right);
    }

public:
    AVLTree(bool allow_dup = true) : root(nullptr), allow_duplicates(allow_dup) {}

    ~AVLTree()
    {
        destroy(root);
    }

    bool insert(int priority, int index)
    {
        size_t size_before = count_nodes(root);
        root = insert_helper(root, priority, index);
        size_t size_after = count_nodes(root);

        // Retorna false se tentou inserir duplicata e não permite
        return size_after > size_before || allow_duplicates;
    }

    void remove(int priority)
    {
        root = remove_helper(root, priority);
    }

    std::vector<int> *find(int priority)
    {
        AVLNode *node = search_helper(root, priority);
        if (!node)
            return nullptr;

        if (allow_duplicates)
        {
            return &(node->indices);
        }
        else
        {
            // Cria vetor temporário com índice único para compatibilidade
            static std::vector<int> temp_vec;
            temp_vec.clear();
            temp_vec.push_back(node->single_index);
            return &temp_vec;
        }
    }

    int *find_single(int priority)
    {
        AVLNode *node = search_helper(root, priority);
        if (!node)
            return nullptr;
        return allow_duplicates ? (node->indices.empty() ? nullptr : &node->indices[0]) : &node->single_index;
    }

    void update_index(int priority, int old_index, int new_index)
    {
        AVLNode *node = search_helper(root, priority);
        if (node)
        {
            if (allow_duplicates)
            {
                for (auto &idx : node->indices)
                {
                    if (idx == old_index)
                    {
                        idx = new_index;
                        break;
                    }
                }
            }
            else
            {
                if (node->single_index == old_index)
                {
                    node->single_index = new_index;
                }
            }
        }
    }

    void remove_index(int priority, int index)
    {
        AVLNode *node = search_helper(root, priority);
        if (node)
        {
            if (allow_duplicates)
            {
                auto it = std::find(node->indices.begin(), node->indices.end(), index);
                if (it != node->indices.end())
                {
                    node->indices.erase(it);

                    if (node->indices.empty())
                    {
                        remove(priority);
                    }
                }
            }
            else
            {
                // Sem duplicatas, sempre remove o nó
                if (node->single_index == index)
                {
                    remove(priority);
                }
            }
        }
    }

    void print()
    {
        if (!root)
        {
            std::cout << "  [Árvore vazia]\n";
            return;
        }
        std::cout << "  Raiz:\n";
        print_helper(root, "  ", false);
    }

    void print_in_order()
    {
        std::cout << "  In-order: ";
        if (!root)
        {
            std::cout << "[vazia]";
        }
        else
        {
            in_order_helper(root);
        }
        std::cout << "\n";
    }

    int size()
    {
        return count_nodes(root);
    }

    int get_height()
    {
        return height(root);
    }
};

template <class T, size_t d, size_t max_size>
class HeapPriorityQueue
{
private:
    std::function<bool(int, int)> priority_function;

    pair<int, T> data[max_size];
    int next_inclusion = 0;
    AVLTree priority_to_indices;
    bool allow_duplicate_priorities;

    int get_child(int pos, int child_num)
    {
        if (child_num >= d)
            return -1;

        pos = pos * d + 1 + child_num;

        if (is_empty_index(pos))
            return -1;

        return pos;
    }

    int get_best_child_of(int pos)
    {
        int best_child = -1, best_value;

        int child = -1, i = 0;
        while ((child = get_child(pos, i++)) != -1)
        {
            if (best_child == -1 || priority_function(data[child].first, best_value))
            {
                best_child = child;
                best_value = data[child].first;
            }
        }
        return best_child;
    }

    int parent_of(int index)
    {
        if (index % d == 0)
            return (index - 1) / d;
        return index / d;
    }

    void swap_up(int pos)
    {
        if (pos == 0)
            return;
        int parent = this->parent_of(pos);
        if (priority_function(this->data[pos].first, this->data[parent].first))
        {
            update_index_in_map(data[pos].first, pos, parent);
            update_index_in_map(data[parent].first, parent, pos);
            std::swap(data[pos], data[parent]);
            swap_up(parent);
        }
    }

    void swap_down(int pos)
    {
        int child = get_best_child_of(pos);

        if (child == -1)
            return;

        if (priority_function(data[child].first, data[pos].first))
        {
            update_index_in_map(data[pos].first, pos, child);
            update_index_in_map(data[child].first, child, pos);
            std::swap(data[pos], data[child]);
            swap_down(child);
        }
    }

    bool is_empty_index(int i)
    {
        return i >= next_inclusion;
    }

    void update_index_in_map(int priority, int old_index, int new_index)
    {
        priority_to_indices.update_index(priority, old_index, new_index);
    }

    void add_to_map(int priority, int index)
    {
        priority_to_indices.insert(priority, index);
    }

    void remove_from_map(int priority, int index)
    {
        priority_to_indices.remove_index(priority, index);
    }

    void print_node(int node, const std::string &prefix = "", bool isLast = true)
    {
        if (node >= next_inclusion)
            return;

        cout << prefix;
        cout << (isLast ? "└── " : "├── ");
        cout << data[node].first << "\n";

        // Coletar todos os filhos
        std::vector<int> children;
        int child = -1, i = 0;
        while ((child = get_child(node, i++)) != -1)
        {
            children.push_back(child);
        }

        // Imprimir cada filho
        for (size_t j = 0; j < children.size(); j++)
        {
            bool lastChild = (j == children.size() - 1);
            std::string newPrefix = prefix + (isLast ? "    " : "│   ");
            print_node(children[j], newPrefix, lastChild);
        }
    }

public:
    bool insert(const pair<int, T> &value)
    {
        if (next_inclusion == max_size)
            return false;

        // Se não permite duplicatas, verificar se já existe
        if (!allow_duplicate_priorities)
        {
            if (priority_to_indices.find(value.first) != nullptr)
            {
                std::cout << "   ⚠️  Prioridade " << value.first << " já existe. Inserção ignorada (duplicatas não permitidas).\n";
                return false;
            }
        }

        data[next_inclusion] = value;
        add_to_map(value.first, next_inclusion);
        swap_up(next_inclusion++);
        return true;
    }

    size_t size()
    {
        return next_inclusion;
    }

    pair<int, T> top()
    {
        if (this->size() == 0)
        {
            throw "no element at the top";
        }

        return data[0];
    }

    void pop()
    {
        if (this->size() == 0)
        {
            throw "no element at the top";
        }

        next_inclusion--;
        remove_from_map(data[0].first, 0);
        if (next_inclusion > 0)
        {
            update_index_in_map(data[next_inclusion].first, next_inclusion, 0);
            std::swap(data[next_inclusion], data[0]);
            swap_down(0);
        }
    }

    void print()
    {
        if (next_inclusion == 0)
        {
            cout << "  [Heap vazio]\n";
            return;
        }
        print_node(0);
        cout << "\nArray: ";
        for (int i = 0; i < next_inclusion; i++)
            cout << data[i].first << " ";
        cout << "\n";
    }

    bool promote(int old_priority, int new_priority)
    {
        std::vector<int> *indices = priority_to_indices.find(old_priority);
        if (!indices || indices->empty())
        {
            return false;
        }

        int i = (*indices)[0];
        if (i >= next_inclusion || data[i].first != old_priority)
        {
            return false;
        }

        remove_from_map(old_priority, i);
        data[i].first = new_priority;
        add_to_map(new_priority, i);

        swap_up(i);
        swap_down(i);

        return true;
    }

    bool promote_with_delta(int priority, int delta)
    {
        std::vector<int> *indices = priority_to_indices.find(priority);
        if (!indices || indices->empty())
        {
            return false;
        }

        int i = (*indices)[0];
        if (i >= next_inclusion || data[i].first != priority)
        {
            return false;
        }

        remove_from_map(priority, i);
        data[i].first += delta;
        add_to_map(data[i].first, i);

        swap_up(i);
        swap_down(i);

        return true;
    }

    std::vector<int> *find_indices_by_priority(int priority)
    {
        return priority_to_indices.find(priority);
    }

    void print_avl()
    {
        priority_to_indices.print();
    }

    void print_in_order()
    {
        priority_to_indices.print_in_order();
    }

    void print_avl_stats()
    {
        std::cout << "  ├─ Nós na AVL: " << priority_to_indices.size() << "\n";
        std::cout << "  ├─ Altura da AVL: " << priority_to_indices.get_height() << "\n";
        std::cout << "  └─ Altura teórica min: " << (int)log2(priority_to_indices.size() + 1) << "\n";
    }

    HeapPriorityQueue(bool allow_dup = true, std::function<bool(int, int)> priority_function = [](int a, int b)
                                             { return a < b; })
        : priority_to_indices(allow_dup), allow_duplicate_priorities(allow_dup)
    {
        this->priority_function = priority_function;
    }
};

void q1()
{
}

void q2()
{
}

void q3()
{
    vector<pair<int, int>> items = {{18, 39}, {41, 17}, {8, 57}, {32, 3}, {27, 14}, {19, 22}, {17, 18}, {7, 14}, {11, 57}, {72, 16}, {31, 16}, {2, 14}, {21, 41}, {12, 7}, {9, 1}, {3, 51}, {41, 11}, {18, 6}};

    HeapPriorityQueue<int, 4, 50> queue([](int a, int b)
                                        { return a > b; });

    for (auto [item, priority] : items)
    {

        std::cout << "adicionando " << item << " , " << priority << "\n";
        queue.insert({priority, item});
        queue.print();
    }

    while (queue.size())
    {
        cout << "\n\nremovendo " << queue.top().second << " , " << queue.top().second << std::endl;
        queue.pop();
        queue.print();
    }

    for (auto [item, priority] : items)
    {
        queue.insert({priority, item});
        queue.print();
    }

    vector<pair<int, int>> promotions = {{11, 57}, {17, 18}, {9, 1}};

    for (auto [_, priority] : promotions)
    {
        auto [best_p, _] = queue.top();

        if (!queue.promote(priority, best_p + 1))
            continue;

        cout << "promovendo " << priority << " para " << best_p + 1 << std::endl;
        queue.print();
        cout << std::endl
             << std::endl;
    }
}

void q4(std::vector<int> &to_add_value)
{
    cout << "\n=== CONFIGURAÇÃO: Heap com AVL ===\n\n";
    cout << "Modos disponíveis:\n";
    cout << "  [COM duplicatas] - Nó AVL usa vector<int> indices\n";
    cout << "  [SEM duplicatas] - Nó AVL usa int único, ignora repetições\n\n";
    cout << "Permitir prioridades duplicadas? (s/n): ";

    char choice;
    std::cin >> choice;
    bool allow_duplicates = (choice == 's' || choice == 'S');

    cout << "\nModo: " << (allow_duplicates ? "COM" : "SEM") << " duplicatas\n";
    cout << std::string(50, '-') << "\n\n";

    HeapPriorityQueue<int, 2, 50> heap(allow_duplicates);

    cout << "=== Inserções Iniciais (primeiros 4 elementos) ===\n\n";

    std::vector<int> first_values = {to_add_value[0], to_add_value[1], to_add_value[2], to_add_value[3]};

    for (auto a : first_values)
    {
        cout << "\n>> Inserindo: " << a << "\n";
        heap.insert({a, a});

        cout << "\nHeap:\n";
        heap.print();

        cout << "\nAVL:\n";
        heap.print_avl();

        cout << "\n"
             << std::string(50, '-') << "\n";
    }

    cout << "\n>> Inserindo elementos restantes: ";
    for (size_t i = 4; i < to_add_value.size(); i++)
    {
        cout << to_add_value[i] << " ";
        heap.insert({to_add_value[i], to_add_value[i]});
    }
    cout << "\n\n"
         << std::string(50, '=') << "\n";
    cout << "=== ESTRUTURAS FINAIS ===\n\n";

    cout << "Heap completo:\n";
    heap.print();

    cout << "\nAVL completa:\n";
    heap.print_avl();

    cout << "\nStats finais:\n";
    heap.print_avl_stats();
    heap.print_in_order();

    cout << "\n\n"
         << std::string(50, '=') << "\n";
    cout << "=== TESTE DE BUSCAS NA AVL ===\n\n";

    std::vector<int> priorities_to_search = {15, -3, 25, 5};

    for (auto priority : priorities_to_search)
    {
        cout << "Buscar " << priority << ": ";
        auto indices_avl = heap.find_indices_by_priority(priority);

        if (indices_avl)
        {
            cout << "Encontrado - índice(s): ";
            for (auto idx : *indices_avl)
            {
                cout << idx << " ";
            }
            cout << "(O(log n) = ~" << (int)log2(heap.size()) << " comparações)\n";
        }
        else
        {
            cout << "Não encontrado\n";
        }
    }

    cout << "\n\n"
         << std::string(50, '=') << "\n";
    cout << "=== TESTE DE PROMOTE ===\n\n";

    vector<pair<int, int>> promotions = {
        {15, -10},
        {10, 2}};

    for (auto [old_priority, new_priority] : promotions)
    {
        cout << "\n>> Promote: " << old_priority << " → " << new_priority << "\n";
        auto indices_before = heap.find_indices_by_priority(old_priority);

        if (indices_before)
        {
            cout << "Antes: prioridade " << old_priority << " no índice " << (*indices_before)[0] << "\n";

            bool success = heap.promote(old_priority, new_priority);

            if (success)
            {
                auto indices_after = heap.find_indices_by_priority(new_priority);
                cout << "Depois: prioridade " << new_priority << " no índice " << (*indices_after)[0] << "\n\n";

                cout << "AVL atualizada:\n";
                heap.print_avl();

                cout << "\nHeap atualizado:\n";
                heap.print();
            }
        }
        else
        {
            cout << "Erro: Prioridade " << old_priority << " não encontrada\n";
        }

        cout << std::string(50, '-') << "\n";
    }
}

void q5()
{
}

void q6()
{
}

void q7()
{
}

int main()
{

    HeapPriorityQueue<int, 2, 50> h;

    std::vector<int> to_add_value = {1, -1, 15, 25, 10, 3, -3, 5, 55, 15, 20};

    for (auto a : to_add_value)
    {

        cout << "\n\nadd " << a << std::endl;
        h.insert({a, a});
        h.print();
    }

    while (h.size())
    {
        cout << "\n\nremove " << h.top().first << std::endl;
        h.pop();
        h.print();
    }

    cout << "\n\n╔══════════════════════════════════════════════════════════════════╗\n";
    cout << "║  Questão 4 - Adição de uma AVL para busca eficiente              ║\n";
    cout << "╚══════════════════════════════════════════════════════════════════╝\n\n";

    q4(to_add_value);

    return 0;
}