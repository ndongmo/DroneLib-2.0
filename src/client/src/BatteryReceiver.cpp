#include "BatteryReceiver.h"

void BatteryReceiver::newBatteryLife(int life)
{
    m_life = life;
}

int BatteryReceiver::getBatteryLife()
{
    return m_life;
}