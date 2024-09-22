#ifndef ACTOR_H_
#define ACTOR_H_

#include "GraphObject.h"
#include "GameConstants.h"
#include <vector>

using namespace std;

// Students:  Add code to this file, Actor.cpp, StudentWorld.h, and StudentWorld.cpp

class StudentWorld;

// Actor
class Actor: public GraphObject{
    
public:
    Actor(int imageID, int startX, int startY, StudentWorld* world, int dir);
    StudentWorld* getWorld(){
return m_world;
    }
    virtual void setHealth(int health){ }
    virtual int getHealth(){ return 0; }
    virtual bool blocksPlayer(){ return true; }
    virtual void gotHurt(){ }
    virtual void doSomething() = 0;
    
    // Attributes of the game elements
    virtual bool isAlive() { return false; } // To remove dead actors from the Game
    virtual bool isSwallowable() { return false; } // Marble identifier
    virtual bool canSwallowMarbles() { return false; } // Pit identifier
    virtual bool canBeControlled() { return false; } // Player identifier
    virtual bool isWalkOverable() { return false; } // Crystal and other Goodes identifier
    virtual bool toAdvanceLevel() { return false; } // Exit identifer
    virtual bool isUndamageable() { return false; } // Wall, Factories etc. identifier
    virtual bool isGoodie() { return false; } // to identify Pickupableitems
    virtual bool isNotMedicinal() { return false; } // Ammo box differentiator from Medicinal Items
    virtual bool extraHeart() { return false; } // ExtraLife identifier
    virtual bool hundredHealth() { return false; } // RestoreHealth identifier
    virtual bool isBot() { return false; } // to identify Bots and their Types
    virtual bool canOnlyShoot() { return false; } // RageBot identifier
    virtual bool canOnlySteal () { return false; } // Thief Bot identifer
    virtual bool canShootandSteal() { return false; } // MeanThiefBot identifier
    virtual bool producesBot() { return false; } // Botfactory which produces ThiefBots & MeanThiefBots
    
private:
    StudentWorld* m_world;
};


// Alive
class Alive : public Actor{

public:
    Alive(int imageID, int startX, int startY, StudentWorld* world, int dir);
    void setHealth(int health){
        m_health = health;
    }
    int getHealth(){
        return m_health;
    }
    virtual void doSomething() = 0;
    virtual bool isAlive() { return true; }
    
private:
    int m_health;
};


// Player
class Player : public Alive{
    
public:
    Player(int startX, int startY, StudentWorld* world);
    virtual void doSomething();
    virtual void gotHurt();
    int getAmmo(){
        return m_ammo;
    }
    void setAmmo(int a){
        m_ammo = a;
    }
    virtual bool canBeControlled() { return true; }
    
private:
    int m_ammo;
};


// Wall
class Wall : public Actor{
    
public:
    Wall(int startX, int startY, StudentWorld* world);
    virtual void doSomething(){
        
    }
    virtual bool isUndamageable() { return true; }
};


// Marble
class Marble : public Alive{
    
public:
    Marble(int startX, int startY, StudentWorld* world);
    virtual void doSomething(){
        
    }
    virtual void gotHurt(){
        setHealth(getHealth()-2);
    }
    virtual bool isSwallowable() { return true; }
};


// Pea
class Pea : public Alive{
    
public:
    Pea(int startX, int startY, StudentWorld* world, int dir, Actor* owner);
    virtual void doSomething();
    virtual bool blocksPlayer(){
        return false;
    }
private:
    Actor* m_owner;
};

// Pit
class Pit : public Alive{
    
public:
    Pit(int startX, int startY, StudentWorld* world);
    virtual void doSomething();
    virtual bool canSwallowMarbles() { return true; }
};

// Pickupable Item
class PickupableItem : public Alive{
public:
    PickupableItem (int imageID, int startX, int startY, StudentWorld* world, int scoreVal);
    virtual void doSomething();
    int getScoreValue(){
        return m_scoreValue;
    }
    virtual bool blocksPlayer(){
        return false;
    }
    virtual bool isGoodie () { return true; }
private:
    int m_scoreValue;
};

// Crystal
class Crystal : public PickupableItem {
public:
    Crystal(int startX, int startY, StudentWorld* world);
    virtual bool isWalkOverable() { return true; }
};

// Extra Life
class ExtraLife : public PickupableItem {
public:
    ExtraLife(int startX, int startY, StudentWorld* world);
    virtual bool extraHeart() { return true; }
};

// Restore Health
class RestoreHealth : public PickupableItem {
public:
    RestoreHealth(int startX, int startY, StudentWorld* world);
    virtual bool hundredHealth() { return true; }
    
};

// Ammo
class Ammo : public PickupableItem {
public:
    Ammo(int startX, int startY, StudentWorld* world);
    virtual bool isNotMedicinal() { return true; }
    
};

// Exit
class Exit : public Actor{
public:
    Exit(int startX, int startY, StudentWorld* world);
    virtual void doSomething();
    virtual bool blocksPlayer(){
        return false;
    }
    virtual bool toAdvanceLevel() { return true; }
};

// Bot
class Bot : public Alive{
public:
    Bot(int imageID, int startX, int startY, StudentWorld* world, int dir);
    virtual void gotHurt();
    int getTicks(){
        return m_ticks;
    }
    int getCurrentTick(){
        return m_currentTick;
    }
    void setCurrentTick(int n){
        m_currentTick = n;
    }

    virtual void doSomething() = 0;
    virtual bool isBot() { return true; }
private:
    int m_ticks;
    int m_currentTick;

};


// RageBot
class RageBot : public Bot{
public:
    RageBot(int startX, int startY, StudentWorld* world, int dir);
    virtual void doSomething();
    virtual bool canOnlyShoot() { return true; }
};


// ThiefBot
class ThiefBot : public Bot{
public:
    ThiefBot(int imageID, int startX, int startY, StudentWorld* world);
    virtual void doSomething();
    virtual void gotHurt();
    void dropBackGoodie();
    void move();
    int generateUniqueRandoms(vector<int> randDirNumbers);
    
    int getPickedGoodie(){
        return m_pickedGoodie;
    }
    void setPickedGoodie(int good){
        m_pickedGoodie = good;
    }
    virtual bool canOnlyShoot() { return false; }
    virtual bool canOnlySteal () { return true; }

private:
    int m_distanceBeforeTurning;
    int m_walkedDistance;
    int m_pickedGoodie;
    
};

// MeanThiefBot
class MeanThiefBot : public ThiefBot{
public:
    MeanThiefBot(int imageID, int startX, int startY, StudentWorld* world);
    virtual void doSomething();
    virtual bool canOnlySteal () { return false; }
    virtual bool canOnlyShoot() { return false; }
    virtual bool canShootandSteal() { return true;}
};

// ThiefBotFactory
class ThiefBotFactory : public Actor{
public:
    enum ProductType { REGULAR, MEAN }; //as shown in professor smallberg's interfaces
    
    ThiefBotFactory(int startX, int startY, StudentWorld* world, ProductType type);
    virtual void doSomething();
    virtual bool isUndamageable() { return true; }
    virtual bool producesBot () { return true; }
private:
    int m_botToProduce;
};



#endif // ACTOR_H_
