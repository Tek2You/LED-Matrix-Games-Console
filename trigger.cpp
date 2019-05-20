#include "trigger.h"

Trigger::Trigger()
{

}

bool Trigger::process()
{
	unsigned long t = millis();
	return process(t);
}
