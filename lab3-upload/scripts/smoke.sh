#!/usr/bin/env bash
# CLI integration test in isolated files; safe to run repeatedly.
set -euo pipefail
exe=$(realpath "${1:-./dbms}")
tmp=$(mktemp -d)
trap 'rm -rf "$tmp"' EXIT
for format in binary json; do
  base="$tmp/$format"
  run() { "$exe" --file "$base" --format "$format" --query "$1"; }
  expect() { local output; output=$(run "$1"); grep -Fq -- "$2" <<< "$output" || { echo "$1: expected $2, got $output" >&2; exit 1; }; }
  for q in 'MPUSH arr A' 'MPUSH arr C' 'MINSERT arr 1 B' 'MSET arr 2 D' 'FPUSHH fl B' 'FPUSHBEFORE fl B A' 'FPUSHAFTER fl B C' 'LPUSHH dl B' 'LPUSHBEFORE dl B A' 'LPUSHAFTER dl B C' 'SPUSH st A' 'SPUSH st B' 'QPUSH qu A' 'QPUSH qu B' 'HADD ht 5' 'HADD ht 105' 'KADD kh 12' 'KADD kh 23' 'TINSERT tr 30' 'TINSERT tr 10' 'TINSERT tr 20'; do run "$q" >/dev/null; done
  expect 'PRINT arr' 'A B D'
  expect 'MGET arr 1' 'B'
  expect 'MLEN arr' '3'
  expect 'FGETFRONT fl' 'A'
  expect 'FGETBACK fl' 'C'
  expect 'LGETFRONT dl' 'A'
  expect 'LGETBACK dl' 'C'
  expect 'PRINT fl' 'B'
  expect 'PRINT dl' 'B'
  expect 'PRINT st' 'B'
  expect 'PRINT qu' 'A'
  expect 'PRINT ht' '105'
  expect 'PRINT kh' '12'
  expect 'PRINT tr' '20 10 30'
  expect 'HFIND ht 105' 'TRUE'
  expect 'KFIND kh 12' 'TRUE'
  expect 'TCOMPLETE tr' 'TRUE'
  expect 'SPOP st' 'B'
  expect 'QPOP qu' 'A'
  run 'FDELBEFORE fl B'; run 'FDELAFTER fl B'; run 'LDELBEFORE dl B'; run 'LDELAFTER dl B'; run 'MDEL arr 1'; run 'HDEL ht 105'; run 'KDEL kh 12'; run 'TDEL tr 20'
  expect 'TFIND tr 20' 'FALSE'
  expect 'HFIND ht 105' 'FALSE'
  expect 'KFIND kh 12' 'FALSE'
  expect 'PRINT unknown' 'нет структуры'
  expect 'FGETBACK empty' 'список пуст'
  expect 'LGETFRONT empty2' 'список пуст'
  if run 'MGET arr abc' >/dev/null; then echo 'invalid number accepted' >&2; exit 1; fi
  if run 'MPUSH arr' >/dev/null; then echo 'missing argument accepted' >&2; exit 1; fi
  if run 'UNKNOWN arr' >/dev/null; then echo 'unknown command accepted' >&2; exit 1; fi
  echo "CLI smoke test ($format): OK"
done
