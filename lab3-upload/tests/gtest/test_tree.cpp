#include <gtest/gtest.h>
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
TEST(TreeTest, NewTreeIsEmpty)
{
    AVLTree tr;
    EXPECT_EQ(tr.getRoot(), nullptr);
}

// insert и search
TEST(TreeTest, InsertAndSearch)
{
    AVLTree tr;
    fill(tr);

    EXPECT_EQ(tr.getRoot()->key, 5);
    EXPECT_NE(tr.search(1), nullptr);
    EXPECT_NE(tr.search(8), nullptr);
    EXPECT_EQ(tr.search(7), nullptr);    // такого нет
}

TEST(TreeTest, InsertGoesLeftAndRight)
{
    AVLTree tr;
    fill(tr);

    EXPECT_EQ(tr.getRoot()->left->key, 3);    // меньше - влево
    EXPECT_EQ(tr.getRoot()->right->key, 8);   // больше - вправо
}

// обходы
TEST(TreeTest, Traversals)
{
    AVLTree tr;
    fill(tr);

    testing::internal::CaptureStdout();
    tr.printPreorder();
    EXPECT_EQ(testing::internal::GetCapturedStdout(), "5 3 1 8 ");

    testing::internal::CaptureStdout();
    tr.printInorder();
    EXPECT_EQ(testing::internal::GetCapturedStdout(), "1 3 5 8 ");

    testing::internal::CaptureStdout();
    tr.printPostorder();
    EXPECT_EQ(testing::internal::GetCapturedStdout(), "1 3 8 5 ");

    testing::internal::CaptureStdout();
    tr.bfs();
    EXPECT_EQ(testing::internal::GetCapturedStdout(), "5 3 8 1 ");
}

TEST(TreeTest, BfsOnEmpty)
{
    AVLTree tr;
    testing::internal::CaptureStdout();
    tr.bfs();
    EXPECT_EQ(testing::internal::GetCapturedStdout(), "");
}

// isComplete
TEST(TreeTest, CompleteTree)
{
    AVLTree tr;
    EXPECT_TRUE(tr.isComplete());        // пустое дерево считается полным

    fill(tr);                            // уровни заполнены слева направо
    EXPECT_TRUE(tr.isComplete());
}

TEST(TreeTest, NotCompleteTree)
{
    AVLTree tr;
    tr.insert(5);
    tr.insert(8);                        // только правый ребенок, слева дырка
    EXPECT_FALSE(tr.isComplete());
}

// handleTree
static void run(json& db, Command command, const string& args)
{
    stringstream ss(args);
    handleTree(command, db, "t", ss);
}

TEST(TreeHandlerTest, AllCommands)
{
    json db = boost::json::object();

    testing::internal::CaptureStdout();
    run(db, Command::TINSERT, "5");
    run(db, Command::TINSERT, "3");
    run(db, Command::TINSERT, "8");
    run(db, Command::TFIND, "3");
    run(db, Command::TFIND, "7");
    run(db, Command::TCOMPLETE, "");
    run(db, Command::TPRINT, "");
    run(db, Command::PRINT, "");             // чужая команда
    string out = testing::internal::GetCapturedStdout();

    EXPECT_NE(out.find("-> TRUE"), string::npos);
    EXPECT_NE(out.find("-> FALSE"), string::npos);
    EXPECT_EQ(boost::json::value_to<string>(entry(db, "t")["type"]), "tree");
    EXPECT_EQ(entry(db, "t")["data"].as_array().size(), 3u);
    EXPECT_EQ(boost::json::value_to<int>(entry(db, "t")["data"].as_array()[0]), 5);    // сохранено в ширину
    EXPECT_EQ(boost::json::value_to<int>(entry(db, "t")["data"].as_array()[1]), 3);
    EXPECT_EQ(boost::json::value_to<int>(entry(db, "t")["data"].as_array()[2]), 8);
}

TEST(TreeHandlerTest, NotComplete)
{
    json db = boost::json::object();

    testing::internal::CaptureStdout();
    run(db, Command::TINSERT, "5");
    run(db, Command::TINSERT, "8");
    run(db, Command::TCOMPLETE, "");
    string out = testing::internal::GetCapturedStdout();

    EXPECT_NE(out.find("-> FALSE"), string::npos);
}

TEST(TreeHandlerTest, WrongTypeIsRejected)
{
    json db = boost::json::object();
    entry(db, "t")["type"] = "array";
    entry(db, "t")["data"] = boost::json::array();

    testing::internal::CaptureStdout();
    run(db, Command::TINSERT, "5");
    testing::internal::GetCapturedStdout();

    EXPECT_EQ(boost::json::value_to<string>(entry(db, "t")["type"]), "array");
}
