#define WEAK_TAGS
#include <open.mp>
#define CGEN_MEMORY 20000
#include <YSI_Visual\y_commands>
#include <sscanf2>
#include "vehicle_plus.inc"
main() {

}

public OnGameModeInit()
{
    SetWorldTime(0);

    ManualVehicleEngineAndLights();
    printf("MAX CARMODS %d", MAX_CARMODS);
    return 1;
}
/*

CMD:bd(playerid, params[])
{
    GivePlayerWeapon(playerid, WEAPON: 35, 300);
    Vehicle_CancelBlinking(GetPlayerVehicleID(playerid));
    return 1;
}


CMD:bl(playerid, params[])
{
    Vehicle_SetBlinking(GetPlayerVehicleID(playerid), E_BLINK_LEFT);
    return 1;
}

CMD:br(playerid, params[])
{
    Vehicle_SetBlinking(GetPlayerVehicleID(playerid), E_BLINK_RIGHT);
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

*/

CMD:vehicle(playerid, params[])
{
    new vehmodel,
        Float: x,
        Float: y,
        Float: z;
    if(sscanf(params, "d", vehmodel)) 
    {
        return SendClientMessage(playerid, -1, "vehicle [model]");
    }
    GetPlayerPos(playerid, x, y, z);
    Vehicle_Create(vehmodel, x+3, y, z, 0, -1, -1, -1, 0, 0);
    return 1;
}

CMD:vehicleex(playerid, params[])
{
    new vehmodel,
        Float: x,
        Float: y,
        Float: z;
    if(sscanf(params, "d", vehmodel)) 
    {
        return SendClientMessage(playerid, -1, "vehicle [model]");
    }
    GetPlayerPos(playerid, x, y, z);
    Vehicle_CreateEx(vehmodel,  x+3, y, z, 0, -1, -1, -1, 0, 0, 300, "ZUGAA", false);
    return 1;
}

CMD:testallwindows(playerid, params[])
{
    new wins;
    new VEHICLE_WINDOWS: window1, VEHICLE_WINDOWS: window2, VEHICLE_WINDOWS: window3, VEHICLE_WINDOWS: window4;
    if(sscanf(params, "d", wins)) 
    {
        return SendClientMessage(playerid, -1, "testallwindows [status]");
    }
    Vehicle_SetWindowsEx(GetPlayerVehicleID(playerid), VEHICLE_WINDOWS: wins, VEHICLE_WINDOWS: wins, VEHICLE_WINDOWS: wins, VEHICLE_WINDOWS: wins); 
    Vehicle_GetWindowsEx(GetPlayerVehicleID(playerid), VEHICLE_WINDOWS: window1, VEHICLE_WINDOWS: window2, VEHICLE_WINDOWS: window3, VEHICLE_WINDOWS: window4);
    SendClientMessage(playerid, -1, "Driver Window %d, Passenger window %d, left rear %d, right rear: %d", window1, window2, window3, window4);
    return 1;
}

CMD:testwindow(playerid, params[])
{
    new wins, stat;
    if(sscanf(params, "dd", wins, stat)) 
    {
        return SendClientMessage(playerid, -1, "testallwindows [window][state]");
    }
    Vehicle_SetWindows(GetPlayerVehicleID(playerid), VEHICLE_WINDOW: wins, VEHICLE_WINDOWS: stat);
    SendClientMessage(playerid, -1, "You set window %d to %d", wins, stat);
    return 1;
}


CMD:testengon(playerid, params[])
{
    Vehicle_SetEngineState(GetPlayerVehicleID(playerid), true);
    return 1;
}

public OnVehicleHealthChange(vehicleid, Float: oldhealth, Float: newhealth)
{
    printf("Vehicleid%d, oldhealth:%02f newhealth:%02f", vehicleid, oldhealth, newhealth);
    return 1;
}

CMD:testvirtualworld(playerid, params[])
{
    new virtualworld;

    if(sscanf(params, "d", virtualworld)) 
    {
        return SendClientMessage(playerid, -1, "testvirtualworld [world]");
    }
    Vehicle_SetVirtualWorld(GetPlayerVehicleID(playerid), virtualworld);
    SendClientMessage(playerid, -1, "Vehicle vw has been set to %d", Vehicle_GetVirtualWorld(GetPlayerVehicleID(playerid)));
    return 1;
}

CMD:testinterior(playerid, params[])
{
    new interior;

    if(sscanf(params, "d", interior)) 
    {
        return SendClientMessage(playerid, -1, "testinterior [interior]");
    }
    Vehicle_SetInterior(GetPlayerVehicleID(playerid), interior);
    SendClientMessage(playerid, -1, "Vehicle interior has been set to %d", Vehicle_GetInterior(GetPlayerVehicleID(playerid)));
    return 1;
}


CMD:testdimension(playerid, params[])
{
    new virtualworld, interior;

    if(sscanf(params, "d", virtualworld, interior)) 
    {
        return SendClientMessage(playerid, -1, "testdimension [world][interior]");
    }
    Vehicle_SetDimensionInfo(GetPlayerVehicleID(playerid), interior, virtualworld);
    interior = -1;
    virtualworld = -1;
    Vehicle_GetDimensionInfo(GetPlayerVehicleID(playerid), interior, virtualworld);
    SendClientMessage(playerid, -1, "Vehicle dimensions are %d interior and %d world", interior, virtualworld);
    return 1;
}
/*
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
    Vehicle_SetEngineState(vehgrp, false);
    return 1;
}
*/

CMD:getcam(playerid, params[])
{
    GivePlayerMoney(playerid, 3000);
    SendClientMessage(playerid, -1, "Camera mode %d", GetPlayerCameraMode(playerid));
    return 1;
}

CMD:testlightson(playerid, params[])
{
    Vehicle_SetLightsState(GetPlayerVehicleID(playerid),VEHICLE_PARAMS_ON);
    return 1;
}

CMD:testlightsdamageget(playerid, params[])
{
    new 
        VEHICLE_LIGHT_CONDITION: front_left,
        VEHICLE_LIGHT_CONDITION: front_right,
        VEHICLE_LIGHT_CONDITION: back;

    Vehicle_GetLightsCondition(GetPlayerVehicleID(playerid), front_left, front_right, back);
    printf("Front Left %d\nFront Right %d\nRear %d", front_left, front_right, back);
    return 1;
}

CMD:testlightsdamageset(playerid, params[])
{
    Vehicle_SetLightsCondition(GetPlayerVehicleID(playerid), VEHICLE_LIGHT_FRONT_RIGHT, VEHICLE_LIGHT_CONDITION_DAMAGED);
    Vehicle_SetLightsCondition(GetPlayerVehicleID(playerid), VEHICLE_LIGHT_BACK, VEHICLE_LIGHT_CONDITION_DAMAGED);
    return 1;
}

CMD:testlightsdamageremovefront(playerid, params[])
{
    Vehicle_SetLightsCondition(GetPlayerVehicleID(playerid), VEHICLE_LIGHT_FRONT_LEFT, VEHICLE_LIGHT_CONDITION_FIXED);
    return 1;
}

CMD:testlightsdamageremoverear(playerid, params[])
{
    Vehicle_SetLightsCondition(GetPlayerVehicleID(playerid), VEHICLE_LIGHT_BACK, VEHICLE_LIGHT_CONDITION_FIXED);
    return 1;
}

CMD:testdoorlock(playerid, params[])
{
    Vehicle_SetDoorsLocked(GetPlayerVehicleID(playerid), true);
    return 1;
}

CMD:testdoorlockget(playerid, params[])
{
    printf("Doors lock %d", Vehicle_GetDoorsLocked(GetPlayerVehicleID(playerid)));
    return 1;
}

CMD:testdoordamageset(playerid, params[])
{
    new door, value;
    if(sscanf(params, "dd", door, value))
    {
        return SendClientMessage(playerid, -1, "/testdoordamageset [door][condition]");
    }
    Vehicle_SetDoorState(GetPlayerVehicleID(playerid), VEHICLE_DOORS: door, VEHICLE_DOOR_CONDITION: value);

    return 1;
}

CMD:testdoordamageget(playerid, params[])
{
    new VEHICLE_DOOR_CONDITION: hood,
        VEHICLE_DOOR_CONDITION: trunk,
        VEHICLE_DOOR_CONDITION: driver_door,
        VEHICLE_DOOR_CONDITION: passenger_door;

    Vehicle_GetDoorState(GetPlayerVehicleID(playerid), hood, trunk, driver_door, passenger_door);
    printf("Hood: %d\nTrunk: %d\nDriver: %d\nPassenger: %d", _:hood, _:trunk, _:driver_door, _:passenger_door);

    return 1;
}

CMD:testcomponentset(playerid, params[])
{
    new component;
    if(sscanf(params, "d", component))
    {
        return SendClientMessage(playerid, -1, "/testcomponentset [componentid]");
    }
    Vehicle_AddComponent(GetPlayerVehicleID(playerid), component);

    return 1;
}

CMD:testcomponentget(playerid, params[])
{
    new slot;
    if(sscanf(params, "d", slot))
    {
        return SendClientMessage(playerid, -1, "/testcomponentget [slot]");
    }
    new comp = Vehicle_GetComponentInSlot(GetPlayerVehicleID(playerid), CARMODTYPE: slot);
    SendClientMessage(playerid, -1, "Component on slot: %d is %d", slot, comp);
    return 1;
}

/*

CMD:testalarmson(playerid, params[]) 
{
    Vehicle_SetAlarms(GetPlayerVehicleID(playerid), true);
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
    Vehicle_SetAlarms(GetPlayerVehicleID(playerid), false);
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


CMD:testwin(playerid, params[])
{
    Vehicle_SetWindows(GetPlayerVehicleID(playerid), true, true, false, true);
    return 1;
}

CMD:wintest(playerid, params[])
{
    new bool: win[4];
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
    GivePlayerWeapon(playerid, WEAPON: strval(params), 600);
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
    GivePlayerWeapon(playerid, WEAPON: 35, 300);
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
    SendClientMessageToAll(-1, "Vehicle %d color1 %d color2 %d", vehicleid, color1, color2);
    return 1;
}*/
