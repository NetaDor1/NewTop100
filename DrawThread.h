#include "CommonObject.h"

//Declares the DrawThread class which contains an overloaded operator() function to handle drawing
// the GUI using the shared CommonObjects.
class DrawThread {
public:
	void operator()(CommonObjects& common);

};
