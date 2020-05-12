#include <iostream>
#include <stdio.h>
#include <string>

const int MAX_TREES = 10;
const int MAX_NODES = 100;
int current_tree = 0;

using std::endl, std::cout, std::string;

struct Node {
    int value { -1 };
    int children_number { 0 };
    int d_sum { 0 };
    int d_product { 1 };
    int d_length { 0 };
    Node* parent { nullptr };
    Node* first_son { nullptr };
    Node* last_son { nullptr };
    Node* brother { nullptr };
};

struct Tree {
    int i { -1 };
    int next_free { 0 };
    Node* root { nullptr };
};


Node* TREES[MAX_TREES];
Node* NODES[MAX_TREES * MAX_NODES];


void print_exp(Node &node) {
    if (node.value == -1) {
        cout << "(";
        print_exp(*node.first_son);
        cout << ")";
    } else {
        cout << node.value;
    }
    if(node.brother != nullptr) {
        cout << ", ";
        print_exp(*node.brother);
    }
}

Tree* new_tree() {
    Node* new_node = new Node { -1 };
    Tree* new_tree = new Tree { current_tree };
    new_tree->root = new_node;
    current_tree++;
    return new_tree;
}

void register_node(Tree &tree, Node &node) {
    NODES[MAX_NODES * tree.i + tree.next_free] = &node;
    tree.next_free++;
}

void add_child(Node &parent, Node &child) {
    if (parent.value != -1) {
        cout << "Ej!" << endl;
        parent.value = -1;
    }
    if (parent.first_son == nullptr) {
        parent.first_son = &child;
    } else {
        parent.last_son->brother = &child;
    }
    parent.last_son = &child;
}

int my_stoi(string str_val) {
    int val = 0;
    for (int i = 0; i < str_val.size(); i++) {
        char char_at_pos = str_val[i];
        if ('0' <= char_at_pos && char_at_pos <= '9') {
            val *= 10;
            val += char_at_pos - '0';
        }
    }
    cout << "  my_stoi(" << str_val << ") -> " << val << endl;
    return val;
}

bool isomorphic(Node *first, Node *second) {
    if (first == nullptr && second == nullptr)
        return true;
    if (first == nullptr || second == nullptr)
        return false;
    return (first->children_number == second->children_number 
         && first->d_length == second->d_length 
         && first->d_sum == second->d_sum 
         && first->d_product == second->d_product);
}

void parse(string input, Node &parent, Tree &tree) {
    cout << "parse(" << input << ")" << endl;
    int level = 0;
    int last_coma = -1;
    bool did_something = false;

    if (input[0] == '(' && input[input.size()-1] == ')') {
        return parse(input.substr(1, input.size() - 2), parent, tree);
    }
    for (int i = 0; i <= input.size(); i++) {
        if (input[i] == '(')
            level++;
        if (input[i] == ')')
            level--;
        if (level > 0)
            continue;

        if ((i == input.size() && last_coma != -1) || input[i+1] == ',') {
            Node* new_node = new Node { -1 };
            parse(input.substr(last_coma+1, i - (last_coma + 1) + 1), *new_node, tree);
            last_coma = i+1;
            add_child(parent, *new_node);
            parent.d_sum += new_node->d_sum;
            parent.d_product += new_node->d_product;
            parent.d_length += new_node->d_length;
            parent.children_number += 1;
            did_something = true;
        }
    }

    if (did_something)
        return;
    

    parent.d_length = 1;
    parent.value = my_stoi(input);
    parent.d_sum = parent.value;
    parent.d_product = parent.value;
}

void test() {
    Node root { -1 };
    Node child1 {};
    Node child2 {};
    Node child3 {};
    Node child4 {};
    Node child5 {};
    child1.value = 99;
    child3.value = 69;
    child4.value = 23;
    child5.value = 44;
    add_child(root, child1);
    add_child(root, child2);
    add_child(child2, child3);
    add_child(child2, child4);
    add_child(child2, child5);
    print_exp(root);
}
int main() {
    string input1 = "(4,(8),(2,((6))))";
    // string input1 = "4,8,(2,6)";
    string input2 = "((8,((4)),(6,2)))";
    // string input2 = "8,4,(6,2)";
    Tree tree1 = *new_tree(1);
    Tree tree2 = *new_tree(2);
    parse(input1, *tree1.root, tree1);
    parse(input2, *tree2.root, tree2);

    print_exp(*tree1.root);
    cout << endl << tree1.root->d_length ;
    cout << endl << tree1.root->children_number;
    cout << endl;
    cout << endl << tree2.root->d_length;
    cout << endl << tree2.root->children_number;
    cout << endl ;
    cout << endl << isomorphic(tree1.root, tree1.root) << endl;
    return 0;
}
