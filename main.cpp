#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <vector>

#define CHILD_NUM 1000000
#define GIFT_NUM 1000
#define n_child_pref 1000
#define n_gift_pref 10

using namespace std;
int wish[CHILD_NUM][10];
int gift[GIFT_NUM][1000];
int answ[CHILD_NUM];
int gift_count[GIFT_NUM];

float avg_normalized_happiness(int *pred){

    int twins = int(0.004 * CHILD_NUM);
    int ratio_gift_happiness = 2;
    int ratio_child_happiness = 2;

    for (int i=0;i<twins;i+=2){
        if (pred[i]!=pred[i+1]) {
            cout << "wrong twin gift!" << endl;
            return -1;
        }
    }
    float max_child_happiness = n_gift_pref * ratio_child_happiness;
    float max_gift_happiness = n_child_pref * ratio_gift_happiness;
    float total_child_happiness = 0;
    float total_gift_happiness = 0;

    for (int c=0;c<CHILD_NUM;++c){
        int g = pred[c];
        if (g>GIFT_NUM){
            cout<<"invalid gift!"<<endl;
            return -1;
        }
        int idx=9;
        while (idx>=0 && wish[c][idx]!=g)
            --idx;
        if (idx>=0)
            total_child_happiness += (10-idx)*ratio_child_happiness;
        else
            total_child_happiness += -1;
        idx=999;
        while (idx>=0 && gift[g][idx]!=c)
            --idx;
        if (idx>=0)
            total_gift_happiness += (1000-idx)*ratio_gift_happiness;
        else
            total_gift_happiness += -1;
    }
    float normalized_child_happiness = total_child_happiness/CHILD_NUM/max_child_happiness;
    float normalized_gift_happiness = total_gift_happiness/GIFT_NUM/max_gift_happiness;
    cout<<"Normalized child happiness: " << normalized_child_happiness <<endl;
    cout<<"Normalized santa happiness: " << normalized_gift_happiness <<endl;
    cout << "total happiness: " << normalized_child_happiness+normalized_gift_happiness<<endl;
    return normalized_child_happiness+normalized_gift_happiness;
}

int main() {
    cout << "Hello, World!" << endl;
    ifstream file( "child_wishlist.csv" );
    int tmp,id,g;
    string s_id,line;
    while ( file.good() )
    {
        getline(file, s_id, ',' );
        id = stoi(s_id);
        for (int i=0;i<10;++i) {
            if (i!=9)
                getline(file, s_id, ',');
            else
                getline(file, s_id, '\n');
            wish[id][i] = stoi(s_id);
        }
        answ[id]=-1;
    }

    ifstream file1( "gift_goodkids.csv" );
    while ( file1.good() )
    {
        getline(file1, s_id, ',');
        id = stoi(s_id);
        for (int i=0;i<1000;++i) {
            if (i!=999)
                getline(file1, s_id, ',');
            else
                getline(file1, s_id, '\n');
            gift[id][i] = stoi(s_id);
        }
        gift_count[id]=0;
    }

    printf("twins\n");
    for (int i=0;i<4000;i+=2){
        g = wish[i][0];
        answ[i] = g;
        answ[i+1] = g;
        gift_count[g] += 2;
    }


    printf("pass 1\n");
    for(int k=0;k<10;++k){
        for(int c=4000;c<CHILD_NUM;++c){
            g = wish[c][k];
            if (gift_count[g]<1000 && answ[c]==-1){
                answ[c] = g;
                gift_count[g] += 1;
            }
        }
    }


    printf("pass 2\n");
    for(int c = 4000; c<CHILD_NUM; ++c){
        if (answ[c]==-1){
            int idx=0;
            while(idx<GIFT_NUM && gift_count[idx]>=1000)
                ++idx;
            answ[c] = idx;
            gift_count[idx] += 1;
        }
    }

    avg_normalized_happiness(answ);
    return 0;
}