//go:build testify

package main

import (
	"github.com/stretchr/testify/assert"
	"testing"
)

func TestAVLWithTestify(t *testing.T) {
	tr := NewAVLTree()
	for _, key := range []int{30, 10, 20} {
		tr.Insert(key)
	}
	assert.Equal(t, 20, tr.GetRoot().Key)
	tr.Remove(20)
	assert.Nil(t, tr.Search(20))
	assert.NotNil(t, tr.Search(10))
}
