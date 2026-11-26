package main

import (
	"encoding/json"
	"testing"
)

// конструктор
func TestQueueNewQueueIsEmpty(t *testing.T) {
	q := NewQueue()
	checkTrue(t, q.IsEmptyQ())
}

// Push и Top
func TestQueueTopIsFirstPushed(t *testing.T) {
	q := NewQueue()
	q.Push("1")
	q.Push("2")

	checkFalse(t, q.IsEmptyQ())
	checkEqual(t, "1", q.Top()) // первым пришел - первым выйдет
}

// Pop
func TestQueuePopRemovesFirst(t *testing.T) {
	q := NewQueue()
	q.Push("1")
	q.Push("2")
	q.Pop()

	checkEqual(t, "2", q.Top())
}

func TestQueuePopLastMakesEmpty(t *testing.T) {
	q := NewQueue()
	q.Push("1")
	q.Pop()
	checkTrue(t, q.IsEmptyQ())

	q.Push("2") // после опустошения снова работает
	checkEqual(t, "2", q.Top())
}

func TestQueuePopAndTopOnEmpty(t *testing.T) {
	q := NewQueue()
	q.Pop()
	checkTrue(t, q.IsEmptyQ())
	checkEqual(t, "", q.Top())
}

// PrintQ
func TestQueuePrintFromHead(t *testing.T) {
	q := NewQueue()
	q.Push("1")
	q.Push("2")

	out := captureOutput(func() { q.PrintQ() })
	checkEqual(t, "1 -> 2 -> nil\n", out)
}

// HandleQueue
func TestQueueHandlerAllCommands(t *testing.T) {
	db := DB{}

	out := captureOutput(func() {
		HandleQueue(QPUSH, db, "q", args("1"))
		HandleQueue(QPUSH, db, "q", args("2"))
		HandleQueue(QPUSH, db, "q", args("3"))
		HandleQueue(QPOP, db, "q", args("")) // выходит 1
		HandleQueue(QPRINT, db, "q", args(""))
		HandleQueue(PRINT, db, "q", args("")) // чужая команда - ничего не делает
	})

	checkContains(t, out, "-> 1")
	checkEqual(t, "queue", db["q"].Type)
	checkEqual(t, []string{"2", "3"}, loadStrings(db, "q"))
}

func TestQueueHandlerPopEmpty(t *testing.T) {
	db := DB{}
	out := captureOutput(func() { HandleQueue(QPOP, db, "q", args("")) })
	checkContains(t, out, "очередь пуста")
}

func TestQueueHandlerWrongTypeIsRejected(t *testing.T) {
	db := DB{"q": {Type: "stack", Data: json.RawMessage("[]")}}
	captureOutput(func() { HandleQueue(QPUSH, db, "q", args("1")) })
	checkEqual(t, "stack", db["q"].Type)
}
