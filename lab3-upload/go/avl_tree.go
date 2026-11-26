package main

import (
	"fmt"
	"io"
	"os"
)

// TreeNode - узел AVL-дерева.
type TreeNode struct {
	Key    int
	Left   *TreeNode
	Right  *TreeNode
	Height int
}

// AVLTree - самобалансирующееся AVL-дерево.
type AVLTree struct {
	root *TreeNode
}

// NewAVLTree создает пустое дерево.
func NewAVLTree() *AVLTree {
	return &AVLTree{}
}

func height(node *TreeNode) int {
	if node == nil {
		return 0
	}
	return node.Height
}

func updateHeight(node *TreeNode) {
	node.Height = 1 + max(height(node.Left), height(node.Right))
}

func rotateLeft(node *TreeNode) *TreeNode {
	right := node.Right
	node.Right = right.Left
	right.Left = node
	updateHeight(node)
	updateHeight(right)
	return right
}

func rotateRight(node *TreeNode) *TreeNode {
	left := node.Left
	node.Left = left.Right
	left.Right = node
	updateHeight(node)
	updateHeight(left)
	return left
}

func balance(node *TreeNode) *TreeNode {
	updateHeight(node)
	factor := height(node.Left) - height(node.Right)
	if factor > 1 {
		if height(node.Left.Right) > height(node.Left.Left) {
			node.Left = rotateLeft(node.Left)
		}
		return rotateRight(node)
	}
	if factor < -1 {
		if height(node.Right.Left) > height(node.Right.Right) {
			node.Right = rotateRight(node.Right)
		}
		return rotateLeft(node)
	}
	return node
}

func insertNode(node *TreeNode, key int) *TreeNode {
	if node == nil {
		return &TreeNode{Key: key, Height: 1}
	}
	if key < node.Key {
		node.Left = insertNode(node.Left, key)
	} else if key > node.Key {
		node.Right = insertNode(node.Right, key)
	} else {
		return node
	}
	return balance(node)
}

func removeNode(node *TreeNode, key int) *TreeNode {
	if node == nil {
		return nil
	}
	if key < node.Key {
		node.Left = removeNode(node.Left, key)
	} else if key > node.Key {
		node.Right = removeNode(node.Right, key)
	} else {
		if node.Left == nil {
			return node.Right
		}
		if node.Right == nil {
			return node.Left
		}
		next := node.Right
		for next.Left != nil {
			next = next.Left
		}
		node.Key = next.Key
		node.Right = removeNode(node.Right, next.Key)
	}
	return balance(node)
}

func (tr *AVLTree) Insert(key int) { tr.root = insertNode(tr.root, key) }
func (tr *AVLTree) Remove(key int) { tr.root = removeNode(tr.root, key) }

// Рекурсивные обходы. Им нужен узел, с которого начинать,
// поэтому снаружи они вызываются через PrintPreorder и т.д.

func (tr *AVLTree) preorder(node *TreeNode) {
	if node == nil {
		return
	}
	fmt.Print(node.Key, " ")
	tr.preorder(node.Left)
	tr.preorder(node.Right)
}

func (tr *AVLTree) inorder(node *TreeNode) {
	if node == nil {
		return
	}
	tr.inorder(node.Left)
	fmt.Print(node.Key, " ")
	tr.inorder(node.Right)
}

func (tr *AVLTree) postorder(node *TreeNode) {
	if node == nil {
		return
	}
	tr.postorder(node.Left)
	tr.postorder(node.Right)
	fmt.Print(node.Key, " ")
}

// PrintPreorder печатает прямой обход.
func (tr *AVLTree) PrintPreorder() {
	tr.preorder(tr.root)
}

// PrintInorder печатает симметричный обход.
func (tr *AVLTree) PrintInorder() {
	tr.inorder(tr.root)
}

// PrintPostorder печатает обратный обход.
func (tr *AVLTree) PrintPostorder() {
	tr.postorder(tr.root)
}

// Bfs печатает обход в ширину.
func (tr *AVLTree) Bfs() {
	if tr.root == nil {
		return
	}

	ochered := []*TreeNode{tr.root}
	for i := 0; i < len(ochered); i++ {
		node := ochered[i]
		fmt.Print(node.Key, " ")
		if node.Left != nil {
			ochered = append(ochered, node.Left)
		}
		if node.Right != nil {
			ochered = append(ochered, node.Right)
		}
	}
}

// IsComplete проверяет, что дерево полное: при обходе в ширину
// после первого пустого места не должно встречаться узлов.
func (tr *AVLTree) IsComplete() bool {
	if tr.root == nil {
		return true
	}

	ochered := []*TreeNode{tr.root}
	sawEmpty := false
	for i := 0; i < len(ochered); i++ {
		node := ochered[i]
		if node == nil {
			sawEmpty = true
		} else {
			if sawEmpty {
				return false
			}
			ochered = append(ochered, node.Left, node.Right)
		}
	}
	return true
}

// Search ищет узел с ключом key, если не нашел - возвращает nil.
func (tr *AVLTree) Search(key int) *TreeNode {
	current := tr.root
	for current != nil {
		if key == current.Key {
			return current
		}
		if key < current.Key {
			current = current.Left
		} else {
			current = current.Right
		}
	}
	return nil
}

// GetRoot возвращает корень дерева.
func (tr *AVLTree) GetRoot() *TreeNode {
	return tr.root
}

// bfsKeys возвращает ключи в порядке обхода в ширину.
// Если вставить их в том же порядке, получится такое же дерево.
func (tr *AVLTree) bfsKeys() []int {
	keys := []int{}
	if tr.root == nil {
		return keys
	}

	ochered := []*TreeNode{tr.root}
	for i := 0; i < len(ochered); i++ {
		node := ochered[i]
		keys = append(keys, node.Key)
		if node.Left != nil {
			ochered = append(ochered, node.Left)
		}
		if node.Right != nil {
			ochered = append(ochered, node.Right)
		}
	}
	return keys
}

// HandleTree выполняет команду над деревом с именем name.
func HandleTree(command Command, db DB, name string, ss io.Reader) {
	if WrongType(db, name, "tree") {
		return
	}

	tr := NewAVLTree()
	if binaryMode {
		tr.LoadBinary(binaryFile(name))
	} else {
		for _, key := range loadInts(db, name) {
			tr.Insert(key)
		}
	}

	var value int
	switch command {
	case TINSERT:
		if _, err := fmt.Fscan(ss, &value); err != nil {
			fmt.Println("неверные аргументы")
			return
		}
		tr.Insert(value)
		fmt.Println("->", value)
	case TDEL:
		if _, err := fmt.Fscan(ss, &value); err != nil {
			fmt.Println("неверные аргументы")
			return
		}
		tr.Remove(value)
	case TFIND:
		if _, err := fmt.Fscan(ss, &value); err != nil {
			fmt.Println("неверные аргументы")
			return
		}
		if tr.Search(value) != nil {
			fmt.Println("-> TRUE")
		} else {
			fmt.Println("-> FALSE")
		}
	case TCOMPLETE:
		if tr.IsComplete() {
			fmt.Println("-> TRUE")
		} else {
			fmt.Println("-> FALSE")
		}
	case TPRINT:
		fmt.Print("прямой:       ")
		tr.PrintPreorder()
		fmt.Println()
		fmt.Print("симметричный: ")
		tr.PrintInorder()
		fmt.Println()
		fmt.Print("обратный:     ")
		tr.PrintPostorder()
		fmt.Println()
		fmt.Print("в ширину:     ")
		tr.Bfs()
		fmt.Println()
	}

	if binaryMode {
		db[name] = &Entry{Type: "tree"}
		tr.SaveBinary(binaryFile(name))
	} else {
		saveInts(db, name, "tree", tr.bfsKeys())
	}
}

// SaveBinary сохраняет дерево в бинарный файл:
// количество ключей, затем ключи в порядке обхода в ширину.
func (tr *AVLTree) SaveBinary(fileName string) {
	out, err := os.Create(fileName)
	if err != nil {
		return
	}
	defer out.Close()

	keys := tr.bfsKeys()
	writeInt(out, len(keys))
	for _, key := range keys {
		writeInt(out, key)
	}
}

// LoadBinary загружает дерево из бинарного файла.
// Старые ключи перед загрузкой удаляются.
func (tr *AVLTree) LoadBinary(fileName string) {
	in, err := os.Open(fileName)
	if err != nil {
		return
	}
	defer in.Close()

	tr.root = nil
	count := readInt(in)
	for i := 0; i < count; i++ {
		tr.Insert(readInt(in))
	}
}
