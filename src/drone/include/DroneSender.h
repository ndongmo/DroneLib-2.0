/*!
* \file DroneSender.h
* \brief Drone frame sender class.
* \author Ndongmo Silatsa Fabrice
* \date 08-03-2019
* \version 2.0
*/

#pragma once

#include <net/NetSender.h>

using namespace net;

/**
 * Drone frame sender service.
 */
class DroneSender : public NetSender
{
public:
	int begin() override;
};