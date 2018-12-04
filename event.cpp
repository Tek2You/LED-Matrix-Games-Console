#include "event.h"

Event::Event() : event_(0)
{

}

void Event::setbuttonUpState(bool state)
{
	if(state != buttonUpState()){
		bitSet(event_,6);
	}
	bitWrite(event_,2,state);
}

void Event::setbuttonDownState(bool state)
{
	if(state != buttonDownState()){
		bitSet(event_,5);
	}
	bitWrite(event_,1,state);
}

void Event::setbuttonRightState(bool state)
{
	if(state != buttonRightState()){
		bitSet(event_,7);
	}
	bitWrite(event_,3,state);
}

void Event::setbuttonLeftState(bool state)
{
	if(state != buttonLeftState()){
		bitSet(event_,4);
	}
	bitWrite(event_,0,state);
}

void Event::clear()
{
	event_ &= ~(event_ & CHANGE);
	event_ &= ~ON_ENTRY;
}
