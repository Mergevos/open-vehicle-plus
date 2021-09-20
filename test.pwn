#include <a_samp>
#include <fixes>
#define CGEN_MEMORY 20000
#include <YSI_Visual\y_commands>
#include "vehicle_framework.inc"
main() {

}

static e,ccc[3];
new VehicleGroup: vehgrp,
    VehicleGroup: eeee;

public OnGameModeInit()
{
    SetWorldTime(0);
    vehgrp = Vehicle_GroupInit("eee");
    e = Vehicle_CreateEx(403, 0, 0, 0, 10, 1, 10, -1, 0, 0,1000, "TEST");
    ccc[1] = Vehicle_CreateEx(598, 0, 3, 3, 10, -1, -1, -1, 0, 0,1000, "TEST"); 
   // ccc[2] = Vehicle_CreateEx(575, 0, 5, 3, 10, 1, 10, -1, 0, 0,1000, "TEST");
    new Float:joj;
    Vehicle_GetHealth(e, joj);
    printf("%f", joj);
    Vehicle_AddToGroup(vehgrp, e);
    Vehicle_AddToGroup(vehgrp, ccc[1]);
        
    printf("%d valid", Vehicle_IsEmptyGroup(eeee));
   // Vehicle_GroupDestroy(vehgrp);
    printf("%d valid2", Vehicle_IsValidGroup(vehgrp));
    Vehicle_AddToGroup(vehgrp, ccc[0]);
    Vehicle_AddToGroup(vehgrp, ccc[1]);
    Vehicle_AddToGroup(vehgrp, ccc[2]);
    printf("%d has", Vehicle_GroupHas(vehgrp, ccc[1]));
    printf("%fd typ", Vehicle_GetMass(e));
    printf("%d has", Vehicle_GroupHas(vehgrp, ccc[1]));


    ManualVehicleEngineAndLights();
    return 1;
}


CMD:bd(playerid, params[])
{
    GivePlayerWeapon(playerid, 35, 300);
    Vehicle_CancelBlinking(e);
    return 1;
}


CMD:bl(playerid, params[])
{
    Vehicle_SetBlinking(e, E_BLINK_LEFT);
    return 1;
}

CMD:br(playerid, params[])
{
    Vehicle_SetBlinking(vehgrp, E_BLINK_RIGHT);
    return 1;
}

CMD:siren(playerid, params[])
{
    new name[222];
    Vehicle_ReturnName(e, name);
    printf("%s", name);
    return 1;
}

CMD:respawn(playerid, params[])
{
    SetVehicleToRespawn(GetPlayerVehicleID(playerid));
    return 1;
}

CMD:trailerid(playerid, params[])
{
    Vehicle_AttachTrailer(e, ccc[0]);
    printf("%d", Vehicle_GetTrailerCab(ccc[0]));
    return 1;
}

CMD:testslots(playerid, params[])
{
    
}

CMD:testengon(playerid, params[])
{
    //printf("Vehicle alarm %d", Vehicle_Params[0][PARAMS_ALARM]);
    Vehicle_SetEngineState(ccc[1], E_ENGINE_STATE_ON);
    return 1;
}

public OnTrailerHook(vehicleid, trailerid)
{
    SendClientMessageToAll(-1, "A");
    return 1;
}


public OnTrailerUnhook(vehicleid, trailerid)
{
    SendClientMessageToAll(-1, "B");
    return 1;
}

CMD:testengoff(playerid, params[])
{
    Vehicle_SetEngineState(vehgrp, E_ENGINE_STATE_OFF);
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

CMD:testnitro(playerid, params[]) 
{
    Vehicle_AddComponent(GetPlayerVehicleID(playerid), 1010);
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
    printf("IsVehicleOccupied %d", Vehicle_IsOccupied(e));
    return 1;

}

CMD:doorinfo(playerid, params[])
{
    new doors, try[3];
    new doorinfo[4];
    new e_DOOR_STATES: tagdoor[4];
    GetVehicleDamageStatus(GetPlayerVehicleID(playerid), try[0], doors, try[1], try[2]);
    decode_doors(doors, doorinfo[0], doorinfo[1], doorinfo[2], doorinfo[3]);
    Vehicle_GetDoorState(GetPlayerVehicleID(playerid), tagdoor[0], tagdoor[1], tagdoor[2], tagdoor[3]);
    printf("decode: boot %d, bonnet %d, driver %d, psngr %d", doorinfo[0], doorinfo[1], doorinfo[2], doorinfo[3]);
    printf("Framework: boot %d, bonnet %d, driver %d, psngr %d", tagdoor[0], tagdoor[1], tagdoor[2], tagdoor[3]);
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

CMD:putplayer(playerid, params[])
{
    PutPlayerInVehicle(playerid, strval(params), 0);
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

CMD:testdcom(playerid, params[])
{
    Vehicle_SetPaintjob(e, 2);

    printf("%d paint", Vehicle_GetPaintjob(e));
    return 1;
}

CMD:testlast(playerid, params[])
{
    printf("%d", Vehicle_GetLastDriver(1));
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

public OnGroupInitialize(VehicleGroup: groupid)
{
    printf("%d gr", _:groupid);
    return 1;
}

CMD:ahelt(playerid, params[])
{
    SetVehicleHealth(ccc[0], 20);
    return 1;
}

CMD:helt(playerid, params[])
{
    GivePlayerMoney(playerid, 3500);
    GivePlayerWeapon(playerid, 35, 300);
    if(Vehicle_GetCategory(ccc[0]) == CATEGORY_TRAILER)
    {
        new Float:h;
        GetVehicleHealth(ccc[0], h);
        printf("%f", h);
    }
    return 1;
}

public OnVehiclePaintjobChange(vehicleid, paintjobid)
{
    new str[120];
    format(str, sizeof(str), "Change to %d vehicle %d", paintjobid, vehicleid);
    SendClientMessageToAll(-1, str);
    return 1;
}

public OnVehicleRespray(playerid, vehicleid, color1, color2)
{
    SendClientMessageToAll(-1, "NJOJOJJOJO");
    va_SendClientMessageToAll(-1, "Vehicle %d color1 %d color2 %d", vehicleid, color1, color2);
    return 1;
}