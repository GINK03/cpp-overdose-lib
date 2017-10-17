# cpp-overdose-lib
C++17のラムダ式と、テンプレートを用いることで、KotlinやScalaなどに似たシンタックスでRのdplyrの該当する機能が動かせることを示します

## もっと関数型ライクにC++を使いたい！
C++の言語の特徴として、その手続き的な側面が多く強調されます  
これはC言語を背景としているからなのですが、近年のC++の言語仕様の拡張によって面白いことができるようになってきました  

例えば、Rという統計言語にはデータのオペレーションが感覚的にわかりやすい、dplyerというライブラリがあります  

これに似せたライブラリ overdose-libを作成したので、ご紹介させてください  

## ラムダとTemplateとautoによる型推定  
ラムダは匿名関数と呼ばれ、リテラルの中に埋め込むことができる名前のない関数です。ラムダ式は変数に束縛することも可能で、変数に代入したラムダ式を取り回して、任意の処理をすることができます  

TemplateはもともとジェネリックなSTLの型を指定することができるものでしたが、型以外にも実数値やラムダ式を引数に取れるし、コンパイル時に動作するので、コンパイル時に実行し計算するものとなっています  

autoは型推論で、型宣言を簡潔にするものですが、一部ジェネリックな動作もすることが期待できて、戻り値の型がテンプレートやコンパイル時の型の取得のdecletypeなどで、自動的に推論させることが可能です。  

これを用いると、関数型言語の一つの要件である第1級関数という関数をデータと同じように扱うことが可能になります  

例えば、任意の関数Fを受け取り、型SのベクタをFで変換してから型Tで返却する関数を返す関数はこのようにかけます。
```cpp
  template<typename S, typename R, typename F>
  auto mapper(const F& f) {
    return [f](std::vector<S> source) {
      std::vector<R> tmp; 
      for(auto s:source) {
        R r = f(s); 
        tmp.push_back(r);
      }
      return tmp;
    };
  };
```

これを>>(ダブルアロー)のオペレータを定義すると、このように書くことができます  
この例では、[1, 2, 3, 4]のベクタを各要素を二乗する操作です  
echoは表中出力に対する出力する関数を返す関数  
```cpp
  auto src = vector{1, 2, 3, 4};
  src >> mapper<int,int>([](int i ){ return i*i;}) >> echo<int>();
```
出力はこのようになります
```console
$ ./bin/a.out
[1,4,9,16]
```

## 色々な関数を実装してみた
RのdplyrやPythonのpandas、Apache SparkのRDDなどを参考にして実装しました
dplyrとSpark RDDと今回作成したoverdose(仮称)の関数と機能の比較表です

| dplyr(R)   | Spark RDD(Scala,Java)| overdose(C++) |
|:-----------:|:------------:|:------------:|
| %>%        | .(dot)      | >>(ダブルアロー)|
| map        | map         | mapper       |
| reduce     | reduce      | reduce       |
| filter     | filter      | filter       |
| group_by   | group_by    | groupBy      |
| arrange    | sortBy      | sortBy      |
| -          | zip         | zipMap       | 
| accumulate | sum         | sum          |
| -          | min         | min          | 
| -          | max         | max          |
| -          | mean        | mean         |
| flatten    | flatten     | -            | 
