#include <iostream>
#include <string>
#include <vector>
#include <thread>
#include <future>
#include "Split.hpp"
#include "Open.hpp"
#include "Vec.hpp"
#include "Ope.hpp"
#include "Range.hpp"
#include "Enumerate.hpp"
#include "List.hpp"
#include "ListArrow.hpp"
#include "Arrow.hpp"

using namespace std;
namespace OD=OVERDOSE;
using namespace OVERDOSE_EXT;

class Data {
  public:
  int id;
  string firstName;
  string lastName;
};
void classMapTest() {
  vector<Data> data = {{1, "Doe", "Smith"}, {2, "Doe", "Alice"}, {3, "Drown", "Bob"} };
  auto ret = data >> mapper<Data,Data>( [](Data& data){ 
    data.id += 1;
    return data;
  })
  >> mapper<Data,int>( [](Data& data) { 
    cout << "Data id:"<< data.id << " " << data.firstName << " " << data.lastName << endl;
    return data.id;
  })
  >> reducer<int>( 0, [](int& y, int x) { 
    y += x;
  });
  cout << "class sum of id: " << ret << endl;
  
  auto ret2 = data >> mapper<Data,string>( [](Data& data) {
    string a = data.firstName + " " + data.lastName;
    return a;
  }) 
  >> reducer<string>( string(""), [](string& y, string x) {
    y += x + " ";
  } );
  cout << "name combined of data: " << ret2 << endl;

  OD::Range(0,100) >> mapper<int,int>( [](int i) {
    string echo = "";
    if( i%15 == 0 ) {
      echo = "FizzBuzz"; 
    } else if( i%5 == 0 ) {
      echo = "Fizz"; 
    } else if( i%3 == 0 ) {
      echo = "Buzz"; 
    } else {
      echo = to_string(i);
    }
    cout << "FizzBuzz: " << i << " " << echo << endl;
    return 0;
  });

  // groupByのテスト
  OD::Range(0,100) >> mapper<int,int>( [](int i) {
    return i*3;
  }) >> filter<int, int>( [](int i) {
    return i%2 == 1;
  }) >> groupBy<int, int>( [](int i){ 
    return i%5;
  }) >> mapper<tuple<int, vector<int>>, int>( [](auto tupl){
    cout << "KEY: " << get<0>(tupl) << endl;
    auto val = get<1>(tupl) >> sortBy<int, int>([](int a){ return a*-1;}) >> joinToString<int>(",");
    cout << "VAL: " << val << endl;
    return 0; 
  });

  // maxのテスト
  auto ma = vector{2, 3, 6, 4, 1} >> max<int>();
  cout << "MAX: " << ma << endl;
  auto mi = vector{2, 3, 6, 4, 1} >> min<int>();
  cout << "MIN: " << mi << endl;
  auto me = vector{2, 3, 6, 4, 1} >> mean<int>();
  cout << "MEAN: " << me << endl;
}

struct Congress {
  int congress;
  int age;
};
void congresExample() {
  std::string head = "";
  vector<string> keys;

  vector<map<string,string>> dataframe;
  for(auto line : OD::Open("resources/congress-terms.csv") ) {
    //cout << line << endl;
    if(head == "") {  head = line; keys = line >> splitWith(","); continue; }
    vector<string> vals = line >> splitWith(",");
    auto ma = keys >> zipMap<string, string>(vals);
    dataframe.push_back(ma);
  }
  dataframe >> mapper<map<string,string>, Congress>( [](auto ma){
    //for(auto [k,v] : ma) {
    //  cout << k << endl;
    // }
    auto congress = Congress{stoi(ma["congress"]), stoi(ma["age"])};
    return congress;
  }) >> groupBy<int, Congress>([](auto congress){
    return congress.congress;
  }) >> mapper<tuple<int,vector<Congress>>, int>( [](auto pair) {
    auto key = get<0>(pair);
    auto ages = get<1>(pair) 
                >> mapper<Congress, int>( [](auto cong) { return cong.age; });
    auto me = ages >> mean<int>();
    auto ma = ages >> max<int>();
    auto mi = ages >> min<int>();

    cout << "WHEN CONGRESS: " << key << " MEAN AGE: " << me << " MAN AGE: " << ma << " MIN AGE: " << mi << endl;
    return 0;
  });
}

void echoTest() {
  auto src = vector{1, 2, 3, 4};
  src >> mapper<int,int>([](int i ){ return i*i;}) >> echo<int>();
}



void mapperTest() {
  vector<int>({1,2,3,4,5,6})
    >> mapper<int,int>([](int i){ return i*i;}) >> echo<int>();
}

void reducerTest() {
  vector<int>({1,2,3,4,5,6})
    >> reducer<int, int>(0, [](int& y, int x){ y = y+x;} )
    >> let<int>([](auto out){ cout << "REDUCED: " << out << endl;});
}

void filterTest() {
  vector<int>({1,2,3,4,5,6,7}) 
    >> filter<int, int>([](int i){
      return i%2 == 0;
    }) >> echo<int>() ;
}

void groupByTest() {
  vector<int> src = OD::Range(1,100);
  src >> groupBy<int, int>( [](auto i){ 
    return i%5;
  }) >> mapper<tuple<int,vector<int>>, int>( [](auto tup){
    auto [key,value] = tup;
    auto join = value >> joinToString<int>(",");
    cout << "GROUPBY TEST KEY: " << key << " VALUE: " << join << endl;//
    return 0;
  });
}

void sortByTest() {
  vector<int> src = {1,2,3,4,5,6,7};
  src >> sortBy<int, int>([](int i){ return i*-1;}) >> echo<int>();
}

void zipTest() {
  vector<int> source = {1,2,3,4,5,6,7};
  vector<string> target = {"a","b","c","d","e","f","g"};
  source >> zip<int, string>(target) >> let<vector<pair<int,string>>>([](auto ts){
    for(auto t1:ts) {
      auto [src, tgt] = t1;
      cout << "SOURCE: " << src << " TARGET: " << tgt << endl;
    }
  });
}

void sumMeanMaxMinTest() {
  vector<int> source = {1,2,3,4,5,6};
  auto print = [](double i) { cout << i << endl; };
  source >> sum<int>() >> let<int>(print);
  source >> mean<int>() >> let<double>(print);
  source >> max<int>() >> let<int>(print);
  source >> min<int>() >> let<int>(print);
}

void flattenTest() {
  vector<vector<int>> src = { {1,2},{2,3},{3,4},{4,5} };
  src >> flatten<int>() >> echo<int>();
}

void distinctTest() {
  vector<pair<string,int>> src = { {"a",1}, {"b",2}, {"b",3}, {"c",4}, {"a",5} };
  src >> distinct<string, pair<string,int>>([](auto t){
    return get<0>(t);
  }) >> mapper<pair<string,int>,int>( [](auto t){
    auto [key,val] = t;
    cout << "DISTINCT: " << key << " VALUE: " << val << endl;
    return 0;
  }) ;
}

void mapperIndexedTest() {
  vector<string> src = {"a", "b", "b", "c", "a"};
  src >> mapperIndexed<string, int>( [](int index, auto str) {
    cout << "Index: " << index << " Val: " << str << endl;
    return 0;
  });
}

#include <future> 
#include <chrono>
#include <queue>
#include <iomanip>

#include "Reflection.hpp"

template <class T0, class T>
std::string dump(const T* t, const T0* t0) {
  int64_t basePosition = reinterpret_cast<int64_t>(t0);
  int64_t memoryPosition = reinterpret_cast<int64_t>(t);
  auto p = reinterpret_cast<unsigned char const*>(t);
  std::ostringstream stream;
  if constexpr (std::is_same_v<T, std::string>) {
    stream << t->c_str();
  } else {
    for (size_t n = 0 ; n < sizeof(T) ; ++n) 
      stream << std::setw(2) << std::setfill('0') << int(p[n]) << " ";
  }
  std::string type;
  if constexpr (std::is_same_v<T, int>) {
    type = "int";
  }
  if constexpr (std::is_same_v<T, double>) {
    type = "double";
  }
  if constexpr (std::is_same_v<T, std::string>) {
    type = "std::string";
  }
  std::ostringstream result_stream;
  result_stream  << type << "<Left>" <<  memoryPosition-basePosition  << "<Left>" << stream.str() << "<Left>" << sizeof(T);
  
  return result_stream.str();
}
#include <any>
#include <string.h>

struct Sample {
  int a;
  double b;
  std::string c; 

  auto posiParam() {
    cout << dump(&a,this) + "<Right>" + dump(&b,this) + "<Right>" + dump(&c,this) << endl;
  };
};

//lexical recovery
void recoverTest() {
  string input = "int<Left>0<Left>01 00 00 00 <Left>4<Right>double<Left>8<Left>00 00 00 00 00 00 00 64 <Left>8<Right>std::string<Left>16<Left>Hoge<Left>32";
  vector<string> each = OD::Split(input, "<Right>");
  for(auto e:each) {
    vector<string> fields = OD::Split(e,"<Left>");
    auto type  = fields[0];
    auto posi  = int64_t(stoi(fields[1]));
    auto rawVec = OD::Split(fields[2], " ");
    auto size = stoi(fields[3]);
    
    auto sample = Sample{0, 0.0, ""};
    if ( type == "int" ) {
      char charStack[rawVec.size()-2];
      int cur = 0;
      for( auto raw: rawVec) {
        //cout << raw << endl;
        charStack[cur] = char(stoi(raw));
        cur ++;
      }
      int rec = *(int*)(&charStack);   
      cout << "recovered int " << rec << " posi " << posi << endl;
      *(int*)(int64_t(&sample) + posi) = rec; // recover here

      cout << "fullfill" << sample.a << endl;
    }
    if ( type == "double" ) {
      char charStack[rawVec.size()-1];
      int cur = 0;
      for( auto raw: rawVec) {
        //cout << raw << endl;
        charStack[cur] = char(stoi(raw));
        cur ++;
      }
      double rec = *(double*)(&charStack);   
      cout << "recovered double " << rec << " posi " << posi << endl;
      *(double*)(int64_t(&sample) + posi) = rec; // recover here
      cout << "fullfill " << sample.b << endl;
    }
    if ( type == "std::string" ) {
      std::string rec = std::string(fields[2]);
      cout << "recovered hoge " << rec << " posi " <<  posi << endl;
      for(auto c:rec) { 
        cout << "char " << c << endl;
      }
      *(std::string*)(int64_t(&sample) + posi) = rec; // recover here
      cout << "fullfill" << sample.c << endl;
    }

  }
}

int main() {
  // create task dataset
  auto iv = OD::Range(0, 1000);
  iv >> concurrent::mapper<int,int>( [](auto i){
    return i;
  }) >> echo<int>() ;
  
  auto sa1 = Sample{1, 2.0, "Hoge"};
  sa1.posiParam();

  recoverTest();
  /* std::queue<int> que;
  for(auto i:iv) que.push(i);
  // check CPU status
  int cpu_num = std::thread::hardware_concurrency();

  // lambda 
  auto task = [](int i) { 
    string ret = "";
    for(int num = 0; num < 100000000; num++) ret += to_string(num%2);
    return ret; 
  };
  
  
  // make initial status
  std::map<int, std::future<std::string>> workers_table;
  for (int i = 0; i < cpu_num; ++i) {
    workers_table[i] = std::async(std::launch::async, task, que.front() );
    que.pop();
    if(que.size() == 0) return 0;
  }

  // pooling checker of workers, and if finished, add new task

  
  while(true) {
    for (auto& [index, element] : workers_table) {
      auto status = element.wait_for(std::chrono::seconds(1));
      if (status == std::future_status::deferred) {
        std::cout << "index " << index << " deferred\n";
      } else if (status == std::future_status::timeout) {
        std::cout << "index " << index << " timeout\n";
      } else if (status == std::future_status::ready) {
        std::cout << "index ready! " << index << " (実質これが終了ということらしい)\n";
        workers_table[index] = std::async(std::launch::async, task, que.front());
        que.pop();
        if(que.size() == 0) return 0;
      }
    }
  } */
  return 0;
  congresExample();
  echoTest();
  mapperTest();
  reducerTest();
  filterTest();
  groupByTest();
  sortByTest();
  zipTest();
  sumMeanMaxMinTest();
  flattenTest();
  distinctTest();
  mapperIndexedTest();
  return 1;
  classMapTest(); 
  return 1;
  cout << "a" << endl;
  vector<int> as = OD::Range(1,100);
  Vec vs(as);
  vs.print();
  vs.print(",");
  vs.test( [](){ cout << "test" << endl; } );
  // いくつかのラムダ式の写像による
  // データ変換の例
  auto y = vs.map<int>( [](int i){ 
      return i*i; 
    } )
    .map<double>( [](int i){
      return double(i) + 0.5;
    })
    .map<int>( [](double i){
      return int(i*10); 
    })
    .filter( [](int i){
      return i%3 == 0;
    })
    .reduce(1, [](int& y, int i) {
      y = y + i;
    });
  cout << "chaine desc," << y << endl;

  // メソッドチェーンの機能に
  // orderByの機能の例
  vs.map<int> ( [](int i) {
    return i*-1;
  })
  .orderBy ( [](int a, int b) {
    return a < b;
  })
  .print(",");

  // 関数の合成で構築する
  auto f = merge<int, int>( [](int i) { return i*3; }, [](int i) { return i*i; } );
  vs.map<int>(f).print(",");
  
  // Enumerateの例
  vector<string> en = {"a", "b", "c"};
  cout << " AAAAA " << endl;
  for( auto [i,s]: OD::Enumerate(en) ) {
    cout << "A:" << i << " " << s << endl;
  }

  // splitしてEnumerateする際
  for(auto [i, str]: OD::Enumerate( OD::Split("1:2:3:4:5", ":")) ) {
    cout << "Split:" << i << " " << str << endl;
  }

  // Openの便利かんすう
  for( auto line : OD::Open("./Makefile") ) {
    cout << "Opened: " << line << endl; 
  }

  // listのstdの拡張
  OD::List l = OD::Range(1,1000);
  l.map<int>( [](int i){ 
    cout << "step1 " << i << endl; 
    return i*3;
  } )
  /*.map<int>( [](int i ){ 
    cout << "step2 " << i << endl;
    return i%5;
  }).toSet<int>()*/
  .map<int>( [](int i) {
    cout << "step3 " << i << endl;
    return i;
  }).groupBy<int>( [](int i) {
    return i%5;
  }).map<int>( [](tuple<int,OD::List<int>> pair) {
    auto key = std::get<0>(pair);
    auto vals = std::get<1>(pair);
    cout << "step4 " << key << endl;
    vals.print(",");
    return 0 ;
  });
  auto a = "10 11 12 1 2 3" >> split >> mapper<string,int>([](string str){return stoi(str);}) >> sum<int>();
  cout << "sum: " << a << endl;
  auto li = "3 5 7 9 11" 
       >> split 
       >> mapper<string,double>([](auto str){return stod(str);}) 
       >> reducer<double>(1.0, [](auto& y, auto d){ return y *= d;}) 
       >> let<double>( [](auto d) { cout << "End: " << d << endl; return d; });
} 
