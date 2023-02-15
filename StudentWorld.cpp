#include "StudentWorld.h"
#include "GameConstants.h"
#include <string>
#include <vector>
#include <iterator>
#include <cstdlib>


using namespace std;

class Actor;

GameWorld* createStudentWorld(string assetDir)
{
	return new StudentWorld(assetDir);
}

StudentWorld::StudentWorld(std::string assetDir)
	: GameWorld(assetDir)
{
	Player = nullptr;
	for (int i = 0; i < VIEW_WIDTH; i++)
	{
		for (int j = 0; j < VIEW_HEIGHT; j++)
		{
				Field[j][i] = nullptr;
		}
	}
	numBarrel = 0;
	sinceSpawnP = 1000;
	numProtestor = 0;
}

StudentWorld::~StudentWorld()
{
	delete Player;
	for (int i = 0; i < VIEW_WIDTH; i++)
	{
		for (int j = 0; j < VIEW_HEIGHT; j++)
		{
			if (Field[j][i] != nullptr)
			{
				delete Field[j][i];
				Field[j][i] = nullptr;
			}
		}
	}
	vector<Actor*>::iterator it = obj.begin();
	while (it != obj.end())
	{
		delete* it;
		it = obj.erase(it);
	}
}

int StudentWorld::init()
{
	srand(time(0));
	numBarrel = 0;
	sinceSpawnP = 1000;
	numProtestor = 0;
	Player = new TunnelMan(this);

	for (int x = 0; x < VIEW_WIDTH; x++)
	{
		for (int y = 0; y < 60; y++)
		{
			if (x < 30 || x > 33 || y < 4)
				Field[y][x] = new Earth(this, x, y);
		}
	}

	generate();//create goodies

	return GWSTATUS_CONTINUE_GAME;
}

void StudentWorld::generate()
{
	int b = getLevel() / 2 + 2;
	int B = min(b, 9);

	int g = 5 - getLevel() / 2;
	int G = max(g, 2);

	int l = 2 + getLevel();
	int L = min(l , 21);
	numBarrel = L;

	for (int i = 0; i < B; i++)//boulders
	{
		int Bx = rand() % 59 + 1;
		int By = rand() % 36 + 20;
		if (checkgen(Bx, By) && (Bx < 26 || Bx > 34))
		{
				obj.push_back(new Boulder(this, Bx, By));
				clearSquare(Bx, By);
		}
		else
			i--;
	}
	for (int i = 0; i < G; i++)//gold
	{
		int Gx = rand() % 61;
		int Gy = rand() % 57;
		if (checkgen(Gx, Gy) && (Gx < 26 || Gx > 34))
		{
			obj.push_back(new Gold(this, Gx, Gy, false));
		}
		else
			i--;
	}
	for (int i = 0; i < L; i++)//Oil
	{
		int Lx = rand() % 61;
		int Ly = rand() % 57;
		if (checkgen(Lx, Ly) && (Lx < 26 || Lx > 34))
		{
			obj.push_back(new Barrel(this, Lx, Ly));
		}
		else
			i--;
	}
}

bool StudentWorld::checkgen(int x, int y)
{
	int j = 0;
	for (; j < obj.size(); j++)
	{
		if (obj[j]->getID() != TID_PROTESTER && obj[j]->getID() != TID_HARD_CORE_PROTESTER && obj[j]->radius_dist(x, y, obj[j]) < 6.0)
		{
			return false;
		}
	}
	return true;
}

string StudentWorld::display()
{
	string level = to_string(getLevel());
	if (level.size() == 1)
		level = " " + level;
	string lives = to_string(getLives());
	int p_health = Player->getHealth() * 100 / 10;
	string health = to_string(p_health);
	for (int i = health.size(); i < 3; i++)
		health = " " + health;
	string water = to_string(Player->getWater());
	if (water.size() == 1)
		water = " " + water;
	string gold = to_string(Player->getGold());
	if (gold.size() == 1)
		gold = " " + gold;
	string oil = to_string(numBarrel);//subtracted from oil per level
	if (oil.size() == 1)
		oil = " " + oil;
	string sonar = to_string(Player->getSonar());
	if (sonar.size() == 1)
		sonar = " " + sonar;
	string score = to_string(getScore());
	for (int i = score.size(); i < 6; i++)
		score = "0" + score;

	return "Scr: " + score + "  Lvl: " + level + "  Lives: " + lives + "  Hlth: " + health + "%" + "  Wtr: " + water + "  Gld: " + gold + "  Sonar: " + sonar + "  Oil Left: " + oil;
}

int StudentWorld::move()
{
	string text = display();
    setGameStatText(text);

	newActors();

	Player->doSomething();//player does something
	if (Player->isdead())
	{
		decLives();
		return GWSTATUS_PLAYER_DIED;
	}
	if (numBarrel == 0)
		return GWSTATUS_FINISHED_LEVEL;

	for (int i = 0; i < obj.size(); i++)//objects does something
	{
		obj[i]->doSomething();
		if (Player->isdead())
		{
			decLives();
			return GWSTATUS_PLAYER_DIED;
		}
		if (numBarrel == 0)
			return GWSTATUS_FINISHED_LEVEL;
	}

	clearDeadObj();//delete all dead objects

	if (Player->isdead())
	{
		decLives();
		return GWSTATUS_PLAYER_DIED;
	}
	if (numBarrel == 0)
		return GWSTATUS_FINISHED_LEVEL;


	return GWSTATUS_CONTINUE_GAME;
}

void StudentWorld::newActors()
{
	//spawn goodies
	int G = getLevel() * 25 + 300;
	if (rand() % G == 0)
	{
		if (rand() % 5 == 0)
		{
			if (checkgen(0, 60))
				obj.push_back(new SonarKit(this, 0, 60));//spawns sonarkits
			else
			{
				for (;;)
				{
					int Wx = rand() % 61;
					int Wy = rand() % 61;
					if (isclear(Wx, Wy))
					{
						if (checkgen(Wx, Wy))
						{
							obj.push_back(new WaterPool(this, Wx, Wy));//spawns waterpools if cant spawn sonar kit
							break;
						}
					}
				}
			}
		}
		else
		{
			for (;;)//spawn water pools
			{
				int Wx = rand() % 61;
				int Wy = rand() % 61;
				if (isclear(Wx, Wy))
				{
					if (checkgen(Wx, Wy))
					{
						obj.push_back(new WaterPool(this, Wx, Wy));
						break;
					}
				}
			}
		}
	}

	//spawn protestors
	int t = 200 - getLevel();
	int T = max(25, t);

	if (sinceSpawnP < T)
		sinceSpawnP++;
	else
	{
		int p = 2 + getLevel() * 1.5;
		int P = min(15, p);
		if (numProtestor < P)
		{
			sinceSpawnP = 0;
			numProtestor++;
			int poh = getLevel() * 10 + 30;
			int POH = min(90, poh);//probability hardcore
			if (rand() % 101 <= POH)
				obj.push_back(new HardProtest(this, TID_HARD_CORE_PROTESTER));
			else
				obj.push_back(new RegProtest(this, TID_PROTESTER));
		}
	}
}

void StudentWorld::clearDeadObj()
{
	vector<Actor*>::iterator it = obj.begin();
	while (it != obj.end())
	{
		if ((*it)->isdead())
		{
			delete* it;
			it = obj.erase(it);
		}
		else it++;
	}
}

void StudentWorld::cleanUp()
{
	delete Player;
	for (int i = 0; i < VIEW_WIDTH; i++)
	{
		for (int j = 0; j < VIEW_HEIGHT; j++)
		{
			if (Field[j][i] != nullptr)
			{
				delete Field[j][i];
				Field[j][i] = nullptr;
			}
		}
	}

	vector<Actor*>::iterator it = obj.begin();
	while (it != obj.end())
	{
		delete* it;
		it = obj.erase(it);
	}

}

void StudentWorld::dig()
{
	int x = Player->getX();
	int y = Player->getY();
	int flag = 0;
	for (int i = x; i <= x + 3; i++)
	{
		for (int j = y; j <= y + 3; j++)
		{
			if (Field[j][i] != nullptr)
			{
				delete Field[j][i];
				Field[j][i] = nullptr;
				flag = 1;
			}
		}
	}
	if (flag == 1)//play souud once per dig call
		playSound(SOUND_DIG);
}

void StudentWorld::clearSquare(int x, int y)
{
	for (int i = x; i <= x + 3; i++)
	{
		for (int j = y; j <= y + 3; j++)
		{
			if(Field[j][i] != nullptr)
			{
				delete Field[j][i];
				Field[j][i] = nullptr;
			}
		}
	}
}

Earth* StudentWorld::getField(int x, int y)
{
	return Field[y][x];
}

TunnelMan*& StudentWorld::getPlayer()
{
	return Player;
}

vector<Actor*>& StudentWorld::getObj()
{
	return obj;
}

void StudentWorld::grabOil()
{
	numBarrel--;
}

const bool StudentWorld::isclear(int x, int y)
{
	int count = 0;
	if (isBoulder(x, y) || x > 60 || x < 0 || y > 60 || y < 0)
		return false;
	if (y == 60)
		return true;
	for (int i = x; i <= x + 3; i++)
	{
		for (int j = y; j <= y + 3; j++)
		{
			if (Field[j][i] == nullptr)
				count++;
		}
	}
	if (count == SPRITE_WIDTH*SPRITE_HEIGHT)
		return true;
	return false;
}

bool StudentWorld::isBoulder(int x, int y)
{
	for (int i = 0; i < obj.size(); i++)
	{
		if (obj[i]->getID() == TID_BOULDER && obj[i]->radius_dist(x, y, obj[i]) < 4.0)//annoyed boulder is falling boulder
		{
			Boulder* B = (Boulder*)(obj[i]);
			if (B->stable(B->getX(), B->getY()))
				return true;
		}
	}
	return false;
}

void StudentWorld::deadProtestor()
{
	numProtestor--;
}
