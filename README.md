# BinaryViewer

## How to Use  
コンソール上で `BinaryViewer.bat` ファイルを実行する。  
1行目はコンパイルとビルドを実行する処理となっており、2行目は BinaryViewer を実行するコマンドが記述されている。  
2行目のコマンドは以下の要素を組み合わせることがで実行内容を変更することができる。(XXXXは任意の16進数)

- 16進ダンプ処理対象ファイルのパス
- ダンプ処理開始アドレス [-bXXXX]
- ダンプ処理終了アドレス [-eXXXX]
- viewrモード指定　[-v]

コマンドのフォーマットは以下のようになる。(要素は順不同)
```
binary_viewer.exe <file name path> -b1234 -e5678 -v
```
