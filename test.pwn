#include <a_samp>
#include <YSI_Visual\y_commands>
#include "vehicle_framework.inc"
main() {}

public OnGameModeInit()
{
    SetWorldTime(0);
    AddStaticVehicle(403, 0, 0, 10, 0, -1, -1);
    ManualVehicleEngineAndLights();
    return 1;
}

CMD:testengon(playerid, params[])
{
    Vehicle_SetEngineState(GetPlayerVehicleID(playerid), E_ENGINE_STATE_ON);
    return 1;
}


CMD:testengoff(playerid, params[])
{
    Vehicle_SetEngineState(GetPlayerVehicleID(playerid), E_ENGINE_STATE_OFF);
    return 1;
}

CMD:testl1on(playerid, params[])
{
    Vehicle_SetLightsState(GetPlayerVehicleID(playerid),  E_LIGHT_STATES: 0,  E_LIGHT_STATES: 1,  E_LIGHT_STATES: 1);
    Vehicle_SetLightsRunState(GetPlayerVehicleID(playerid), E_LIGHTS_ON);
    return 1;
}