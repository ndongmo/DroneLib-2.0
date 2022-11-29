/*!
* \file main.cpp
* \brief main file.
* \author Ndongmo Silatsa Fabrice
* \date 08-03-2019
* \version 2.0
*/

#include <PCController.h>
#include <utils/Logger.h>

int main(int argc, char *argv[])
{
	PCController ctrl;
	
	if(ctrl.begin() != -1) {
		ctrl.start();
	} else {
		logE << "The application cannot be started!" << std::endl;
	}

	return 0;
}