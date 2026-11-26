#include <boost/test/unit_test.hpp>
#include "capture.h"
#include "queue.h"

// конструктор
BOOST_AUTO_TEST_CASE(QueueTest_NewQueueIsEmpty)
{
    Queue q;
    BOOST_CHECK(q.isEmptyQ());
}

// push и top
BOOST_AUTO_TEST_CASE(QueueTest_TopIsFirstPushed)
{
    Queue q;
    q.push("1");
    q.push("2");

    BOOST_CHECK(!q.isEmptyQ());
    BOOST_CHECK_EQUAL(q.top(), "1");         // первым пришел - первым выйдет
}

// pop
BOOST_AUTO_TEST_CASE(QueueTest_PopRemovesFirst)
{
    Queue q;
    q.push("1");
    q.push("2");
    q.pop();

    BOOST_CHECK_EQUAL(q.top(), "2");
}

BOOST_AUTO_TEST_CASE(QueueTest_PopLastMakesEmpty)
{
    Queue q;
    q.push("1");
    q.pop();
    BOOST_CHECK(q.isEmptyQ());

    q.push("2");                     // после опустошения снова работает
    BOOST_CHECK_EQUAL(q.top(), "2");
}

BOOST_AUTO_TEST_CASE(QueueTest_PopAndTopOnEmpty)
{
    Queue q;
    q.pop();
    BOOST_CHECK(q.isEmptyQ());
    BOOST_CHECK_EQUAL(q.top(), "");
}

// printQ
BOOST_AUTO_TEST_CASE(QueueTest_PrintFromHead)
{
    Queue q;
    q.push("1");
    q.push("2");

    startCapture();
    q.printQ();
    string out = stopCapture();

    BOOST_CHECK(out == "1 -> 2 -> nullptr\n");
}

// handleQueue
static void run(json& db, Command command, const string& args)
{
    stringstream ss(args);
    handleQueue(command, db, "q", ss);
}

BOOST_AUTO_TEST_CASE(QueueHandlerTest_AllCommands)
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

    BOOST_CHECK_NE(out.find("-> 1"), string::npos);
    BOOST_CHECK_EQUAL(boost::json::value_to<string>(entry(db, "q")["type"]), "queue");
    BOOST_CHECK_EQUAL(entry(db, "q")["data"].as_array().size(), 2u);
    BOOST_CHECK_EQUAL(boost::json::value_to<string>(entry(db, "q")["data"].as_array()[0]), "2");
    BOOST_CHECK_EQUAL(boost::json::value_to<string>(entry(db, "q")["data"].as_array()[1]), "3");
}

BOOST_AUTO_TEST_CASE(QueueHandlerTest_PopEmpty)
{
    json db = boost::json::object();

    startCapture();
    run(db, Command::QPOP, "");
    string out = stopCapture();

    BOOST_CHECK_NE(out.find("очередь пуста"), string::npos);
}

BOOST_AUTO_TEST_CASE(QueueHandlerTest_WrongTypeIsRejected)
{
    json db = boost::json::object();
    entry(db, "q")["type"] = "stack";
    entry(db, "q")["data"] = boost::json::array();

    startCapture();
    run(db, Command::QPUSH, "1");
    stopCapture();

    BOOST_CHECK_EQUAL(boost::json::value_to<string>(entry(db, "q")["type"]), "stack");
}
