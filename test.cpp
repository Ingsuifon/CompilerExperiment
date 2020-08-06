#include <bits/stdc++.h>
using namespace std;

struct edge {
    char val;        //读取的字符
    int next;        //转移到的下一状态
};
vector<vector<edge>> e;  //状态转移邻接表
set<char> alphabet;  //字符集
set<int> allState;   //DFA的所有状态
set<int> term;       //DFA的终止状态集
set<int> nonterm;    //DFA的非终止状态集
int nums;            //DFA的总状态数

//----------------NFA-----------------
int id = 0;  //状态
int l = -1, r = -1;    //当前处理的外围过程包含的最左和最右状态
int justpre, justnext; //临时最左和最右，服务于闭包，当读取普通字符时作为始末状态，当处理完中间子过程后作为其始末状态
string s;    //待转换的正则表达式
stack<int> st, en, Left, Right;  //过程始末状态栈，记录每个过程最左和最右状态的栈，用于还原

void alpha(char c) {  //读取普通字符
    justpre = id++;
    justnext = id++;
    e[justpre].push_back({c, justnext});
    if(l == -1) {
        l = justpre;
        r = justnext;
    }
    else {
        e[r].push_back({'^', justpre});
        r = justnext;
    }
}

void leftParenthesis() {  //读取'('
    int ns = id++;
    int ne = id++;
    st.push(ns);
    en.push(ne);
    if(l == -1) {
        Left.push(ns);
        Right.push(ne);
    }
    else {
        e[r].push_back({'^', ns});
        r = ne;
        Left.push(l);
        Right.push(ne);
        l = r = -1;
    }
}

void rightParenthesis() {  //读取')'
    justpre = st.top();
    justnext = en.top();
    st.pop();
    en.pop();
    e[justpre].push_back({'^', l});
    e[r].push_back({'^', justnext});
    l = Left.top();
    r = Right.top();
    Left.pop();
    Right.pop();
}

void Or() {  //读取'|'
    int nows = st.top();
    int nowe = en.top();
    e[nows].push_back({'^', l});
    e[r].push_back({'^', nowe});
    l = r = -1;
}

void star() {  //读取'*'
    e[justpre].push_back({'^', justnext});
    e[justnext].push_back({'^', justpre});
}

void print_NFA() {
    cout << s << "的NFA：" <<endl;
	for(int i = 0;i < id; i++) {
		for(auto it: e[i]){
			cout << i << "--" << it.val << "-->" << it.next << endl;
		}
	}
	cout << endl;
}

void RegToNFA() {
    e.resize(s.length() << 1);
    st.push(id++);
    en.push(id++);
    for(char ch: s) {
        switch(ch) {
        case '(':
            leftParenthesis();
            break;
        case ')':
            rightParenthesis();
            break;
        case '|':
            Or();
            break;
        case '*':
            star();
            break;
        default:
            alpha(ch);
            break;
        }
    }
    int finalStart = st.top();
    int finalEnd = en.top();
    st.pop();
    en.pop();
    e[finalStart].push_back({'^', l});
    e[r].push_back({'^', finalEnd});
    print_NFA();
}

//----------------DFA-----------------
int did = 0;
vector<vector<edge>> dfae;

struct state {
    bool containedFinal;   //是否包含终止状态
    set<int> ns;
};

unordered_map<int,state> mp;
bool vis[200];
bool svis[200];

state bfs(set<int> &u, char c) {
    fill(vis, vis + 200, false);
    queue<int> clos;
    set<int> follow;
    bool bfg = false;
    for(int tp: u) {
        for(auto it: e[tp]) {
            if(it.val != c)
                continue;
            if(vis[it.next])
                continue;
            vis[it.next] = true;
            follow.insert(it.next);
            if(it.next == 1)
                bfg = true;
            clos.push(it.next);
            while(!clos.empty()) {       //空转移
                int t = clos.front();
                clos.pop();
                for(auto it2: e[t]) {
                    if(it2.val != '^')
                        continue;
                    if(vis[it2.next])
                        continue;
                    vis[it2.next] = true;
                    follow.insert(it2.next);
                    if(it2.next == 1)
                        bfg = true;
                    clos.push(it2.next);
                }
            }
        }
    }
    return state {bfg, follow};
}

bool isDifferent(set<int> &x, set<int> &y) {   //比较集合
    if((int)x.size() != (int)y.size())
        return true;
    set<int>::iterator ix, iy;
    for(ix = x.begin(), iy = y.begin(); ix != x.end(); ix++, iy++)
        if(*ix != *iy)
            return true;
    return false;
}

void print_set(set<int> s) {
    cout << "集合为：" << endl;
    cout << "{ ";
	for(auto i: s) {
		cout << i << " ";
	}
	cout << '}';
	cout << endl;
}

void print_DFA() {
    cout << "状态集合：" << endl;
	for(int i = 0; i < did; ++i) {
		cout << i;
		if(mp[i].containedFinal)
            cout << "是终态，";
		else
            cout << "不是终态，";
		print_set(mp[i].ns);
	}
	cout << "DFA：" << endl;
	for(int i = 0; i < did; ++i) {
		for(auto it: dfae[i]){
			cout << i << "--" << it.val << "-->" << it.next <<endl;
		}
	}
	cout << endl;
}

void print_term() {
    for(int i: term)
        cout << i << ' ';
    cout << endl;
}

void NFAtoDFA() {
    dfae.resize(e.size());
    set<int> nu {0};
    state newStartState = bfs(nu, '^');
    if(newStartState.containedFinal)
        term.insert(0);
    else
        nonterm.insert(0);
    newStartState.ns.insert(0);   //初始状态集也包括原来的初始状态
    mp[did++] = newStartState;
    allState.insert(0);
    if(newStartState.containedFinal)
        term.insert(0);
    queue<int> q;
    q.push(0);
    nu.clear();
    svis[0] = true;
    while(!q.empty()) {
        int tp = q.front();
        q.pop();
        set<int> tpset = mp[tp].ns;
        for(char c: alphabet) {
            state nextset = bfs(tpset, c);
            if(!nextset.ns.size())
                continue;
            bool flag = false;
            for(auto it: mp) {
                if(!isDifferent(it.second.ns, nextset.ns)) {
                    dfae[tp].push_back({c, it.first});
                    flag = true;
                    break;
                }
            }
            if(flag)
                continue;
            allState.insert(did);
            if(nextset.containedFinal)
                term.insert(did);
            else
                nonterm.insert(did);
            mp[did] = nextset;
            dfae[tp].push_back({c, did});
            q.push(did++);
        }
    }
    nums = nonterm.size() + term.size();
    print_DFA();
}

//----------------minDFA-----------------
vector<vector<edge>> mdfe;
unordered_map<int,state> divide;
vector<int> reject;
int mid = 0;
struct elem {
    vector<vector<int>> pre;
    bool indistinguishable;
    elem() {
        indistinguishable = true;
    }
};

int transfer(int p, char c) {
    int next = -1;
    for(auto it: dfae[p]) {
        if(it.val != c)
            continue;
        next = it.next;
        break;
    }
    return next;
}

void init_table(vector<vector<elem>> &t) {
    t.resize(nums);
    for(int i = 0; i < nums; i++)
        t[i].resize(nums);
    for(int nte: nonterm) {
        for(int te: term) {
            t[nte][te].indistinguishable = false;
            t[te][nte].indistinguishable = false;
        }
    }
}

void cross(vector<vector<elem>> &t) {
    for(int i = 0; i < nums - 1; i++) {
        for(int j = i + 1; j < nums; j++) {
            if(t[i][j].indistinguishable == false)
                continue;
            for(char c: alphabet) {
                int inext = transfer(i, c);
                int jnext = transfer(j, c);
                if(inext == jnext)
                    continue;
                if((inext == i && jnext == j) || (inext == j && jnext == i))
                    continue;
                if((inext != -1 && jnext == -1) || (inext == -1 && jnext != -1) || t[inext][jnext].indistinguishable == false) {
                    t[i][j].indistinguishable = false;
                    if(!t[i][j].pre.empty()) {
                        queue<vector<vector<int>>> q;
                        q.push(t[i][j].pre);
                        while(!q.empty()) {
                            vector<vector<int>> tmp = q.front();
                            q.pop();
                            for(vector<int> it: tmp) {
                                t[it[0]][it[1]].indistinguishable = false;
                                if(!t[it[0]][it[1]].pre.empty())
                                    q.push(t[it[0]][it[1]].pre);
                            }
                        }
                    }
                    break;
                }
                else
                    t[inext][jnext].pre.push_back({i, j});
            }
        }
    }
}

void buildQuotientSet(vector<vector<elem>> &t) {
    vector<bool> vis(nums);
    fill(vis.begin(), vis.end(), false);
    for(int i = 0; i < nums; i++) {
        if(vis[i])
            continue;
        set<int> s;
        bool isFinal = nonterm.find(i) == nonterm.end();
        queue<int> q;
        q.push(i);
        while(!q.empty()) {
            int tmp = q.front();
            q.pop();
            s.insert(tmp);
            reject[tmp] = mid;
            vis[tmp] = true;
            for(int j = tmp + 1; j < nums; j++)
                if(t[tmp][j].indistinguishable)
                    q.push(j);
        }
        divide[mid++] = state {isFinal, s};
    }
}

void buildNewTransfer() {
    for(int i = 0; i < mid; i++) {
        for(int it: divide[i].ns) {
            for(char c: alphabet) {
                int next = transfer(it, c);
                if(next == -1)
                    continue;
                bool contain = false;
                for(auto t: mdfe[i]) {
                    if(t.val == c) {
                        contain = true;
                        break;
                    }
                }
                if(contain)
                    continue;
                mdfe[i].push_back({c, reject[next]});
            }
        }
    }
}

void print_minDFA() {
    cout << "商集：" << endl;
	for(int i = 0; i < mid; ++i) {
		cout << i;
		if(divide[i].containedFinal)
            cout << "是终态，";
		else
            cout << "不是终态，";
		print_set(divide[i].ns);
	}
	cout << "minDFA是：" << endl;
	for(int i = 0; i < did; ++i) {
		for(auto it: mdfe[i]){
			cout << i << "--" << it.val << "-->" << it.next <<endl;
		}
	}
}

void minDFA() {
    mdfe.resize(e.size());
    if(nums <= 0)
        return;
    reject.resize(nums);
    vector<vector<elem>> table;
    init_table(table);
    cross(table);
    buildQuotientSet(table);
    buildNewTransfer();
    print_minDFA();
}

int main() {
    cout << "请输入正则表达式：";
    cin >> s;
    for(char c: s)
        if(c != '(' && c != ')' && c != '|' && c != '*' && c != '^')
            alphabet.insert(c);
    RegToNFA();
    NFAtoDFA();
    minDFA();
    return 0;
}
