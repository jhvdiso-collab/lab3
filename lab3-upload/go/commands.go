package main

import (
	"encoding/json"
	"fmt"
	"strings"
)

var binaryMode bool
var binaryBase string

// SetBinaryMode включает или выключает бинарное хранение структур.
func SetBinaryMode(enabled bool, base string) {
	binaryMode = enabled
	binaryBase = base
}

// binaryFile возвращает имя бинарного файла отдельной структуры.
func binaryFile(name string) string {
	safe := strings.NewReplacer("/", "_", "\\", "_").Replace(name)
	return binaryBase + "." + safe + ".bin"
}

// Command - команда, которую ввел пользователь.
type Command int

const (
	MPUSH Command = iota
	MINSERT
	MGET
	MSET
	MDEL
	MLEN
	MPRINT

	FPUSHH
	FPUSHT
	FPUSHA
	FPUSHB
	FDELH
	FDELT
	FDELA
	FDELB
	FDEL
	FFIND
	FPRINT
	FPRINTR
	FGETFRONT
	FGETBACK

	LPUSHH
	LPUSHT
	LPUSHA
	LPUSHB
	LDELH
	LDELT
	LDELA
	LDELB
	LDEL
	LFIND
	LPRINT
	LPRINTR
	LGETFRONT
	LGETBACK

	SPUSH
	SPOP
	SPRINT

	QPUSH
	QPOP
	QPRINT

	TINSERT
	TDEL
	TFIND
	TCOMPLETE
	TPRINT

	HADD
	HDEL
	HFIND
	HPRINT

	KADD
	KDEL
	KFIND
	KPRINT

	PRINT
	UNKNOWN
)

// commandNames связывает текст команды с ее значением.
var commandNames = map[string]Command{
	"MPUSH": MPUSH, "MINSERT": MINSERT, "MGET": MGET, "MSET": MSET,
	"MDEL": MDEL, "MLEN": MLEN, "MPRINT": MPRINT,

	"FPUSHH": FPUSHH, "FPUSHT": FPUSHT, "FPUSHA": FPUSHA, "FPUSHAFTER": FPUSHA, "FPUSHB": FPUSHB, "FPUSHBEFORE": FPUSHB,
	"FDELH": FDELH, "FDELT": FDELT, "FDELA": FDELA, "FDELAFTER": FDELA, "FDELB": FDELB, "FDELBEFORE": FDELB,
	"FDEL": FDEL, "FFIND": FFIND, "FPRINT": FPRINT, "FPRINTR": FPRINTR, "FGETFRONT": FGETFRONT, "FGETBACK": FGETBACK,

	"LPUSHH": LPUSHH, "LPUSHT": LPUSHT, "LPUSHA": LPUSHA, "LPUSHAFTER": LPUSHA, "LPUSHB": LPUSHB, "LPUSHBEFORE": LPUSHB,
	"LDELH": LDELH, "LDELT": LDELT, "LDELA": LDELA, "LDELAFTER": LDELA, "LDELB": LDELB, "LDELBEFORE": LDELB,
	"LDEL": LDEL, "LFIND": LFIND, "LPRINT": LPRINT, "LPRINTR": LPRINTR, "LGETFRONT": LGETFRONT, "LGETBACK": LGETBACK,

	"SPUSH": SPUSH, "SPOP": SPOP, "SPRINT": SPRINT,

	"QPUSH": QPUSH, "QPOP": QPOP, "QPRINT": QPRINT,

	"TINSERT": TINSERT, "TDEL": TDEL, "TFIND": TFIND, "TCOMPLETE": TCOMPLETE, "TPRINT": TPRINT,

	"HADD": HADD, "HDEL": HDEL, "HFIND": HFIND, "HPRINT": HPRINT,

	"KADD": KADD, "KDEL": KDEL, "KFIND": KFIND, "KPRINT": KPRINT,

	"PRINT": PRINT,
}

// ToCommand превращает текст команды в Command.
// Если такой команды нет, возвращает UNKNOWN.
func ToCommand(text string) Command {
	if command, ok := commandNames[text]; ok {
		return command
	}
	return UNKNOWN
}

// Entry - одна структура в файле базы: ее тип и данные.
type Entry struct {
	Type string          `json:"type"`
	Data json.RawMessage `json:"data,omitempty"`
}

// DB - вся база: имя структуры и ее запись.
type DB map[string]*Entry

// WrongType проверяет, что под этим именем не лежит структура другого типа.
func WrongType(db DB, name, typ string) bool {
	entry, ok := db[name]
	if ok && entry.Type != typ {
		fmt.Println(name, "это не", typ)
		return true
	}
	return false
}

// loadStrings достает из базы данные структуры, которая хранит строки.
func loadStrings(db DB, name string) []string {
	var items []string
	if entry, ok := db[name]; ok {
		_ = json.Unmarshal(entry.Data, &items)
	}
	return items
}

// loadInts достает из базы данные структуры, которая хранит числа.
func loadInts(db DB, name string) []int {
	var items []int
	if entry, ok := db[name]; ok {
		_ = json.Unmarshal(entry.Data, &items)
	}
	return items
}

// saveStrings записывает структуру со строками в базу.
func saveStrings(db DB, name, typ string, items []string) {
	if items == nil {
		items = []string{}
	}
	data, _ := json.Marshal(items)
	db[name] = &Entry{Type: typ, Data: data}
}

// saveInts записывает структуру с числами в базу.
func saveInts(db DB, name, typ string, items []int) {
	if items == nil {
		items = []int{}
	}
	data, _ := json.Marshal(items)
	db[name] = &Entry{Type: typ, Data: data}
}
