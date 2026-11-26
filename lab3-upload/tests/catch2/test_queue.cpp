#include <catch2/catch_test_macros.hpp>
#include "capture.h"
#include "queue.h"

// конструктор
TEST_CASE("QueueTest.NewQueueIsEmpty")
{
    Queue q;
    CHECK(q.isEmptyQ());
}

// push и top
TEST_CASE("QueueTest.TopIsFirstPushed")
{
    Queue q;
    q.push("1");
    q.push("2");

    CHECK_FALSE(q.isEmptyQ());
    CHECK(q.top() == "1");         // первым пришел - первым выйдет
}

// pop
TEST_CASE("QueueTest.PopRemovesFirst")
{
    Queue q;
    q.push("1");
    q.push("2");
    q.pop();

    CHECK(q.top() == "2");
}

TEST_CASE("QueueTest.PopLastMakesEmpty")
{
    Queue q;
    q.push("1");
    q.pop();
    CHECK(q.isEmptyQ());

    q.push("2");                     // после опустошения снова работает
    CHECK(q.top() == "2");
}

TEST_CASE("QueueTest.PopAndTopOnEmpty")
{
    Queue q;
    q.pop();
    CHECK(q.isEmptyQ());
    CHECK(q.top() == "");
}

// printQ
TEST_CASE("QueueTest.PrintFromHead")
{
    Queue q;
    q.push("1");
    q.push("2");

    startCapture();
    q.printQ();
    string out = stopCapture();

    CHECK(out == "1 -> 2 -> nullptr\n");
}

// handleQueue
static void run(json& db, Command command, const string& args)
{
    stringstream ss(args);
    handleQueue(command, db, "q", ss);
}

TEST_CASE("QueueHandlerTest.AllCommands")
{
    json db = boost::json::object();

    startCapture();
    run(db, Command::QPUSH, "1");
    run(db, Command::QPUSH, "2");
    run(db, Command::QPUSH, "3");
    run(db, Command::QPOP, "");            // выходит 1
    run(db, Command::QPRINT, "");
    run(db, Command::PRINT, "");           // чужая команда - ничего не делает
    string out = stopCapture();

    CHECK(out.find("-> 1") != string::npos);
    CHECK(boost::json::value_to<string>(entry(db, "q")["type"]) == "queue");
    CHECK(entry(db, "q")["data"].as_array().size() == 2u);
    CHECK(boost::json::value_to<string>(entry(db, "q")["data"].as_array()[0]) == "2");
    CHECK(boost::json::value_to<string>(entry(db, "q")["data"].as_array()[1]) == "3");
}

TEST_CASE("QueueHandlerTest.PopEmpty")
{
    json db = boost::json::object();

    startCapture();
    run(db, Command::QPOP, "");
    string out = stopCapture();

    CHECK(out.find("очередь пуста") != string::npos);
}

TEST_CASE("QueueHandlerTest.WrongTypeIsRejected")
{
    json db = boost::json::object();
    entry(db, "q")["type"] = "stack";
    entry(db, "q")["data"] = boost::json::array();

    startCapture();
    run(db, Command::QPUSH, "1");
    stopCapture();

    CHECK(boost::json::value_to<string>(entry(db, "q")["type"]) == "stack");
}
