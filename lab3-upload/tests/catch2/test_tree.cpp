#include <catch2/catch_test_macros.hpp>
#include "capture.h"
#include "avl_tree.h"

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
TEST_CASE("TreeTest.NewTreeIsEmpty")
{
    AVLTree tr;
    CHECK(tr.getRoot() == nullptr);
}

// insert и search
TEST_CASE("TreeTest.InsertAndSearch")
{
    AVLTree tr;
    fill(tr);

    CHECK(tr.getRoot()->key == 5);
    CHECK(tr.search(1) != nullptr);
    CHECK(tr.search(8) != nullptr);
    CHECK(tr.search(7) == nullptr);    // такого нет
}

TEST_CASE("TreeTest.InsertGoesLeftAndRight")
{
    AVLTree tr;
    fill(tr);

    CHECK(tr.getRoot()->left->key == 3);    // меньше - влево
    CHECK(tr.getRoot()->right->key == 8);   // больше - вправо
}

// обходы
TEST_CASE("TreeTest.Traversals")
{
    AVLTree tr;
    fill(tr);

    startCapture();
    tr.printPreorder();
    CHECK(stopCapture() == "5 3 1 8 ");

    startCapture();
    tr.printInorder();
    CHECK(stopCapture() == "1 3 5 8 ");

    startCapture();
    tr.printPostorder();
    CHECK(stopCapture() == "1 3 8 5 ");

    startCapture();
    tr.bfs();
    CHECK(stopCapture() == "5 3 8 1 ");
}

TEST_CASE("TreeTest.BfsOnEmpty")
{
    AVLTree tr;
    startCapture();
    tr.bfs();
    CHECK(stopCapture() == "");
}

// isComplete
TEST_CASE("TreeTest.CompleteTree")
{
    AVLTree tr;
    CHECK(tr.isComplete());        // пустое дерево считается полным

    fill(tr);                            // уровни заполнены слева направо
    CHECK(tr.isComplete());
}

TEST_CASE("TreeTest.NotCompleteTree")
{
    AVLTree tr;
    tr.insert(5);
    tr.insert(8);                        // только правый ребенок, слева дырка
    CHECK_FALSE(tr.isComplete());
}

// handleTree
static void run(json& db, Command command, const string& args)
{
    stringstream ss(args);
    handleTree(command, db, "t", ss);
}

TEST_CASE("TreeHandlerTest.AllCommands")
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

    CHECK(out.find("-> TRUE") != string::npos);
    CHECK(out.find("-> FALSE") != string::npos);
    CHECK(boost::json::value_to<string>(entry(db, "t")["type"]) == "tree");
    CHECK(entry(db, "t")["data"].as_array().size() == 3u);
    CHECK(boost::json::value_to<int>(entry(db, "t")["data"].as_array()[0]) == 5);    // сохранено в ширину
    CHECK(boost::json::value_to<int>(entry(db, "t")["data"].as_array()[1]) == 3);
    CHECK(boost::json::value_to<int>(entry(db, "t")["data"].as_array()[2]) == 8);
}

TEST_CASE("TreeHandlerTest.NotComplete")
{
    json db = boost::json::object();

    startCapture();
    run(db, Command::TINSERT, "5");
    run(db, Command::TINSERT, "8");
    run(db, Command::TCOMPLETE, "");
    string out = stopCapture();

    CHECK(out.find("-> FALSE") != string::npos);
}

TEST_CASE("TreeHandlerTest.WrongTypeIsRejected")
{
    json db = boost::json::object();
    entry(db, "t")["type"] = "array";
    entry(db, "t")["data"] = boost::json::array();

    startCapture();
    run(db, Command::TINSERT, "5");
    stopCapture();

    CHECK(boost::json::value_to<string>(entry(db, "t")["type"]) == "array");
}
