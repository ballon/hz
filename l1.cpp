#include<bits/stdc++.h>
#include<thread>
using namespace std;
const int inf = 1000000001;
const int MOD = 1000000007;
typedef long long Int;
#define FOR(i,a,b) for(int i=(a); i<=(b);++i)
#define mp make_pair
#define pb push_back
#define sz(s) (int)((s).size())

const int MEMORY_SIZE = 1000;
pair<int,int> mem[MEMORY_SIZE+1];


mutex mtx;

struct query {
    int mode;
    int thread_id;
    int inx;
    int len;
};

queue< query > queries;

const int Q = 10;

void run(int thread_id) {
    cerr<<thread_id<<endl;
    set<int> alloc_inx;
    for(int iter=0; iter<Q; ++iter) {
        if(rand()%2 == 0 || sz(alloc_inx)<2) {
            int len = rand() % (MEMORY_SIZE/2) + 1;
            mtx.lock();
            //cerr<<len<<" "<<thread_id<<endl;
            queries.push({0, thread_id, iter, len});
            mtx.unlock();
            alloc_inx.insert(iter);
        }
        else {
            int need_inx = rand() % alloc_inx.size();
            int i = 0;
            for(int inx : alloc_inx) {
                if(i++ == need_inx) {
                    alloc_inx.erase(inx);
                    mtx.lock();
                    queries.push({1, thread_id, inx, -1});
                    mtx.unlock();
                    break;
                }
            }
        }
    }
}

int prefix[MEMORY_SIZE+1];

int main() {
    freopen("input.txt", "r", stdin);//freopen("output.txt", "w", stdout);
    const int workers = 3;
    vector<thread> threads;
    for(int i=0; i<workers; ++i) {
        threads.pb(thread(run, i));
    }


    memset(mem, -1, sizeof(mem));

    int iter=Q*workers;
    while(iter>0) {
        if(queries.empty()) { mem[0]=make_pair(-1, -1); continue;}
        cout<<"wait";
        mtx.lock();
        query cur = queries.front();
        queries.pop();
        mtx.unlock();
        cout<<"done\n";
        iter--;

        cout<<"Mode:    "<<cur.mode<<endl;
        cout<<"Thread:  "<<cur.thread_id<<endl;
        cout<<"Index:   "<<cur.inx<<endl;
        cout<<"Length:  "<<cur.len<<endl;
        cout<<"----------------------"<<endl;

        if(cur.mode == 0) {
            int len = cur.len;
            prefix[0]=0;
            for(int i=1; i<MEMORY_SIZE; ++i) prefix[i] = prefix[i-1] + (mem[i].first>=0);
            bool ok = false;
            for(int i=1; i<MEMORY_SIZE-len; ++i) if(prefix[i+len-1] == prefix[i-1]) {
                for(int j=i; j<i+len; ++j)
                    mem[j] = make_pair(cur.thread_id, cur.inx);
                ok=true;
                cout<<"For thread "<<cur.thread_id<<" allocated "<<len<<" memory starting with "<<i<<endl;
                break;
            }
            if(!ok) cout<<"Failed..."<<endl;
        } else {
            for(int i=1; i<MEMORY_SIZE; ++i)
                if(mem[i].first==cur.thread_id && mem[i].second==cur.inx) {
                    int j=i;
                    while(j<MEMORY_SIZE && mem[i]==mem[j]) ++j;
                    cout<<"CLEARED "<<i<<" "<<j-1<<" inclusive."<<endl;
                    for(int c=i; c<j; ++c) mem[c]=make_pair(-1, -1);
                    break;
                }
        }
    }

    for(int i=0; i<threads.size(); ++i) threads[i].join();
}
