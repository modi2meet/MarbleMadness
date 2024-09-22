#include "StudentWorld.h"
#include "GameConstants.h"
#include "Level.h"
#include <iostream>
#include <sstream>
#include <string>
#include <iomanip>

using namespace std;

// Students:  Add code to this file, StudentWorld.h, Actor.h, and Actor.cpp


GameWorld* createStudentWorld(string assetPath)
{
    return new StudentWorld(assetPath);
}

int StudentWorld::init()
{
    m_levelComplete = false;
    
    if(getLevel() > 99) 
        return GWSTATUS_PLAYER_WON;
    
    string curLevel = "level0" + to_string(getLevel()) + ".txt";
    Level lev(assetPath());
    Level::LoadResult result = lev.loadLevel(curLevel);
    if (result == Level::load_fail_file_not_found ||
        result == Level::load_fail_bad_format)
        return GWSTATUS_LEVEL_ERROR;
    m_totalCrystals=0; // reset crystals for the new level
    for (int x = 0; x < VIEW_WIDTH; x++) {
        for (int y = 0; y < VIEW_WIDTH; y++) {
            Level::MazeEntry item = lev.getContentsOf(x, y);
            switch (item) {
                case Level::player:
                    m_player = new Player(x,y,this);
                    break;
                case Level::wall:
                    m_actors.push_back(new Wall(x,y,this));
                    break;
                case Level::marble:
                    m_actors.push_back(new Marble(x,y,this));
                    break;
                case Level::pit:
                    m_actors.push_back(new Pit(x,y,this));
                    break;
                case Level::crystal:
                    m_actors.push_back(new Crystal(x,y,this));
                    m_totalCrystals++;
                    break;
                case Level::extra_life:
                    m_actors.push_back(new ExtraLife(x,y,this));
                    break;
                case Level::restore_health:
                    m_actors.push_back(new RestoreHealth(x,y,this));
                    break;
                case Level::ammo:
                    m_actors.push_back(new Ammo(x,y,this));
                    break;
                case Level::exit:
                    m_actors.push_back(new Exit(x,y,this));
                    break;
                case Level::horiz_ragebot:
                    m_actors.push_back(new RageBot(x,y,this,Actor::right));
                    break;
                case Level::vert_ragebot:
                    m_actors.push_back(new RageBot(x,y,this,Actor::down));
                    break;
                case Level::thiefbot_factory:
                    m_actors.push_back(new ThiefBotFactory(x,y,this,ThiefBotFactory::REGULAR));
                    break;
                case Level::mean_thiefbot_factory:
                    m_actors.push_back(new ThiefBotFactory(x,y,this,ThiefBotFactory::MEAN));
                    break;
                default:
                    break;
            }
            
        }
    }
    
    return GWSTATUS_CONTINUE_GAME;
    
}

int StudentWorld::move()
{
    updateDisplayText();
    
    m_player->doSomething();
    
    for(int i = 0; i<m_actors.size(); i++){
        m_actors[i]->doSomething();
        
        if(m_player->getHealth() <= 0){
            decLives();
            return GWSTATUS_PLAYER_DIED;
        }
        
        if(m_levelComplete){
            return GWSTATUS_FINISHED_LEVEL;
        }
    }
    
    removeDeadActors();
    
    if(m_bonus > 0) 
        m_bonus--;
    
    if(m_player->getHealth() <= 0){
        decLives();
        return GWSTATUS_PLAYER_DIED;
    }
    
    if(m_levelComplete){
        return GWSTATUS_FINISHED_LEVEL;
    }
    
    return GWSTATUS_CONTINUE_GAME;
}

void StudentWorld::cleanUp()
{
    delete m_player;
    
    while(!m_actors.empty()){
        delete m_actors[0];
        m_actors.erase(m_actors.begin());
    }
}

StudentWorld::~StudentWorld()
{
    //delete m_player;  //already exists
    
    while(!m_actors.empty()){
        delete m_actors[0];
        m_actors.erase(m_actors.begin());
    }
}

void StudentWorld::addActorToVector(Actor* a)
{
    m_actors.push_back(a);
}

bool StudentWorld::isThiefBotOnLocation(int x, int y)
{
    for(int i = 0; i< m_actors.size(); i++){
        if(m_actors[i]->canOnlySteal() == true || m_actors[i]->canShootandSteal() == true)
            if(m_actors[i]->getX() == x && m_actors[i]->getY() == y)
                return true;
    }
    
    return false;
}


Actor* StudentWorld::isThiefBotOnGoodie(ThiefBot* bot)
{
    for(int i = 0; i<m_actors.size(); i++){
        if(m_actors[i]->extraHeart() == true || m_actors[i]->hundredHealth() == true || m_actors[i]->isNotMedicinal() == true)
            if(m_actors[i]->getX() == bot->getX() && m_actors[i]->getY() == bot->getY())
            {
                return m_actors[i];
            }
    }
    
    return nullptr;
}

bool StudentWorld::shouldBotFirePea(Bot* bot)
{
    if((m_player->getY() == bot->getY()))
    {
        if(bot->getDirection() == Actor::left && m_player->getX() < bot->getX()) {
            for(int x = bot->getX()-1; x>=m_player->getX(); x--){
                Actor* temp = isActorOnLocation(x,bot->getY());
                if(temp != nullptr && (temp->isSwallowable() == true || temp->isUndamageable()==true || temp->canOnlyShoot() == true || temp->canOnlySteal() == true || temp->canShootandSteal()== true || temp->producesBot() == true))
                {
                    return false;
                }
            }
            return true;
        }else if(bot->getDirection() == Actor::right && m_player->getX() > bot->getX()){
            for(int x = bot->getX()+1; x<=m_player->getX(); x++){
                Actor* temp = isActorOnLocation(x,bot->getY());
                    if(temp != nullptr && (temp->isSwallowable() == true || temp->isUndamageable()==true || temp->canOnlyShoot() == true || temp->canOnlySteal() == true || temp->canShootandSteal() == true || temp->producesBot() == true))
                    {
                        return false;
                    }
            }
            return true;
        }
        
        return false;
    }
    
    else if((m_player->getX() == bot->getX()))
    {
        if(bot->getDirection() == Actor::down && m_player->getY() < bot->getY()) {
            for(int y = bot->getY()-1; y>=m_player->getY(); y--){
                Actor* temp = isActorOnLocation(bot->getX(),y);
                if(temp != nullptr && (temp->isSwallowable() == true || temp->isUndamageable()==true || temp->canOnlyShoot() == true || temp->canOnlySteal() == true || temp->canShootandSteal()== true || temp->producesBot() == true))
                {
                    return false;
                }
            }
            return true;
        }else if(bot->getDirection() == Actor::up && m_player->getY() > bot->getY()){
            for(int y = bot->getY()+1; y<=m_player->getY(); y++){
                Actor* temp = isActorOnLocation(bot->getX(),y);
                if(temp != nullptr && (temp->isSwallowable() == true || temp->isUndamageable()==true || temp->canOnlyShoot() == true || temp->canOnlySteal() == true || temp->canShootandSteal()== true || temp->producesBot() == true))
                {
                    return false;
                }
            }
            return true;
        }
        
        return false;
    }

    
    return false;
}

bool StudentWorld::canBotMove(int x, int y)
{
    
    if(m_player->getX() == x && m_player->getY() == y){
        return false;
    }
    
    for(int i = 0; i<m_actors.size(); i++){
        if(m_actors[i]->getX() == x && m_actors[i]->getY() == y)
        {
            if(m_actors[i]->isSwallowable() == true || m_actors[i]->isUndamageable()==true || m_actors[i]->canSwallowMarbles() == true || m_actors[i]->canOnlyShoot() == true || m_actors[i]->canOnlySteal() == true || m_actors[i]->canShootandSteal() == true || m_actors[i]->producesBot() == true)
            {
                return false;
            }
        }
    }
    
    return true;
}

bool StudentWorld::isPlayerAboveExit(Exit* e)
{
    if(e->isVisible() && m_player->getX() == e->getX() && m_player->getY() == e->getY())
    {
        increaseScore(m_bonus + 2000);
        m_levelComplete = true;
        return true;
    }
    
    return false;
}

bool StudentWorld::collectedAllCrystals()
{
    return m_totalCrystals == m_collectedCrystals;
}

bool StudentWorld::isPlayerAbovePickupableItem(PickupableItem* p)
{
    if(m_player->getX() == p->getX() && m_player->getY() == p->getY()){

        increaseScore(p->getScoreValue());

        if(p->isWalkOverable() == true)
        {
            m_collectedCrystals++;
        }
        else if(p->extraHeart()  == true)
        {
            incLives();
        }
        else if(p->hundredHealth() == true)
        {
            m_player->setHealth(20);
        }
        else if(p->isNotMedicinal() == true)
        {
            m_player->setAmmo(m_player->getAmmo() + 20);
        }
        
        return true;
    }

    return false;
}

bool StudentWorld::canPeaMove(int x, int y, Actor* owner)
{
    if (owner == nullptr) {
            return false; // or handle the error condition appropriately
        }
        // Check if the owner is alive (has health greater than 0)
        if (owner->getHealth() <= 0) {
            return false; // or handle the error condition appropriately
        }
    // Player
    if(owner->canBeControlled() != true){
        if(m_player->getX() == x && m_player->getY() == y)
        {
            m_player->gotHurt();
            return false;
        }
    }
    bool returnValue = true;
    // Actors
    for(int i = 0; i<m_actors.size(); i++){
        if(m_actors[i]->getX() == x && m_actors[i]->getY() == y)
        {
            // when it hits walls and bot factories, do nothing
            if(m_actors[i]->isUndamageable()==true){
                returnValue = false;
            }
            
            if(m_actors[i]->producesBot() == true){
                returnValue = false;
            }
            
            if (m_actors[i]->isSwallowable() == true){
                m_actors[i]->gotHurt();
                returnValue = false;
            }
            
            if(owner->canOnlyShoot() == false && m_actors[i]->canOnlyShoot() == true){
                m_actors[i]->gotHurt();
                
                if(m_actors[i]->getHealth() <= 0 && owner->canBeControlled() == true)
                    increaseScore(100);
                returnValue = false;
            }
            
            if(m_actors[i]->canOnlySteal() == true){
                m_actors[i]->gotHurt();
                
                if(m_actors[i]->getHealth() <= 0 && owner->canBeControlled() == true)
                    increaseScore(10);
                returnValue = false;
            }
            
            if(owner->canShootandSteal() == false && m_actors[i]->canShootandSteal() == true){
                m_actors[i]->gotHurt();
                
                if(m_actors[i]->getHealth() <= 0 && owner->canBeControlled() == true)
                    increaseScore(20);
                returnValue = false;
            }
            
        }
        
        
    }
    
    return returnValue;
    
}

bool StudentWorld::isMarbleAbovePit(Pit* h)
{
    for(int i = 0; i<m_actors.size(); i++){
          
        if(m_actors[i]->isSwallowable() == true && m_actors[i]->getX() == h->getX() && m_actors[i]->getY() == h->getY())
        {
            m_actors[i]->setHealth(0);
            return true;
        }
    }
    return false;
}

bool StudentWorld::canMarbleMove(int x, int y)
{
    Actor *actor = isActorOnLocation(x, y);

    if(actor != nullptr){
        if(actor->canSwallowMarbles() == true ) return true;
        else return false;
    }

    return true;
}

bool StudentWorld::canPlayerMove(int x, int y)
{
    bool blocksPlayer = false;
    
     for(int i = 0; i<m_actors.size(); i++){
    
        if(m_actors[i]->getX() == x && m_actors[i]->getY() == y){
            if(m_actors[i]->isSwallowable() == true){ // moving marble alongside player
                int dir = m_player->getDirection();
                switch (dir) {
                    case Actor::up:
                        if(canMarbleMove(x, y+1))
                            m_actors[i]->moveTo(x, y+1);
                        break;
                    case Actor::down:
                        if(canMarbleMove(x, y-1))
                            m_actors[i]->moveTo(x, y-1);
                        break;
                    case Actor::left:
                        if(canMarbleMove(x-1, y))
                            m_actors[i]->moveTo(x-1, y);
                        break;
                    case Actor::right:
                        if(canMarbleMove(x+1, y))
                            m_actors[i]->moveTo(x+1, y);
                        break;
                    default:
                        break;
                }
            }
            
         }
    
        // All other actors
        if(m_actors[i]->getX() == x && m_actors[i]->getY() == y)
        {
            if(m_actors[i]->blocksPlayer()) blocksPlayer = true;
        }
     }
    
    if(blocksPlayer) return false;
    
    return true;
}


void StudentWorld::updateDisplayText()
{
    // Game statistics to be displayed
    ostringstream oss;
    
    int score = getScore();
    oss.fill('0');
    oss << "Score: " << setw(7) << score;
    oss << "  ";
    
    int level = getLevel();
    oss.fill('0');
    oss << "Level: " << setw(2) << level;
    oss << "  ";
    
    int lives = getLives();
    oss.fill(' ');
    oss << "Lives: " << setw(2) << lives;
    oss << "  ";
    
    int health =  ((double)(m_player->getHealth())/20) * 100;
    oss.fill(' ');
    oss << "Health: " << setw(3) << health;
    oss << "%" << "  ";
    
    int ammo = m_player->getAmmo();
    oss.fill(' ');
    oss << "Ammo: " << setw(3) << ammo;
    oss << "  ";
    
    int bonus = m_bonus;
    oss.fill(' ');
    oss << "Bonus: " << setw(4) << bonus;
    oss << "  ";
    
    setGameStatText(oss.str());
}

void StudentWorld::removeDeadActors()
{ // Cleaning up dirty space
    for(int i = 0; i<m_actors.size(); i++){
        Actor *currentActor = m_actors[i];
        
        if(currentActor->isAlive() && currentActor->getHealth() <= 0){
            currentActor->setVisible(false);
            delete m_actors[i];
            m_actors.erase(m_actors.begin()+i);
            continue;
        }
    }
}


Actor* StudentWorld::isActorOnLocation(int x, int y)
{
    for(int i = 0; i<m_actors.size(); i++){
        if(m_actors[i]->getX() == x && m_actors[i]->getY() == y)
            return m_actors[i];
    }
    
    return nullptr;
}
