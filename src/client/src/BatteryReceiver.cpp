#include "BatteryReceiver.h"

void BatteryReceiver::newBatteryLife(int life)
{
    m_life = life;
}

int BatteryReceiver::getBetteryLife()
{
    return m_life;
}