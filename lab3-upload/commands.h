#pragma once
#include <iostream>
#include <string>
#include <sstream>
#include <boost/json.hpp>
using namespace std;
using json = boost::json::value;

inline boost::json::object& entry(json& db, const string& name)
{
    auto& value = db.as_object()[name];
    if (value.is_null()) value.emplace_object();
    return value.as_object();
}

enum class Command
{
    MPUSH, MINSERT, MGET, MSET, MDEL, MLEN, MPRINT,
    FPUSHH, FPUSHT, FPUSHA, FPUSHB, FDELH, FDELT, FDELA, FDELB, FDEL, FFIND, FPRINT, FPRINTR, FGETFRONT, FGETBACK,
    LPUSHH, LPUSHT, LPUSHA, LPUSHB, LDELH, LDELT, LDELA, LDELB, LDEL, LFIND, LPRINT, LPRINTR, LGETFRONT, LGETBACK,
    SPUSH, SPOP, SPRINT,
    QPUSH, QPOP, QPRINT,
    TINSERT, TDEL, TFIND, TCOMPLETE, TPRINT,
    HADD, HDEL, HFIND, HPRINT,
    KADD, KDEL, KFIND, KPRINT,
    PRINT,
    UNKNOWN
};

Command toCommand(const string& text);
bool wrongType(json& db, const string& name, const string& type);
extern bool binaryMode;
extern string binaryBase;
string binaryFile(const string& name);
