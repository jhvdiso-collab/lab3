#include <boost/test/unit_test.hpp>
#include "capture.h"
#include "stack.h"

// конструктор
BOOST_AUTO_TEST_CASE(StackTest_NewStackIsEmpty)
{
    Stack st;
    BOOST_CHECK(st.isEmptyStack());
}

// push и top
BOOST_AUTO_TEST_CASE(StackTest_TopIsLastPushed)
{
    Stack st;
    st.push("1");
    st.push("2");

    BOOST_CHECK(!st.isEmptyStack());
    BOOST_CHECK_EQUAL(st.top(), "2");
}

// pop
BOOST_AUTO_TEST_CASE(StackTest_PopRemovesTop)
{
    Stack st;
    st.push("1");
    st.push("2");
    st.pop();

    BOOST_CHECK_EQUAL(st.top(), "1");
}

BOOST_AUTO_TEST_CASE(StackTest_PopAndTopOnEmpty)
{
    Stack st;
    st.pop();                        // на пустом стеке ничего не ломается
    BOOST_CHECK(st.isEmptyStack());
    BOOST_CHECK_EQUAL(st.top(), "");
}

// print
BOOST_AUTO_TEST_CASE(StackTest_PrintFromTop)
{
    Stack st;
    st.push("1");
    st.push("2");

    startCapture();
    st.print();
    string out = stopCapture();

    BOOST_CHECK(out == "2 -> 1 -> nullptr\n");
}

// handleStack
static void run(json& db, Command command, const string& args)
{
    stringstream ss(args);
    handleStack(command, db, "st", ss);
}

BOOST_AUTO_TEST_CASE(StackHandlerTest_AllCommands)
{
    json db = boost::json::object();

    startCapture();
    run(db, Command::SPUSH, "1");
    run(db, Command::SPUSH, "2");
    run(db, Command::SPUSH, "3");
    run(db, Command::SPOP, "");            // снимает 3
    run(db, Command::SPRINT, "");
    run(db, Command::PRINT, "");           // чужая команда - ничего не делает
    string out = stopCapture();

    BOOST_CHECK_NE(out.find("-> 3"), string::npos);
    BOOST_CHECK_EQUAL(boost::json::value_to<string>(entry(db, "st")["type"]), "stack");
    BOOST_CHECK_EQUAL(entry(db, "st")["data"].as_array().size(), 2u);
    BOOST_CHECK_EQUAL(boost::json::value_to<string>(entry(db, "st")["data"].as_array()[0]), "1");   // в json снизу вверх
    BOOST_CHECK_EQUAL(boost::json::value_to<string>(entry(db, "st")["data"].as_array()[1]), "2");
}

BOOST_AUTO_TEST_CASE(StackHandlerTest_PopEmpty)
{
    json db = boost::json::object();

    startCapture();
    run(db, Command::SPOP, "");
    string out = stopCapture();

    BOOST_CHECK_NE(out.find("стек пуст"), string::npos);
}

BOOST_AUTO_TEST_CASE(StackHandlerTest_WrongTypeIsRejected)
{
    json db = boost::json::object();
    entry(db, "st")["type"] = "queue";
    entry(db, "st")["data"] = boost::json::array();

    startCapture();
    run(db, Command::SPUSH, "1");
    stopCapture();

    BOOST_CHECK_EQUAL(boost::json::value_to<string>(entry(db, "st")["type"]), "queue");
}
