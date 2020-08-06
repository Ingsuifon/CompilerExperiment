#include<bits/stdc++.h>
using namespace std;

struct edge {
    char val;
    int next;
};

vector<edge> v[200];
int id = 0;
string s;
//--------------------NFA-----------------------
stack<int> st, ed, str;

void connect(char ch) {
    int nst, nsst;
    nst = st.top();
    st.pop();
    nsst = st.top();
    if(ch != '@')
        v[nsst].push_back({ch, nst});
}

void func1() {
    ed.push(++id);
    str.push('(');
}

void func2() {
    int nst = st.top();
    int ned = ed.top();
    v[nst].push_back({'^', ned});
    char ch = str.top();
    while(ch != '(') {
       connect(ch);
       str.pop();
       ch = str.top();
    }
    str.pop();
    str.push('@');
    ned = ed.top();
    st.push(ned);
    ed.pop();
}

void func3() {
    int nst = st.top();
    int ned = ed.top();
    v[nst].push_back({'^', ned});
    char ch = str.top();
    while(ch != '(' && ch != 'S') {
        connect(ch);
        str.pop();
        ch = str.top();
    }
}

void func4() {
    int nst = st.top();
    st.pop();
    int nsst = st.top();
    v[nst].push_back({'^', nsst});
    v[nsst].push_back({'^', nst});
    st.push(nst);
}

void func5(char c) {
	str.push(c);
	st.push(++id);
}

void print_nfa() {
    cout << s << "NFA" <<endl;
	for(int i = 0;i <= id; ++i){
		for(auto it: v[i]){
			cout << i << "--" << it.val << "->" << it.next << endl;
		}
	}
}

void ToNFA() {
    str.push('S');
    st.push(id);
    ed.push(++id);
    for(char c: s) {
        if( c == '(')
            func1();
        else if(c == ')')
            func2();
        else if(c == '|')
            func3();
        else if(c == '*')
            func4();
        else
            func5(c);
    }
    char ch = str.top();
    if(st.top() != 0)
    v[st.top()].push_back({'^', 1});
    while(ch != 'S') {
        connect(ch);
        str.pop();
        ch = str.top();
    }
    print_nfa();
}

//--------------------DFA-----------------------
vector<edge> dv[200];
int did = 0;
struct node {
    bool flag;
    set<int> ns;
};
unordered_map<int,node> mp;
bool vis[200];
bool svis[200];
set<char> diff;

node bfs(set<int> u, char c) {
    fill(vis, vis + 200, 0);
    queue<int> q;
    for(auto t: u)
        q.push(t);
    set<int> nextv;
    bool bfg = false;
    while(!q.empty()) {
        int tp = q.front();
        q.pop();
        for(auto it: v[tp]) {
            if(it.val != c)
                continue;
            if(vis[it.next] == true)
                continue;
            vis[it.next] = true;
            nextv.insert(it.next);
            q.push(it.next);
            if(it.next == 1)
                bfg = true;
        }
    }
    return node {bfg, nextv};
}

bool check(set<int> x, set<int> y) {
    if((int)x.size() != (int)y.size())
        return true;
    set<int>::iterator ix, iy;
    for(ix = x.begin(), iy = y.begin(); ix != x.end(); ix++, iy++)
        if(*ix != *iy)
            return true;
    return false;
}

node merge(node a, node b) {
    node c;
    if(a.flag || b.flag)
        c.flag = true;
    c.ns.insert(a.ns.begin(), a.ns.end());
    c.ns.insert(b.ns.begin(), b.ns.end());
    return c;
}

void print_set(set<int> s) {
    cout << "集合为：" << endl;
    for(auto i: s)
        cout << i << " " ;
    cout << endl;
}

void print_dfa() {
    cout << "序号集合：" << endl;
    for(int i = 0; i <= did; ++i) {
        cout << i << " ";
        if(mp[i].flag)
            cout << "是终态，";
        else
            cout << "不是终态，";
        print_set(mp[i].ns);
    }
    cout << "DFA：" << endl;
    for(int i = 0; i <= did; i++)
        for(auto it: dv[i])
            cout << i << "--" << it.val << "->" << it.next << endl;
}

void ToDFA() {
    set<int> nu;
    node nd;
    for(int i = 0; i <= id; i++)
        for(auto it: v[i])
            if(it.val != '^')
                diff.insert(it.val);
    nu.insert(0);
    nd = bfs(nu, '^');    //原初始状态的空闭包
    mp[0] = nd;           //新初始状态集，0为代表元素
    queue<int> q;
    q.push(0);
    nu.clear();
    svis[0] = true;
    while(!q.empty()) {
        int tp = q.front();
        q.pop();
        set<int> tpset = mp[tp].ns;
        for(char c: diff) {
            node cnd = bfs(tpset, c);
        }
    }
}
int main1()
{
    s = "0(010)";
    ToNFA();
    return 0;
}
