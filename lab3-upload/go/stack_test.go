package main

import (
	"encoding/json"
	"testing"
)

// конструктор
func TestStackNewStackIsEmpty(t *testing.T) {
	st := NewStack()
	checkTrue(t, st.IsEmptyStack())
}

// Push и Top
func TestStackTopIsLastPushed(t *testing.T) {
	st := NewStack()
	st.Push("1")
	st.Push("2")

	checkFalse(t, st.IsEmptyStack())
	checkEqual(t, "2", st.Top())
}

// Pop
func TestStackPopRemovesTop(t *testing.T) {
	st := NewStack()
	st.Push("1")
	st.Push("2")
	st.Pop()

	checkEqual(t, "1", st.Top())
}

func TestStackPopAndTopOnEmpty(t *testing.T) {
	st := NewStack()
	st.Pop() // на пустом стеке ничего не ломается
	checkTrue(t, st.IsEmptyStack())
	checkEqual(t, "", st.Top())
}

// Print
func TestStackPrintFromTop(t *testing.T) {
	st := NewStack()
	st.Push("1")
	st.Push("2")

	out := captureOutput(func() { st.Print() })
	checkEqual(t, "2 -> 1 -> nil\n", out)
}

// HandleStack
func TestStackHandlerAllCommands(t *testing.T) {
	db := DB{}

	out := captureOutput(func() {
		HandleStack(SPUSH, db, "st", args("1"))
		HandleStack(SPUSH, db, "st", args("2"))
		HandleStack(SPUSH, db, "st", args("3"))
		HandleStack(SPOP, db, "st", args("")) // снимает 3
		HandleStack(SPRINT, db, "st", args(""))
		HandleStack(PRINT, db, "st", args("")) // чужая команда - ничего не делает
	})

	checkContains(t, out, "-> 3")
	checkEqual(t, "stack", db["st"].Type)
	checkEqual(t, []string{"1", "2"}, loadStrings(db, "st")) // в json снизу вверх
}

func TestStackHandlerPopEmpty(t *testing.T) {
	db := DB{}
	out := captureOutput(func() { HandleStack(SPOP, db, "st", args("")) })
	checkContains(t, out, "стек пуст")
}

func TestStackHandlerWrongTypeIsRejected(t *testing.T) {
	db := DB{"st": {Type: "queue", Data: json.RawMessage("[]")}}
	captureOutput(func() { HandleStack(SPUSH, db, "st", args("1")) })
	checkEqual(t, "queue", db["st"].Type)
}
