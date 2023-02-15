#ifndef ACTOR_H_
#define ACTOR_H_

#include "GraphObject.h"
#include "StudentWorld.h"
#include <stack>
#include <vector>
#include <utility>

class StudentWorld;

// Students:  Add code to this file, Actor.cpp, StudentWorld.h, and StudentWorld.cpp
class Actor : public GraphObject {
public:
	Actor(StudentWorld* World, int imageID, int startX, int startY, Direction startDirection, double size = 1.0, unsigned int depth = 0);
	virtual ~Actor();
	virtual void doSomething() = 0;
	StudentWorld* getWorld();//returns current World
	const bool isdead();//returns if is dead
	void setdead();//sets actor as dead
	double radius_dist(Actor* one, Actor* two);//finds the radius between the center of two actors
	double radius_dist(int x1, int y1, Actor* two);//find the radius between the center of the 4x4 square at x1, y1, and an actor
	const int getID();//returns actor ID
	const int getHealth();//returns actor's health
	const bool isannoyed();//returns if is annoyed
	virtual void annoy(int points);//annoys actors
	virtual bool inProtestorRadius(int x, int y, double radius, int index);


private:
	StudentWorld* m_World;
	bool dead;
	int m_id;

protected:
	bool annoyed;
	int hitPoints;
};

class Earth : public Actor
{
public:
	Earth(StudentWorld* World, int x, int y);
	~Earth();
	void doSomething();
private:

};

class TunnelMan : public Actor
{
public:
	TunnelMan(StudentWorld* World);
	~TunnelMan();
	void doSomething();
	void grabGold();//add gold to inventory
	void grabSonar();//add sonar to inventory
	void grabWater();//add water to inventory
	void annoy(int points);//annoy TunnelMan

	//gets number in inventory
	const int getWater();
	const int getSonar();
	const int getGold();
private:
	int water;
	int sonarCharge;
	int gold;
	bool blocked(int x, int y);//can not move bc blocked by boulder
};

class Boulder : public Actor
{
public:
	Boulder(StudentWorld* World, int x, int y);
	~Boulder();
	void doSomething();
	bool stable(int x, int y);//checks if there is earth under the boulder
	void inProtestorRadius(int x, int y, double radius);//annoys any protestor in radius

private:
	int wait_time;
	void fall();
};

class Squirt : public Actor
{
public:
	Squirt(StudentWorld* World, int x, int y, Direction dir);
	~Squirt();
	void doSomething();
	bool inProtestorRadius(int x, int y, double radius);//decrease protestor health by 2 if in radius
private:
	int distance;
};

class Barrel : public Actor
{
public:
	Barrel(StudentWorld* World, int x, int y);
	~Barrel();
	void doSomething();
};

class Gold : public Actor
{
public:
	Gold(StudentWorld* World, int x, int y, bool drop);
	~Gold();
	void doSomething();
private:
	bool dropped;
	int drop_time;
};


class SonarKit : public Actor
{
public:
	SonarKit(StudentWorld* World, int x, int y);
	~SonarKit();
	void doSomething();
private:
	int time;
	int maxTime;
};

class WaterPool : public Actor
{
public:
	WaterPool(StudentWorld* World, int x, int y);
	~WaterPool();
	void doSomething();
private:
	int time;
	int maxTime;
};

class RegProtest : public Actor
{
public:
	RegProtest(StudentWorld* World, int imageID);
	~RegProtest();
	virtual void doSomething();
	void annoy(int points);//decrease hitPoints by points
	virtual void bribe();//become annoyed

protected:
	int restTime;
	int sinceShout;
	int sincePerp;
	int numMove;
	int TimetoRest;
	std::vector<std::pair<int, int>> path;
	int maze[61][61];

	bool ishorizontal(Direction& next_dir, int& next_x);//checks if horizonal line of sight to player
	bool isvertical(Direction& next_dir, int& next_y);//checks vertial line of sight to player
	bool isperp(Direction& next_dir);//checks for perpendicular movements
	void followPath();//move according to path
	bool faceingPlayerDir();//checks if direction is towards player
	void findNewMove(Direction& next_dir);//finds a new direction to move and steps to move
	bool createMaze(int exit_x, int exit_y);//create a maze with a path to the exit
	void findPath(int exit_x, int exit_y);//creates a path according to the maze
	void resetMaze();//reset the ints the maze
};

class HardProtest : public RegProtest
{
public:
	HardProtest(StudentWorld* World, int imageID);
	~HardProtest();
	void doSomething();
	void annoy(int points);//decrease hitPoints by points
	void bribe();//stare at gold
private:
	int M;
};

#endif // ACTOR_H_
