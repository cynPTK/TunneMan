#include "Actor.h"
#include "StudentWorld.h"
#include <cmath>
#include <algorithm>
#include <vector>
#include <string>
#include <queue>
#include <utility>

// Students:  Add code to this file (if you wish), Actor.h, StudentWorld.h, and StudentWorld.cpp

Actor::Actor(StudentWorld* World, int imageID, int startX, int startY, Direction startDirection, double size, unsigned int depth) :GraphObject(imageID, startX, startY, startDirection, size, depth)
{
	m_World = World;
	dead = false;
	m_id = imageID;
	annoyed = false;
	hitPoints = 0;
}

Actor::~Actor()
{
	setVisible(false);
}

StudentWorld* Actor::getWorld()
{
	return m_World;
}

const bool Actor::isdead()
{
	return dead;
}

void Actor::setdead()
{
	dead = true;
}

double Actor::radius_dist(Actor* one, Actor* two)//calculates the radius distance between two actors
{

	double x = (one->getX() + 2) - (two->getX() + 2);
	double y = (one->getY() + 2) - (two->getY() + 2);
	double dist = x * x + y * y;
	return sqrt(dist);
}

double Actor::radius_dist(int x1, int y1, Actor* two)//calculates the radius distance between two actors
{
	double x = (x1 + 2) - (two->getX() + 2);
	double y = (y1 + 2) - (two->getY() + 2);
	double dist = x * x + y * y;
	return sqrt(dist);
}

const int Actor::getID()
{
	return m_id;
}

const bool Actor::isannoyed()
{
	return annoyed;
}

void Actor::annoy(int points)
{
	hitPoints -= points;
	if (hitPoints <= 0)
		setdead();
}

const int Actor::getHealth()
{
	return hitPoints;
}

bool Actor::inProtestorRadius(int x, int y, double radius, int index)
{
	std::vector<Actor*> obj = getWorld()->getObj();
	for (int i = 0; i < obj.size(); i++)
	{
		if ((obj[i]->getID() == TID_PROTESTER || obj[i]->getID() == TID_HARD_CORE_PROTESTER) && obj[i]->radius_dist(x, y, obj[i]) < radius)
		{
			index = i;
			return true;
		}
	}
	return false;
}

Earth::Earth(StudentWorld* World, int x, int y) :Actor(World, TID_EARTH, x, y, right, 0.25, 3)
{
	setVisible(true);
}

Earth::~Earth()
{
	setdead();
	setVisible(false);
}

void Earth::doSomething()
{
	return;
}

TunnelMan::TunnelMan(StudentWorld* World) : Actor(World, TID_PLAYER, 30, 60, right, 1.0, 0)
{
	setVisible(true);
	hitPoints = 10;
	water = 5;
	sonarCharge = 1;
	gold = 0;
}

TunnelMan::~TunnelMan()
{
	setdead();
	setVisible(false);
}

void TunnelMan::doSomething()
{
	if (isdead())
		return;
	int ch;
	if (getWorld()->getKey(ch) == true)
	{
		int next_x = getX();
		int next_y = getY();

		switch (ch)
		{
		case KEY_PRESS_LEFT:
			if (getDirection() != left)//if tunnelman is not facing direction, face direction, or if at border, dont do anything
			{
				setDirection(left);
				break;
			}
			else if (getX() == 0)
				return;
			next_x--;
			break;
		case KEY_PRESS_RIGHT:
			if (getDirection() != right)
			{
				setDirection(right);
				break;
			}
			else if (getX() == 60)
				return;
			next_x++;
			break;
		case KEY_PRESS_UP:
			if (getDirection() != up)
			{
				setDirection(up);
				break;
			}
			else if (getY() == 60)
				return;
			next_y++;
			break;
		case KEY_PRESS_DOWN:
			if (getDirection() != down)
			{
				setDirection(down);
				break;
			}
			else if (getY() == 0)
				return;
			next_y--;
			break;
		case KEY_PRESS_SPACE://squirt
			if (water != 0)
			{
				int sX = getX();
				int sY = getY();
				switch (getDirection())
				{
				case up:
					sY += 4;
					break;
				case down:
					sY -= 4;
					break;
				case left:
					sX -= 4;
					break;
				case right:
					sX += 4;
					break;
				}
				if (getWorld()->isclear(sX, sY))
					getWorld()->getObj().push_back(new Squirt(getWorld(), sX, sY, getDirection()));
				water--;
				getWorld()->playSound(SOUND_PLAYER_SQUIRT);
			}
			break;
		case KEY_PRESS_ESCAPE://kill self
			hitPoints = 0;
			setdead();
			break;
		case 'Z':
		case 'z':
			if (sonarCharge != 0)//use sonar
			{
				sonarCharge--;
				std::vector<Actor*> obj = getWorld()->getObj();
				for (int i = 0; i < obj.size(); i++)
				{
					if (radius_dist(obj[i], this) < 12.0)
						obj[i]->setVisible(true);
				}
			}
			break;
		case KEY_PRESS_TAB://drop gold
			if (gold > 0)
			{
				getWorld()->getObj().push_back(new Gold(getWorld(), getX(), getY(), true));//true means it is dorpped and can not be picked back up by player
				gold--;
			}
			break;
		default:
			break;
		}

		if (!blocked(next_x, next_y))//if blocked by boulder, dont move
		{
			moveTo(next_x, next_y);
			getWorld()->dig();
		}
	}
}

void TunnelMan::annoy(int points)
{
	hitPoints -= points;
	if (hitPoints <= 0)
	{
		getWorld()->playSound(SOUND_PLAYER_GIVE_UP);
		setdead();
	}
}

void TunnelMan::grabGold()
{
	gold++;
}
void TunnelMan::grabSonar()
{
	sonarCharge++;
}
void TunnelMan::grabWater()
{
	water += 5;
}
const int TunnelMan::getWater()
{
	return water;
}
const int TunnelMan::getSonar()
{
	return sonarCharge;
}
const int TunnelMan::getGold()
{
	return gold;
}

bool TunnelMan::blocked(int x, int y)
{
	std::vector <Actor*> obj = getWorld()->getObj();
	for (int i = 0; i < obj.size(); i++)
	{
		if (obj[i]->getID() == TID_BOULDER && radius_dist(x, y, obj[i]) < 4.0)//4.0 will allow to be within 4x4 square of boulder without touching boulder
		{
			return true;
		}
	}
	return false;
}

Boulder::Boulder(StudentWorld* World, int x, int y) :Actor(World, TID_BOULDER, x, y, down, 1.0, 1)
{
	wait_time = 0;
	setVisible(true);
}

Boulder::~Boulder()
{
	setVisible(false);
}

bool Boulder::stable(int x, int y)
{
	if (getWorld()->getField(x, y - 1) == nullptr &&
		getWorld()->getField(x + 1, y - 1) == nullptr &&
		getWorld()->getField(x + 2, y - 1) == nullptr &&
		getWorld()->getField(x + 3, y - 1) == nullptr)
		return false;
	return true;
}

void Boulder::fall()
{
	int index;
	if (getY() > 0 && !stable(getX(), getY()))//fall until it bottom or earth
	{
		moveTo(getX(), getY() - 1);
		getWorld()->playSound(SOUND_FALLING_ROCK);
		if (radius_dist(this, getWorld()->getPlayer()) < 3.0)
			getWorld()->getPlayer()->annoy(100);
		inProtestorRadius(getX(), getY(), 3.0);

	}
	else
		setdead();
}

void Boulder::inProtestorRadius(int x, int y, double radius)
{
	std::vector<Actor*> obj = getWorld()->getObj();
	for (int i = 0; i < obj.size(); i++)
	{
		if ((obj[i]->getID() == TID_PROTESTER || obj[i]->getID() == TID_HARD_CORE_PROTESTER) && obj[i]->radius_dist(x, y, obj[i]) < radius)
		{
			obj[i]->annoy(100);
			if (obj[i]->isannoyed())
				getWorld()->increaseScore(500);
		}
	}
}

void Boulder::doSomething()
{
	if (!isdead())
	{
		if (wait_time >= 30)
		{
			fall();
		}
		else if (!stable(getX(), getY()))
		{
			wait_time++;
		}
	}
}

Squirt::Squirt(StudentWorld* World, int x, int y, Direction dir) :Actor(World, TID_WATER_SPURT, x, y, dir, 1.0, 1)
{
	distance = 4;
	setVisible(true);
}

Squirt::~Squirt()
{
	setVisible(false);
}

void Squirt::doSomething()
{
	int index = 0;
	if (inProtestorRadius(getX(), getY(), 3.0))
		setdead();
	Direction d = getDirection();
	int next_x = getX();
	int next_y = getY();
	switch (d)
	{
	case up:
		next_y++;
		break;
	case down:
		next_y--;
		break;
	case left:
		next_x--;
		break;
	case right:
		next_x++;
		break;
	}
	if (distance == 0 || !getWorld()->isclear(next_x, next_y))
		setdead();
	else
	{
		moveTo(next_x, next_y);
		distance--;
	}
}

bool Squirt::inProtestorRadius(int x, int y, double radius)
{
	int flag = 0;
	std::vector<Actor*> obj = getWorld()->getObj();
	for (int i = 0; i < obj.size(); i++)
	{
		if ((obj[i]->getID() == TID_PROTESTER || obj[i]->getID() == TID_HARD_CORE_PROTESTER) && obj[i]->radius_dist(x, y, obj[i]) < radius)//finds protestor
		{
			obj[i]->annoy(2);
			if (obj[i]->isannoyed() && (obj[i]->getID() == TID_PROTESTER))
				getWorld()->increaseScore(100);
			else if (obj[i]->isannoyed() && obj[i]->getID() == TID_HARD_CORE_PROTESTER)
				getWorld()->increaseScore(250);
			flag = 1;

		}
	}
	if (flag == 1)
		return true;
	return false;
}

Barrel::Barrel(StudentWorld* World, int x, int y) :Actor(World, TID_BARREL, x, y, right, 1.0, 2)
{
	setVisible(false);
}

Barrel::~Barrel()
{
	setVisible(false);
}

void Barrel::doSomething()
{
	if (isdead())
		return;
	if (!isVisible() && radius_dist(this, getWorld()->getPlayer()) <= 4.0)//player finds
	{
		setVisible(true);
		return;
	}
	else if (radius_dist(this, getWorld()->getPlayer()) <= 3.0)//player collects
	{
		setdead();
		getWorld()->playSound(SOUND_FOUND_OIL);
		getWorld()->increaseScore(1000);
		getWorld()->grabOil();
	}
}

Gold::Gold(StudentWorld* World, int x, int y, bool drop) :Actor(World, TID_GOLD, x, y, right, 1.0, 2)
{
	dropped = drop;
	drop_time = 0;
	if (dropped == true)
		setVisible(true);
	setVisible(false);
}

Gold::~Gold()
{
	setVisible(false);
}

void Gold::doSomething()
{
	int index = 0;
	if (isdead())
		return;
	if (drop_time == 100)
		setdead();
	else if (!isVisible() && radius_dist(this, getWorld()->getPlayer()) <= 4.0)//player finds
	{
		setVisible(true);
		return;
	}
	else if (radius_dist(this, getWorld()->getPlayer()) <= 3.0 && isVisible() && !dropped)//player collects
	{
		setdead();
		getWorld()->playSound(SOUND_GOT_GOODIE);
		getWorld()->increaseScore(10);
		getWorld()->getPlayer()->grabGold();
	}
	else if (dropped && inProtestorRadius(getX(), getY(), 3.0, index))
	{
		setdead();
		getWorld()->playSound(SOUND_PROTESTER_FOUND_GOLD);
		if (getWorld()->getObj()[index]->getID() == TID_PROTESTER)//regular
		{
			getWorld()->increaseScore(25);
			RegProtest* p = (RegProtest*)(getWorld()->getObj()[index]);//downcasting
			p->bribe();
		}
		else//hardcore
		{
			getWorld()->increaseScore(50);
			HardProtest* p = (HardProtest*)(getWorld()->getObj()[index]);//downcasting
			p->bribe();
		}
	}
	else if (dropped)
		drop_time++;
}

SonarKit::SonarKit(StudentWorld* World, int x, int y) : Actor(World, TID_SONAR, x, y, right, 1.0, 2)
{
	setVisible(true);
	time = 0;
	int t = 300 - 10 * getWorld()->getLevel();
	maxTime = std::max(100, t);
}

SonarKit::~SonarKit()
{
	setVisible(false);
}

void SonarKit::doSomething()
{
	if (isdead())
		return;
	if (time == maxTime)//temp state over
		setdead();
	else if (radius_dist(this, getWorld()->getPlayer()) <= 3.0)//player collects
	{
		setdead();
		getWorld()->playSound(SOUND_GOT_GOODIE);
		getWorld()->getPlayer()->grabSonar();
		getWorld()->increaseScore(75);
	}
	else
		time++;
}

WaterPool::WaterPool(StudentWorld* World, int x, int y) :Actor(World, TID_WATER_POOL, x, y, right, 1.0, 2)
{
	setVisible(true);
	time = 0;
	int t = 300 - 10 * getWorld()->getLevel();
	maxTime = std::max(100, t);
}

WaterPool::~WaterPool()
{
	setVisible(false);
}

void WaterPool::doSomething()
{
	if (isdead())
		return;
	if (time == maxTime)//temp state over
		setdead();
	else if (radius_dist(this, getWorld()->getPlayer()) <= 3.0)//player collects
	{
		setdead();
		getWorld()->playSound(SOUND_GOT_GOODIE);
		getWorld()->getPlayer()->grabWater();
		getWorld()->increaseScore(100);
	}
	else
		time++;
}

RegProtest::RegProtest(StudentWorld* World, int imageID) :Actor(World, imageID, 60, 60, left, 1.0, 0)
{
	restTime = 0;
	sinceShout = 100;
	sincePerp = 500;
	numMove = rand() % 53 + 8;
	int t = 3 - getWorld()->getLevel() / 4;
	TimetoRest = std::max(0, t);
	setVisible(true);
	hitPoints = 5;
	for (int x = 0; x < 61; x++)
	{
		for (int y = 0; y < 61; y++)
			maze[y][x] = -1;//undiscovered
	}
}

RegProtest::~RegProtest()
{
	setVisible(false);
}

void RegProtest::doSomething()
{
	;
	int next_x = getX();
	int next_y = getY();
	Direction dir = getDirection();
	if (restTime != TimetoRest)
		restTime++;
	else
	{
		int t = 3 - getWorld()->getLevel() / 4;
		TimetoRest = std::max(0, t);
		restTime = 0;
		sinceShout++;
		sincePerp++;

		if (isdead())
			return;
		else if (annoyed == true)//leave field state
		{
			if (getX() == 60 && getY() == 60)
			{
				setdead();
				getWorld()->deadProtestor();//decrement number of protestor on field
			}
			else
				followPath();//leave oil field
			return;
		}

		if (radius_dist(this, getWorld()->getPlayer()) <= 4.0 && faceingPlayerDir())
		{
			if (sinceShout > 15)
			{
				getWorld()->playSound(SOUND_PROTESTER_YELL);
				getWorld()->getPlayer()->annoy(2);
				sinceShout = 0;
				return;
			}
		}

		if ((ishorizontal(dir, next_x) || isvertical(dir, next_y)))//adjusts dir or coordinate if return true
		{
			setDirection(dir);
			moveTo(next_x, next_y);
			numMove = 0;//force to pick new dir
			return;
		}

		if (numMove >= 0)
		{
			numMove--;
			if (numMove <= 0)
				findNewMove(dir);
		}

		if (sincePerp > 200 && isperp(dir))
		{
			sincePerp = 0;
			setDirection(dir);
			numMove = rand() % 53 + 8;
		}

		switch (getDirection())
		{
		case up:
			next_y++;
			break;
		case down:
			next_y--;
			break;
		case left:
			next_x--;
			break;
		case right:
			next_x++;
			break;
		}

		if (getWorld()->isclear(next_x, next_y) && next_y <= 60 && next_y >= 0 && next_x <= 60 && next_x >= 0)
		{
			moveTo(next_x, next_y);
		}
		else
			numMove = 0;
	}
}

bool RegProtest::ishorizontal(Direction& next_dir, int& next_x)
{
	if (radius_dist(this, getWorld()->getPlayer()) > 4.0)
	{
		int PlayerY = getWorld()->getPlayer()->getY();
		if (PlayerY != getY())
			return false;
		int PlayerX = getWorld()->getPlayer()->getX();
		int start = PlayerX;
		int end = getX();
		Direction dir = left;
		int next = getX() - 1;
		if (PlayerX > getX())//player is to the right
		{
			start = getX();
			end = PlayerX;
			dir = right;
			next = getX() + 1;
		}
		while (start < end)//checks for blockage
		{
			if (getWorld()->isclear(start, getY()))
				start++;
			else
				return false;
		}
		next_dir = dir;
		next_x = next;
		return true;
	}
	return false;

}

bool RegProtest::isvertical(Direction& next_dir, int& next_y)
{
	if (radius_dist(this, getWorld()->getPlayer()) > 4.0)
	{
		int PlayerY = getWorld()->getPlayer()->getY();
		int PlayerX = getWorld()->getPlayer()->getX();
		if (PlayerX != getX())
			return false;
		int start = PlayerY;
		int end = getY();
		Direction dir = down;
		int next = getY() - 1;
		if (PlayerY > getY())//player is up
		{
			start = getY();
			end = PlayerY;
			dir = up;
			next = getY() + 1;
		}
		while (start < end)//checks for blockage
		{
			if (getWorld()->isclear(getX(), start))
				start++;
			else
				return false;
		}
		next_dir = dir;
		next_y = next;
		return true;
	}
	return false;
}

bool RegProtest::isperp(Direction& next_dir)
{
	switch (getDirection())
	{
	case up:
	case down://if currently going up or down
		if (getWorld()->isclear(getX() + 1, getY()) && getWorld()->isclear(getX() - 1, getY()))//both left and right are open
		{
			int dirnum = rand() % 2;//randomly select right or left
			if (dirnum == 0)
				next_dir = left;
			else
				next_dir = right;
		}
		else if (getWorld()->isclear(getX() + 1, getY()))//right it open
			next_dir = right;
		else if (getWorld()->isclear(getX() - 1, getX()))//left is open
			next_dir = left;
		else
			return false;
		return true;
	case left:
	case right:
		if (getWorld()->isclear(getX(), getY() + 1) && getWorld()->isclear(getX(), getY() - 1) && getY() != 60)//both up and down is open
		{
			int dirnum = rand() % 2;//radomly select right or left
			if (dirnum == 0)
				next_dir = up;
			else
				next_dir = down;
		}
		else if (getWorld()->isclear(getX(), getY() + 1) && getY() != 60)//up is open
			next_dir = up;
		else if (getWorld()->isclear(getX(), getY() - 1))//down is open
			next_dir = down;
		else
			return false;
		return true;
	}
	return true;
}

bool RegProtest::faceingPlayerDir()
{
	switch (getDirection())
	{
	case up:
		if (getY() < getWorld()->getPlayer()->getY())
			return true;
	case down:
		if (getY() > getWorld()->getPlayer()->getY())
			return true;
	case left:
		if (getX() > getWorld()->getPlayer()->getX())
			return true;
	case right:
		if (getX() < getWorld()->getPlayer()->getX())
			return true;
	}
	return false;
}

void RegProtest::findNewMove(Direction& next_dir)
{
	for (;;)
	{
		int nX = getX();
		int nY = getY();
		int randdir = rand() % 4;
		switch (randdir)
		{
		case 0://up
			nY++;
			next_dir = up;
			break;
		case 1://down
			nY--;
			next_dir = down;
			break;
		case 2://left
			nX--;
			next_dir = left;
			break;
		case 3://right
			nX++;
			next_dir = right;
			break;
		}
		if (getWorld()->isclear(nX, nY) && nY <= 60)//cant move above y = 60
		{
			setDirection(next_dir);
			numMove = rand() % 53 + 8;
			break;
		}
	}
}

void RegProtest::annoy(int points)
{
	if (annoyed == false)
	{
		hitPoints -= points;
		if (getHealth() <= 0)
		{
			annoyed = true;
			getWorld()->playSound(SOUND_PROTESTER_GIVE_UP);
			restTime = 0;
			createMaze(60, 60);
			int t = 3 - getWorld()->getLevel() / 4;
			TimetoRest = std::max(0, t);
		}
		else
		{
			getWorld()->playSound(SOUND_PROTESTER_ANNOYED);
			int n = 100 - getWorld()->getLevel() * 10;
			TimetoRest = std::max(50, n);
		}
	}
}

void RegProtest::followPath()
{
	if (!path.empty())
	{
		std::pair<int, int> coord = path[path.size() - 1];
		int next_y = coord.first;
		int next_x = coord.second;
		path.pop_back();

		if (next_y < getY())
			setDirection(down);
		else if (next_y > getY())
			setDirection(up);
		else if (next_x < getX())
			setDirection(left);
		else if (next_x > getX())
			setDirection(right);

		moveTo(next_x, next_y);
	}
}

bool RegProtest::createMaze(int exit_x, int exit_y)
{
	std::queue<std::pair<int, int>> search;
	int x = getX();
	int y = getY();
	search.push({ y, x });
	maze[y][x] = 0;//first step

	if (search.size() <= 0)
		return false;

	while (!search.empty())
	{
		int count = 0;
		std::pair<int, int> coord = search.front();
		y = coord.first;
		x = coord.second;
		search.pop();

		if (x == exit_x && y == exit_y)
		{
			findPath(exit_x, exit_y);
			return true;
		}

		if (x != 0 && getWorld()->isclear(x - 1, y) && maze[y][x - 1] == -1)//left
		{
			maze[y][x - 1] = maze[y][x] + 1;
			search.push({ y, x - 1 });
		}
		if (x != 60 && getWorld()->isclear(x + 1, y) && maze[y][x + 1] == -1)//right
		{
			maze[y][x + 1] = maze[y][x] + 1;
			search.push({ y, x + 1 });
		}
		if (y != 0 && getWorld()->isclear(x, y - 1) && maze[y - 1][x] == -1)//down
		{
			maze[y - 1][x] = maze[y][x] + 1;
			search.push({ y - 1, x });
		}
		if (y != 60 && getWorld()->isclear(x, y + 1) && maze[y + 1][x] == -1)//up
		{
			maze[y + 1][x] = maze[y][x] + 1;
			search.push({ y + 1, x });
		}

	}
	return false;
}

void RegProtest::findPath(int exit_x, int exit_y)
{
	path.clear();
	int x = exit_x;
	int y = exit_y;
	int step = maze[y][x];
	path.push_back({ y, x });

	while (step >= 0)
	{
		std::pair<int, int> coord = path[path.size() - 1];
		int y = coord.first;
		int x = coord.second;
		step = maze[y][x];
		if (step == 1)//protestor's next step
			return;

		if (x != 0 && maze[y][x - 1] == step - 1)//right
		{
			path.push_back({ y, x - 1 });
		}
		else if (x != 60 && maze[y][x + 1] == step - 1)//left
		{
			path.push_back({ y, x + 1 });
		}
		else if (y != 0 && maze[y - 1][x] == step - 1)//up
		{
			path.push_back({ y - 1, x });
		}
		else if (y != 60 && maze[y + 1][x] == step - 1)//down
		{
			path.push_back({ y + 1, x });
		}
	}
}

void RegProtest::resetMaze()
{
	for (int x = 0; x < 61; x++)
	{
		for (int y = 0; y < 61; y++)
			maze[y][x] = -1;//undiscovered
	}
}

void RegProtest::bribe()
{
	annoy(1000);
}

HardProtest::HardProtest(StudentWorld* World, int imageID) :RegProtest(World, imageID)
{
	hitPoints = 20;
	M = 16 + getWorld()->getLevel() * 2;
}

HardProtest::~HardProtest()
{
	setVisible(false);
}

void HardProtest::doSomething()
{
	int next_x = getX();
	int next_y = getY();
	Direction dir = getDirection();
	if (restTime != TimetoRest)
		restTime++;
	else
	{
		int t = 3 - getWorld()->getLevel() / 4;
		TimetoRest = std::max(0, t);
		restTime = 0;
		sinceShout++;
		sincePerp++;

		if (isdead())
			return;
		else if (annoyed == true)//leave maze state
		{
			if (getX() == 60 && getY() == 60)
			{
				setdead();
				getWorld()->deadProtestor();
			}
			else
				followPath();
			return;
		}

		if (radius_dist(this, getWorld()->getPlayer()) < 4.0 && faceingPlayerDir())//shout
		{
			if (sinceShout > 15)
			{
				getWorld()->playSound(SOUND_PROTESTER_YELL);
				getWorld()->getPlayer()->annoy(2);
				sinceShout = 0;
				return;
			}
		}

		if (radius_dist(this, getWorld()->getPlayer()) > 4.0)//path to player
		{
			resetMaze();
			createMaze(getWorld()->getPlayer()->getX(), getWorld()->getPlayer()->getY());
			if (maze[getWorld()->getPlayer()->getY()][getWorld()->getPlayer()->getX()] <= M)
			{
				followPath();
				return;
			}
		}

		if (ishorizontal(dir, next_x) || isvertical(dir, next_y))//only changes value if returns true
		{
			setDirection(dir);
			moveTo(next_x, next_y);
			numMove = 0;//force to pick new dir
			return;
		}

		if (numMove >= 0)
		{
			numMove--;
			if (numMove <= 0)
				findNewMove(dir);
		}

		if (sincePerp > 200 && isperp(dir))//move perpendicular
		{
			sincePerp = 0;
			setDirection(dir);
			numMove = rand() % 53 + 8;
		}

		switch (getDirection())
		{
		case up:
			next_y++;
			break;
		case down:
			next_y--;
			break;
		case left:
			next_x--;
			break;
		case right:
			next_x++;
			break;
		}

		if (getWorld()->isclear(next_x, next_y) && next_y <= 60 && next_y >= 0 && next_x <= 60 && next_x >= 0)
			moveTo(next_x, next_y);
		else
			numMove = 0;
	}
}

void HardProtest::annoy(int points)
{
	if (annoyed == false)
	{
		hitPoints -= points;
		if (hitPoints <= 0)
		{
			annoyed = true;
			getWorld()->playSound(SOUND_PROTESTER_GIVE_UP);
			restTime = 0;
			resetMaze();
			createMaze(60, 60);
			int t = 3 - getWorld()->getLevel() / 4;
			TimetoRest = std::max(0, t);
		}
		else
		{
			getWorld()->playSound(SOUND_PROTESTER_ANNOYED);
			int n = 100 - getWorld()->getLevel() * 10;
			TimetoRest = std::max(50, n);
		}
	}
}

void HardProtest::bribe()
{
	int stare = 100 - getWorld()->getLevel() * 10;
	TimetoRest = std::max(50, stare);
}