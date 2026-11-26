package main

import (
	"encoding/json"
	"fmt"
	"os"
	"strconv"
	"strings"
)

// handlePrint печатает структуру любого типа по ее имени.
func handlePrint(db DB, name string, ss *strings.Reader) {
	entry, ok := db[name]
	if !ok {
		fmt.Println("нет структуры", name)
		return
	}

	switch entry.Type {
	case "array":
		HandleArray(MPRINT, db, name, ss)
	case "flist":
		HandleFlist(FPRINT, db, name, ss)
	case "dlist":
		HandleDlist(LPRINT, db, name, ss)
	case "stack":
		HandleStack(SPRINT, db, name, ss)
	case "queue":
		HandleQueue(QPRINT, db, name, ss)
	case "tree":
		HandleTree(TPRINT, db, name, ss)
	case "hash":
		HandleHash(HPRINT, db, name, ss)
	case "kukushka":
		HandleKukushka(KPRINT, db, name, ss)
	}
}

func main() { os.Exit(run(os.Args)) }

func run(argv []string) int {
	if len(argv) != 5 && len(argv) != 7 {
		fmt.Println("Использование: ./dbms --file <файл> [--format json|binary] --query '<команда>'")
		return 1
	}
	if argv[1] != "--file" {
		fmt.Println("неверные параметры")
		return 1
	}

	fileName := argv[2]
	format := "json"
	var query string
	if len(argv) == 5 {
		if argv[3] != "--query" {
			fmt.Println("неверные параметры")
			return 1
		}
		query = argv[4]
	} else {
		if argv[3] != "--format" || (argv[4] != "json" && argv[4] != "binary") || argv[5] != "--query" {
			fmt.Println("неверные параметры")
			return 1
		}
		format = argv[4]
		query = argv[6]
	}

	binary := format == "binary"
	SetBinaryMode(binary, fileName)
	dbFile := fileName
	if binary {
		dbFile += ".meta.json"
	}

	db := DB{}
	content, err := os.ReadFile(dbFile)
	if err == nil && len(content) > 0 {
		if err := json.Unmarshal(content, &db); err != nil {
			fmt.Println("ошибка:", err)
			return 1
		}
	} else if err != nil && !os.IsNotExist(err) {
		fmt.Println("ошибка:", err)
		return 1
	}

	ss := strings.NewReader(query)
	var cmd, name string
	fmt.Fscan(ss, &cmd, &name)

	command := ToCommand(cmd)
	if command == UNKNOWN {
		fmt.Println("неизвестная команда")
		return 1
	}
	if name == "" || strings.ContainsAny(name, `/\`) {
		fmt.Println("укажи имя структуры")
		return 1
	}

	if !validArgs(command, strings.Fields(query)[2:]) {
		fmt.Println("неверные аргументы")
		return 1
	}
	if db == nil {
		fmt.Println("некорректная база")
		return 1
	}
	if entry, ok := db[name]; ok && entry == nil {
		fmt.Println("некорректная запись")
		return 1
	}
	switch cmd[0] {
	case 'M':
		HandleArray(command, db, name, ss)
	case 'F':
		HandleFlist(command, db, name, ss)
	case 'L':
		HandleDlist(command, db, name, ss)
	case 'S':
		HandleStack(command, db, name, ss)
	case 'Q':
		HandleQueue(command, db, name, ss)
	case 'T':
		HandleTree(command, db, name, ss)
	case 'H':
		HandleHash(command, db, name, ss)
	case 'K':
		HandleKukushka(command, db, name, ss)
	case 'P':
		handlePrint(db, name, ss)
	default:
		fmt.Println("неизвестная команда")
	}

	out, err := json.MarshalIndent(db, "", "    ")
	if err != nil {
		fmt.Println("ошибка:", err)
		return 1
	}
	if err := os.WriteFile(dbFile, append(out, '\n'), 0644); err != nil {
		fmt.Println("ошибка:", err)
		return 1
	}
	return 0
}

// validArgs checks arity and numeric input (32-bit, the same as C++ binary).
func validArgs(command Command, args []string) bool {
	count, numeric := 0, false
	switch command {
	case MINSERT, MSET:
		count, numeric = 2, true
	case MGET, MDEL, TINSERT, TDEL, TFIND, HADD, HDEL, HFIND, KADD, KDEL, KFIND:
		count, numeric = 1, true
	case MPUSH, FPUSHH, FPUSHT, FDELA, FDELB, FDEL, FFIND, LPUSHH, LPUSHT, LDELA, LDELB, LDEL, LFIND, SPUSH, QPUSH:
		count = 1
	case FPUSHA, FPUSHB, LPUSHA, LPUSHB:
		count = 2
	}
	if len(args) != count {
		return false
	}
	if numeric {
		_, err := strconv.ParseInt(args[0], 10, 32)
		return err == nil
	}
	return true
}
