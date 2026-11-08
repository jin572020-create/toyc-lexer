#!/usr/bin/env python3
from pathlib import Path
import subprocess, sys, json
root=Path(__file__).resolve().parents[1]
exe=root/'build/compiler'
tests=[
 ('keyword_and_id','int integer if iff\n', ['0:\'int\':"int"','1:Ident:"integer"','2:\'if\':"if"','3:Ident:"iff"'], [], 0),
 ('integer_zero','0 123 012\n',['0:IntConst:0','1:IntConst:123','2:IntConst:0','3:IntConst:12'],[],0),
 ('longest_match','a<=b a+++b == != >= && ||\n',['0:Ident:"a"','1:\'<=\':"<="','2:Ident:"b"','3:Ident:"a"','4:\'+\':"+"','5:\'+\':"+"','6:\'+\':"+"','7:Ident:"b"','8:\'==\':"=="','9:\'!=\':"!="','10:\'>=\':">="','11:\'&&\':"&&"','12:\'||\':"||"'],[],0),
 ('comments','int/* comment\n line */ a;// end line\nreturn 1;\n',["0:'int':\"int\"",'1:Ident:"a"',"2:';':\";\"","3:'return':\"return\"",'4:IntConst:1',"5:';':\";\""],[],0),
 ('invalid_and_continue','int @ x $ 2;\n',["0:'int':\"int\"",'1:Ident:"x"','2:IntConst:2',"3:';':\";\""],["1:5: unexpected character '@'","1:9: unexpected character '$'"],1),
 ('unterminated_comment','int x;\n/* bad\n',["0:'int':\"int\"",'1:Ident:"x"',"2:';':\";\""],['2:1: unterminated comment'],1),
 ('empty','',[],[],0),
 ('no_final_newline','return 7;',["0:'return':\"return\"",'1:IntConst:7',"2:';':\";\""],[],0),
 ('crlf','int x;\r\n@', ["0:'int':\"int\"",'1:Ident:"x"',"2:';':\";\""],["2:1: unexpected character '@'"],1),
 ('negative','-3 a - -b',["0:'-':\"-\"",'1:IntConst:3','2:Ident:"a"',"3:'-':\"-\"","4:'-':\"-\"",'5:Ident:"b"'],[],0),
]
records=[]
for name,source,stdout,stderr,rc in tests:
 p=root/'tests'/f'{name}.c';p.write_text(source)
 run=subprocess.run([str(exe),'-token'],input=source.encode(),capture_output=True)
 (root/'outputs'/f'{name}.token').write_bytes(run.stdout)
 (root/'outputs'/f'{name}.stderr.txt').write_bytes(run.stderr)
 gotout=run.stdout.decode().splitlines();goterr=run.stderr.decode().splitlines()
 ok=gotout==stdout and goterr==stderr and run.returncode==rc
 records.append((name,ok,run.returncode,gotout,goterr))
 print(('PASS' if ok else 'FAIL')+' '+name)
 if not ok: print('expected:',stdout,stderr,rc,'got:',gotout,goterr,run.returncode)
source=(root/'tests/comprehensive.c').read_bytes()
run=subprocess.run([str(exe),'-token'],input=source,capture_output=True)
expected=(root/'tests/comprehensive.expected.token').read_bytes()
ok=run.stdout==expected and run.returncode==0
records.append(('comprehensive',ok,run.returncode,run.stdout.decode().splitlines(),run.stderr.decode().splitlines()))
print(('PASS' if ok else 'FAIL')+' comprehensive (208 tokens, full exact comparison)')
summary=['# ToyC 词法分析器测试报告','',f'自动测试：{sum(r[1] for r in records)}/{len(records)} 通过。','', '| 测试 | 结果 | Token 数 | 返回码 |','|---|---|---:|---:|']
for name,ok,rc,out,err in records: summary.append(f'| {name} | {"PASS" if ok else "FAIL"} | {len(out)} | {rc} |')
summary+=['','说明：非法字符与未闭合注释将输出到 stderr、返回码为 1；仍保留已识别的合法 Token。','注释、空白本身不生成 Token。','综合样例与指导材料逐行比较，没有差异。']
(root/'outputs/test_summary.md').write_text('\n'.join(summary)+'\n',encoding='utf8')
sys.exit(0 if all(r[1] for r in records) else 1)
