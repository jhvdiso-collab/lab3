package main

import (
	"os"
	"path/filepath"
	"testing"
)

func TestCLI(t *testing.T) {
	for _, format := range []string{"json", "binary"} {
		file := filepath.Join(t.TempDir(), "data")
		exec := func(q string) string {
			var rc int
			out := captureOutput(func() { rc = run([]string{"dbms", "--file", file, "--format", format, "--query", q}) })
			if rc != 0 {
				t.Fatalf("%s: exit %d: %s", q, rc, out)
			}
			return out
		}
		for _, q := range []string{"MPUSH a A", "MINSERT a 0 B", "MSET a 1 C", "FPUSHH f A", "FPUSHAFTER f A B", "FPUSHBEFORE f A Z", "FDELBEFORE f A", "FDELAFTER f A", "LPUSHH l A", "LPUSHAFTER l A B", "LPUSHBEFORE l A Z", "LDELBEFORE l A", "LDELAFTER l A", "SPUSH s S", "QPUSH q Q", "HADD h 105", "HADD h 5", "KADD k 12", "TINSERT t 30", "TINSERT t 10", "TINSERT t 20"} {
			exec(q)
		}
		for _, item := range []struct{ query, want string }{{"PRINT a", "B C"}, {"FGETFRONT f", "A"}, {"FGETBACK f", "A"}, {"LGETFRONT l", "A"}, {"LGETBACK l", "A"}, {"PRINT f", "A"}, {"PRINT l", "A"}, {"PRINT s", "S"}, {"PRINT q", "Q"}, {"PRINT h", "105"}, {"PRINT k", "12"}, {"PRINT t", "20 10 30"}, {"PRINT missing", "нет структуры"}, {"FGETFRONT empty", "список пуст"}, {"LGETBACK empty2", "список пуст"}} {
			checkContains(t, exec(item.query), item.want)
		}
		exec("TDEL t 20")
		checkContains(t, exec("TFIND t 20"), "FALSE")
		exec("SPOP s")
		exec("QPOP q")
		if format == "binary" {
			_, err := os.Stat(file + ".t.bin")
			if err != nil {
				t.Fatal(err)
			}
		} else {
			data, err := os.ReadFile(file)
			if err != nil {
				t.Fatal(err)
			}
			checkContains(t, string(data), `"type": "tree"`)
		}
	}
}

func TestCLIInvalid(t *testing.T) {
	file := filepath.Join(t.TempDir(), "data")
	for _, argv := range [][]string{{"dbms"}, {"dbms", "--bad", file, "--query", "PRINT a"}, {"dbms", "--file", file, "--bad", "PRINT a"}, {"dbms", "--file", file, "--format", "bad", "--query", "PRINT a"}} {
		checkEqual(t, 1, run(argv))
	}
	for _, q := range []string{"BOGUS a", "MPUSH", "MGET a abc", "MPUSH a", "MSET a 0", "PRINT a extra", "MPUSH a/b A", "TINSERT t 2147483648", "MGET a 1x"} {
		checkEqual(t, 1, run([]string{"dbms", "--file", file, "--query", q}))
	}
	if _, err := os.Stat(file); !os.IsNotExist(err) {
		t.Fatal("invalid command created a file")
	}
	if err := os.WriteFile(file, []byte("broken"), 0600); err != nil {
		t.Fatal(err)
	}
	checkEqual(t, 1, run([]string{"dbms", "--file", file, "--query", "PRINT a"}))
}
