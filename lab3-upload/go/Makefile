build:
	go build -o dbms .

fmt:
	gofmt -w .

lint:
	go vet ./...
	test -z "$$(gofmt -l .)"

test:
	go test -v ./...

# Дополнительный тест с testify (скачивается зависимость из go.mod).
test-testify:
	go test -tags testify -v ./...

coverage:
	go test ./... -coverprofile=coverage.out
	go tool cover -func=coverage.out | tail -1
	go tool cover -html=coverage.out -o coverage.html
	go tool cover -func=coverage.out | awk '/^total:/ { if ($$3+0 < 85) exit 1; found=1 } END { if (!found) exit 1 }'

bench:
	go test -run=^$$ -bench=. -benchmem -benchtime=100ms ./...

clean:
	rm -f dbms coverage.out coverage.html

.PHONY: build fmt lint test test-testify coverage bench clean
