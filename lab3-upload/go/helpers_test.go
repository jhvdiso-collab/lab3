package main

import (
	"io"
	"os"
	"reflect"
	"strings"
	"testing"
)

// captureOutput перехватывает все, что функция f печатает в консоль,
// и возвращает это строкой. Нужна, чтобы проверять печать
// и чтобы сообщения об ошибках не мусорили в выводе тестов.
func captureOutput(f func()) string {
	old := os.Stdout
	r, w, _ := os.Pipe()
	os.Stdout = w

	f()

	w.Close()
	os.Stdout = old
	out, _ := io.ReadAll(r)
	return string(out)
}

// args превращает аргументы команды в поток, так же как это делает main.
func args(s string) *strings.Reader {
	return strings.NewReader(s)
}

func checkEqual(t *testing.T, want, got any, _ ...any) {
	t.Helper()
	if !reflect.DeepEqual(want, got) {
		t.Errorf("want %#v, got %#v", want, got)
	}
}
func checkTrue(t *testing.T, got bool) {
	t.Helper()
	if !got {
		t.Error("expected true")
	}
}
func checkFalse(t *testing.T, got bool) {
	t.Helper()
	if got {
		t.Error("expected false")
	}
}
func isNil(v any) bool {
	if v == nil {
		return true
	}
	x := reflect.ValueOf(v)
	switch x.Kind() {
	case reflect.Pointer, reflect.Map, reflect.Slice, reflect.Interface, reflect.Chan, reflect.Func:
		return x.IsNil()
	}
	return false
}
func checkNil(t *testing.T, v any) {
	t.Helper()
	if !isNil(v) {
		t.Errorf("expected nil, got %v", v)
	}
}
func checkNotNil(t *testing.T, v any) {
	t.Helper()
	if isNil(v) {
		t.Fatal("expected non-nil")
	}
}
func checkContains(t *testing.T, got, want string) {
	t.Helper()
	if !strings.Contains(got, want) {
		t.Errorf("%q does not contain %q", got, want)
	}
}
func checkLen(t *testing.T, v any, n int) {
	t.Helper()
	if reflect.ValueOf(v).Len() != n {
		t.Errorf("expected length %d, got %v", n, v)
	}
}
func checkEmpty(t *testing.T, v any) { t.Helper(); checkLen(t, v, 0) }
func checkNoFileExists(t *testing.T, path string) {
	t.Helper()
	if _, err := os.Stat(path); !os.IsNotExist(err) {
		t.Errorf("expected no file %s, got %v", path, err)
	}
}
