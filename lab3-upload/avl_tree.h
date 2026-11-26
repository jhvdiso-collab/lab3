#pragma once
#include "serialization.h"
#include <vector>
#include "commands.h"
#include <iostream>
#include <vector>
using namespace std;

struct TreeNode
{
    int key;
    TreeNode *left = nullptr;
    TreeNode *right = nullptr;
    int height = 1;
};

class AVLTree
{
private:
    TreeNode *root = nullptr;

    // рекурсивные помощники: им нужен узел, с которого начинать
    static int height(const TreeNode* node);
    static void updateHeight(TreeNode* node);
    static TreeNode* rotateLeft(TreeNode* node);
    static TreeNode* rotateRight(TreeNode* node);
    static TreeNode* balance(TreeNode* node);
    static TreeNode* insertNode(TreeNode* node, int key);
    static TreeNode* removeNode(TreeNode* node, int key);
    void preorder(TreeNode *node) const;
    void inorder(TreeNode *node) const;
    void postorder(TreeNode *node) const;
    void freeTree(TreeNode *node);

public:
    void saveBinary(const string &fileName) const;
    void loadBinary(const string &fileName);
    AVLTree();
    ~AVLTree();
    AVLTree(const AVLTree&) = delete;
    AVLTree& operator=(const AVLTree&) = delete;

    void insert(int key);
    void remove(int key);
    TreeNode *search(int key) const;
    bool isComplete() const;
    void printPreorder() const;
    void printInorder() const;
    void printPostorder() const; //функция нужна для handletree она работает с полями и принимает корень и дальеш она вызвается в хендли три
    void bfs() const;
    TreeNode *getRoot() const;
};

void handleTree(Command command, json& db, const string& name, stringstream& ss);