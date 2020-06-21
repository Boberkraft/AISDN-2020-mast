#include <iostream>
#include <stdio.h>
#include <string>

#define BUFFER_SIZE (1024*10)
char mybuffer[BUFFER_SIZE + 1] = { 0 };
unsigned int mybuffer_pos = BUFFER_SIZE;

void read_ahead() {
    if (mybuffer_pos + 128> BUFFER_SIZE) {
        memmove(mybuffer, mybuffer + mybuffer_pos, BUFFER_SIZE - mybuffer_pos);
        fread(mybuffer + BUFFER_SIZE - mybuffer_pos, 1, BUFFER_SIZE - (BUFFER_SIZE - mybuffer_pos), stdin);
        mybuffer_pos = 0;
    }
}

const int MAX_TREES = 150;
const int MAX_NODES = 100;
int current_tree = 0;

using namespace std;

struct Node {
    int value{ -1 };
    int children_number{ 0 };
    int d_sum{ 0 };
    int d_product{ 1 };
    int d_length{ 0 };
    int d_product_sum_p{ 1 };
    int d_product_sum_s{ 0 };
    bool hidden{ false };
    bool needs_recalculating{ false };
    Node* parent{ nullptr };
    Node* first_son{ nullptr };
    Node* last_son{ nullptr };
    Node* brother{ nullptr };
};

inline bool is_leaf(Node* node) {
    return node->value != -1;
}


struct Tree {
    int i{ -1 };
    int leafs{ 0 };
    Node* root{ nullptr };
};


Tree* TREES[MAX_TREES];
Node* NODES[MAX_TREES * MAX_NODES];

Tree* new_tree() {
    Node* new_node = new Node{ -1 };
    Tree* new_tree = new Tree{ current_tree };
    new_tree->root = new_node;
    TREES[current_tree] = new_tree;
    current_tree++;
    return new_tree;
}


Tree* get_tree(int i) {
    return TREES[i];
}

void print_exp(Node& node) {
    if (node.value == -1) {
        cout << "(";
        print_exp(*node.first_son);
        cout << ")";
    }
    else {
        if (node.hidden) {
            cout << "[";
        }
        cout << node.value;
        if (node.hidden) {
            cout << "]";
        }
    }
    if (node.brother != nullptr) {
        cout << ",";
        print_exp(*node.brother);
    }
}

// i 3 zwraca node z value = 4
Node* get_node(Tree& tree, int i) {
    return NODES[MAX_NODES * tree.i + i];
}
void register_node(Tree& tree, Node& node) {
    NODES[MAX_NODES * tree.i + node.value - 1] = &node;
    tree.leafs++;
}

void recalculate(Node& node) {
    if (node.value != -1 ) {
        node.d_length = 1;
        node.d_sum = node.value;
        node.d_product = node.value;
        node.children_number = 1;
        node.d_product_sum_p = 1;
        node.d_product_sum_s = 0;
    }
    else {
        node.d_sum = 0;
        node.d_product = 1;
        node.d_length = 0;
        node.children_number = 0;
        node.d_product_sum_p = 1;
        node.d_product_sum_s = 0;
        Node* last_processed_son = nullptr;
        for (Node* son = node.first_son; son != nullptr; son = son->brother) {
            if (!son->hidden && son->value != 69) {
                recalculate(*son);
                if (son->d_sum == 0) {
                    continue;
                }
                node.d_sum += son->d_sum;
                node.d_product *= son->d_sum;
                node.d_length += son->d_length;
                node.d_product_sum_p += son->d_product_sum_p * son->d_sum * son->d_product;
                node.d_product_sum_s *= son->d_product_sum_s + son->d_sum + son->d_product;
                node.children_number += 1;
                last_processed_son = son;
            }
        }

        if (node.children_number == 1) {
            node.d_sum = last_processed_son->d_sum;
            node.d_product = last_processed_son->d_product;
            node.d_length = last_processed_son->d_length;
            node.children_number = last_processed_son->children_number;
            node.d_product_sum_p = last_processed_son->d_product_sum_p;
            node.d_product_sum_s = last_processed_son->d_product_sum_s;
        }

    }
}

void add_child(Node& parent, Node& child) {
    if (parent.first_son == nullptr) {
        parent.first_son = &child;
    }
    else {
        parent.last_son->brother = &child;
    }
    parent.last_son = &child;
}

int my_stoi(int start, int end) {
    int val = 0;
    for (int i = start; i < end; i++) {
        char char_at_pos = mybuffer[i];
        if ('0' <= char_at_pos && char_at_pos <= '9') {
            val *= 10;
            val += char_at_pos - '0';
        }
    }

    return val;
}

bool isomorphic(Node& first, Node& second) {
    return (first.d_product_sum_p == second.d_product_sum_p
        && first.d_product_sum_s == second.d_product_sum_s
        && first.children_number == second.children_number
        && first.d_length == second.d_length
        && first.d_sum == second.d_sum
        && first.d_product == second.d_product
        );
}


bool combinations_of_k(int start, int k, Tree& tree1, Tree& tree2) {
    if (k == 0) {
        recalculate(*tree1.root);
        recalculate(*tree2.root);
        return isomorphic(*tree1.root, *tree2.root)
    }
    
    for (int i = start; i < tree1.leafs; i++) {
        Node* node1 = get_node(tree1, i);
        Node* node2 = get_node(tree2, i);
        node1->hidden = true;
        node2->hidden = true;

        if (combinations_of_k(i + 1, k - 1, tree1, tree2)) {
            node1->hidden = false;
            node2->hidden = false;
            return true;
        }

        node1->hidden = false;
        node2->hidden = false;
    }
    return false;
}

int progressing_powerset(Tree& tree1, Tree& tree2) {
    for (int i = 1; i < tree1.leafs; i++) {
        if (combinations_of_k(0, i, tree1, tree2)) {
            return i;
        }
    }

    cout << endl << "Brak odpowiedzi?" << endl;
    return 420;
}

/*
3
(9,3,5,(1,6,(10,7)),4,(2,8));
(7,9,5,4,6,(3,8),(10,1),2);
(7,9,5,4,6,(3,8),(10,1),2);
*/

void parse(int start, int end, Node& parent, Tree& tree) {
    int level = 0;
    int last_coma = start - 1;
    bool did_something = false;
    if (mybuffer[start] == '(' && mybuffer[end - 1] == ')') {
        bool its_single = true;
        level = 0;
        for (int i = start; i < end; i++) {
            if (mybuffer[i] == '(')
                level++;
            if (mybuffer[i] == ')')
                level--;
            if (level == 0 && mybuffer[i] == ',') {
                its_single = false;
                break;
            }
        }

        if (its_single) {
            return parse(start + 1, end - 1, parent, tree);
        }

    }
    level = 0;
    for (int i = start; i <=
        end; i++) {
        if (mybuffer[i] == '(')
            level++;
        if (mybuffer[i] == ')')
            level--;
        if (level > 0)
            continue;

        if ((i == end && last_coma != start - 1) || (i + 1 != end && mybuffer[i + 1] == ',')) {
            Node* new_node = new Node{ -1 };
            parse(last_coma + 1, i + 1, *new_node, tree);
            last_coma = i + 1;
            add_child(parent, *new_node);
            did_something = true;
        }
    }


    parent.value = my_stoi(start, end);
    register_node(tree, parent);

}



void deploy();
void ismorphic_test();

int main() {
    deploy();
    // ismorphic_test();
    return 312;
}

void deploy() {
    std::ios_base::sync_with_stdio(false);
    std::cin.tie(NULL);
    std::cout.tie(NULL);

    int tests = 0;
    scanf_s("%d\n", &tests);
    int i = 0;
    for (;tests > i; i++) {
        read_ahead();
        unsigned int mybuffer_pos_end = mybuffer_pos;
        while (mybuffer[mybuffer_pos_end] != ';') {
            ++mybuffer_pos_end;
        }

        Tree* tree = new_tree();
        parse(mybuffer_pos, mybuffer_pos_end, *tree->root, *tree);
        mybuffer_pos = mybuffer_pos_end + 2;
        mybuffer_pos_end += 3;
    }
    for (int i = 0; i < tests; i++) {
        for (int j = i + 1; j < tests; j++) {
            printf("%d\n", progressing_powerset(*get_tree(i), *get_tree(j)));
        }
    }
}


void test(bool expected, string input1, string input2) {
    Tree tree1 = *new_tree();
    Tree tree2 = *new_tree();
    parse(input1, *tree1.root, tree1);
    parse(input2, *tree2.root, tree2);

    // cout << "Tree1:" << endl;
    recalculate(*tree1.root);
    // cout << "Tree2:" << endl;
    recalculate(*tree2.root);
    if (isomorphic(tree1.root, tree2.root) == expected) {
        cout << ".";
    } else {
        cout << "FAIL!" << endl;
        cout << input1 << " == ";
        print_exp(*tree1.root);
        cout << endl;

        cout << input2 << " == ";
        print_exp(*tree2.root);
        cout << endl;
        if (expected) {
            cout << "Sa izomorficzne!" << endl;
        } else {
            cout << "Nie sa izomorficzne!" << endl;
        }
        cout << endl;
    }
}
void ismorphic_test() {
    test(false, "(1,(2,3))", "((1,2),3)");
    test(true, "(1,(2,3))", "((2,3),1)");
    test(false, "(1)", "(2)");
    test(false, "(1,2,3)", "((1,2),3)");
    test(true, "(1,2,3)", "(2,3,1)");
    test(true, "(4,(8),(2,((6))))", "((8,((4)),(6,2)))");
    test(false, "(10,6,3),(9,4)", "((3,4,6),(9,10))");
    test(true, "(1,(3,2))", "((3,2),1)");
    test(true, "(2,(3))", "((3),2)");
    test(true, "(1,(2,3))", "(((2,(3)),(1)))");
    test(true, "8,3", "8,69,3");
    test(true, "5", "69,5");
    test(true, "1", "1,69");
    test(true, "(1,2)", "(69,69,69,69,(1,2))");
    test(true, "(1,69,69,(69,6,3),69,69)", "(69,69,((6,69,(69,3)),(1,69)))");
    test(true, "1,69,(69,2,69)", "1,2");
    test(true, "1,(69,69))", "1");
    test(false, "1,(69,2,3))", "(69,1),2,(69,3)");
    test(false, "1,(69,2,3))", "(69,1),2,(69,3)");
    test(false, "2,(1,3)", "1,(2,3)");
    test(false, "1,10,(2,3,4)", "2,3,(1,4,10)");
}
