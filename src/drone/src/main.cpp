/*!
* \file main.cpp
* \brief main file.
* \author Ndongmo Silatsa Fabrice
* \date 08-03-2019
* \version 2.0
*/

#include <DroneController.h>
#include <utils/Logger.h>

int main(int argc, char *argv[])
{
	DroneController ctrl;
	
	if(ctrl.begin() != -1) {
		ctrl.start();
		ctrl.end();
	} else {
		logE << "The Drone process cannot be started!" << std::endl;
	}

	return 0;
}