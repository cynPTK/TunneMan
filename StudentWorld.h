#ifndef STUDENTWORLD_H_
#define STUDENTWORLD_H_

#include "GameWorld.h"
#include "GameConstants.h"
#include "Actor.h"
#include <string>
#include <vector>

class Actor;
class TunnelMan;
class Earth;

// Students:  Add code to this file, StudentWorld.cpp, Actor.h, and Actor.cpp

class StudentWorld : public GameWorld
{
public:
	StudentWorld(std::string assetDir);

	~StudentWorld();

	virtual int init();

	virtual int move();

	virtual void cleanUp();

	void dig();

	void clearSquare(int x, int y);

	Earth* getField(int x, int y);

	TunnelMan*& getPlayer();

	std::vector<Actor*>& getObj();

	void grabOil();

	const bool isclear(int x, int y);

	bool isBoulder(int x, int y);

	void deadProtestor();
private:
	Earth* Field[64][64];
	TunnelMan* Player;
	std::vector<Actor*> obj;
	int numBarrel;
	int sinceSpawnP;
	int numProtestor;

	void generate();
	bool checkgen(int x, int y);
	std::string display();
	void newActors();
	void clearDeadObj();
};

#endif // STUDENTWORLD_H_
