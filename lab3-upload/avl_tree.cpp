#include "avl_tree.h"
#include <algorithm>

AVLTree::AVLTree()
{
    root = nullptr;
}

AVLTree::~AVLTree()
{
    freeTree(root);
}

void AVLTree::freeTree(TreeNode* node)
{
    if (node == nullptr) return;
    freeTree(node -> left);
    freeTree(node -> right);
    delete node;
}

int AVLTree::height(const TreeNode* node)
{
    return node ? node->height : 0;
}

void AVLTree::updateHeight(TreeNode* node)
{
    node->height = 1 + std::max(height(node->left), height(node->right));
}

TreeNode* AVLTree::rotateLeft(TreeNode* node)
{
    TreeNode* right = node->right;
    node->right = right->left;
    right->left = node;
    updateHeight(node);
    updateHeight(right);
    return right;
}

TreeNode* AVLTree::rotateRight(TreeNode* node)
{
    TreeNode* left = node->left;
    node->left = left->right;
    left->right = node;
    updateHeight(node);
    updateHeight(left);
    return left;
}

TreeNode* AVLTree::balance(TreeNode* node)
{
    updateHeight(node);
    int factor = height(node->left) - height(node->right);
    if (factor > 1)
    {
        if (height(node->left->right) > height(node->left->left))
            node->left = rotateLeft(node->left);
        return rotateRight(node);
    }
    if (factor < -1)
    {
        if (height(node->right->left) > height(node->right->right))
            node->right = rotateRight(node->right);
        return rotateLeft(node);
    }
    return node;
}

TreeNode* AVLTree::insertNode(TreeNode* node, int key)
{
    if (!node) return new TreeNode{key};
    if (key < node->key) node->left = insertNode(node->left, key);
    else if (key > node->key) node->right = insertNode(node->right, key);
    else return node; // уникальные ключи
    return balance(node);
}

TreeNode* AVLTree::removeNode(TreeNode* node, int key)
{
    if (!node) return nullptr;
    if (key < node->key) node->left = removeNode(node->left, key);
    else if (key > node->key) node->right = removeNode(node->right, key);
    else
    {
        if (!node->left || !node->right)
        {
            TreeNode* child = node->left ? node->left : node->right;
            delete node;
            return child;
        }
        TreeNode* next = node->right;
        while (next->left) next = next->left;
        node->key = next->key;
        node->right = removeNode(node->right, next->key);
    }
    return balance(node);
}

void AVLTree::insert(int key)
{
    root = insertNode(root, key);
}

void AVLTree::remove(int key)
{
    root = removeNode(root, key);
}

void AVLTree::preorder(TreeNode* node) const
{
    if (node == nullptr) return;
    cout << node -> key << " ";
    preorder(node -> left);
    preorder(node -> right);
}

void AVLTree::inorder(TreeNode* node) const
{
    if (node == nullptr) return;
    inorder(node -> left);
    cout << node -> key << " ";
    inorder(node -> right);
}

void AVLTree::postorder(TreeNode* node) const
{
    if (node == nullptr) return;
    postorder(node -> left);
    postorder(node -> right);
    cout << node -> key << " ";
}
//обходу нужен корень, корень спрятан в объекте, printPreorder - это метод, который достает его изнутри и передает в обход.
void AVLTree::printPreorder() const
{
    preorder(root);
}

void AVLTree::printInorder() const
{
    inorder(root);
}

void AVLTree::printPostorder() const
{
    postorder(root);
}

void AVLTree::bfs() const
{
    if (root == nullptr) return;

    vector<TreeNode*> ochered;
    ochered.push_back(root);
    for (size_t i = 0; i < ochered.size(); i++)
    {
        TreeNode* node = ochered[i];
        cout << node -> key << " ";
        if (node -> left != nullptr) ochered.push_back(node -> left);
        if (node -> right != nullptr) ochered.push_back(node -> right);
    }
}

bool AVLTree::isComplete() const
{
    if (root == nullptr) return true;

    vector<TreeNode*> ochered;
    ochered.push_back(root);
    bool sawEmpty = false;

    for (size_t i = 0; i < ochered.size(); i++)
    {
        TreeNode* node = ochered[i];

        if (node == nullptr)
        {
            sawEmpty = true;
        }
        else
        {
            if (sawEmpty) return false;
            ochered.push_back(node -> left);
            ochered.push_back(node -> right);
        }
    }
    return true;
}

TreeNode* AVLTree::search(int key) const
{
    TreeNode* current = root;
    while (current != nullptr)
    {
        if (key == current -> key) return current;
        if (key < current -> key)
        {
            current = current -> left;
        }
        else
        {
            current = current -> right;
        }
    }
    return nullptr;
}

TreeNode* AVLTree::getRoot() const
{
    return root;
}

void handleTree(Command command, json& db, const string& name, stringstream& ss)
{
    if (wrongType(db, name, "tree")) return;

    AVLTree tr;
    if (binaryMode) tr.loadBinary(binaryFile(name));
    else if (db.as_object().contains(name))
    {
        for (auto& element : entry(db, name)["data"].as_array()) tr.insert(boost::json::value_to<int>(element));
    }

    int value;
    switch (command)
    {
        case Command::TINSERT:
            if (!(ss >> value)) { cout << "неверные аргументы\n"; return; }
            tr.insert(value);
            cout << "-> " << value << "\n";
            break;
        case Command::TDEL:
            if (!(ss >> value)) { cout << "неверные аргументы\n"; return; }
            tr.remove(value);
            break;
        case Command::TFIND:
            if (!(ss >> value)) { cout << "неверные аргументы\n"; return; }
            if (tr.search(value) != nullptr) cout << "-> TRUE\n";
            else cout << "-> FALSE\n";
            break;
        case Command::TCOMPLETE:
            if (tr.isComplete()) cout << "-> TRUE\n";
            else cout << "-> FALSE\n";
            break;
        case Command::TPRINT:
            cout << "прямой:       "; tr.printPreorder();  cout << "\n";
            cout << "симметричный: "; tr.printInorder();   cout << "\n";
            cout << "обратный:     "; tr.printPostorder(); cout << "\n";
            cout << "в ширину:     "; tr.bfs();            cout << "\n";
            break;
        default:
            break;
    }

    // сохраняем обходом в ширину, чтобы при загрузке дерево собралось таким же
    boost::json::array data;
    vector<TreeNode*> ochered;
    if (tr.getRoot() != nullptr) ochered.push_back(tr.getRoot());
    for (size_t i = 0; i < ochered.size(); i++)
    {
        TreeNode* node = ochered[i];
        data.emplace_back(node -> key);
        if (node -> left != nullptr) ochered.push_back(node -> left);
        if (node -> right != nullptr) ochered.push_back(node -> right);
    }
    if (binaryMode) { entry(db, name)["type"] = "tree"; tr.saveBinary(binaryFile(name)); }
    else { entry(db, name)["type"] = "tree"; entry(db, name)["data"] = data; }
}

// сериализация: [количество] [ключи в порядке обхода в ширину]
// при загрузке вставляем в том же порядке - дерево получится таким же
void AVLTree::saveBinary(const string& fileName) const
{
    vector<TreeNode*> ochered;
    if (root != nullptr) ochered.push_back(root);
    for (size_t i = 0; i < ochered.size(); i++)
    {
        TreeNode* node = ochered[i];
        if (node -> left != nullptr) ochered.push_back(node -> left);
        if (node -> right != nullptr) ochered.push_back(node -> right);
    }

    ofstream out(fileName, ios::binary);
    writeInt(out, static_cast<int>(ochered.size()));
    for (TreeNode* node : ochered)
    {
        writeInt(out, node -> key);
    }
}

void AVLTree::loadBinary(const string& fileName)
{
    ifstream in(fileName, ios::binary);
    if (!in.is_open()) return;

    freeTree(root);                            // очищаем дерево перед загрузкой
    root = nullptr;
    int count = readInt(in);
    for (int i = 0; i < count; i++)
    {
        insert(readInt(in));
    }
}
