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

CMD:testdoorlock(playerid, params[])
{
    Vehicle_SetDoorsLockState(GetPlayerVehicleID(playerid), E_DOOR_STATE_ON);
    return 1;
}

CMD:testdoors(playerid, params[]) 
{
    Vehicle_SetDoorState(GetPlayerVehicleID(playerid), 4, 4, 1, 1);
    
    return 1;

}

CMD:doorinfo(playerid, params[])
{
    new doors, try[3];
    new doorinfo[4];
    GetVehicleDamageStatus(GetPlayerVehicleID(playerid), try[0], doors, try[1], try[2]);
    decode_doors(doors, doorinfo[0], doorinfo[1], doorinfo[2], doorinfo[3]);
    printf("boot %d, bonnet %d, driver %d, psngr %d", doorinfo[0], doorinfo[1], doorinfo[2], doorinfo[3]);
    return 1;
}

decode_doors(doors, &bonnet, &boot, &driver_door, &passenger_door)
{
    bonnet = doors & 7;
    boot = doors >> 8 & 7;
    driver_door = doors >> 16 & 7;
    passenger_door = doors >> 24 & 7;
}
