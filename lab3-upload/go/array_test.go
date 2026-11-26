package main

import (
	"encoding/json"
	"strconv"
	"testing"
)

// конструктор
func TestArrayNewArrayIsEmpty(t *testing.T) {
	arr := NewArray()
	checkEqual(t, 0, arr.Length())
}

// PushBack
func TestArrayPushBackAddsToEnd(t *testing.T) {
	arr := NewArray()
	arr.PushBack("a")
	arr.PushBack("b")

	checkEqual(t, 2, arr.Length())
	checkEqual(t, "a", arr.GetAt(0))
	checkEqual(t, "b", arr.GetAt(1))
}

// resizeArray (через PushBack)
func TestArrayGrowsBeyondStartCapacity(t *testing.T) {
	arr := NewArray()
	for i := 0; i < 10; i++ { // вместимость 4, должно расшириться
		arr.PushBack(strconv.Itoa(i))
	}
	checkEqual(t, 10, arr.Length())
	checkEqual(t, "9", arr.GetAt(9))
}

// InsertAt
func TestArrayInsertInMiddle(t *testing.T) {
	arr := NewArray()
	arr.PushBack("a")
	arr.PushBack("c")
	arr.InsertAt("b", 1)

	checkEqual(t, "a", arr.GetAt(0))
	checkEqual(t, "b", arr.GetAt(1))
	checkEqual(t, "c", arr.GetAt(2))
}

func TestArrayInsertWhenFull(t *testing.T) {
	arr := NewArray()
	for i := 0; i < 4; i++ { // заполнили всю вместимость
		arr.PushBack("x")
	}
	arr.InsertAt("y", 0) // InsertAt должен сам расширить массив
	checkEqual(t, 5, arr.Length())
	checkEqual(t, "y", arr.GetAt(0))
}

func TestArrayInsertWrongIndex(t *testing.T) {
	arr := NewArray()
	arr.PushBack("a")
	captureOutput(func() { arr.InsertAt("x", 5) }) // такого индекса нет
	checkEqual(t, 1, arr.Length())
}

// RemoveAt
func TestArrayRemoveShiftsElements(t *testing.T) {
	arr := NewArray()
	arr.PushBack("a")
	arr.PushBack("b")
	arr.PushBack("c")
	arr.RemoveAt(0)

	checkEqual(t, 2, arr.Length())
	checkEqual(t, "b", arr.GetAt(0))
}

func TestArrayRemoveWrongIndex(t *testing.T) {
	arr := NewArray()
	arr.PushBack("a")
	captureOutput(func() { arr.RemoveAt(3) })
	checkEqual(t, 1, arr.Length())
}

// SetAt
func TestArraySetAtChangesValue(t *testing.T) {
	arr := NewArray()
	arr.PushBack("a")
	arr.SetAt("b", 0)
	checkEqual(t, "b", arr.GetAt(0))
}

func TestArraySetAtWrongIndex(t *testing.T) {
	arr := NewArray()
	arr.PushBack("a")
	captureOutput(func() { arr.SetAt("b", 3) })
	checkEqual(t, "a", arr.GetAt(0)) // старое значение осталось
}

// GetAt
func TestArrayGetAtWrongIndex(t *testing.T) {
	arr := NewArray()
	var got string
	captureOutput(func() { got = arr.GetAt(0) })
	checkEqual(t, "", got) // при ошибке возвращает пустую строку
}

// PrintArray
func TestArrayPrintShowsElements(t *testing.T) {
	arr := NewArray()
	arr.PushBack("a")
	arr.PushBack("b")

	out := captureOutput(func() { arr.PrintArray() })
	checkEqual(t, "a b (size=2, capacity=4)\n", out)
}

// HandleArray
func TestArrayHandlerAllCommands(t *testing.T) {
	db := DB{}

	captureOutput(func() {
		HandleArray(MPUSH, db, "arr", args("a"))     // [a]
		HandleArray(MPUSH, db, "arr", args("c"))     // [a, c]
		HandleArray(MINSERT, db, "arr", args("1 b")) // [a, b, c]
		HandleArray(MSET, db, "arr", args("0 z"))    // [z, b, c]
		HandleArray(MDEL, db, "arr", args("2"))      // [z, b]
		HandleArray(MGET, db, "arr", args("1"))
		HandleArray(MLEN, db, "arr", args(""))
		HandleArray(MPRINT, db, "arr", args(""))
		HandleArray(PRINT, db, "arr", args("")) // чужая команда - ничего не делает
	})

	checkEqual(t, "array", db["arr"].Type)
	checkEqual(t, []string{"z", "b"}, loadStrings(db, "arr"))
}

func TestArrayHandlerWrongTypeIsRejected(t *testing.T) {
	db := DB{"arr": {Type: "stack", Data: json.RawMessage("[]")}} // под этим именем уже стек

	out := captureOutput(func() { HandleArray(MPUSH, db, "arr", args("a")) })

	checkEqual(t, "stack", db["arr"].Type) // тип не поменялся
	checkContains(t, out, "это не array")  // и вывелось сообщение
}
