package main

import (
	"encoding/json"
	"testing"
)

// fillTree строит дерево для тестов:
//
//	    5
//	   / \
//	  3   8
//	 /
//	1
func fillTree(tr *AVLTree) {
	tr.Insert(5)
	tr.Insert(3)
	tr.Insert(8)
	tr.Insert(1)
}

// конструктор
func TestTreeNewAVLTreeIsEmpty(t *testing.T) {
	tr := NewAVLTree()
	checkNil(t, tr.GetRoot())
}

// Insert и Search
func TestTreeInsertAndSearch(t *testing.T) {
	tr := NewAVLTree()
	fillTree(tr)

	checkEqual(t, 5, tr.GetRoot().Key)
	checkNotNil(t, tr.Search(1))
	checkNotNil(t, tr.Search(8))
	checkNil(t, tr.Search(7)) // такого нет
}

func TestTreeInsertGoesLeftAndRight(t *testing.T) {
	tr := NewAVLTree()
	fillTree(tr)

	checkEqual(t, 3, tr.GetRoot().Left.Key)  // меньше - влево
	checkEqual(t, 8, tr.GetRoot().Right.Key) // больше - вправо
}

// обходы
func TestTreeTraversals(t *testing.T) {
	tr := NewAVLTree()
	fillTree(tr)

	checkEqual(t, "5 3 1 8 ", captureOutput(func() { tr.PrintPreorder() }))
	checkEqual(t, "1 3 5 8 ", captureOutput(func() { tr.PrintInorder() }))
	checkEqual(t, "1 3 8 5 ", captureOutput(func() { tr.PrintPostorder() }))
	checkEqual(t, "5 3 8 1 ", captureOutput(func() { tr.Bfs() }))
}

func TestTreeBfsOnEmpty(t *testing.T) {
	tr := NewAVLTree()
	checkEqual(t, "", captureOutput(func() { tr.Bfs() }))
}

// IsComplete
func TestTreeCompleteTree(t *testing.T) {
	tr := NewAVLTree()
	checkTrue(t, tr.IsComplete()) // пустое дерево считается полным

	fillTree(tr) // уровни заполнены слева направо
	checkTrue(t, tr.IsComplete())
}

func TestTreeNotCompleteTree(t *testing.T) {
	tr := NewAVLTree()
	tr.Insert(5)
	tr.Insert(8) // только правый ребенок, слева дырка
	checkFalse(t, tr.IsComplete())
}

// HandleTree
func TestTreeHandlerAllCommands(t *testing.T) {
	db := DB{}
	run := func(command Command, a string) { HandleTree(command, db, "t", args(a)) }

	out := captureOutput(func() {
		run(TINSERT, "5")
		run(TINSERT, "3")
		run(TINSERT, "8")
		run(TFIND, "3")
		run(TFIND, "7")
		run(TCOMPLETE, "")
		run(TPRINT, "")
		run(PRINT, "") // чужая команда
	})

	checkContains(t, out, "-> TRUE")
	checkContains(t, out, "-> FALSE")
	checkEqual(t, "tree", db["t"].Type)
	checkEqual(t, []int{5, 3, 8}, loadInts(db, "t")) // сохранено в ширину
}

func TestTreeHandlerNotComplete(t *testing.T) {
	db := DB{}
	run := func(command Command, a string) { HandleTree(command, db, "t", args(a)) }

	out := captureOutput(func() {
		run(TINSERT, "5")
		run(TINSERT, "8")
		run(TCOMPLETE, "")
	})

	checkContains(t, out, "-> FALSE")
}

func TestTreeHandlerWrongTypeIsRejected(t *testing.T) {
	db := DB{"t": {Type: "array", Data: json.RawMessage("[]")}}
	captureOutput(func() { HandleTree(TINSERT, db, "t", args("5")) })
	checkEqual(t, "array", db["t"].Type)
}

func validateAVL(t *testing.T, node *TreeNode) int {
	t.Helper()
	if node == nil {
		return 0
	}
	l, r := validateAVL(t, node.Left), validateAVL(t, node.Right)
	if l-r > 1 || r-l > 1 || node.Height != 1+max(l, r) {
		t.Fatalf("unbalanced node %d: %d %d height %d", node.Key, l, r, node.Height)
	}
	if node.Left != nil && node.Left.Key >= node.Key {
		t.Fatal("bad left key")
	}
	if node.Right != nil && node.Right.Key <= node.Key {
		t.Fatal("bad right key")
	}
	return node.Height
}
func TestAVLRotationsAndRemove(t *testing.T) {
	for _, keys := range [][]int{{30, 20, 10}, {10, 20, 30}, {30, 10, 20}, {10, 30, 20}} {
		tr := NewAVLTree()
		for _, k := range keys {
			tr.Insert(k)
		}
		tr.Insert(20)
		checkEqual(t, 20, tr.GetRoot().Key)
		validateAVL(t, tr.GetRoot())
	}
	tr := NewAVLTree()
	for i := 0; i < 100; i++ {
		tr.Insert(i)
		validateAVL(t, tr.GetRoot())
	}
	for i := 0; i < 100; i += 2 {
		tr.Remove(i)
		validateAVL(t, tr.GetRoot())
		checkNil(t, tr.Search(i))
	}
	for i := 1; i < 100; i += 2 {
		checkNotNil(t, tr.Search(i))
		tr.Remove(i)
		validateAVL(t, tr.GetRoot())
	}
	tr.Remove(1000)
	checkNil(t, tr.GetRoot())
}
func TestAVLDeleteCommand(t *testing.T) {
	db := DB{}
	captureOutput(func() {
		for _, key := range []string{"30", "10", "20"} {
			HandleTree(TINSERT, db, "t", args(key))
		}
		HandleTree(TDEL, db, "t", args("20"))
	})
	checkEqual(t, 2, len(loadInts(db, "t")))
}
