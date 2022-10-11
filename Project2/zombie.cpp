// Project Identifier: 9504853406CBAC39EE89AA3AD238AA12CA198043
#include <iostream>
#include <vector>
#include <deque>
#include <queue>
#include <getopt.h>
#include <string.h>
#include <algorithm>
#include "P2random.h"

using namespace std;

struct Zomb{
    string name;
    unsigned int distance;
    unsigned int speed;
    unsigned int health;
    unsigned int round_generated;
};
struct Killedzombs{
    string name;
    unsigned  int active_round;
};
struct Zomb_for_active{
    string name;
    unsigned  int active_round;
};
struct cmp{
    bool operator()(const Zomb *a, const Zomb *b){
        if((a->distance/a->speed)==(b->distance/b->speed))
            if(a->health==b->health)
                return a->name>b->name;
            else
                return a->health>b->health;
        else
            return (a->distance/a->speed)>(b->distance/b->speed);
    }
};

bool cmp_alive(const Zomb_for_active *a, const Zomb_for_active *b ){
        if((a->active_round)==(b->active_round))
            return a->name>b->name;
        else
            return a->active_round>b->active_round;

}
bool cmp_alive2(const Zomb_for_active *a, const Zomb_for_active *b ){
    if((a->active_round)==(b->active_round))
        return a->name>b->name;
    else
        return a->active_round<b->active_round;
}

class Option {
public:
    void get_options(int argc, char** argv);
    bool get_help(){
        return help;
    }
    bool get_verb(){
        return verb;
    }
    unsigned int get_stats(){
        return stats;
    }
    bool get_median(){
        return median;
    }
private:
    bool help= false,verb=false;
    unsigned int stats=0;
    bool median=false;

};
void Option::get_options(int argc, char** argv) {
    int option_index = 0, option = 0;
    struct option longOpts[] = {
            { "help", no_argument, nullptr, 'h' },
            { "verbose", no_argument, nullptr, 'v' },
            { "statistics", required_argument, nullptr, 's' },
            { "median", no_argument, nullptr, 'm' },
            { nullptr, 0, nullptr, '\0' }
    };
    while ((option = getopt_long(argc, argv, "hvs:m", longOpts, &option_index)) != -1) {
        switch (option) {
            case 'h':
                help=true;
                break;
            case 'v':
                verb= true;
                break;
            case 's':
                stats=stoi(optarg);
                break;
            case 'm':
                median=true;
                break;
            default:
                break;
        }
    }
}
int main(int argc,char**argv) {
    priority_queue<Zomb*, vector<Zomb*>, cmp> zombs;

    bool m_start=true;
    deque<Zomb> zombs_create_order;

    deque<Killedzombs> killed_zombs;
    priority_queue<unsigned int,vector<unsigned int>,less<>> large2small;
    priority_queue<unsigned int,vector<unsigned int>,greater<>> small2large;

    
    Option option1;
    try {
        option1.get_options(argc, argv);
        if(option1.get_help()){
            cout<<"help\n";
            return 1;
        }
    }
    catch (std::runtime_error& e) {
        std::cerr << e.what() << std::endl;
        return 1;
    }


    string temp;

    unsigned int quiver_capacity;
    unsigned int random_seed;
    unsigned int max_distance;
    unsigned int max_speed;
    unsigned int max_health;

    getline(cin,temp);

    cin>>temp;
    cin>>quiver_capacity;
    cin>>temp;
    cin>>random_seed;
    cin>>temp;
    cin>>max_distance;
    cin>>temp;
    cin>>max_speed;
    cin>>temp;
    cin>>max_health;
    P2random::initialize(random_seed,max_distance,max_speed,max_health);

    unsigned int round=1,round_input=1,random_zombs,named_zombs,i;
    unsigned int arrows;
    Zomb winner,temp_zomb;
    bool alive=true;
    auto iterator_for_zomb=zombs_create_order.begin();


    while(!cin.eof()){
        getline(cin,temp);
        getline(cin,temp);
        cin>>temp;
        cin>>round_input;
        cin>>temp;
        cin>>random_zombs;
        cin>>temp;
        cin>>named_zombs;

        iterator_for_zomb = zombs_create_order.begin();
        //move
        while(round<=round_input){
            if (option1.get_verb())
                cout << "Round: " << round << '\n';
            arrows = quiver_capacity;
            iterator_for_zomb = zombs_create_order.begin();
            while (iterator_for_zomb != zombs_create_order.end()) {
                if (iterator_for_zomb->health == 0) {
                    iterator_for_zomb++;
                } else {
                    if ((iterator_for_zomb)->speed < (iterator_for_zomb)->distance)
                        (iterator_for_zomb)->distance -= (iterator_for_zomb)->speed;
                    else {
                        (iterator_for_zomb)->distance = 0;
                        if (alive) {
                            alive = false;
                            winner = {(iterator_for_zomb)->name, (iterator_for_zomb)->distance,
                                      (iterator_for_zomb)->speed, (iterator_for_zomb)->health,
                                      (iterator_for_zomb)->round_generated};
                        }
                    }
                    if (option1.get_verb())
                        cout << "Moved: " << (iterator_for_zomb)->name << " (distance: "
                             << (iterator_for_zomb)->distance << ", speed: " << (iterator_for_zomb)->speed <<
                             ", health: " << (iterator_for_zomb)->health << ")\n";

                    iterator_for_zomb++;
                }
            }
            if (!alive) {
                cout << "DEFEAT IN ROUND " << round << "! " << winner.name << " ate your brains! \n";
                break;
            }

            if(round==round_input){
                for (i = 0; i < random_zombs; i++) {
                    temp_zomb = {P2random::getNextZombieName(), P2random::getNextZombieDistance(),
                                 P2random::getNextZombieSpeed(), P2random::getNextZombieHealth(), round};
                    if (option1.get_verb()) {
                        cout << "Created: " << temp_zomb.name << " (distance: " << temp_zomb.distance << ", speed: "
                             << temp_zomb.speed <<
                             ", health: " << temp_zomb.health << ")\n";
                    }
                    zombs_create_order.push_back(temp_zomb);

                    zombs.push(&zombs_create_order.back());

                }
                for (i = 0; i < named_zombs; i++) {
                    cin >> temp_zomb.name;
                    cin >> temp;
                    cin >> temp_zomb.distance;
                    cin >> temp;
                    cin >> temp_zomb.speed;
                    cin >> temp;
                    cin >> temp_zomb.health;
                    temp_zomb.round_generated = round;
                    if (option1.get_verb()) {
                        cout << "Created: " << temp_zomb.name << " (distance: " << temp_zomb.distance << ", speed: "
                             << temp_zomb.speed <<
                             ", health: " << temp_zomb.health << ")\n";
                    }
                    zombs_create_order.push_back(temp_zomb);
                    zombs.push(&zombs_create_order.back());
                }
            }

            while (arrows > 0) {
                if (zombs.empty())
                    break;

                if (zombs.top()->health > arrows) {
                    zombs.top()->health -=arrows;
                    break;
                } else {

                    killed_zombs.push_back({zombs.top()->name, round - zombs.top()->round_generated + 1});
                    if(option1.get_median()){
                        if(m_start){
                            small2large.push(round - zombs.top()->round_generated + 1);
                            m_start= false;
                        }else{
                            if((round - zombs.top()->round_generated + 1)>=small2large.top()){
                                small2large.push(round - zombs.top()->round_generated + 1);
                            }else{
                                large2small.push(round - zombs.top()->round_generated + 1);
                            }

                            if(large2small.size()>small2large.size()+1){
                                small2large.push(large2small.top());
                                large2small.pop();
                            }else if(small2large.size()>large2small.size()+1){
                                large2small.push(small2large.top());
                                small2large.pop();
                            }
                        }
                    }

                    arrows -= zombs.top()->health;
                    zombs.top()->health = 0;
                    if (option1.get_verb()) {
                        cout << "Destroyed: " << zombs.top()->name << " (distance: " << zombs.top()->distance
                             << ", speed: " << zombs.top()->speed <<
                             ", health: 0)\n";
                    }
                    zombs.pop();
                }
            }

            if (option1.get_median()&&!killed_zombs.empty()) {
                if(large2small.size()==small2large.size()){
                    cout << "At the end of round " << round << ", the median zombie lifetime is "
                         << (large2small.top()+small2large.top())/2 << '\n';
                }else if(large2small.size()==small2large.size()+1){
                    cout << "At the end of round " << round << ", the median zombie lifetime is "
                         << large2small.top() << '\n';
                }else{
                    cout << "At the end of round " << round << ", the median zombie lifetime is "
                         << small2large.top() << '\n';
                }
            }
            round++;
        }
        if (!alive) {
            break;
        }
    }

    while(!zombs.empty()&&alive){
        if (option1.get_verb()){
            cout << "Round: " << round << '\n';
        }
        arrows = quiver_capacity;
        iterator_for_zomb=zombs_create_order.begin();

        while (iterator_for_zomb != zombs_create_order.end()) {
            if (iterator_for_zomb->health == 0) {
                iterator_for_zomb++;
            } else {
                if ((iterator_for_zomb)->speed < (iterator_for_zomb)->distance)
                    (iterator_for_zomb)->distance -= (iterator_for_zomb)->speed;
                else {
                    (iterator_for_zomb)->distance = 0;
                    if (alive) {
                        alive = false;
                        winner = {(iterator_for_zomb)->name, (iterator_for_zomb)->distance,
                                  (iterator_for_zomb)->speed, (iterator_for_zomb)->health,
                                  (iterator_for_zomb)->round_generated};
                    }
                }
                if (option1.get_verb())
                    cout << "Moved: " << (iterator_for_zomb)->name << " (distance: "
                         << (iterator_for_zomb)->distance << ", speed: " << (iterator_for_zomb)->speed <<
                         ", health: " << (iterator_for_zomb)->health << ")\n";
                iterator_for_zomb++;
            }
        }

        if (!alive) {
            cout << "DEFEAT IN ROUND " << round << "! " << winner.name << " ate your brains! \n";
            break;
        }

        while (arrows > 0&&!zombs.empty()) {
            
            if (zombs.top()->health > arrows) {
                zombs.top()->health-=arrows;
                break;
            } else {

                killed_zombs.push_back({zombs.top()->name, round - zombs.top()->round_generated + 1});

                if(option1.get_median()){
                    if(m_start){
                        small2large.push(round - zombs.top()->round_generated + 1);
                        m_start= false;
                    }else{
                        if((round - zombs.top()->round_generated + 1)>=small2large.top()){
                            small2large.push(round - zombs.top()->round_generated + 1);
                        }else{
                            large2small.push(round - zombs.top()->round_generated + 1);
                        }

                        if(large2small.size()>small2large.size()+1){
                            small2large.push(large2small.top());
                            large2small.pop();
                        }else if(small2large.size()>large2small.size()+1){
                            large2small.push(small2large.top());
                            small2large.pop();
                        }
                    }
                }


                arrows-=zombs.top()->health;

                zombs.top()->health = 0;
                if (option1.get_verb()) {
                    cout << "Destroyed: " << zombs.top()->name << " (distance: " << zombs.top()->distance
                         << ", speed: " << zombs.top()->speed <<
                         ", health: 0)\n";
                }
                zombs.pop();
            }
        }

        if (option1.get_median()&&!killed_zombs.empty()) {
            if(large2small.size()==small2large.size()){
                cout << "At the end of round " << round << ", the median zombie lifetime is "
                     << (large2small.top()+small2large.top())/2 << '\n';
            }else if(large2small.size()==small2large.size()+1){
                cout << "At the end of round " << round << ", the median zombie lifetime is "
                     << large2small.top() << '\n';
            }else{
                cout << "At the end of round " << round << ", the median zombie lifetime is "
                     << small2large.top() << '\n';
            }
        }
        round++;
    }

    if(!alive)
        round++;

    unsigned int s=option1.get_stats();
    if(alive&&!killed_zombs.empty()){
        cout<<"VICTORY IN ROUND "<<round-1<<"! "<<killed_zombs.back().name<<" was the last zombie.\n";
    }

    if(s!=0){
        auto it_for_kill=killed_zombs.begin();

        cout<<"Zombies still active: "<<zombs.size()<<'\n';

        i=0;
        cout<<"First zombies killed:\n";
        while(i<s){
            if(it_for_kill==killed_zombs.end())
                break;
            cout<<it_for_kill->name<<" "<<i+1<<'\n';
            it_for_kill++;
            i++;
        }

        cout<<"Last zombies killed:\n";
        it_for_kill=killed_zombs.end();
        if(!killed_zombs.empty()){
            it_for_kill--;
            if(option1.get_stats()>killed_zombs.size()){
                auto s1=killed_zombs.size();
                if(s1!=0){
                    while(1){
                        cout<<it_for_kill->name<<" "<<s1<<'\n';
                        if(it_for_kill==killed_zombs.begin())
                            break;
                        it_for_kill--;
                        if(s1==1){
                            break;
                        }
                        s1--;
                    }
                }
            }else{
                i=s;
                while(1){
                    cout<<it_for_kill->name<<" "<<i<<'\n';
                    if(it_for_kill==killed_zombs.begin()||i==1)
                        break;
                    it_for_kill--;
                    i--;
                }
            }
        }

        vector<Zomb_for_active *> active;
        vector<Zomb_for_active * > active2;
        it_for_kill=killed_zombs.begin();
        Zomb_for_active *temp1;
        while(it_for_kill!=killed_zombs.end()){
            temp1->name=it_for_kill->name;
            temp1->active_round=it_for_kill->active_round;
            active.push_back(temp1);
            active2.push_back(temp1);

            it_for_kill++;
        }

        while(!zombs.empty()){
            temp1->name=zombs.top()->name;
            temp1->active_round=round-zombs.top()->round_generated;
            active.push_back(temp1);
            active2.push_back(temp1);
            zombs.pop();
        }

        std::sort(active.begin(),active.end(),cmp_alive);
        std::sort(active2.begin(),active2.end(),cmp_alive2);


        i=0;
        cout<<"Most active zombies:\n";
        s=option1.get_stats();
        while(i<s){
            if(active2.empty()){
                break;
            }
            cout<<active2.back()->name<<" "<<active2.back()->active_round<<'\n';
            active2.pop_back();
            i++;
        }

        i=0;
        cout<<"Least active zombies:\n";
        s=option1.get_stats();
        while(i<s){
            if(active.empty()){
                break;
            }
            cout<<active.back()->name<<" "<<active.back()->active_round<<'\n';
            active.pop_back();
            i++;
        }

    }

    return 0;
}


