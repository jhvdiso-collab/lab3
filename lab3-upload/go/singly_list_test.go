package main

import (
	"encoding/json"
	"testing"
)

// showFlist печатает список в строку, чтобы удобно сравнивать.
func showFlist(fl *Flist) string {
	return captureOutput(func() { fl.PrintList() })
}

// конструктор
func TestFlistNewListIsEmpty(t *testing.T) {
	fl := NewFlist()
	checkTrue(t, fl.IsEmptyList())
	checkNil(t, fl.GetHead())
}

// PushFront и PopFront
func TestFlistPushFrontAddsToStart(t *testing.T) {
	fl := NewFlist()
	fl.PushFront("b")
	fl.PushFront("a")
	checkEqual(t, "a -> b -> nil\n", showFlist(fl))
}

func TestFlistPopFront(t *testing.T) {
	fl := NewFlist()
	fl.AddTail("a")
	fl.AddTail("b")
	fl.PopFront()
	checkEqual(t, "b -> nil\n", showFlist(fl))

	fl.PopFront()
	fl.PopFront() // на пустом ничего не ломается
	checkTrue(t, fl.IsEmptyList())
}

// AddTail и PopTail
func TestFlistAddTailAddsToEnd(t *testing.T) {
	fl := NewFlist()
	fl.AddTail("a")
	fl.AddTail("b")
	checkEqual(t, "a -> b -> nil\n", showFlist(fl))
}

func TestFlistPopTail(t *testing.T) {
	fl := NewFlist()
	fl.AddTail("a")
	fl.AddTail("b")
	fl.AddTail("c")
	fl.PopTail()
	checkEqual(t, "a -> b -> nil\n", showFlist(fl))

	fl.PopTail()
	fl.PopTail() // удалили последний элемент
	checkTrue(t, fl.IsEmptyList())

	fl.PopTail() // на пустом ничего не ломается
	checkTrue(t, fl.IsEmptyList())
}

// AddOne - вставка после узла
func TestFlistAddOneAfterNode(t *testing.T) {
	fl := NewFlist()
	fl.AddTail("a")
	fl.AddTail("c")
	fl.AddOne(fl.FindValue("a"), "b")
	checkEqual(t, "a -> b -> c -> nil\n", showFlist(fl))

	fl.AddOne(nil, "x") // нет узла - ничего не делаем
	checkEqual(t, "a -> b -> c -> nil\n", showFlist(fl))
}

// DeleteNode - удаление после узла
func TestFlistDeleteNodeAfterNode(t *testing.T) {
	fl := NewFlist()
	fl.AddTail("a")
	fl.AddTail("b")
	fl.AddTail("c")
	fl.DeleteNode(fl.FindValue("a"))
	checkEqual(t, "a -> c -> nil\n", showFlist(fl))

	fl.DeleteNode(fl.FindValue("c")) // после последнего никого нет
	fl.DeleteNode(nil)
	checkEqual(t, "a -> c -> nil\n", showFlist(fl))
}

// AddBefore
func TestFlistAddBefore(t *testing.T) {
	fl := NewFlist()
	fl.AddTail("b")
	fl.AddTail("d")
	fl.AddBefore(fl.FindValue("b"), "a") // перед головой
	fl.AddBefore(fl.FindValue("d"), "c") // в середину
	checkEqual(t, "a -> b -> c -> d -> nil\n", showFlist(fl))

	fl.AddBefore(nil, "x")
	checkEqual(t, "a -> b -> c -> d -> nil\n", showFlist(fl))
}

// DeleteBefore
func TestFlistDeleteBefore(t *testing.T) {
	fl := NewFlist()
	fl.AddTail("a")
	fl.AddTail("b")
	fl.AddTail("c")
	fl.AddTail("d")

	fl.DeleteBefore(fl.FindValue("d")) // удалит c
	checkEqual(t, "a -> b -> d -> nil\n", showFlist(fl))

	fl.DeleteBefore(fl.FindValue("b")) // удалит голову a
	checkEqual(t, "b -> d -> nil\n", showFlist(fl))

	fl.DeleteBefore(fl.FindValue("b")) // перед головой никого нет
	fl.DeleteBefore(nil)
	checkEqual(t, "b -> d -> nil\n", showFlist(fl))
}

// FindValue
func TestFlistFindValue(t *testing.T) {
	fl := NewFlist()
	fl.AddTail("a")
	checkNotNil(t, fl.FindValue("a"))
	checkNil(t, fl.FindValue("z"))
}

// DeleteValue
func TestFlistDeleteValue(t *testing.T) {
	fl := NewFlist()
	fl.DeleteValue("a") // пустой список

	fl.AddTail("a")
	fl.AddTail("b")
	fl.AddTail("c")
	fl.DeleteValue("a") // голова
	fl.DeleteValue("c") // хвост
	fl.DeleteValue("z") // такого нет
	checkEqual(t, "b -> nil\n", showFlist(fl))
}

// PrintReverse
func TestFlistPrintReverse(t *testing.T) {
	fl := NewFlist()
	fl.AddTail("a")
	fl.AddTail("b")

	out := captureOutput(func() { fl.PrintReverse() })
	checkEqual(t, "b a \n", out)
}

// HandleFlist
func TestFlistHandlerAllCommands(t *testing.T) {
	db := DB{}
	run := func(command Command, a string) { HandleFlist(command, db, "fl", args(a)) }

	out := captureOutput(func() {
		run(FPUSHT, "b")   // b
		run(FPUSHH, "a")   // a b
		run(FPUSHT, "e")   // a b e
		run(FPUSHA, "b c") // a b c e
		run(FPUSHB, "e d") // a b c d e
		run(FPUSHA, "z x") // нет z
		run(FPUSHB, "z x") // нет z
		run(FDELH, "")     // b c d e
		run(FDELT, "")     // b c d
		run(FDELA, "b")    // b d
		run(FDELA, "z")    // нет z
		run(FDELB, "d")    // d
		run(FDELB, "z")    // нет z
		run(FPUSHT, "k")   // d k
		run(FDEL, "d")     // k
		run(FFIND, "k")
		run(FFIND, "z")
		run(FPRINT, "")
		run(FPRINTR, "")
		run(PRINT, "") // чужая команда
	})

	checkContains(t, out, "-> TRUE")
	checkContains(t, out, "-> FALSE")
	checkEqual(t, "flist", db["fl"].Type)
	checkEqual(t, []string{"k"}, loadStrings(db, "fl"))
}

func TestFlistHandlerWrongTypeIsRejected(t *testing.T) {
	db := DB{"fl": {Type: "dlist", Data: json.RawMessage("[]")}}
	captureOutput(func() { HandleFlist(FPUSHT, db, "fl", args("a")) })
	checkEqual(t, "dlist", db["fl"].Type)
}
