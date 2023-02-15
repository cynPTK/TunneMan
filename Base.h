#ifndef BASE_H
#define BASE_H
#include "GraphObject.h"

class gBase: public GraphObject {
public:
	gBase();
	~gBase();
	//setVisble
	virtual void doSomething();
private:
};

#endif //!BASE_H