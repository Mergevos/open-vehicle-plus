#define WEAK_TAGS
#include <open.mp>
#define CGEN_MEMORY 20000

#include <YSI_Visual\y_commands>
#include <sscanf2>
#include "vehicle_plus.inc"

main() 
{
}

public OnGameModeInit()
{
    SetWorldTime(0);

    ManualVehicleEngineAndLights();
    return 1;
}

public OnVehicleRespray(playerid, vehicleid, colour1, colour2)
{
    SendClientMessage(playerid, -1, "Vehicle resprayed triggered clb %d %d", colour1, colour2);
    return 1;
}

public OnTrailerHook(vehicleid, trailerid)
{
    SendClientMessageToAll(-1, "Trailerid %d attached to vehicle %d", trailerid, vehicleid);
    return 1;
}

public OnTrailerUnhook(vehicleid, trailerid)
{
    SendClientMessageToAll(-1, "Trailerid %d detached from vehicle %d", trailerid, vehicleid);
    return 1;
}

CMD:testtrailerattach(playerid, params[])
{
    new vehid;
    if(sscanf(params, "d", vehid)) 
    {
        return SendClientMessage(playerid, -1, "testtrailerattach [vehid]");
    }
    Vehicle_AttachTrailer(GetPlayerVehicleID(playerid), vehid);
    SendClientMessage(playerid, -1, "Trailer %d has been attached to vehicle %d", Vehicle_GetTrailer(GetPlayerVehicleID(playerid)), Vehicle_GetTrailerCab(vehid));

    return 1;
}

CMD:testtrailerdettach(playerid, params[])
{
    Vehicle_DetachTrailer(GetPlayerVehicleID(playerid));
   
    return 1;
}

CMD:testrespawntime(playerid, params[])
{
    new 
        bool: occupied = Vehicle_IsOccupied(GetPlayerVehicleID(playerid)),
        respawn_time = Vehicle_GetRespawnedTime(GetPlayerVehicleID(playerid)),
        occupied_time = Vehicle_GetOccupiedTime(GetPlayerVehicleID(playerid));

    SendClientMessage(playerid, -1, "Vehicle %d occupied %d respawned time %d occupied time %d", GetPlayerVehicleID(playerid), occupied, respawn_time, occupied_time);

    return 1;
}

CMD:bd(playerid, params[])
{
    Vehicle_CancelBlinking(GetPlayerVehicleID(playerid));
    return 1;
}

CMD:be(playerid, params[])
{
    Vehicle_SetBlinking(GetPlayerVehicleID(playerid), VEHICLE_BLINKERS_EMERGENCY);
    return 1;
}

CMD:bl(playerid, params[])
{
    Vehicle_SetBlinking(GetPlayerVehicleID(playerid), VEHICLE_BLINKERS_LEFT);
    return 1;
}

CMD:br(playerid, params[])
{
    Vehicle_SetBlinking(GetPlayerVehicleID(playerid), VEHICLE_BLINKERS_RIGHT);
    return 1;
}


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

CMD:testpaintjob(playerid, params[])
{
    new pntjob;
    if(sscanf(params, "d", pntjob)) 
    {
        return SendClientMessage(playerid, -1, "testpaintjob [paintjob]");
    }
    Vehicle_SetPaintjob(GetPlayerVehicleID(playerid), pntjob);
    SendClientMessage(playerid, -1, "Changed paintjob to %d", Vehicle_GetPaintjob(GetPlayerVehicleID(playerid)));
    return 1;
}

CMD:testcolors(playerid, params[])
{
    new cl1,cl2;
    if(sscanf(params, "dd", cl1,cl2)) 
    {
        return SendClientMessage(playerid, -1, "testcolors [cl cl2]");
    }
    Vehicle_SetColour(GetPlayerVehicleID(playerid), cl1, cl2);
    cl1 = -1;
    cl2 = -1;
    Vehicle_GetColour(GetPlayerVehicleID(playerid),cl1,cl2);
    SendClientMessage(playerid, -1, "Your colors are %d and %d", cl1, cl2);
    return 1;
}

public OnVehiclePaintjobChange(vehicleid, paintjobid)
{
    SendClientMessageToAll(-1, "Changed paintjob to %d", paintjobid);
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

CMD:testdestroy(playerid, params[])
{
    Vehicle_Destroy(GetPlayerVehicleID(playerid));
    return 1;
}

public OnVehicleDestroy(vehicleid)
{
    SendClientMessageToAll(-1, "Vehicle %d has been destroyed", vehicleid);
    return 1;
}

CMD:testlightsdamageremovefront(playerid, params[])
{
    Vehicle_SetLightsCondition(GetPlayerVehicleID(playerid), VEHICLE_LIGHT_FRONT_LEFT, VEHICLE_LIGHT_CONDITION_FIXED);
    return 1;
}

CMD:testpanels(playerid, params[])
{
    new front_left, front_right, back_left, back_right, front_bumper, back_bumper, windshield;

    if(sscanf(params, "dddddd", front_left, front_right, back_left, back_right, front_bumper, back_bumper)) 
    {
        return SendClientMessage(playerid, -1, "testpanels [panels [1 2 3 4 5 6]]");
    }
    Vehicle_SetPanelsCondition(GetPlayerVehicleID(playerid), VEHICLE_PANELS_CONDITION: front_left, VEHICLE_PANELS_CONDITION: front_right, VEHICLE_PANELS_CONDITION: back_left, VEHICLE_PANELS_CONDITION: back_right, VEHICLE_PANELS_CONDITION: front_bumper, VEHICLE_PANELS_CONDITION: back_bumper);
    SendClientMessage(playerid, -1, "Vehicle panels updated, getting values..");
    front_left = -1, front_right = -1, back_left = -1, back_right = -1, front_bumper = -1, back_bumper = -1, windshield = -1;
    Vehicle_GetPanelsCondition(GetPlayerVehicleID(playerid), VEHICLE_PANELS_CONDITION: front_left, VEHICLE_PANELS_CONDITION: front_right, VEHICLE_PANELS_CONDITION: back_left, VEHICLE_PANELS_CONDITION: back_right, VEHICLE_PANELS_CONDITION: front_bumper, VEHICLE_PANELS_CONDITION: back_bumper, VEHICLE_PANELS_CONDITION: windshield);
    SendClientMessageToAll(-1, "Front left %d\nFront Right %d\nBack left %d\nBack right %d\nFront bumper %d\nRear bumper\nWindshield", front_left, front_right, back_left, back_right, front_bumper, back_bumper, windshield);
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
    Vehicle_SetDoorCondition(GetPlayerVehicleID(playerid), VEHICLE_DOORS: door, VEHICLE_DOOR_CONDITION: value);

    return 1;
}

CMD:testdoordamageget(playerid, params[])
{
    new VEHICLE_DOOR_CONDITION: hood,
        VEHICLE_DOOR_CONDITION: trunk,
        VEHICLE_DOOR_CONDITION: driver_door,
        VEHICLE_DOOR_CONDITION: passenger_door;

    Vehicle_GetDoorCondition(GetPlayerVehicleID(playerid), hood, trunk, driver_door, passenger_door);
    printf("Hood: %d\nTrunk: %d\nDriver: %d\nPassenger: %d", _:hood, _:trunk, _:driver_door, _:passenger_door);

    return 1;
}

CMD:testtyres(playerid, params[])
{
    new tyre, value;
    if(sscanf(params, "dd", tyre, value))
    {
        return SendClientMessage(playerid, -1, "/testtyres [value]");
    }
    Vehicle_SetTyreCondition(GetPlayerVehicleID(playerid), VEHICLE_TYRE: tyre, VEHICLE_TYRE_CONDITION: value);
    new VEHICLE_TYRE_CONDITION: tire1, VEHICLE_TYRE_CONDITION: tire2, VEHICLE_TYRE_CONDITION: tire3, VEHICLE_TYRE_CONDITION: tire4;
    Vehicle_GetTyreCondition(GetPlayerVehicleID(playerid), tire1, tire2, tire3, tire4);
    SendClientMessage(playerid, -1, "zadnja desna %d, prednja desna %d, zadnja leva %d, prednja leva %d", tire1, tire2, tire3, tire4); 
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



CMD:testalarmson(playerid, params[]) 
{
    Vehicle_SetAlarms(GetPlayerVehicleID(playerid), true);
    return 1;
}

CMD:testalarmsoff(playerid, params[]) 
{
    Vehicle_SetAlarms(GetPlayerVehicleID(playerid), false);
    return 1;
}