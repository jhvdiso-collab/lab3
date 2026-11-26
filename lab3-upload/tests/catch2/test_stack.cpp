#include <catch2/catch_test_macros.hpp>
#include "capture.h"
#include "stack.h"

// конструктор
TEST_CASE("StackTest.NewStackIsEmpty")
{
    Stack st;
    CHECK(st.isEmptyStack());
}

// push и top
TEST_CASE("StackTest.TopIsLastPushed")
{
    Stack st;
    st.push("1");
    st.push("2");

    CHECK_FALSE(st.isEmptyStack());
    CHECK(st.top() == "2");
}

// pop
TEST_CASE("StackTest.PopRemovesTop")
{
    Stack st;
    st.push("1");
    st.push("2");
    st.pop();

    CHECK(st.top() == "1");
}

TEST_CASE("StackTest.PopAndTopOnEmpty")
{
    Stack st;
    st.pop();                        // на пустом стеке ничего не ломается
    CHECK(st.isEmptyStack());
    CHECK(st.top() == "");
}

// print
TEST_CASE("StackTest.PrintFromTop")
{
    Stack st;
    st.push("1");
    st.push("2");

    startCapture();
    st.print();
    string out = stopCapture();

    CHECK(out == "2 -> 1 -> nullptr\n");
}

// handleStack
static void run(json& db, Command command, const string& args)
{
    stringstream ss(args);
    handleStack(command, db, "st", ss);
}

TEST_CASE("StackHandlerTest.AllCommands")
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

    CHECK(out.find("-> 3") != string::npos);
    CHECK(boost::json::value_to<string>(entry(db, "st")["type"]) == "stack");
    CHECK(entry(db, "st")["data"].as_array().size() == 2u);
    CHECK(boost::json::value_to<string>(entry(db, "st")["data"].as_array()[0]) == "1");   // в json снизу вверх
    CHECK(boost::json::value_to<string>(entry(db, "st")["data"].as_array()[1]) == "2");
}

TEST_CASE("StackHandlerTest.PopEmpty")
{
    json db = boost::json::object();

    startCapture();
    run(db, Command::SPOP, "");
    string out = stopCapture();

    CHECK(out.find("стек пуст") != string::npos);
}

TEST_CASE("StackHandlerTest.WrongTypeIsRejected")
{
    json db = boost::json::object();
    entry(db, "st")["type"] = "queue";
    entry(db, "st")["data"] = boost::json::array();

    startCapture();
    run(db, Command::SPUSH, "1");
    stopCapture();

    CHECK(boost::json::value_to<string>(entry(db, "st")["type"]) == "queue");
}
