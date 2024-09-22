#include "Actor.h"
#include "StudentWorld.h"
#include <iostream>
#include <cstdlib>

using namespace std;

// Students:  Add code to this file, Actor.h, StudentWorld.h, and StudentWorld.cpp

// Actor
Actor::Actor(int imageID, int startX, int startY, StudentWorld *world, int dir)
    : GraphObject(imageID, startX, startY, dir)
{
    m_world = world;
    setVisible(true);
}


//Alive
Alive::Alive(int imageID, int startX, int startY, StudentWorld* world,int dir) 
    : Actor(imageID, startX, startY, world, dir)
{ }

// Player
Player::Player(int startX, int startY, StudentWorld* world) 
    : Alive(IID_PLAYER, startX, startY, world, right)
{
    //as given in spec
    setHealth(20);
    m_ammo = 20;
    setDirection(right);
}

void Player::doSomething()
{
    if(getHealth() == 0){ //dead
        return;
    }else{
        int ch;
        if(getWorld()->getKey(ch)){
            switch (ch) {
                case KEY_PRESS_ESCAPE:
                {
                    setHealth(0);
                    break;
                }
                    
                case KEY_PRESS_SPACE:
                {
                    if(m_ammo > 0){
                        Pea *b = new Pea(getX(), getY(), getWorld(), getDirection(), this);
                        getWorld()->addActorToVector(b);
                        m_ammo--;
                        getWorld()->playSound(SOUND_PLAYER_FIRE);
                    }
                    break;
                }
                    
                case KEY_PRESS_LEFT:
                {
                    setDirection(left);
                    if(getWorld()->canPlayerMove(getX()-1,getY()))
                        moveTo(getX()-1, getY());
                    break;
                }
                
                case KEY_PRESS_RIGHT:
                {
                    setDirection(right);
                    if(getWorld()->canPlayerMove(getX()+1,getY()))
                        moveTo(getX()+1, getY());
                    break;
                }

                case KEY_PRESS_UP:
                {
                    setDirection(up);
                    if(getWorld()->canPlayerMove(getX(),getY()+1))
                        moveTo(getX(), getY()+1);
                    break;
                }

                case KEY_PRESS_DOWN:
                {
                    setDirection(down);
                    if(getWorld()->canPlayerMove(getX(),getY()-1))
                        moveTo(getX(), getY()-1);
                    break;
                }
                    
                default:
                {
                    break;
                }
            }
        }
    }
}

void Player::gotHurt(){

    setHealth(getHealth()-2); //decrease current health by 2 points
    
    if(getHealth() > 0){
        getWorld()->playSound(SOUND_PLAYER_IMPACT);
    }else{
        getWorld()->playSound(SOUND_PLAYER_DIE);
    }
        
};

// Wall
Wall::Wall(int startX, int startY, StudentWorld* world) 
    : Actor(IID_WALL, startX, startY, world, none)
{ }


// Marble
Marble::Marble(int startX, int startY, StudentWorld* world)
    : Alive(IID_MARBLE, startX, startY, world, none)
{
    //as per the spec
    setHealth(10);
}


// Pea
Pea::Pea(int startX, int startY, StudentWorld* world, int dir, Actor* owner) 
    : Alive(IID_PEA, startX, startY, world, dir)
{
    setHealth(1);
    m_owner = owner;
}

void Pea::doSomething()
{
    if(getHealth() == 0) // pea does not exist
        return;
    else{
        getWorld()->canPeaMove(getX(), getY(), m_owner); // who shoots the pea
        switch (getDirection()) { //direction of pea
            case up:
                if(getWorld()->canPeaMove(getX(), getY()+1, m_owner)){
                    moveTo(getX(), getY()+1);
                }else setHealth(0);
                
                break;
            case down:
                if(getWorld()->canPeaMove(getX(), getY()-1, m_owner)){
                    moveTo(getX(), getY()-1);
                }else setHealth(0);

                break;
            case left:
                if(getWorld()->canPeaMove(getX()-1, getY(), m_owner)){
                    moveTo(getX()-1, getY());
                }else setHealth(0);

                break;
            case right:
                if(getWorld()->canPeaMove(getX()+1, getY(), m_owner)){
                    moveTo(getX()+1, getY());
                }else setHealth(0);

                break;
            default:
                break;
        }
    }
}

// Pit
Pit::Pit(int startX, int startY, StudentWorld* world) 
    : Alive(IID_PIT, startX, startY, world, none)
{
    setHealth(1);
}

void Pit::doSomething()
{
    if(getHealth() == 0) return;
    else{
        if(getWorld()->isMarbleAbovePit(this)){
            setHealth(0);
        }
    }
}

PickupableItem::PickupableItem (int imageID, int startX, int startY, StudentWorld* world, int scoreVal) 
    : Alive(imageID, startX, startY, world, none)
{
    setHealth(1);
    m_scoreValue = scoreVal;
}

void PickupableItem::doSomething()
{
    if(getHealth() == 0) return;
    else{
        if(getWorld()->isPlayerAbovePickupableItem(this)){
            getWorld()->playSound(SOUND_GOT_GOODIE);
            setHealth(0);
        }
    }
}
// Constructing Elements
// Crystal
Crystal::Crystal(int startX, int startY, StudentWorld* world) 
    : PickupableItem (IID_CRYSTAL, startX, startY, world, 50)
{ }

// Extra Life
ExtraLife::ExtraLife(int startX, int startY, StudentWorld* world) 
    : PickupableItem (IID_EXTRA_LIFE, startX, startY, world, 1000)
{ }

// Restore Health
RestoreHealth::RestoreHealth(int startX, int startY, StudentWorld* world) 
    : PickupableItem (IID_RESTORE_HEALTH, startX, startY, world, 500)
{ }

// Ammo
Ammo::Ammo(int startX, int startY, StudentWorld* world) 
: PickupableItem (IID_AMMO, startX, startY, world, 100)
{ }

// Exit
Exit::Exit(int startX, int startY, StudentWorld* world) 
: Actor(IID_EXIT, startX, startY, world, none)
{
    setVisible(false);
}

void Exit::doSomething()
{
    if(getWorld()->collectedAllCrystals()){
        if(!isVisible()){
            setVisible(true);
            getWorld()->playSound(SOUND_REVEAL_EXIT);
        }
    }
    
    if(isVisible()){
        if(getWorld()->isPlayerAboveExit(this)){
            getWorld()->playSound(SOUND_FINISHED_LEVEL);
        }
    }
}

// Bot
Bot::Bot(int imageID, int startX, int startY, StudentWorld* world, int dir) 
    : Alive(imageID, startX, startY, world, dir)
{
    m_currentTick = 1;
    
    m_ticks = (28 - getWorld()->getLevel()) / 4; // value indicating how frequently it’s allowed to take an action
    if (m_ticks < 3)
        m_ticks = 3;
}

void Bot::gotHurt()
{
    
    setHealth(getHealth()-2);
    if(getHealth() > 0)
        getWorld()->playSound(SOUND_ROBOT_IMPACT);
    else{
        getWorld()->playSound(SOUND_ROBOT_DIE);
    }
}


// RageBot
RageBot::RageBot(int startX, int startY, StudentWorld* world, int dir) 
    : Bot(IID_RAGEBOT, startX, startY, world, dir)
{
    setHealth(10);
}

void RageBot::doSomething()
{
    if(getHealth() <= 0) 
        return;
    else if(getCurrentTick() == getTicks()){
        if(getHealth()!=0 && getWorld()->shouldBotFirePea(this)){
            Pea *b = new Pea(getX(), getY(), getWorld(), getDirection(), this);
            getWorld()->addActorToVector(b);
            getWorld()->playSound(SOUND_ENEMY_FIRE);
        }else{
                switch (getDirection()) {
                    case up:
        
                        if(getWorld()->canBotMove(getX(), getY()+1))
                            moveTo(getX(), getY()+1);
                        else
                            setDirection(down); //switch direction
                        break;
                    case down:
                        if(getWorld()->canBotMove(getX(), getY()-1))
                            moveTo(getX(), getY()-1);
                        else
                            setDirection(up); //switch direction
                        break;
                    case left:
                        if(getWorld()->canBotMove(getX()-1, getY()))
                            moveTo(getX()-1, getY());
                        else
                            setDirection(right); //switch direction
                        break;
                    case right:
                        if(getWorld()->canBotMove(getX()+1, getY()))
                            moveTo(getX()+1, getY());
                        else
                            setDirection(left); //switch direction
                        break;
                    default:
                        break;
                }
            
        }
        
        setCurrentTick(1);
    }else{
        setCurrentTick(getCurrentTick()+1);
    }
}


// ThiefBot
ThiefBot::ThiefBot(int imageID, int startX, int startY, StudentWorld* world) 
    : Bot(imageID, startX, startY, world, right)
{
    setHealth(5);
    m_walkedDistance = 0;
    m_distanceBeforeTurning = (rand() % 6) + 1; // as per spec
    m_pickedGoodie = 0;
    
}

void ThiefBot::doSomething()
{
    if(getHealth() <= 0){
        return;
    }else if(getCurrentTick() == getTicks()){
        // Does not include crystals
        Actor* goodie = getWorld()->isThiefBotOnGoodie(this);
        
        // Pick up goodie, does not include crystals
        if(goodie != nullptr && m_pickedGoodie == 0){
            int random = (rand() % 10) + 1;
            if(random == 1){
                // time to steal goodie hehe
                if (goodie->isNotMedicinal()==true)
                    m_pickedGoodie = 1;
                else if (goodie->extraHeart()==true)
                    m_pickedGoodie = 2;
                else if (goodie->hundredHealth()==true)
                    m_pickedGoodie = 3;
                goodie->setHealth(0);
                getWorld()->playSound(SOUND_ROBOT_MUNCH);
            }else{
                move();
            }
        }else{
            move();
        }

        setCurrentTick(1);
    }else{
        setCurrentTick(getCurrentTick()+1);
    }
    
}

void ThiefBot::gotHurt()
{
    Bot::gotHurt(); //calls gotHurt in Bot
    
    if(getHealth() <= 0){
        if(m_pickedGoodie != 0){
            dropBackGoodie();
        }
    }
}


void ThiefBot::dropBackGoodie()
{
    // Time to give back goodie cause you died :(
    if(m_pickedGoodie == 3){ RestoreHealth *droppedGoodie = new RestoreHealth(getX(),getY(),getWorld());
        getWorld()->addActorToVector(droppedGoodie);
    }
    else if(m_pickedGoodie == 2){ ExtraLife *droppedGoodie = new ExtraLife(getX(),getY(),getWorld());
        getWorld()->addActorToVector(droppedGoodie);
    }
    else if(m_pickedGoodie == 1){ Ammo *droppedGoodie = new Ammo(getX(),getY(),getWorld());
        getWorld()->addActorToVector(droppedGoodie);
    }
}

void ThiefBot::move()
{
    // Can't move if there's an obstacle silly ;)
    bool foundObstruction = false;
    if(m_walkedDistance != m_distanceBeforeTurning){

        switch (getDirection()) {
            case up:
                if(getWorld()->canBotMove(getX(), getY()+1)){
                    moveTo(getX(), getY()+1);
                    m_walkedDistance++;
                    return;
                } else foundObstruction = true;
                break;
            case down:
                if(getWorld()->canBotMove(getX(), getY()-1)){
                    moveTo(getX(), getY()-1);
                    m_walkedDistance++;
                    return;
                } else foundObstruction = true;
                break;
            case left:
                if(getWorld()->canBotMove(getX()-1, getY())){
                    moveTo(getX()-1, getY());
                    m_walkedDistance++;
                    return;
                } else foundObstruction = true;
                break;
            case right:
                if(getWorld()->canBotMove(getX()+1, getY())){
                    moveTo(getX()+1, getY());
                    m_walkedDistance++;
                    return;
                } else foundObstruction = true;
                break;
            default:
                break;
        }
        
    }
    
    if(m_walkedDistance == m_distanceBeforeTurning || foundObstruction)
    {
        
        m_distanceBeforeTurning = (rand() % 6) + 1;
        m_walkedDistance = 0;
        int randomDirNum = (rand() % 4) + 1;
        std::vector<int> randomVector;
        
        int firstLookedDirNum = randomDirNum;
        bool finallyMoved = false;
        
        for(int check = 0; check < 4; check++){
            
            // Up
            if(randomDirNum == 1) {
                if(getWorld()->canBotMove(getX(), getY()+1)){
                    setDirection(up);
                    moveTo(getX(), getY()+1);
                    finallyMoved = true;
                    break;
                } else{
                    randomVector.push_back(randomDirNum);
                    randomDirNum = generateUniqueRandoms(randomVector);
                }
            }
            
            // Down
            else if(randomDirNum == 2) {
                if(getWorld()->canBotMove(getX(), getY()-1)){
                    setDirection(down);
                    moveTo(getX(), getY()-1);
                    finallyMoved = true;
                    break;
                } else{
                    randomVector.push_back(randomDirNum);
                    randomDirNum = generateUniqueRandoms(randomVector);
                }
            }
            
            // Left
            else if(randomDirNum == 3){
                if(getWorld()->canBotMove(getX()-1, getY())){
                    setDirection(left);
                    moveTo(getX()-1, getY());
                    finallyMoved = true;
                    break;
                } else{
                    randomVector.push_back(randomDirNum);
                    randomDirNum = generateUniqueRandoms(randomVector);
                }
            }
            
            // Right
            else if(randomDirNum == 4){
                if(getWorld()->canBotMove(getX()+1, getY())){
                    setDirection(right);
                    moveTo(getX()+1, getY());
                    finallyMoved = true;
                    break;
                } else{
                    randomVector.push_back(randomDirNum);
                    randomDirNum = generateUniqueRandoms(randomVector);
                }
            }
        }
        
        
        if(finallyMoved){
            m_walkedDistance++;
        }else{
            switch (firstLookedDirNum) {
                case 1:
                    setDirection(up);
                    break;
                case 2:
                    setDirection(down);
                    break;
                case 3:
                    setDirection(left);
                    break;
                case 4:
                    setDirection(right);
                    break;
                default:
                    break;
            }
        }
        
    }

}


int ThiefBot::generateUniqueRandoms(vector<int> randDirNumbers) // Helper function for random numbers
{
    if(randDirNumbers.size() == 4){
        return -1;
    }
    
    bool foundARandom = false;
    while (!foundARandom){
        int somethingSimilar = 0;
        int tempRand = (rand() % 4) + 1;
        
        for(int i = 0; i<randDirNumbers.size(); i++){
            if(tempRand == randDirNumbers[i])
                somethingSimilar++;
        }
        
        if(somethingSimilar == 0){
            foundARandom = true;
            return tempRand;
        }
    }
    
    return -1;
}

// MeanThiefBot
MeanThiefBot::MeanThiefBot(int imageID, int startX, int startY, StudentWorld* world) 
    : ThiefBot(imageID, startX, startY, world)
{
    setHealth(8);
}

void MeanThiefBot::doSomething()
{

    if(getHealth() <= 0){
        return;
    }else if(getCurrentTick() == getTicks()){
        // Fire pea
        if(getWorld()->shouldBotFirePea(this)){
            Pea *b = new Pea(getX(), getY(), getWorld(), getDirection(), this);
            getWorld()->addActorToVector(b);
            getWorld()->playSound(SOUND_ENEMY_FIRE);
        
        // Pick up goodie, does not include crystal
        }else if(getWorld()->isThiefBotOnGoodie(this) != nullptr && getPickedGoodie() == 0){
            int random = (rand() % 10) + 1;
            if(random == 1){
                if (getWorld()->isThiefBotOnGoodie(this)->isNotMedicinal())
                    setPickedGoodie(1);
                else if (getWorld()->isThiefBotOnGoodie(this)->extraHeart())
                    setPickedGoodie(2);
                else if (getWorld()->isThiefBotOnGoodie(this)->hundredHealth())
                    setPickedGoodie(3);
                getWorld()->isThiefBotOnGoodie(this)->setHealth(0);
                getWorld()->playSound(SOUND_ROBOT_MUNCH);
            }else{
                move();
            }
        }else{
            move();
        }
        
        setCurrentTick(1);
    }else{
        setCurrentTick(getCurrentTick()+1);
    }
    
}

// ThiefBot Factory
ThiefBotFactory::ThiefBotFactory(int startX, int startY, StudentWorld* world, ProductType type)
    : Actor(IID_ROBOT_FACTORY, startX, startY, world, none)
{
    m_botToProduce = type;
}

void ThiefBotFactory::doSomething()
{
    int x = getX();
    int y = getY();
    int bots = 0;
    // Checking how many bots near the factor according to limitations
    // Look up
    int i = 1;
    while((y-i)>=0 && i<=3){
        if(getWorld()->isThiefBotOnLocation(x, y-i))
        { bots++; }
        i++;
    }
    
    // Look down
    i = 1;
    while((y+i) < VIEW_HEIGHT && i<=3){
        if(getWorld()->isThiefBotOnLocation(x, y+i))
        { bots++; }
        i++;
    }
    
    // Look left
    i = 1;
    while((x-i)>=0 && i<=3){
        if(getWorld()->isThiefBotOnLocation(x-i, y))
        { bots++; }
        i++;
    }

    
    // Look right
    i = 1;
    while((x+i) < VIEW_WIDTH && i<=3){
        if(getWorld()->isThiefBotOnLocation(x+i, y))
        { bots++; }
        i++;
    }

    if(bots < 3 && getWorld()->isThiefBotOnLocation(getX(), getY()) == false){
        int randomNum = (rand() % 50) + 1;// Odds of producing a MTB & TB
        if(randomNum == 1){
             if(m_botToProduce == REGULAR){
                ThiefBot *bot = new ThiefBot(IID_THIEFBOT,x, y, getWorld());
                getWorld()->addActorToVector(bot);
                getWorld()->playSound(SOUND_ROBOT_BORN);
            } else if(m_botToProduce == MEAN){
                MeanThiefBot *bot = new MeanThiefBot(IID_MEAN_THIEFBOT,x, y, getWorld());
                getWorld()->addActorToVector(bot);
                getWorld()->playSound(SOUND_ROBOT_BORN);
            }
        }
    }
}
