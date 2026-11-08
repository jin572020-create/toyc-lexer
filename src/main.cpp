#include <algorithm>
#include <array>
#include <cctype>
#include <iostream>
#include <map>
#include <queue>
#include <set>
#include <sstream>
#include <string>
#include <vector>

using namespace std;
namespace {
constexpr int ASCII = 128;
struct State {
    vector<int> epsilon;
    vector<pair<set<int>, int>> edges;
    int accept = -1;
};
struct Rule { string kind; int priority; };
struct Fragment { int start, end; };
struct Token { string kind, lexeme; int line, column; };

struct Automaton {
    vector<State> nfa;
    vector<Rule> rules;
    vector<array<int, ASCII>> delta;
    vector<int> accepting;
    int start = 0;
    int fresh() { nfa.emplace_back(); return (int)nfa.size()-1; }
    Fragment literal(const string& s) {
        int a=fresh(), cur=a;
        for (unsigned char c : s) { int nxt=fresh(); nfa[cur].edges.push_back({{c},nxt}); cur=nxt; }
        return {a,cur};
    }
    Fragment charset(const set<int>& chars) {
        int a=fresh(), b=fresh(); nfa[a].edges.push_back({chars,b}); return {a,b};
    }
    Fragment concat(Fragment x, Fragment y) { nfa[x.end].epsilon.push_back(y.start); return {x.start,y.end}; }
    Fragment alternate(Fragment x, Fragment y) {
        int a=fresh(), b=fresh(); nfa[a].epsilon={x.start,y.start};
        nfa[x.end].epsilon.push_back(b); nfa[y.end].epsilon.push_back(b); return {a,b};
    }
    Fragment star(Fragment x) {
        int a=fresh(), b=fresh(); nfa[a].epsilon={x.start,b};
        nfa[x.end].epsilon={x.start,b}; return {a,b};
    }
    static set<int> letters() {
        set<int> s={'_'};
        for(int c='a';c<='z';++c)s.insert(c);
        for(int c='A';c<='Z';++c) s.insert(c);
        return s;
    }
    static set<int> digits(bool nonzero=false) {
        set<int> s; for(int c=nonzero?'1':'0';c<='9';++c)s.insert(c); return s;
    }
    void add(const string& kind, Fragment f) {
        int idx=(int)rules.size(); rules.push_back({kind,idx});
        nfa[f.end].accept=idx; nfa[0].epsilon.push_back(f.start);
    }
    Automaton() {
        fresh(); // merged Thompson-style NFA start
        // Keyword rules precede identifier: accepting priority resolves equal-length ties.
        for(const char* k : {"int","void","const","if","else","while","break","continue","return"})
            add("'"+string(k)+"'",literal(k));
    }
    void finishRules() {
        auto alpha=letters(), alnum=alpha, dg=digits();
        alnum.insert(dg.begin(),dg.end());
        add("Ident",concat(charset(alpha),star(charset(alnum))));
        add("IntConst",alternate(literal("0"),concat(charset(digits(true)),star(charset(dg)))));
        for(const char* op : {"==","!=","<=",">=","&&","||","+","-","*","/","%","=","<",">","!","(",")","{","}",";",","})
            add("'"+string(op)+"'",literal(op));
        determinize(); minimize();
    }
    set<int> closure(set<int> s) const {
        queue<int> q; for(int i:s)q.push(i);
        while(!q.empty()) {int v=q.front();q.pop();for(int u:nfa[v].epsilon)if(s.insert(u).second)q.push(u);}
        return s;
    }
    int label(const set<int>& s) const {
        int best=-1;
        for(int v:s)if(nfa[v].accept>=0 && (best<0||nfa[v].accept<best)) best=nfa[v].accept;
        return best;
    }
    void determinize() {
        map<set<int>,int> ids;
        vector<set<int>> subsets;
        auto insert=[&](const set<int>& x) -> int {
            auto it=ids.find(x); if(it!=ids.end())return it->second;
            int id=(int)subsets.size(); ids.emplace(x,id);subsets.push_back(x);
            delta.emplace_back();delta.back().fill(-1); accepting.push_back(label(x));return id;
        };
        insert(closure({0}));
        for(size_t i=0;i<subsets.size();++i) {
            for(int c=0;c<ASCII;++c) {
                set<int> next;
                for(int v:subsets[i]) for(const auto& e:nfa[v].edges)
                    if(e.first.count(c)) next.insert(e.second);
                if(!next.empty()) delta[i][c]=insert(closure(next));
            }
        }
    }
    void minimize() { // accepting-label-preserving partition refinement
        int n=(int)delta.size();
        vector<int> cls(n), next(n);
        map<int,int> labels;
        for(int i=0;i<n;++i) {
            if(!labels.count(accepting[i]))labels[accepting[i]]=(int)labels.size();
            cls[i]=labels[accepting[i]];
        }
        for(;;) {
            map<vector<int>,int> signatures;
            for(int i=0;i<n;++i) {
                vector<int> sig;sig.reserve(ASCII+1);sig.push_back(accepting[i]);
                for(int c=0;c<ASCII;++c) sig.push_back(delta[i][c]<0?-1:cls[delta[i][c]]);
                auto it=signatures.find(sig);
                if(it==signatures.end()) it=signatures.emplace(move(sig),(int)signatures.size()).first;
                next[i]=it->second;
            }
            bool same=true;
            for(int i=0;i<n&&same;++i)for(int j=0;j<i;++j)
                if((cls[i]==cls[j])!=(next[i]==next[j])) {same=false;break;}
            cls=next; if(same)break;
        }
        int k=*max_element(cls.begin(),cls.end())+1;
        vector<array<int,ASCII>> nd(k);for(auto& row:nd)row.fill(-1);
        vector<int> na(k,-1);
        for(int i=0;i<n;++i) {
            na[cls[i]]=accepting[i];
            for(int c=0;c<ASCII;++c)if(delta[i][c]>=0)nd[cls[i]][c]=cls[delta[i][c]];
        }
        start=cls[0];delta=move(nd);accepting=move(na);
    }
    void dump() const {
        cout<<"state\taccept\ttransitions (printable ASCII)\n";
        for(size_t i=0;i<delta.size();++i) {
            cout<<i<<(i==(size_t)start?" [start]":"")<<'\t';
            if(accepting[i]>=0)cout<<rules[accepting[i]].kind;else cout<<'-';
            cout<<'\t';
            for(int c=32;c<127;++c) if(delta[i][c]>=0)
                cout<<char(c)<<"->"<<delta[i][c]<<' ';
            cout<<'\n';
        }
    }
};

class Lexer {
    const Automaton& a;
    const string& s;
    size_t pos=0;
    int line=1,column=1, errors=0;
    void advance() {
        if(s[pos]=='\n'){++line;column=1;}else ++column;
        ++pos;
    }
    void skip() {
        for(;;) {
            if(pos>=s.size())return;
            if(s[pos]==' '||s[pos]=='\t'||s[pos]=='\n'||s[pos]=='\r') {advance();continue;}
            if(pos+1<s.size()&&s[pos]=='/'&&s[pos+1]=='/') {
                while(pos<s.size()&&s[pos]!='\n') advance();
                continue;
            }
            if(pos+1<s.size()&&s[pos]=='/'&&s[pos+1]=='*') {
                int l=line,c=column;advance();advance();bool closed=false;
                while(pos<s.size()) {
                    if(pos+1<s.size()&&s[pos]=='*'&&s[pos+1]=='/'){
                        advance();advance();closed=true;break;
                    }
                    advance();
                }
                if(!closed){cerr<<l<<':'<<c<<": unterminated comment\n";++errors;}
                continue;
            }
            return;
        }
    }
public:
    Lexer(const Automaton& a,const string& s):a(a),s(s){}
    vector<Token> scan() {
        vector<Token> tokens;
        while(pos<s.size()) {
            skip(); if(pos>=s.size())break;
            size_t start=pos, bestPos=pos;
            int l=line,c=column, bestLine=line,bestCol=column;
            int state=a.start,bestRule=-1;
            while(pos<s.size()&&(unsigned char)s[pos]<ASCII&&a.delta[state][(unsigned char)s[pos]]>=0) {
                state=a.delta[state][(unsigned char)s[pos]];advance();
                if(a.accepting[state]>=0){bestRule=a.accepting[state];bestPos=pos;bestLine=line;bestCol=column;}
            }
            if(bestRule>=0){
                pos=bestPos;line=bestLine;column=bestCol;
                tokens.push_back({a.rules[bestRule].kind,s.substr(start,pos-start),l,c});
            }else{
                pos=start;line=l;column=c;
                cerr<<line<<':'<<column<<": unexpected character ";
                if((unsigned char)s[pos]>=32&&(unsigned char)s[pos]<127)cerr<<"'"<<s[pos]<<"'";
                else cerr<<"(byte "<<int((unsigned char)s[pos])<<")";
                cerr<<'\n';++errors;advance();
            }
        }
        return tokens;
    }
    int errorCount()const{return errors;}
};
string normalized(string s) {
    string t;t.reserve(s.size());
    for(size_t i=0;i<s.size();++i){if(s[i]=='\r'){if(i+1<s.size()&&s[i+1]=='\n')++i;t+='\n';}else t+=s[i];}
    return t;
}
} // namespace
int main(int argc,char** argv){
    if(argc!=2||(string(argv[1])!="-token"&&string(argv[1])!="-dump-dfa")){
        cerr<<"Usage: compiler -token | -dump-dfa\n";return 2;
    }
    Automaton automaton;automaton.finishRules();
    if(string(argv[1])=="-dump-dfa"){automaton.dump();return 0;}
    string src((istreambuf_iterator<char>(cin)),istreambuf_iterator<char>());
    src=normalized(move(src));
    Lexer lexer(automaton,src);auto tokens=lexer.scan();
    for(size_t i=0;i<tokens.size();++i){
        const auto& t=tokens[i];cout<<i<<':'<<t.kind<<':';
        if(t.kind=="IntConst") {
            // Integers are lexical digit strings; range validation is diagnostic only.
            try {size_t parsed=0;stoll(t.lexeme,&parsed,10);cout<<t.lexeme;}
            catch(const out_of_range&){cerr<<t.line<<':'<<t.column<<": integer out of signed 64-bit range\n";}
        } else cout<<'"'<<t.lexeme<<'"';
        cout<<'\n';
    }
    return lexer.errorCount()?1:0;
}
