#include "controller/BatteryController.h"
#include "component/Motor.h"

#include <net/NetSender.h>
#include <utils/Constants.h>
#include <utils/Logger.h>

using namespace component;

namespace controller {

BatteryController::BatteryController(NetSender& sender) : m_sender(sender) {
    m_name = "BatteryMonitor";
    m_delay = BATTERY_LAPS;
}

int BatteryController::begin() {
    int ret = 1;
#ifdef __arm__
    ret = m_pca.open_adc_ads7830();
#endif
    return ret;
}

int BatteryController::end() {
    int ret = ComponentController::end();
#ifdef __arm__
    m_pca.close();
#endif
    return ret;
}

void BatteryController::handleActions() {
    float voltage = 1.0f;
    #ifdef __arm__
        voltage = m_pca.get_adc_ads7830(BatteryChannel::BAT_TOTAL);
    #endif
    m_components[0] = (int)((voltage * 100 * 3) / BATTERY_VOLT_MAX);
    sendLifePercentage();
}

void BatteryController::sendLifePercentage() {
    m_sender.sendFrame(NS_ID_BATTERY_LIFE, NS_FRAME_TYPE_DATA, "1", m_components[0]);
}

} // controller