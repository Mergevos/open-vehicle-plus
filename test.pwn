#include <a_samp>
#define CGEN_MEMORY 20000

#include <YSI_Visual\y_commands>
#include <sscanf2>
//#define VEHICLE_PLUS_SAMP_COMPAT
#include "vehicle_plus.inc"

main() 
{
}

public OnGameModeInit()
{

    SetWorldTime(0);
    
    new veh = CreateVehicle(411, 0, 0, 0, 0, 0, 0, 0);

    ManualVehicleEngineAndLights();
    Vehicle_SetTyrePoppingGlobal(false); //bulletproof

    new VehicleGroup: groupid1;
    new Error: retcode = Vehicle_GroupInit("Test_name", groupid1);
    if(IsError(retcode))
    {
        print("Hej");
    }


    //test win number
    new data;
    new Float: float_data;
    new VEHICLE_TYRE_CONDITION: tire;
    new Error: ret = Vehicle_GetWindowsNumber(3, data);
    if(IsError(ret))
    {
        print("Some error");
        Handled(true);
    }
    else
    {
        printf("%d windows", data);
    }
    Vehicle_GetTyreCondition(veh, VEHICLE_TYRE_FRONT_LEFT, tire);
    printf("%d tyre", tire); //should be -1
    new Error: retc = Vehicle_GetFuelLevel(veh, VEHICLE_UNIT_KILOWATT_HOUR, float_data);
    if(IsError(retc))
    {
        printf("%d", _:retc);
        if(retc == Error: 1)
        {
            print("Invveh");
        }
        else if(retc == Error: 2)
        {
            print("Kilow");
        }
        Handled(true);
    }


    DestroyVehicle(veh);
    // hook test
    return 1;
}

public OnVehicleDestroy(vehicleid)
{
    printf("Vehicle %d has been destroyed", vehicleid);
    return 1;
}

public OnVehicleFirstSpawn(vehicleid)
{
    printf("Vehicle %d just spawned", vehicleid);
    return 1;
}

public OnVehicleRespray(playerid, vehicleid, color1, color2)
{
    SendClientMessage(playerid, -1, "Vehicle resprayed triggered clb %d %d", color1, color2);
    return 1;
}

CMD:testteleport(playerid, params[])
{
    new Float: x, Float: y, Float: z;
    GetPlayerPos(playerid, x, y, z);
    SetVehiclePos(1, x, y+4, z);
    return 1;
}

public OnVehicleDrivenDistanceUpdate(vehicleid)
{
    new Float:travelled_imperial, Float:travelled_metric, Float:fuel_imperial, Float:fuel_metric;
    new Float:distance_empty_imperial, Float: distance_empty_metric;
    new Float: from_refill_imperial, Float: from_refill_metric;
    new Float: consumption_imperial, Float: consumption_metric;

    Vehicle_GetDistanceTravelled(vehicleid, VEHICLE_UNIT_IMPERIAL, travelled_imperial);
    Vehicle_GetDistanceTravelled(vehicleid, VEHICLE_UNIT_METRIC, travelled_metric);

    Vehicle_GetFuelLevel(vehicleid, VEHICLE_UNIT_IMPERIAL, fuel_imperial);
    Vehicle_GetFuelLevel(vehicleid, VEHICLE_UNIT_METRIC, fuel_metric);


    SendClientMessage(0, -1, "Vehicle SPEED %d, Testing miles %f vs kilometres %f" , Vehicle_Speed(vehicleid), travelled_imperial, travelled_metric);
    SendClientMessage(0, -1, "Vehicle fuel level %f gal %f lit", 
        fuel_imperial, fuel_metric);
    
    Vehicle_GetDistanceCanPass(vehicleid, VEHICLE_UNIT_IMPERIAL, distance_empty_imperial);
    Vehicle_GetDistanceCanPass(vehicleid, VEHICLE_UNIT_METRIC, distance_empty_metric);
    
    SendClientMessage(0, -1, "distance to empty %f mi %f km",
        distance_empty_imperial, distance_empty_metric); 

    Vehicle_GetDistanceFromLastRefill(vehicleid, VEHICLE_UNIT_IMPERIAL, from_refill_imperial); 
    Vehicle_GetDistanceFromLastRefill(vehicleid, VEHICLE_UNIT_METRIC, from_refill_metric);
    Vehicle_GetFuelConsumption(vehicleid, VEHICLE_UNIT_IMPERIAL, consumption_imperial);
    Vehicle_GetFuelConsumption(vehicleid, VEHICLE_UNIT_METRIC, consumption_metric);

    SendClientMessage(0, -1, "distance from refill %f mi %f km fuel cons %f gal/100mi %f l/100km",
        from_refill_imperial, from_refill_metric, consumption_imperial, consumption_metric);

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
    new Error: e = Vehicle_AttachTrailer(GetPlayerVehicleID(playerid), vehid);
    if(IsError(e))
    {
        Handled();
    }
    new trailer, cab;
    Vehicle_GetTrailer(GetPlayerVehicleID(playerid), trailer);
    Vehicle_GetTrailerCab(vehid, cab);
    SendClientMessage(playerid, -1, "Trailer %d has been attached to vehicle %d", trailer, cab);

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
        respawn_time,
        occupied_time;

    Vehicle_GetRespawnedTime(GetPlayerVehicleID(playerid), respawn_time);
    Vehicle_GetOccupiedTime(GetPlayerVehicleID(playerid), occupied_time);

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
    new id = Vehicle_Create(vehmodel, x+3, y, z, 0, -1, -1, -1, 0, 0);

    Vehicle_SetFuelTankCapacity(id, VEHICLE_UNIT_METRIC, 300);
    Vehicle_SetFuelConsumption(id, VEHICLE_UNIT_METRIC, 50); // /100kmh
    Vehicle_SetFuelLevel(id, VEHICLE_UNIT_METRIC, 3);
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
    Vehicle_TryTurningEngine(GetPlayerVehicleID(playerid));
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
    Vehicle_GetVirtualWorld(GetPlayerVehicleID(playerid), virtualworld);
    SendClientMessage(playerid, -1, "Vehicle vw has been set to %d", virtualworld);
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
    Vehicle_GetInterior(GetPlayerVehicleID(playerid), interior);
    SendClientMessage(playerid, -1, "Vehicle interior has been set to %d", interior);
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

CMD:testlightsdamagegetex(playerid, params[])
{
    new 
        VEHICLE_LIGHT_CONDITION: front_left,
        VEHICLE_LIGHT_CONDITION: front_right,
        VEHICLE_LIGHT_CONDITION: back;

    Vehicle_GetLightsConditionEx(GetPlayerVehicleID(playerid), front_left, front_right, back);
    printf("Front Left %d\nFront Right %d\nRear %d", front_left, front_right, back);
    return 1;
}

CMD:alarmson(playerid, params[])
{
    Vehicle_SetAlarms(GetPlayerVehicleID(playerid), true);
    return 1;
}

CMD:alarmsoff(playerid, params[])
{
    Vehicle_SetAlarms(GetPlayerVehicleID(playerid), false);
    return 1;
}

CMD:testlightsdamageget(playerid, params[])
{
    new VEHICLE_LIGHT_CONDITION: condition;
    Vehicle_GetLightsCondition(GetPlayerVehicleID(playerid), VEHICLE_LIGHT_FRONT_LEFT, condition);
    printf("Front Left %d", _:condition);
    return 1;
}

CMD:testlightsdamageset(playerid, params[])
{
    Vehicle_SetLightsCondition(GetPlayerVehicleID(playerid), VEHICLE_LIGHT_FRONT_RIGHT, VEHICLE_LIGHT_CONDITION_DAMAGED);
    Vehicle_SetLightsCondition(GetPlayerVehicleID(playerid), VEHICLE_LIGHT_BACK, VEHICLE_LIGHT_CONDITION_DAMAGED);
    return 1;
}



CMD:testlightsdamagesetex(playerid, params[])
{
    Vehicle_SetLightsConditionEx(GetPlayerVehicleID(playerid), VEHICLE_LIGHT_CONDITION_DAMAGED, VEHICLE_LIGHT_CONDITION_DAMAGED, VEHICLE_LIGHT_CONDITION_DAMAGED);
    return 1;
}

CMD:testdestroy(playerid, params[])
{
    Vehicle_Destroy(GetPlayerVehicleID(playerid));
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
    Vehicle_SetPanelsConditionEx(GetPlayerVehicleID(playerid), VEHICLE_PANELS_CONDITION: front_left, VEHICLE_PANELS_CONDITION: front_right, VEHICLE_PANELS_CONDITION: back_left, VEHICLE_PANELS_CONDITION: back_right, VEHICLE_PANELS_CONDITION: front_bumper, VEHICLE_PANELS_CONDITION: back_bumper);
    SendClientMessage(playerid, -1, "Vehicle panels updated, getting values..");
    front_left = -1, front_right = -1, back_left = -1, back_right = -1, front_bumper = -1, back_bumper = -1, windshield = -1;
    Vehicle_GetPanelsConditionEx(GetPlayerVehicleID(playerid), VEHICLE_PANELS_CONDITION: front_left, VEHICLE_PANELS_CONDITION: front_right, VEHICLE_PANELS_CONDITION: back_left, VEHICLE_PANELS_CONDITION: back_right, VEHICLE_PANELS_CONDITION: front_bumper, VEHICLE_PANELS_CONDITION: back_bumper, VEHICLE_PANELS_CONDITION: windshield);
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
    new bool: output_data;
    Vehicle_GetDoorsLocked(GetPlayerVehicleID(playerid), output_data);
    printf("Doors lock %d", output_data);
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

    Vehicle_GetDoorConditionEx(GetPlayerVehicleID(playerid), hood, trunk, driver_door, passenger_door);
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
    Vehicle_GetTyreConditionEx(GetPlayerVehicleID(playerid), tire1, tire2, tire3, tire4);
    SendClientMessage(playerid, -1, "zadnja desna %d, prednja desna %d, zadnja leva %d, prednja leva %d", tire1, tire2, tire3, tire4); 

//    SendClientMessage(playerid, -1, "Test samo prednje desne %d", Vehicle_GetTyreCondition(GetPlayerVehicleID(playerid), VEHICLE_TYRE_FRONT_RIGHT));
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
    new comp;
    Vehicle_GetComponentInSlot(GetPlayerVehicleID(playerid), CARMODTYPE: slot, comp);
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