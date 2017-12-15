#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <algorithm>

#define CHILD_NUM 1000000
#define GIFT_NUM 1000
#define n_child_pref 1000
#define n_gift_pref 10

using namespace std;
int wish[CHILD_NUM][10];
int gift[GIFT_NUM][1000];
int answ[CHILD_NUM];
int gift_count[GIFT_NUM];
short temp[1000000][1000];
int pop_children[CHILD_NUM];
int pop_gift[GIFT_NUM];

typedef struct {
    int c;
    short g;
    short score;
} edge;

vector<edge> data;

bool compare(edge a, edge b) {
    return a.score > b.score;
}

float avg_normalized_happiness(int *pred) {

    int twins = int(0.004 * CHILD_NUM);
    int ratio_gift_happiness = 2;
    int ratio_child_happiness = 2;

    for (int i = 0; i < twins; i += 2) {
        if (pred[i] != pred[i + 1]) {
            cout << "wrong twin gift!" << endl;
            return -1;
        }
    }
    float max_child_happiness = n_gift_pref * ratio_child_happiness;
    float max_gift_happiness = n_child_pref * ratio_gift_happiness;
    float total_child_happiness = 0;
    float total_happiness = 0;
    float total_gift_happiness = 0;

    for (int c = 0; c < CHILD_NUM; ++c) {
        int g = pred[c];
        if (g > GIFT_NUM) {
            cout << "invalid gift!" << endl;
            return -1;
        }
        int idx = 9;
        while (idx >= 0 && wish[c][idx] != g)
            --idx;
        if (idx >= 0)
            total_child_happiness += (10 - idx) * ratio_child_happiness;
        else
            total_child_happiness += -1;
        idx = 999;
        while (idx >= 0 && gift[g][idx] != c)
            --idx;
        if (idx >= 0)
            total_gift_happiness += (1000 - idx) * ratio_gift_happiness;
        else
            total_gift_happiness += -1;
        total_happiness += temp[c][g];
    }
    float normalized_child_happiness = total_child_happiness / CHILD_NUM / max_child_happiness;
    float normalized_gift_happiness = total_gift_happiness / GIFT_NUM / 1000 / max_gift_happiness;
    cout << "Normalized child happiness: " << normalized_child_happiness << endl;
    cout << "Normalized santa happiness: " << normalized_gift_happiness << endl;
    cout << "total happiness: " << normalized_child_happiness + normalized_gift_happiness << endl;
    float normalized_happiness = total_happiness / 2000000.0/1000;
    cout << "Normalized happiness: " << normalized_happiness << endl;
    return normalized_child_happiness + normalized_gift_happiness;
//    return normalized_child_happiness;
}

short get_score(int c, int g) {
    int idx = 9;
    short score = 0;
    while (idx >= 0 && wish[c][idx] != g)
        --idx;
    if (idx >= 0)
        score += (10 - idx) * 100;
    else
        score += -100;
    idx = 999;
    while (idx >= 0 && gift[g][idx] != c)
        --idx;
    if (idx >= 0)
        score += (1000 - idx);
    else
        score += -1;
    return score;
}

int main() {
    cout << "Hello, World!" << endl;
    ifstream file("child_wishlist.csv");
    int tmp, id, g;
    string s_id, line;
    while (file.good()) {
        getline(file, s_id, ',');
        id = stoi(s_id);
        for (int i = 0; i < 10; ++i) {
            if (i != 9)
                getline(file, s_id, ',');
            else
                getline(file, s_id, '\n');
            wish[id][i] = stoi(s_id);
        }
        answ[id] = -1;
    }

    ifstream file1("gift_goodkids.csv");
    while (file1.good()) {
        getline(file1, s_id, ',');
        id = stoi(s_id);
        for (int i = 0; i < 1000; ++i) {
            if (i != 999)
                getline(file1, s_id, ',');
            else
                getline(file1, s_id, '\n');
            gift[id][i] = stoi(s_id);
        }
        gift_count[id] = 0;
    }

    for (int c = 0; c < CHILD_NUM; ++c)
        for (int g = 0; g < GIFT_NUM; ++g)
            temp[c][g] = -101;

    // initialize popularity
    for (int c = 0; c < CHILD_NUM; ++c)
        pop_children[c] = 0;

    for (int g = 0; g < GIFT_NUM; ++g)
        pop_gift[g] = 0;

    // are the coeficients 1 for children and 10 for santa correct?
    // children: (10-idx)/10/1M
    // santa: (1000-idx)/1000/1M
    // update children's happiness
    for (int c = 0; c < CHILD_NUM; ++c)
        for (int k = 0; k < 10; ++k) {
            temp[c][wish[c][k]] = 200 * (10 - k)-1;
            pop_gift[wish[c][k]] += 200 * (10 - k);
        }

    // update santa's happiness
    for (int g = 0; g < GIFT_NUM; ++g)
        for (int k = 0; k < 1000; ++k) {
            if (temp[gift[g][k]][g] == -101)
                temp[gift[g][k]][g] = 2*(1000 - k)-100;
            else
                temp[gift[g][k]][g] += 2*(1000 - k)+1;
            pop_children[gift[g][k]] += 2*(1000 - k);
        }


    printf("twins\n");
    // find the best gift for twins
    long sum = 0;
    for (int i = 0; i < 4000; i += 2) {
        int max_happiness = -20000;
        int final_g = 0;
        for (int g = 0; g < 1000; ++g) {
            if (gift_count[g] <= 998)
                if (temp[i][g] + temp[i + 1][g] > max_happiness) {
                    final_g = g;
                    max_happiness = temp[i][g] + temp[i + 1][g];
                }
        }
        answ[i] = final_g;
        answ[i + 1] = final_g;
        gift_count[final_g] += 2;
        sum += temp[i][final_g] + temp[i+1][final_g];
    }
    cout<<"twins' happiness: "<< sum/2000000.0/1000;

    // insert non-twin edge into a vector
    for (int c = 4000; c < CHILD_NUM; ++c)
        for (int g = 0; g < GIFT_NUM; ++g)
            if (temp[c][g] > -101) {
                edge d;
                d.c = c;
                d.g = g;
                d.score = temp[c][g];
                data.push_back(d);
            }
    cout << data.size() << endl;
    // sort the data
    std::sort(data.begin(), data.end(), compare);

    printf("pass 1\n");

    long picked = 0;
    sum = 0;
    long sum1 = 0;
    for (unsigned long t = 0; t < data.size(); ++t) {
        if (answ[data[t].c] == -1 && gift_count[data[t].g] < 1000) {
            answ[data[t].c] = data[t].g;
            gift_count[data[t].g] += 1;
            picked += 1;
            sum1 += data[t].score;
        }
        sum += data[t].score;
    }
    cout << "picked edge: " << picked << endl;
    cout << "TOTAL happiness: " << sum/2000.0/1000000<<endl;
    cout << "total picked happiness: " << sum1/2000.0/1000000<<endl;

    printf("pass 2\n");
    for (int c = 4000; c < CHILD_NUM; ++c) {
        if (answ[c] == -1) {
            int idx = 0;
            while (idx < GIFT_NUM && gift_count[idx] >= 1000)
                ++idx;
            answ[c] = idx;
            gift_count[idx] += 1;
        }
    }
    for (int g = 0; g < GIFT_NUM; ++g)
        if (gift_count[g] != 1000) {
            cout << "exceed gift num: " << g << ' ' << gift_count[g] << endl;
            return -1;
        }
    avg_normalized_happiness(answ);
    return 0;
}