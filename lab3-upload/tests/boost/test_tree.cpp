#include <boost/test/unit_test.hpp>
#include "capture.h"
#include "avl_tree.h"
#include <algorithm>

// дерево для тестов:
//       5
//      / \
//     3   8
//    /
//   1
static void fill(AVLTree& tr)
{
    tr.insert(5);
    tr.insert(3);
    tr.insert(8);
    tr.insert(1);
}

// конструктор
BOOST_AUTO_TEST_CASE(TreeTest_NewTreeIsEmpty)
{
    AVLTree tr;
    BOOST_CHECK(tr.getRoot() == nullptr);
}

// insert и search
BOOST_AUTO_TEST_CASE(TreeTest_InsertAndSearch)
{
    AVLTree tr;
    fill(tr);

    BOOST_CHECK_EQUAL(tr.getRoot()->key, 5);
    BOOST_CHECK(tr.search(1) != nullptr);
    BOOST_CHECK(tr.search(8) != nullptr);
    BOOST_CHECK(tr.search(7) == nullptr);    // такого нет
}

BOOST_AUTO_TEST_CASE(TreeTest_InsertGoesLeftAndRight)
{
    AVLTree tr;
    fill(tr);

    BOOST_CHECK_EQUAL(tr.getRoot()->left->key, 3);    // меньше - влево
    BOOST_CHECK_EQUAL(tr.getRoot()->right->key, 8);   // больше - вправо
}

// обходы
BOOST_AUTO_TEST_CASE(TreeTest_Traversals)
{
    AVLTree tr;
    fill(tr);

    startCapture();
    tr.printPreorder();
    BOOST_CHECK_EQUAL(stopCapture(), "5 3 1 8 ");

    startCapture();
    tr.printInorder();
    BOOST_CHECK_EQUAL(stopCapture(), "1 3 5 8 ");

    startCapture();
    tr.printPostorder();
    BOOST_CHECK_EQUAL(stopCapture(), "1 3 8 5 ");

    startCapture();
    tr.bfs();
    BOOST_CHECK_EQUAL(stopCapture(), "5 3 8 1 ");
}

BOOST_AUTO_TEST_CASE(TreeTest_BfsOnEmpty)
{
    AVLTree tr;
    startCapture();
    tr.bfs();
    BOOST_CHECK_EQUAL(stopCapture(), "");
}

// isComplete
BOOST_AUTO_TEST_CASE(TreeTest_CompleteTree)
{
    AVLTree tr;
    BOOST_CHECK(tr.isComplete());        // пустое дерево считается полным

    fill(tr);                            // уровни заполнены слева направо
    BOOST_CHECK(tr.isComplete());
}

BOOST_AUTO_TEST_CASE(TreeTest_NotCompleteTree)
{
    AVLTree tr;
    tr.insert(5);
    tr.insert(8);                        // только правый ребенок, слева дырка
    BOOST_CHECK(!tr.isComplete());
}

// handleTree
static void run(json& db, Command command, const string& args)
{
    stringstream ss(args);
    handleTree(command, db, "t", ss);
}

BOOST_AUTO_TEST_CASE(TreeHandlerTest_AllCommands)
{
    json db = boost::json::object();

    startCapture();
    run(db, Command::TINSERT, "5");
    run(db, Command::TINSERT, "3");
    run(db, Command::TINSERT, "8");
    run(db, Command::TFIND, "3");
    run(db, Command::TFIND, "7");
    run(db, Command::TCOMPLETE, "");
    run(db, Command::TPRINT, "");
    run(db, Command::PRINT, "");             // чужая команда
    string out = stopCapture();

    BOOST_CHECK_NE(out.find("-> TRUE"), string::npos);
    BOOST_CHECK_NE(out.find("-> FALSE"), string::npos);
    BOOST_CHECK_EQUAL(boost::json::value_to<string>(entry(db, "t")["type"]), "tree");
    BOOST_CHECK_EQUAL(entry(db, "t")["data"].as_array().size(), 3u);
    BOOST_CHECK_EQUAL(boost::json::value_to<int>(entry(db, "t")["data"].as_array()[0]), 5);    // сохранено в ширину
    BOOST_CHECK_EQUAL(boost::json::value_to<int>(entry(db, "t")["data"].as_array()[1]), 3);
    BOOST_CHECK_EQUAL(boost::json::value_to<int>(entry(db, "t")["data"].as_array()[2]), 8);
}

BOOST_AUTO_TEST_CASE(TreeHandlerTest_NotComplete)
{
    json db = boost::json::object();

    startCapture();
    run(db, Command::TINSERT, "5");
    run(db, Command::TINSERT, "8");
    run(db, Command::TCOMPLETE, "");
    string out = stopCapture();

    BOOST_CHECK_NE(out.find("-> FALSE"), string::npos);
}

BOOST_AUTO_TEST_CASE(TreeHandlerTest_WrongTypeIsRejected)
{
    json db = boost::json::object();
    entry(db, "t")["type"] = "array";
    entry(db, "t")["data"] = boost::json::array();

    startCapture();
    run(db, Command::TINSERT, "5");
    stopCapture();

    BOOST_CHECK_EQUAL(boost::json::value_to<string>(entry(db, "t")["type"]), "array");
}

static int checkAVL(const TreeNode* node)
{
    if (!node) return 0;
    int left = checkAVL(node->left), right = checkAVL(node->right);
    BOOST_CHECK(left - right >= -1 && left - right <= 1);
    BOOST_CHECK_EQUAL(node->height, 1 + std::max(left, right));
    if (node->left) BOOST_CHECK_LT(node->left->key, node->key);
    if (node->right) BOOST_CHECK_GT(node->right->key, node->key);
    return node->height;
}

BOOST_AUTO_TEST_CASE(AVL_RotationsAndRemove)
{
    for (const auto& keys : vector<vector<int>>{{30,20,10},{10,20,30},{30,10,20},{10,30,20}})
    {
        AVLTree tree;
        for (int key : keys) tree.insert(key);
        tree.insert(20);
        BOOST_REQUIRE(tree.getRoot());
        BOOST_CHECK_EQUAL(tree.getRoot()->key, 20);
        checkAVL(tree.getRoot());
    }
    AVLTree tree;
    for (int i = 0; i < 100; ++i) { tree.insert(i); checkAVL(tree.getRoot()); }
    for (int i = 0; i < 100; i += 2) { tree.remove(i); BOOST_CHECK(!tree.search(i)); checkAVL(tree.getRoot()); }
    for (int i = 1; i < 100; i += 2) { BOOST_CHECK(tree.search(i)); tree.remove(i); checkAVL(tree.getRoot()); }
    tree.remove(1000);
    BOOST_CHECK(!tree.getRoot());
}

BOOST_AUTO_TEST_CASE(AVL_DeleteCommand)
{
    json db = boost::json::object();
    startCapture();
    run(db, Command::TINSERT, "30");
    run(db, Command::TINSERT, "10");
    run(db, Command::TINSERT, "20");
    run(db, Command::TDEL, "20");
    stopCapture();
    BOOST_CHECK_EQUAL(entry(db, "t")["data"].as_array().size(), 2u);
}
