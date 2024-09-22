#ifndef STUDENTWORLD_H_
#define STUDENTWORLD_H_

#include "GameWorld.h"
#include "Actor.h"
#include <string>
#include <vector>

using namespace std;

// Students:  Add code to this file, StudentWorld.cpp, Actor.h, and Actor.cpp

class StudentWorld : public GameWorld
{
public:
    StudentWorld(string assetPath)
     : GameWorld(assetPath)
    { }
    
    virtual int init();
    virtual int move();
    virtual void cleanUp();
    
    virtual ~StudentWorld();

    void addActorToVector(Actor* a);
    
    bool isThiefBotOnLocation(int x, int y);
    
    Actor* isThiefBotOnGoodie(ThiefBot* bot);
    
    bool shouldBotFirePea(Bot* bot);
    
    bool canBotMove(int x, int y);
    
    bool isPlayerAboveExit(Exit* e);
    
    bool collectedAllCrystals();
    
    bool isPlayerAbovePickupableItem (PickupableItem* p);
    
    bool canPeaMove(int x, int y, Actor* owner);
    
    bool isMarbleAbovePit(Pit* h);
    
    bool canMarbleMove(int x, int y);
    
    bool canPlayerMove(int x, int y);
    
private:
    vector<Actor*> m_actors;
    Player *m_player;
    int m_bonus = 1000; // starting bonus
    int m_totalCrystals = 0; //  cant start off with more than 0 crystals
    int m_collectedCrystals = 0;
    bool m_levelComplete = false;
    
    void updateDisplayText();
    void removeDeadActors();
    Actor* isActorOnLocation(int x, int y);

};

#endif // STUDENTWORLD_H_
