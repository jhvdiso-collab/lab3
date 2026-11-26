#!/usr/bin/env bash
set -euo pipefail
exe=$(realpath "${1:-./dbms}")
format=${2:-binary}
tmp=$(mktemp -d)
trap 'rm -rf "$tmp"' EXIT
base="$tmp/demo"
if [[ $format == json ]]; then base+=.json; fi
run() { printf '\n$ ./dbms --file %s --format %s --query "%s"\n' "$base" "$format" "$1"; "$exe" --file "$base" --format "$format" --query "$1"; }
section() { printf '\n===== %s =====\n' "$1"; }
section '1. Массив'
for q in 'MPUSH arr A' 'MPUSH arr C' 'MINSERT arr 1 B' 'MGET arr 1' 'MSET arr 2 D' 'MDEL arr 0' 'MLEN arr' 'PRINT arr'; do run "$q"; done
section '2. Односвязный список'
for q in 'FPUSHH fl B' 'FPUSHBEFORE fl B A' 'FPUSHAFTER fl B C' 'PRINT fl' 'FGETFRONT fl' 'FGETBACK fl' 'FDELBEFORE fl B' 'FDELAFTER fl B' 'PRINT fl'; do run "$q"; done
section '3. Двусвязный список'
for q in 'LPUSHH dl B' 'LPUSHBEFORE dl B A' 'LPUSHAFTER dl B C' 'PRINT dl' 'LPRINTR dl' 'LDELAFTER dl B' 'LGETBACK dl' 'PRINT dl'; do run "$q"; done
section '4. Стек LIFO'
for q in 'SPUSH st A' 'SPUSH st B' 'PRINT st' 'SPOP st' 'PRINT st'; do run "$q"; done
section '5. Очередь FIFO'
for q in 'QPUSH qu A' 'QPUSH qu B' 'PRINT qu' 'QPOP qu' 'PRINT qu'; do run "$q"; done
section '6. Метод цепочек'
for q in 'HADD ht 5' 'HADD ht 105' 'HPRINT ht' 'HFIND ht 105' 'HDEL ht 105' 'HFIND ht 105'; do run "$q"; done
section '7. Метод Кукушки'
for q in 'KADD kh 12' 'KADD kh 23' 'KPRINT kh' 'KFIND kh 12' 'KDEL kh 12' 'KFIND kh 12'; do run "$q"; done
section '8. AVL-дерево'
for q in 'TINSERT tr 30' 'TINSERT tr 10' 'TINSERT tr 20' 'TPRINT tr' 'TFIND tr 10' 'TDEL tr 20' 'TPRINT tr' 'TFIND tr 20'; do run "$q"; done
section '9. Сериализация и восстановление'
run 'PRINT arr'
if [[ $format == binary ]]; then
  ls -l "$base".*
  od -An -tx1 "$base.arr.bin"
else
  cat "$base"
fi
section '10. Проверка некорректного ввода'
if run 'MGET arr abc'; then echo 'Ошибка: некорректная команда принята' >&2; exit 1; else echo 'Некорректный ввод отклонён'; fi
