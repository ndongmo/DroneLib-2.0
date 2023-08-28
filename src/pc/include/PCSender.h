/*!
* \file PCSender.h
* \brief PC frame sender class.
* \author Ndongmo Silatsa Fabrice
* \date 08-03-2019
* \version 2.0
*/

#pragma once

#include <net/NetSender.h>
#include <utils/Structs.h>

using namespace net;
using namespace utils;

/**
 * PC frame sender service.
 */
class PCSender : public NetSender
{
public:
    /**
     * Construct a new PCSender object
     */
    PCSender();

	int begin() override;

    /**
	 * Sends navigation data.
     * 
     * @param deltatime elapsed time since the last call
     * @param dir drone direction
     * @param speed drone speed 
	 */
	void sendNav(int deltatime, DroneDir dir, DroneSpeed speed);

    /**
	 * Sends camera rotation angle.
     * 
     * @param deltatime elapsed time since the last call
     * @param axe camera axe
     * @param angle desired camera rotation angle
	 */
	void sendCamera(int deltatime, DroneCamera axe, int angle);

    /**
	 * Sends buzz request.
     * 
     * @param deltatime elapsed time since the last call
	 */
	void sendBuzz(int deltatime);
};