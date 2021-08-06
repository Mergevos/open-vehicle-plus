#include <a_samp>
#include <YSI_Visual\y_commands>
#include "vehicle_framework.inc"
#include <YSI_Extra\y_inline_timers>
#include <streamer>
main() {}
new tmpobjid1;
new tmpobjid2;
new tmpobjid3;
new tmpobjid4;
new tmpobjid5;
new tmpobjid6;
new tmpobjid7;
new tmpobjid;
static e;

public OnGameModeInit()
{
    SetWorldTime(0);
    e = AddStaticVehicle(411, 0, 0, 7, 0, -1, -1);

    printf("%d", e);
    printf("Is manual %d", Vehicle_IsManual(e));
    ManualVehicleEngineAndLights();
    return 1;
}

CMD:testengon(playerid, params[])
{
    //printf("Vehicle alarm %d", Vehicle_Params[0][PARAMS_ALARM]);
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
    Vehicle_SetLightsState(GetPlayerVehicleID(playerid),  E_LIGHT_ON, E_LIGHT_OFF, E_LIGHT_ON);
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

CMD:testvel(playerid, param[])
{
    SetVehicleVelocity(e, 34, 35, 200);
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

CMD:gettires(playerid, params[])
{
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

CMD:testblinks(playerid, params[]) 
{
 //   Vehicle_SetDoorState(GetPlayerVehicleID(playerid), 4, 4, 1, 1);

    print("ej0");
    AttachDynamicObjectToVehicle(tmpobjid1, e, 0.990, 2.520, -0.020, 0.000, 0.000, 0.000);
    AttachDynamicObjectToVehicle(tmpobjid2, e, -0.999, 2.521, 0.000, 0.000, 0.000, 0.000);
    AttachDynamicObjectToVehicle(tmpobjid3, e, 1.080, 1.050, 0.000, 0.000, 0.000, 0.000);
    AttachDynamicObjectToVehicle(tmpobjid4, e, -0.960, -2.331, 0.000, 0.000, 0.000, 0.000);
    AttachDynamicObjectToVehicle(tmpobjid5, e, 0.960, -2.300, 0.000, 0.000, 0.000, 0.000);
    AttachDynamicObjectToVehicle(tmpobjid6, e, -1.081, 1.040, 0.000, 0.000, 0.000, 0.000);

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


CMD:testwin(playerid, params[])
{
    Vehicle_SetWindows(GetPlayerVehicleID(playerid), E_WINDOW_OPENED, E_WINDOW_OPENED, E_WINDOW_CLOSED, E_WINDOW_OPENED);
    return 1;
}

CMD:wintest(playerid, params[])
{
    new e_WINDOWS_STATES: win[4];
    Vehicle_GetWindows(GetPlayerVehicleID(playerid), win[0], win[1], win[2], win[3]);
    new str[129];
    format(str, sizeof(str), "driver %d, psngr: %d, rl %d rr %d", win[0], win[1], win[2], win[3]);
    SendClientMessage(playerid, -1, str);
    return 1;
}


CMD:testplate(playerid, params[]) 
{
    Vehicle_SetNumberPlate(GetPlayerVehicleID(playerid), "TEST123");
    return 1;
}

CMD:platetest(playerid, params[])
{
    new pl[32];
    Vehicle_GetNumberPlate(GetPlayerVehicleID(playerid), pl);
    printf("%s", pl);
    return 1;
}

CMD:paneltest(playerid, params[])
{

    Vehicle_SetPanelStates(GetPlayerVehicleID(playerid), E_PANEL_CRUSHED, E_PANEL_HANGING_LOOSE, E_PANEL_UNDAMAGED, E_PANEL_UNDAMAGED);
    return 1;
}

CMD:getpaneltest(playerid, params[])
{
    new e_PANEL_STATES: panels[4];
    Vehicle_GetPanelStates(GetPlayerVehicleID(playerid), panels[0], panels[1], panels[2], panels[3]);
    new str[128];
    format(str, sizeof(str), "FL: %d, FR: %d, BL: %d, BR: %d", panels[0], panels[1], panels[2], panels[3]);
    SendClientMessageToAll(-1, str);
    return 1;
}

CMD:bumpertest(playerid, params[])
{

    Vehicle_SetBumperStates(GetPlayerVehicleID(playerid), E_PANEL_CRUSHED, E_PANEL_CRUSHED);
    Vehicle_SetBumperStates(GetPlayerVehicleID(playerid), E_PANEL_CRUSHED, E_PANEL_HANGING_LOOSE);
    return 1;
}

CMD:getbumpertest(playerid, params[])
{
    new e_PANEL_STATES: bumper[2];
    Vehicle_GetBumperStates(GetPlayerVehicleID(playerid), bumper[0], bumper[1]);
    new str[128];
    format(str, sizeof(str), "F: %d, R: %d", bumper[0], bumper[1]);
    SendClientMessageToAll(-1, str);
    return 1;
}

CMD:getwindshieldtest(playerid, params[])
{
    new e_PANEL_STATES: winds;
    Vehicle_GetWindshieldState(GetPlayerVehicleID(playerid), winds);
    new str[128];
    format(str, sizeof(str), "W: %d", winds);
    SendClientMessageToAll(-1, str);
    return 1;
}

CMD:weapon(playerid, params[])
{
    if(isnull(params)) {
        return 0;
    }
    GivePlayerWeapon(playerid, strval(params), 600);
    return 1;
}