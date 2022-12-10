/*!
* \file main.cpp
* \brief main file.
* \author Ndongmo Silatsa Fabrice
* \date 08-03-2019
* \version 2.0
*/

#include <PCController.h>
#include <utils/Logger.h>
#include <utils/Config.h>
#include <utils/Constants.h>

#include <fstream>
#include <signal.h>
#include <unistd.h>

/** The PC controller */
static PCController ctrl;

/**
 * @brief Handler method for ctrl_c event.
 * 
 * @param signum 
 */
static void ctrl_c_handler(int signum)
{
    ctrl.stop();
}

/**
 * @brief Set the up handlers object
 */
static void setup_handlers(void)
{
    struct sigaction sa = {};
    sa.sa_handler = ctrl_c_handler;

    sigaction(SIGINT, &sa, NULL);
    sigaction(SIGTERM, &sa, NULL);
}

int main(int argc, char *argv[])
{
	setup_handlers();

	if (!Config::exists()) {
		std::ofstream configFile(CONFIG_FILE);
        configFile << "{\n\t\"" << DRONE_ADDRESS << "\":\"" << DRONE_IPV4_ADDRESS_DEFAULT << "\"\n}";
        configFile.close();
	}

	if(ctrl.begin() != -1) {
		ctrl.start();
		ctrl.end();
	} else {
		logE << "The application cannot be started!" << std::endl;
	}

	return 0;
}