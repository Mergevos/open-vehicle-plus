#include <a_samp>
#include <YSI_Visual\y_commands>
#include "vehicle_framework.inc"
main() {}

public OnGameModeInit()
{
    SetWorldTime(0);
    new e = AddStaticVehicle(503, 0, 0, 7, 0, -1, -1);
    printf("%d", e);
    printf("Vehicle alarm %d", Vehicle_IsManual(1));
    ManualVehicleEngineAndLights();
    return 1;
}

CMD:testengon(playerid, params[])
{
    printf("Vehicle alarm %d", Vehicle_Params[0][PARAMS_ALARM]);
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
    new e_LIGHT_STATES: lights[3];
    Vehicle_SetLightsState(GetPlayerVehicleID(playerid),  e_LIGHT_STATES: 0,  e_LIGHT_STATES: 1,  e_LIGHT_STATES: 1);
    Vehicle_SetLightsRunState(GetPlayerVehicleID(playerid), E_LIGHTS_ON);

    Vehicle_GetLightsState(GetPlayerVehicleID(playerid), lights[0], lights[1], lights[2]);
    printf("Front left: %d\nFront Right: %d\nBack: %d", lights[0], lights[1], lights[2]);
    return 1;
}

CMD:testalarmson(playerid, params[]) 
{
    Vehicle_SetAlarms(GetPlayerVehicleID(playerid), E_ALARMS_ON);
    return 1;
}


CMD:testalarmsoff(playerid, params[]) 
{
    Vehicle_SetAlarms(GetPlayerVehicleID(playerid), E_ALARMS_OFF);
    return 1;
}

CMD:testtires(playerid, params[])
{
    Vehicle_SetTireState(GetPlayerVehicleID(playerid), E_TIRE_POPPED, E_TIRE_POPPED, E_TIRE_INFLATED, E_TIRE_INFLATED);
    new e_TIRE_STATUS: tires[4];
    Vehicle_GetTireState(GetPlayerVehicleID(playerid), tires[0], tires[1], tires[2], tires[3]);
    printf("Back rght: %d, Front rght %d, back left %d, front left %d", tires[0], tires[1], tires[2], tires[3]);
    return 1;
}