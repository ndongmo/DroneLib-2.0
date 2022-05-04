#include "Controller.h"

#include <utils/Logger.h>
#include <utils/Config.h>
#include <utils/Constants.h>

using namespace utils;

int Controller::begin()
{	
    int err = 1;


	return err;
}

int Controller::end()
{
	int err = 1;

	return err;
}

void Controller::start()
{
	run();
}

void Controller::run()
{
	m_running = true;

	while (m_running) {
		
		
	}
	end();
}