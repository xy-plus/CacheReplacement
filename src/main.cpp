#include <algorithm>
#include <cstdio>
#include <cstdlib>
#include <fstream>
#include <iostream>
#include <string>

#include "cache_manager.h"

// #define LOG

using namespace std;

typedef unsigned long long ull;

int cacheSize, assoc, lineSize;
ReplacemntPolicy replPolicy;
bool writeAlloc;
string fileName, fileFmt;

void getArgs(int argc, char** argv) {
    if (argc != 8) {
        cout << "need 7 args: cacheSize, assoc, lineSize, replPolicy, "
                "writeAlloc, fileSize, fileFmt"
             << endl;
        exit(-1);
    }
    cacheSize = atoi(argv[1]);
    assoc = atoi(argv[2]);
    lineSize = atoi(argv[3]);
    string s(argv[4]);
    transform(s.begin(), s.end(), s.begin(), ::tolower);
    if (s == "lru") {
        replPolicy = REPL_LRU_T;
    } else if (s == "random") {
        replPolicy = REPL_RANDOM_T;
    } else if (s == "plru") {
        replPolicy = REPL_PLRU_T;
    } else if (s == "protect_lru") {
        replPolicy = REPL_PROTECT_LRU_T;
    } else {
        cout << "unknown replace policy" << endl;
        exit(-1);
    }
    if (argv[5][0] == 'y' || argv[5][0] == 'Y') {
        writeAlloc = true;
    } else {
        writeAlloc = false;
    }
    fileName = argv[6];
    fileFmt = argv[7];
}

string cmd;
ull addr;
ull cnt = 0;
ull hit = 0;
ull miss = 0;

void run1(char** argv) {
    CacheManager cache(cacheSize, assoc, lineSize, replPolicy, writeAlloc);
#ifdef LOG
    ofstream fout(string("log/") + fileName + "_" +
                  to_string(cacheSize / 1024) + "_" + to_string(assoc) + "_" +
                  to_string(lineSize) + "_" + argv[4] + "_" +
                  (writeAlloc ? "1" : "0"));
#endif
    while (cin >> cmd >> hex >> addr) {
        AccessTypes type = ACCESS_UNSUPPORT0;
        if (cmd == "r" || cmd == "l") {
            type = ACCESS_LOAD;
        } else {
            type = ACCESS_STORE;
        }
        if (cache.LookupAndFillCache(addr, type)) {
            ++hit;
#ifdef LOG
            fout << "Hit" << endl;
#endif
        } else {
#ifdef LOG
            fout << "Miss" << endl;
#endif
            ++miss;
        }
        ++cnt;
        if (cnt % 100000 == 0) {
            cout << "cnt: " << dec << cnt << endl;
        }
    }
}

void run2(char** argv) {
    CacheManager cache(cacheSize, assoc, lineSize, replPolicy, writeAlloc);
#ifdef LOG
    ofstream fout(string("log/") + fileName + "_" +
                  to_string(cacheSize / 1024) + "_" + to_string(assoc) + "_" +
                  to_string(lineSize) + "_" + argv[4] + "_" +
                  (writeAlloc ? "1" : "0"));
#endif
    while (cin >> hex >> addr) {
        AccessTypes type = ACCESS_LOAD;
        if (cache.LookupAndFillCache(addr, type)) {
            ++hit;
#ifdef LOG
            fout << "hit" << endl;
#endif
        } else {
            ++miss;
#ifdef LOG
            fout << "miss" << endl;
#endif
        }
        ++cnt;
        if (cnt % 10000 == 0) {
            cout << "cnt: " << dec << cnt << endl;
        }
    }
}

// int cacheSize, assoc, lineSize;
// ReplacemntPolicy replPolicy;
// bool writeAlloc;
// string fileName, fileFmt;

int main(int argc, char** argv) {
    getArgs(argc, argv);
    freopen((string("../trace/") + fileName + ".trace").c_str(), "r", stdin);
    if (fileFmt == "1")
        run1(argv);
    else
        run2(argv);
    cout << endl;
    cout << "file name: " << dec << fileName << endl;
    cout << "cache size: " << cacheSize / 1024 << "KB" << endl;
    cout << "assoc: " << assoc << endl;
    cout << "line size: " << lineSize << "B" << endl;
    cout << "replace policy: " << argv[4] << endl;
    cout << "wirte alloc: " << (writeAlloc ? "true" : "false") << endl;
    cout << "miss rate: " << miss * 100.0 / cnt << "%" << endl;
    cout << endl;
}
