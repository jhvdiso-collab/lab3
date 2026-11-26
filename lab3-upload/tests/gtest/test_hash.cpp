#include <gtest/gtest.h>
#include "hash_table.h"

// ===== таблица с цепочками =====
// 5, 105 и 205 попадают в одну ячейку 5 (хеш = сумма свертки % 100),
// поэтому на них удобно проверять цепочки

TEST(HashTableTest, AddAndSearch)
{
    HashTable ht;
    ht.add(5);
    ht.add(123456);

    EXPECT_TRUE(ht.search(5));
    EXPECT_TRUE(ht.search(123456));
    EXPECT_FALSE(ht.search(7));
}

TEST(HashTableTest, DuplicateIsIgnored)
{
    HashTable ht;
    ht.add(5);
    ht.add(5);
    EXPECT_EQ(ht.getAll().size(), 1u);
}

TEST(HashTableTest, CollisionsGoToOneChain)
{
    HashTable ht;
    ht.add(5);
    ht.add(105);
    ht.add(205);

    testing::internal::CaptureStdout();
    ht.print();
    string out = testing::internal::GetCapturedStdout();

    EXPECT_EQ(out, "5: 205 -> 105 -> 5 -> nullptr\n");   // новые встают в начало цепочки
}

TEST(HashTableTest, RemoveFromChain)
{
    HashTable ht;
    ht.add(5);
    ht.add(105);
    ht.add(205);              // цепочка: 205 -> 105 -> 5

    ht.remove(105);           // из середины
    ht.remove(205);           // из начала
    ht.remove(999);           // такого нет

    EXPECT_FALSE(ht.search(105));
    EXPECT_FALSE(ht.search(205));
    EXPECT_TRUE(ht.search(5));
}

TEST(HashTableTest, NegativeKey)
{
    HashTable ht;
    ht.add(-7);
    EXPECT_TRUE(ht.search(-7));
}

// ===== кукушка =====
// 1 и 12 дают одинаковый hash1 (12 % 11 = 1), поэтому второй вытесняет
// первого во вторую таблицу

TEST(KukushkaTest, InsertAndSearch)
{
    Kukushka kt;
    kt.insert(42);
    EXPECT_TRUE(kt.search(42));
    EXPECT_FALSE(kt.search(7));
}

TEST(KukushkaTest, DuplicateIsIgnored)
{
    Kukushka kt;
    kt.insert(42);
    kt.insert(42);
    EXPECT_EQ(kt.getAll().size(), 1u);
}

TEST(KukushkaTest, CollisionMovesToSecondTable)
{
    Kukushka kt;
    kt.insert(1);
    kt.insert(12);            // выталкивает 1 во вторую таблицу

    testing::internal::CaptureStdout();
    kt.print();
    string out = testing::internal::GetCapturedStdout();

    EXPECT_EQ(out, "t1: [1]=12 \nt2: [1]=1 \n");
    EXPECT_TRUE(kt.search(1));     // находится во второй таблице
    EXPECT_TRUE(kt.search(12));    // находится в первой
}

TEST(KukushkaTest, RemoveFromBothTables)
{
    Kukushka kt;
    kt.insert(1);
    kt.insert(12);

    kt.remove(12);            // из первой таблицы
    kt.remove(1);             // из второй
    kt.remove(999);           // такого нет

    EXPECT_FALSE(kt.search(1));
    EXPECT_FALSE(kt.search(12));
    EXPECT_EQ(kt.getAll().size(), 0u);
}

TEST(KukushkaTest, GrowsWhenFull)
{
    Kukushka kt;
    for (int i = 0; i < 100; i++)  // в начале 2 таблицы по 11 ячеек - придется расширяться
    {
        kt.insert(i);
    }
    for (int i = 0; i < 100; i++)
    {
        EXPECT_TRUE(kt.search(i));
    }
    EXPECT_EQ(kt.getAll().size(), 100u);
}

// ===== handleHash =====
static void runHash(json& db, Command command, const string& args)
{
    stringstream ss(args);
    handleHash(command, db, "h", ss);
}

TEST(HashHandlerTest, AllCommands)
{
    json db = boost::json::object();

    testing::internal::CaptureStdout();
    runHash(db, Command::HADD, "5");
    runHash(db, Command::HADD, "105");
    runHash(db, Command::HADD, "7");
    runHash(db, Command::HDEL, "7");
    runHash(db, Command::HFIND, "5");
    runHash(db, Command::HFIND, "7");
    runHash(db, Command::HPRINT, "");
    runHash(db, Command::PRINT, "");         // чужая команда
    string out = testing::internal::GetCapturedStdout();

    EXPECT_NE(out.find("-> TRUE"), string::npos);
    EXPECT_NE(out.find("-> FALSE"), string::npos);
    EXPECT_EQ(boost::json::value_to<string>(entry(db, "h")["type"]), "hash");
    EXPECT_EQ(entry(db, "h")["data"].as_array().size(), 2u);
}

TEST(HashHandlerTest, WrongTypeIsRejected)
{
    json db = boost::json::object();
    entry(db, "h")["type"] = "kukushka";
    entry(db, "h")["data"] = boost::json::array();

    testing::internal::CaptureStdout();
    runHash(db, Command::HADD, "5");
    testing::internal::GetCapturedStdout();

    EXPECT_EQ(boost::json::value_to<string>(entry(db, "h")["type"]), "kukushka");
}

// ===== handleKukushka =====
static void runKuk(json& db, Command command, const string& args)
{
    stringstream ss(args);
    handleKukushka(command, db, "k", ss);
}

TEST(KukushkaHandlerTest, AllCommands)
{
    json db = boost::json::object();

    testing::internal::CaptureStdout();
    runKuk(db, Command::KADD, "1");
    runKuk(db, Command::KADD, "12");
    runKuk(db, Command::KADD, "7");
    runKuk(db, Command::KDEL, "7");
    runKuk(db, Command::KFIND, "1");
    runKuk(db, Command::KFIND, "7");
    runKuk(db, Command::KPRINT, "");
    runKuk(db, Command::PRINT, "");          // чужая команда
    string out = testing::internal::GetCapturedStdout();

    EXPECT_NE(out.find("-> TRUE"), string::npos);
    EXPECT_NE(out.find("-> FALSE"), string::npos);
    EXPECT_EQ(boost::json::value_to<string>(entry(db, "k")["type"]), "kukushka");
    EXPECT_EQ(entry(db, "k")["data"].as_array().size(), 2u);
}

TEST(KukushkaHandlerTest, WrongTypeIsRejected)
{
    json db = boost::json::object();
    entry(db, "k")["type"] = "hash";
    entry(db, "k")["data"] = boost::json::array();

    testing::internal::CaptureStdout();
    runKuk(db, Command::KADD, "5");
    testing::internal::GetCapturedStdout();

    EXPECT_EQ(boost::json::value_to<string>(entry(db, "k")["type"]), "hash");
}
