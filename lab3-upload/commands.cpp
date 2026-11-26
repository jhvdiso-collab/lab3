#include "commands.h"
#include <algorithm>
bool binaryMode = false;
string binaryBase;
string binaryFile(const string& name) { string safe = name; for (char& c : safe) if (c == '/' || c == '\\') c = '_'; return binaryBase + "." + safe + ".bin"; }


Command toCommand(const string& text)
{
    if (text == "MPUSH") return Command::MPUSH;
    if (text == "MINSERT") return Command::MINSERT;
    if (text == "MGET") return Command::MGET;
    if (text == "MSET") return Command::MSET;
    if (text == "MDEL") return Command::MDEL;
    if (text == "MLEN") return Command::MLEN;
    if (text == "MPRINT") return Command::MPRINT;

    if (text == "FPUSHH") return Command::FPUSHH;
    if (text == "FPUSHT") return Command::FPUSHT;
    if (text == "FPUSHA" || text == "FPUSHAFTER") return Command::FPUSHA;
    if (text == "FPUSHB" || text == "FPUSHBEFORE") return Command::FPUSHB;
    if (text == "FDELH") return Command::FDELH;
    if (text == "FDELT") return Command::FDELT;
    if (text == "FDELA" || text == "FDELAFTER") return Command::FDELA;
    if (text == "FDELB" || text == "FDELBEFORE") return Command::FDELB;
    if (text == "FDEL") return Command::FDEL;
    if (text == "FFIND") return Command::FFIND;
    if (text == "FPRINT") return Command::FPRINT;
    if (text == "FGETFRONT") return Command::FGETFRONT;
    if (text == "FGETBACK") return Command::FGETBACK;
    if (text == "FPRINTR") return Command::FPRINTR;

    if (text == "LPUSHH") return Command::LPUSHH;
    if (text == "LPUSHT") return Command::LPUSHT;
    if (text == "LPUSHA" || text == "LPUSHAFTER") return Command::LPUSHA;
    if (text == "LPUSHB" || text == "LPUSHBEFORE") return Command::LPUSHB;
    if (text == "LDELH") return Command::LDELH;
    if (text == "LDELT") return Command::LDELT;
    if (text == "LDELA" || text == "LDELAFTER") return Command::LDELA;
    if (text == "LDELB" || text == "LDELBEFORE") return Command::LDELB;
    if (text == "LDEL") return Command::LDEL;
    if (text == "LFIND") return Command::LFIND;
    if (text == "LPRINT") return Command::LPRINT;
    if (text == "LGETFRONT") return Command::LGETFRONT;
    if (text == "LGETBACK") return Command::LGETBACK;
    if (text == "LPRINTR") return Command::LPRINTR;

    if (text == "SPUSH") return Command::SPUSH;
    if (text == "SPOP") return Command::SPOP;
    if (text == "SPRINT") return Command::SPRINT;

    if (text == "QPUSH") return Command::QPUSH;
    if (text == "QPOP") return Command::QPOP;
    if (text == "QPRINT") return Command::QPRINT;

    if (text == "TINSERT") return Command::TINSERT;
    if (text == "TDEL") return Command::TDEL;
    if (text == "TFIND") return Command::TFIND;
    if (text == "TCOMPLETE") return Command::TCOMPLETE;
    if (text == "TPRINT") return Command::TPRINT;

    if (text == "HADD") return Command::HADD;
    if (text == "HDEL") return Command::HDEL;
    if (text == "HFIND") return Command::HFIND;
    if (text == "HPRINT") return Command::HPRINT;

    if (text == "KADD") return Command::KADD;
    if (text == "KDEL") return Command::KDEL;
    if (text == "KFIND") return Command::KFIND;
    if (text == "KPRINT") return Command::KPRINT;
    
    if (text == "PRINT") return Command::PRINT;
    return Command::UNKNOWN;
}

bool wrongType(json& db, const string& name, const string& type)
{
    if (db.as_object().contains(name) && entry(db, name)["type"] != type.c_str())
    {
        cout << name << " это не " << type << "\n";
        return true;
    }
    return false;
}
