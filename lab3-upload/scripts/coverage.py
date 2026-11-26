#!/usr/bin/env python3
"""Create a line-by-line HTML report from GCC gcov JSON data."""
import gzip
import html
import json
from pathlib import Path
import subprocess
import sys

root = Path(__file__).resolve().parent.parent
build = Path(sys.argv[1]).resolve()
output = Path(sys.argv[2]).resolve()
names = 'array singly_list doubly_list stack queue hash_table avl_tree commands main'.split()
output.mkdir(parents=True, exist_ok=True)
subprocess.run(['gcov', '-j', *[str(build / (name + '.gcno')) for name in names]], cwd=build, check=True, stdout=subprocess.DEVNULL)
style = '''<style>body{font:15px system-ui,sans-serif;margin:35px;max-width:1100px;color:#182432}table{border-collapse:collapse;width:100%}th,td{padding:8px 12px;text-align:left;border-bottom:1px solid #d7dce2}th{background:#edf1f5}a{color:#075985}.hit{background:#e0f4e4}.miss{background:#fce1e1}.neutral{color:#666}pre{margin:0;white-space:pre-wrap;font-size:13px}small{color:#56616c}td.num{width:60px;text-align:right;font-variant-numeric:tabular-nums}</style>'''
def page(title, body):
    return '<!doctype html><html lang="en"><meta charset="utf-8"><title>'+html.escape(title)+'</title>'+style+'<body>'+body+'</body></html>'
rows = []
sections = []
covered = total = 0
for name in names:
    filename = name + '.cpp'
    with gzip.open(build / (name + '.gcov.json.gz'), 'rt') as stream:
        data = json.load(stream)
    record = next(item for item in data['files'] if Path(item['file']).name == filename and not item['file'].startswith('/usr/'))
    counts = {}
    for line in record['lines']:
        number = line['line_number']
        counts[number] = counts.get(number, 0) + line['count']
    hit = sum(count > 0 for count in counts.values())
    n = len(counts)
    covered += hit
    total += n
    pct = 100 * hit / n
    print(f'{filename:20s} {hit:4d}/{n:<4d} {pct:6.2f}%')
    rows.append(f'<tr><td><a href="#{name}">{filename}</a></td><td>{hit}</td><td>{n}</td><td>{pct:.2f}%</td></tr>')
    detail = []
    for number, source in enumerate((root / filename).read_text().splitlines(), 1):
        count = counts.get(number)
        cls = 'neutral' if count is None else ('hit' if count else 'miss')
        detail.append(f'<tr class="{cls}"><td class="num">{number}</td><td class="num">{count if count is not None else ""}</td><td><pre>{html.escape(source)}</pre></td></tr>')
    sections.append(f'<h2 id="{name}">{filename} — {pct:.2f}%</h2><p><a href="#top">Back to summary</a></p><table><tr><th>Line</th><th>Hits</th><th>Source</th></tr>'+''.join(detail)+'</table>')
pct = 100 * covered / total
print(f'TOTAL                {covered}/{total}  {pct:.2f}%')
summary = f'<h1 id="top">C++ line coverage — Lab 3</h1><h2>{pct:.2f}% ({covered}/{total} executable lines)</h2><p>Threshold: 85%. Result: {"PASS" if pct >= 85 else "FAIL"}. Boost.Test and CLI integration tests, GCC/gcov.</p><small>Scope: nine project .cpp files; excludes test code, external libraries and headers. Line coverage is not branch coverage.</small><table><tr><th>File</th><th>Covered</th><th>Executable</th><th>Coverage</th></tr>'+''.join(rows)+f'<tr><th>Total</th><th>{covered}</th><th>{total}</th><th>{pct:.2f}%</th></tr></table><p>Green: executed. Red: not executed. Uncolored: non-executable.</p>'
(output / 'index.html').write_text(page('Lab 3 C++ line coverage', summary+''.join(sections)), encoding='utf-8')
raise SystemExit(0 if pct >= 85 else 1)
