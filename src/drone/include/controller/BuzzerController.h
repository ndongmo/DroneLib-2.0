/*!
* \file BuzzerController.h
* \brief Buzzer generic controller class.
* \author Ndongmo Silatsa
* \date 28-08-2023
* \version 2.0
*/

#pragma once

#include "component/Buzzer.h"
#include "ComponentController.h"

#include <utils/Structs.h>

using namespace component;

namespace controller
{

/**
 * Buzzer controller class in charge of Buzzers manipulation.
 */
class BuzzerController : public ComponentController<BuzzerAction, BUZZER_COUNT>
{
public:    
    BuzzerController();
    
    int begin() override;
    int end() override;

    /**
     * Turn the Buzzer on/off.
     * @param state on/off
     */
    void turn(bool state);

    /**
     * Play Buzzers animation regarding the given state.
     * @param state app curent state
     */
    void play(utils::AppState state);

protected:
    void handleActions() override;

    /**
     * Update the Buzzer on/off with the given Buzzer action.
     * @param a Buzzer action
     */
    void updateBuzzer(const BuzzerAction& a);
};
} // controller