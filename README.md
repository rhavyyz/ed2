## A classe Btree


```Btree```<$tipo\  de\  dado\  armazenado$  ,  $ordem\  da\  arvore$>

e tem os metodos:

```c++

Btree<T, size> b;

bool found = b.find(value);
bool inserted = b.insert(value); 
bool deleted = b.del(value);

b.print();
b.clear();

```

## Função main:

Na função main temos esboçadas as insercoes e deleções da questao 7.

```c++
BTree<int, 2> b;

std::vector<int> elements = {11, 59 , 25, 28, 73, 79, 61, 25 , 57, 41, 38 , 2 , 4 , 12, 44 , 34, 45, 32 , 1 , 72, 19 , 72, 17, 61, 33, 512, 17, 43, 54, 44, 6, 11, 17, 28, 35};

std::vector<int>del_elements = {45, 72, 1, 41, 12, 33, 79};

// Funcoes auxiliares para ajudar os testes
insert_many(b, elements);
find_many(b, elements);
delete_many(b, del_elements);
find_many(b, del_elements);
```


## Compilacao e execução

A seguir os metodos de compilação que usamos:

- Via `GNU G++`
```bash  
$ g++ -std=c++20 -o b b.cpp 
```

- Via cmake
```
mkdir build
cd build

cmake ..

make

./b
```

Mas qualquer compilador de c++ 20 deve ser capaz de gerar um executavel valido.
