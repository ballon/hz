#include<bits/stdc++.h>
#include<fstream>
#include<thread>
//#include<omp.h>
//#include<mutex>

using namespace std;
const int inf = 1000000000;
const int MOD = 1000000007;
typedef long long Int;
#define FOR(i,a,b) for(int i=(a); i<=(b);++i)
#define mp make_pair
#define pb push_back
#define sz(s) (int)((s).size())


class Fork {
    mutex mtx;
public:
    Fork() {
        cout<<" fork constructor"<<endl;
    }
    void take() {
        mtx.lock();
    }
    void put() {
        mtx.unlock();
    }
};

class Philosopher {
    Fork *left, *right;
    int id;
public:
    Philosopher() {
    }
    Philosopher(int _id, Fork* _left, Fork* _right) {
        id = _id;
        left = _left;
        right = _right;
    }
    void run() {
        for(int times=0; times<10000; ++times) {
            left->take();
            cout<<"phil "<<id<<" take left fork\n";
            right->take();
            cout<<"phil "<<id<<" take right fork\n";
            cout<<"phi "<<id<<" is eating"<<endl;
            left->put();
            right->put();
            cout<<"phil "<<id<<" put everything back\n";
        }
    }
};

int main() {
    //freopen("input.txt", "r", stdin);//freopen("output.txt","w",stdout);
    const int n = 10;

    Fork *f[n];
    for(int i=0; i<n; ++i) f[i]=new Fork();

    Philosopher *p[n];
    for(int i=0; i<n; ++i) {
        int i1 = i;
        int i2 = (i+1)%n;
        if(i1>i2) swap(i1, i2);
        p[i] = new Philosopher(i, f[i1], f[i2]);
    }

    vector<thread> threads;
    for(int i=0; i<n; ++i) {
        threads.pb(thread(&Philosopher::run, p[i]));
    }
    for(int i=0; i<n; ++i) threads[i].join();
    cout<<"DONE"<<endl;
}
