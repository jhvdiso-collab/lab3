#include "array.h"
#include "singly_list.h"
#include "doubly_list.h"
#include "stack.h"
#include "queue.h"
#include "avl_tree.h"
#include "hash_table.h"
#include <fstream>
#include <string>
#include <iostream>
#include <vector>
#include <charconv>

void handlePrint(json& db, const string& name, stringstream& ss)
{
    if (!db.as_object().contains(name))
    {
        cout << "нет структуры " << name << "\n";
        return;
    }

    if (entry(db, name)["type"] == "array") handleArray(Command::MPRINT, db, name, ss);
    else if (entry(db, name)["type"] == "flist") handleFlist(Command::FPRINT, db, name, ss);
    else if (entry(db, name)["type"] == "dlist") handleDlist(Command::LPRINT, db, name, ss);
    else if (entry(db, name)["type"] == "stack") handleStack(Command::SPRINT, db, name, ss);
    else if (entry(db, name)["type"] == "queue") handleQueue(Command::QPRINT, db, name, ss);
    else if (entry(db, name)["type"] == "tree") handleTree(Command::TPRINT, db, name, ss);
    else if (entry(db, name)["type"] == "hash") handleHash(Command::HPRINT, db, name, ss);
    else if (entry(db, name)["type"] == "kukushka") handleKukushka(Command::KPRINT, db, name, ss);
}

// Проверка числа аргументов и целых чисел до изменения базы.
bool validArgs(Command command, stringstream& ss)
{
    int count = 0;
    bool numeric = false;
    switch (command)
    {
        case Command::MINSERT: case Command::MSET: count = 2; numeric = true; break;
        case Command::MGET: case Command::MDEL: case Command::TINSERT: case Command::TDEL:
        case Command::TFIND: case Command::HADD: case Command::HDEL: case Command::HFIND:
        case Command::KADD: case Command::KDEL: case Command::KFIND: count = 1; numeric = true; break;
        case Command::MPUSH: case Command::FPUSHH: case Command::FPUSHT: case Command::FDELA:
        case Command::FDELB: case Command::FDEL: case Command::FFIND: case Command::LPUSHH:
        case Command::LPUSHT: case Command::LDELA: case Command::LDELB: case Command::LDEL:
        case Command::LFIND: case Command::SPUSH: case Command::QPUSH: count = 1; break;
        case Command::FPUSHA: case Command::FPUSHB: case Command::LPUSHA: case Command::LPUSHB: count = 2; break;
        default: break;
    }
    stringstream copy(ss.str());
    copy.seekg(ss.tellg());
    if (ss.eof()) return count == 0;
    vector<string> args;
    string arg;
    while (copy >> arg) args.push_back(arg);
    if (args.size() != static_cast<size_t>(count)) return false;
    if (numeric)
    {
        int value;
        stringstream number(args[0]);
        if (!(number >> value) || !number.eof()) return false;
    }
    return true;
}

int main(int argc, char* argv[])
{
    try
    {
    if (argc != 5 && argc != 7)
    {
        cout << "Использование: ./dbms --file <файл> [--format json|binary] --query '<команда>'\n";
        return 1;
    }

    if (string(argv[1]) != "--file")
    {
        cout << "неверные параметры\n";
        return 1;
    }

    string fileName = argv[2];
    string query;
    if (argc == 5)
    {
        if (string(argv[3]) != "--query")
        {
            cout << "неверные параметры\n";
            return 1;
        }
        query = argv[4];
        binaryMode = true;
    }
    else
    {
        if (string(argv[3]) != "--format" ||
            (string(argv[4]) != "json" && string(argv[4]) != "binary") ||
            string(argv[5]) != "--query")
        {
            cout << "неверные параметры\n";
            return 1;
        }
        binaryMode = (string(argv[4]) == "binary");
        query = argv[6];
    }

    binaryBase = fileName;
    string dbFile = binaryMode ? fileName + ".meta.json" : fileName;

    json db = boost::json::object();
    ifstream in(dbFile);
    if (in.is_open() && in.peek() != EOF)
    {
        db = boost::json::parse(in);
    }
    in.close();

    stringstream ss(query);
    string cmd;
    string name;
    if (!(ss >> cmd >> name)) { cout << "неверные аргументы\n"; return 1; }

    Command command = toCommand(cmd);
    if (command == Command::UNKNOWN)
    {
        cout << "неизвестная команда\n";
        return 1;
    }
    if (name.empty() || name.find_first_of("/\\") != string::npos)
    {
        cout << "укажи имя структуры\n";
        return 1;
    }

    if (!validArgs(command, ss)) { cout << "неверные аргументы\n"; return 1; }
    if (!db.is_object()) { cout << "некорректная база\n"; return 1; }
    switch (cmd[0])
    {
        case 'M': handleArray(command, db, name, ss);    break;
        case 'F': handleFlist(command, db, name, ss);    break;
        case 'L': handleDlist(command, db, name, ss);    break;
        case 'S': handleStack(command, db, name, ss);    break;
        case 'Q': handleQueue(command, db, name, ss);    break;
        case 'T': handleTree(command, db, name, ss);     break;
        case 'H': handleHash(command, db, name, ss);     break;
        case 'K': handleKukushka(command, db, name, ss); break;
        case 'P': handlePrint(db, name, ss);             break;
        default: cout << "неизвестная команда\n"; break;
    }

    ofstream out(dbFile);
    if (!out) { cerr << "не удалось открыть файл базы\n"; return 1; }
    out << boost::json::serialize(db) << "\n";
    out.close();
    if (!out) { cerr << "ошибка записи базы\n"; return 1; }
    }
    catch(const std::exception& e)
    {
        std::cerr << e.what() << '\n';
        return 1;
    }
    return 0;
}