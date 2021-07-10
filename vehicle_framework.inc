#include <a_samp>
//#define RUN_TESTS
#include <YSI_Data\y_foreach>
#include <YSI_Coding\y_hooks>
#include <YSI_Game\y_vehicledata>
#include <YSI_Core\y_testing>
#include <YSF>

#if defined UGMP_Enable
    #include <ugmp>
#endif // not yet finished

#if defined EMPTY_DAMAGE 
    #include <weapon-config>
#endif

#if defined _YSF_included
    #define YSF_Enable
#endif

#if !defined isnull
    #define isnull(%1) ((!(%1[0])) || (((%1[0]) == '\1') && (!(%1[1]))))
#endif

// -- 
// Script-Side
// --


#if !defined MAX_NUMBER_PLATE
    #define MAX_NUMBER_PLATE 32
#endif

#if !defined MAX_VEHICLE_GROUPS
    #define MAX_VEHICLE_GROUPS 12
#endif

static const Vehicle_gscPaintjob[] = 
{
    483,
    534,
    535,
    536,
    558,
    559,
    560,
    561,
    562,
    565,
    567,
    575,
    576
};

static const 
    DIMENSION_INTERIOR = 0,
    DIMENSION_VIRTUAL_WORLD = 1;

static 
    Vehicle_gsModel[MAX_VEHICLES],
    Vehicle_gsNumberPlate[MAX_VEHICLES][MAX_NUMBER_PLATE],
    Vehicle_gsRespawnDelay[MAX_VEHICLES],
    Vehicle_gsDimensionInfo[MAX_VEHICLES][2],
    Vehicle_gsPaintjob[MAX_VEHICLES],
    Vehicle_gsColorInfo[MAX_VEHICLES][2],
    Vehicle_gsLastDriver[MAX_VEHICLES],
    Vehicle_gsOccupiedTick[MAX_VEHICLES],
    Vehicle_gsSirenState[MAX_VEHICLES],
    Vehicle_gsRespawnTick[MAX_VEHICLES],

    Vehicle_gsIsOccupied[MAX_VEHICLES][4],
    Vehicle_gsTrailer[MAX_VEHICLES][2],

    Float: Vehicle_gsHealth[MAX_VEHICLES],
    Float: Vehicle_gsPosition[MAX_VEHICLES][4];

static
    Player_LastVehicleID[MAX_PLAYERS];


// --
// Params
// --

enum PARAMS
{
    e_ENGINE_STATES: PARAMS_ENGINE,
    e_LIGHT_RUN_STATE: PARAMS_LIGHTS,
    e_ALARMS_STATE: PARAMS_ALARM, 
    e_DOOR_LOCK_STATES: PARAMS_DOORS,
    e_BONNET_STATES: PARAMS_BONNET,
    e_BOOT_STATES: PARAMS_BOOT,
    e_OBJECTIVE_STATES: PARAMS_OBJECTIVE
}

enum WINDOWS
{
    e_WINDOWS_STATES: WINDOWS_DRIVER = 0,
    e_WINDOWS_STATES: WINDOWS_PASSENGER,
    e_WINDOWS_STATES: WINDOWS_REAR_LEFT,
    e_WINDOWS_STATES: WINDOWS_REAR_RIGHT
}

static
    Vehicle_gsParams[MAX_VEHICLES][PARAMS],
    Vehicle_gsWindows[MAX_VEHICLES][WINDOWS];

/**
  * <summary> Creates a vehicle.</summary>
  * <param name="modelid">vehicle model (400-611)</param>
  * <param name="Float: x">x vehicle rotation</param>
  * <param name="Float: y">y vehicle rotation</param>
  * <param name="Float: z">z vehicle rotation</param>
  * <param name="Float: rotation">vehicle angle</param>
  * <param name="respawn_delay">The time(ms) when the vehicle will be respawned again.</param>
  * <param name="interior">interior id, if you want to create a vehicle inside the building</param>
*/
stock Vehicle_Create(modelid, Float: x, Float: y, Float: z, Float: rotation, color1, color2, respawn_delay, interior, virtual_world, addsiren=0) 
{
    new idx;
    if(modelid == 537 || modelid == 538)
    {
        idx = AddStaticVehicleEx(modelid, x, y, z, rotation, color1, color2, respawn_delay, addsiren);
    }
    else 
    {
        idx = CreateVehicle(modelid, x, y, z, rotation, color1, color2, respawn_delay, addsiren); 
    }

    if(!IsValidVehicle(idx)) 
    {
        return INVALID_VEHICLE_ID;
    }


    Vehicle_gsModel[idx] = modelid;
    Vehicle_gsPosition[idx][0] = x;
    Vehicle_gsPosition[idx][1] = y;
    Vehicle_gsPosition[idx][2] = z;
    Vehicle_gsPosition[idx][3] = rotation;
    Vehicle_gsColorInfo[idx][0] = color1;
    Vehicle_gsColorInfo[idx][1] = color2;
    Vehicle_gsPaintjob[idx] = -1;
    Vehicle_gsSirenState[idx] = addsiren;
    Vehicle_gsTrailer[idx][0] = 0;
    Vehicle_gsTrailer[idx][1] = 0;
    Vehicle_gsRespawnDelay[idx] = respawn_delay;
    Vehicle_gsDimensionInfo[idx][DIMENSION_INTERIOR] = interior;
    Vehicle_gsDimensionInfo[idx][DIMENSION_VIRTUAL_WORLD] = virtual_world;
    Vehicle_gsOccupiedTick[idx] = -1;
    Vehicle_gsRespawnTick[idx] = gettime();

    Vehicle_gsLastDriver[idx] = -1;
    Vehicle_gsIsOccupied[idx][0] = -1;
    Vehicle_gsIsOccupied[idx][1] = -1;
    Vehicle_gsIsOccupied[idx][2] = -1;
    Vehicle_gsIsOccupied[idx][3] = -1;
    
    SetVehicleVirtualWorld(idx, virtual_world);
    LinkVehicleToInterior(idx, interior);
    return idx;
}

/**
 * <summary> Creates a vehicle with additional params.</summary>
 * <param name="modelid">vehicle model (400-611)</param>
 * <param name="Float: x">x vehicle rotation</param>
 * <param name="Float: y">y vehicle rotation</param>
 * <param name="Float: z">z vehicle rotation</param>
 * <param name="Float: rotation">vehicle angle</param>
 * <param name="respawn_delay">The time(ms) when the vehicle will be respawned again.</param>
 * <param name="interior">interior id, if you want to create a vehicle inside the building</param>
 * <param name="Float: health">vehicle health (damage)</param>
 * <param name="plate">the text that will appear on the vehicle plates</param>
*/
stock Vehicle_CreateEx(modelid, Float: x, Float: y, Float: z, Float: rotation, color1, color2, respawn_delay, interior, virtual_world, Float: health, const plate[MAX_NUMBER_PLATE], addsiren=0) 
{
    new idx;
    if(modelid == 537 || modelid == 538)
    {
        idx = AddStaticVehicleEx(modelid, x, y, z, rotation, color1, color2, respawn_delay, addsiren);
    }
    else 
    {
        idx = CreateVehicle(modelid, x, y, z, rotation, color1, color2, respawn_delay, addsiren);
    }
    
    Vehicle_gsModel[idx] = modelid;
    Vehicle_gsPosition[idx][0] = x;
    Vehicle_gsPosition[idx][1] = y;
    Vehicle_gsPosition[idx][2] = z;
    Vehicle_gsPosition[idx][3] = rotation;
    Vehicle_gsSirenState[idx] = addsiren;
    Vehicle_gsColorInfo[idx][0] = color1;
    Vehicle_gsColorInfo[idx][1] = color2;
    Vehicle_gsPaintjob[idx] = -1;
    Vehicle_gsTrailer[idx][0] = 0;
    Vehicle_gsTrailer[idx][1] = 0;
    Vehicle_gsRespawnDelay[idx] = respawn_delay;
    Vehicle_gsDimensionInfo[idx][DIMENSION_INTERIOR] = interior;
    Vehicle_gsDimensionInfo[idx][DIMENSION_VIRTUAL_WORLD] = virtual_world;
    Vehicle_gsHealth[idx] = health;
    Vehicle_gsOccupiedTick[idx] = -1;
    Vehicle_gsRespawnTick[idx] = gettime();

    Vehicle_gsLastDriver[idx] = -1;
    Vehicle_gsIsOccupied[idx][0] = -1;
    Vehicle_gsIsOccupied[idx][1] = -1;
    Vehicle_gsIsOccupied[idx][2] = -1;
    Vehicle_gsIsOccupied[idx][3] = -1;
    
    strcopy(Vehicle_gsNumberPlate[idx], plate);
    
    SetVehicleHealth(idx, health);
    SetVehicleNumberPlate(idx, plate);
    SetVehicleVirtualWorld(idx, virtual_world);
    LinkVehicleToInterior(idx, interior);
    return idx;
}

// --
// Groups 
// --

#define VEHICLE_FRAMEWORK_TAGS \
    {_, VehicleGroup}

#define INVALID_VEHICLE_GROUP_ID -1

static
    Vehicle_gsGroupName[VehicleGroup: MAX_VEHICLE_GROUPS][24],
    Vehicle_gsGroupIsActive[VehicleGroup: MAX_VEHICLE_GROUPS],
    Iterator: Vehicle_gsActGps<MAX_VEHICLE_GROUPS>,
    Iterator: Vehicle_gsGroups[VehicleGroup: MAX_VEHICLE_GROUPS]<MAX_VEHICLES>;

stock bool: Vehicle_IsGroupActive(VehicleGroup: groupid)
{
    if(Vehicle_gsGroupIsActive[groupid] == 1) 
    {
        return true;
    }
    return false;
}

stock VehicleGroup: Vehicle_GetGroupByName(const name[])
{
    foreach(new i : Vehicle_gsActGps) 
    {
        if(!strcmp(name, Vehicle_gsGroupName[VehicleGroup: i]) && !isnull(name)) 
        {
            return VehicleGroup: i;
        }
    }
    return VehicleGroup: INVALID_VEHICLE_GROUP_ID;
}

stock bool: Vehicle_GroupHas(VehicleGroup: groupid, vehicleid)
{
    if(Vehicle_IsGroupActive(groupid) && IsValidVehicle(vehicleid) && Iter_Contains(Vehicle_gsGroups[groupid], vehicleid))
    {
        return true;
    }
    return false;
}

stock VehicleGroup: Vehicle_GroupInit(const name[]) 
{
    new VehicleGroup: idx = VehicleGroup: Iter_Alloc(Vehicle_gsActGps);
    if(idx == VehicleGroup: INVALID_ITERATOR_SLOT || Vehicle_GetGroupByName(name) == VehicleGroup: INVALID_VEHICLE_GROUP_ID)
    {
        return VehicleGroup: INVALID_VEHICLE_GROUP_ID;
    }
    strcopy(Vehicle_gsGroupName[idx], name);
    Vehicle_gsGroupIsActive[idx] = 1;
    return idx;
}

stock bool: Vehicle_IsValidGroup(VehicleGroup: groupid)
{
    if(groupid == VehicleGroup: INVALID_VEHICLE_GROUP_ID)
    {
        return false;
    }
    return true;
}

stock Vehicle_AddToGroup(VehicleGroup: groupid, vehicleid)
{
    if(!Vehicle_IsGroupActive(groupid) || !IsValidVehicle(vehicleid))
    {
        return 0;
    }
    Iter_Add(Vehicle_gsGroups[groupid], vehicleid);
    return 1;
}

stock Vehicle_RemoveFromGroup(VehicleGroup: groupid, vehicleid)
{
    if(!Vehicle_IsGroupActive(groupid) || !Iter_Contains(Vehicle_gsGroups[groupid], vehicleid))
    {
        return 0;
    }
    Iter_Remove(Vehicle_gsGroups[groupid], vehicleid);
    return 1;
}

// -- 
// Engine 
// --

enum e_ENGINE_STATES 
{
    E_ENGINE_STATE_OFF = 0,
    E_ENGINE_STATE_ON,
};

/**
 * <summary> Gets the engine state.</summary>
 * <param name="vehicleid"> Vehicle from which we get engine state.</param>
 * <returns> Engine state. </returns>
 */

stock e_ENGINE_STATES: Vehicle_GetEngineState(vehicleid) 
{
    GetVehicleParamsEx(vehicleid, Vehicle_gsParams[vehicleid][PARAMS_ENGINE], Vehicle_gsParams[vehicleid][PARAMS_LIGHTS], Vehicle_gsParams[vehicleid][PARAMS_ALARM], Vehicle_gsParams[vehicleid][PARAMS_DOORS], Vehicle_gsParams[vehicleid][PARAMS_BONNET], Vehicle_gsParams[vehicleid][PARAMS_BOOT], Vehicle_gsParams[vehicleid][PARAMS_OBJECTIVE]);

    return Vehicle_gsParams[vehicleid][PARAMS_ENGINE];
}

/**
 * <summary> Sets the engine state.</summary>
 * <param name="vehicleid"> Vehicle which we set the engine state.</param>
 * <param name="engine_state"> Engine state to set.</param>
 * <returns> SetVehicleParamsEx values. </returns>
*/

stock Vehicle_SetEngineState(vehicleid, e_ENGINE_STATES: engine_state) 
{

    GetVehicleParamsEx(vehicleid, Vehicle_gsParams[vehicleid][PARAMS_ENGINE], Vehicle_gsParams[vehicleid][PARAMS_LIGHTS], Vehicle_gsParams[vehicleid][PARAMS_ALARM], Vehicle_gsParams[vehicleid][PARAMS_DOORS], Vehicle_gsParams[vehicleid][PARAMS_BONNET], Vehicle_gsParams[vehicleid][PARAMS_BOOT], Vehicle_gsParams[vehicleid][PARAMS_OBJECTIVE]);
    Vehicle_gsParams[vehicleid][PARAMS_ENGINE] = engine_state;
    new const ret = SetVehicleParamsEx(vehicleid, Vehicle_gsParams[vehicleid][PARAMS_ENGINE], Vehicle_gsParams[vehicleid][PARAMS_LIGHTS], Vehicle_gsParams[vehicleid][PARAMS_ALARM], Vehicle_gsParams[vehicleid][PARAMS_DOORS], Vehicle_gsParams[vehicleid][PARAMS_BONNET], Vehicle_gsParams[vehicleid][PARAMS_BOOT], Vehicle_gsParams[vehicleid][PARAMS_OBJECTIVE]);
    
    return ret;
}

// --
// Boot
// --

enum e_BOOT_STATES 
{
    E_BOOT_CLOSED = 0,
    E_BOOT_OPENED
}

/**
 * <summary> Gets the engine state.</summary>
 * <param name="vehicleid"> Vehicle from which we get engine state.</param>
 * <returns> Engine state. </returns>
 */

stock e_BOOT_STATES: Vehicle_GetBootState(vehicleid) 
{
    GetVehicleParamsEx(vehicleid, Vehicle_gsParams[vehicleid][PARAMS_ENGINE], Vehicle_gsParams[vehicleid][PARAMS_LIGHTS], Vehicle_gsParams[vehicleid][PARAMS_ALARM], Vehicle_gsParams[vehicleid][PARAMS_DOORS], Vehicle_gsParams[vehicleid][PARAMS_BONNET], Vehicle_gsParams[vehicleid][PARAMS_BOOT], Vehicle_gsParams[vehicleid][PARAMS_OBJECTIVE]);

    return Vehicle_gsParams[vehicleid][PARAMS_BOOT];
}

/**
 * <summary> Sets the boot state.</summary>
 * <param name="vehicleid"> Vehicle which we set the boot state.</param>
 * <param name="engine_state"> Boot state to set.</param>
 * <returns> SetVehicleParamsEx values. </returns>
*/

stock Vehicle_SetBootState(vehicleid, e_BOOT_STATES: boot_state) 
{

    GetVehicleParamsEx(vehicleid, Vehicle_gsParams[vehicleid][PARAMS_ENGINE], Vehicle_gsParams[vehicleid][PARAMS_LIGHTS], Vehicle_gsParams[vehicleid][PARAMS_ALARM], Vehicle_gsParams[vehicleid][PARAMS_DOORS], Vehicle_gsParams[vehicleid][PARAMS_BONNET], Vehicle_gsParams[vehicleid][PARAMS_BOOT], Vehicle_gsParams[vehicleid][PARAMS_OBJECTIVE]);
    Vehicle_gsParams[vehicleid][PARAMS_BOOT] = boot_state;
    new const ret = SetVehicleParamsEx(vehicleid, Vehicle_gsParams[vehicleid][PARAMS_ENGINE], Vehicle_gsParams[vehicleid][PARAMS_LIGHTS], Vehicle_gsParams[vehicleid][PARAMS_ALARM], Vehicle_gsParams[vehicleid][PARAMS_DOORS], Vehicle_gsParams[vehicleid][PARAMS_BONNET], Vehicle_gsParams[vehicleid][PARAMS_BOOT], Vehicle_gsParams[vehicleid][PARAMS_OBJECTIVE]);
    
    return ret;
}

// --
// Bonnet
// --

enum e_BONNET_STATES 
{
    E_BONNET_CLOSED = 0,
    E_BONNET_OPENED
}

/**
 * <summary> Gets the bonnet state.</summary>
 * <param name="vehicleid"> Vehicle from which we get bonnet state.</param>
 * <returns> Bonnet state. </returns>
 */

stock e_BOOT_STATES: Vehicle_GetBonnetState(vehicleid) 
{
    GetVehicleParamsEx(vehicleid, Vehicle_gsParams[vehicleid][PARAMS_ENGINE], Vehicle_gsParams[vehicleid][PARAMS_LIGHTS], Vehicle_gsParams[vehicleid][PARAMS_ALARM], Vehicle_gsParams[vehicleid][PARAMS_DOORS], Vehicle_gsParams[vehicleid][PARAMS_BONNET], Vehicle_gsParams[vehicleid][PARAMS_BOOT], Vehicle_gsParams[vehicleid][PARAMS_OBJECTIVE]);

    return Vehicle_gsParams[vehicleid][PARAMS_BONNET];
}

/**
 * <summary> Sets the bonnet state.</summary>
 * <param name="vehicleid"> Vehicle which we set the bonnet state.</param>
 * <param name="engine_state"> Bonnet state to set.</param>
 * <returns> SetVehicleParamsEx values. </returns>
*/

stock Vehicle_SetBonnetState(vehicleid, e_BONNET_STATES: bonnet_state) 
{

    GetVehicleParamsEx(vehicleid, Vehicle_gsParams[vehicleid][PARAMS_ENGINE], Vehicle_gsParams[vehicleid][PARAMS_LIGHTS], Vehicle_gsParams[vehicleid][PARAMS_ALARM], Vehicle_gsParams[vehicleid][PARAMS_DOORS], Vehicle_gsParams[vehicleid][PARAMS_BONNET], Vehicle_gsParams[vehicleid][PARAMS_BOOT], Vehicle_gsParams[vehicleid][PARAMS_OBJECTIVE]);
    Vehicle_gsParams[vehicleid][PARAMS_BONNET] = bonnet_state;
    new const ret = SetVehicleParamsEx(vehicleid, Vehicle_gsParams[vehicleid][PARAMS_ENGINE], Vehicle_gsParams[vehicleid][PARAMS_LIGHTS], Vehicle_gsParams[vehicleid][PARAMS_ALARM], Vehicle_gsParams[vehicleid][PARAMS_DOORS], Vehicle_gsParams[vehicleid][PARAMS_BONNET], Vehicle_gsParams[vehicleid][PARAMS_BOOT], Vehicle_gsParams[vehicleid][PARAMS_OBJECTIVE]);
    
    return ret;
}

// --
// Objective
// --

enum e_OBJECTIVE_STATES 
{
    E_OBJECTIVE_OFF = 0,
    E_OBJECTIVE_ON
}

/**
 * <summary> Gets the objective state.</summary>
 * <param name="vehicleid"> Vehicle from which we get Objective state.</param>
 * <returns> Objective state. </returns>
 */

stock e_BOOT_STATES: Vehicle_GetObjectiveState(vehicleid) 
{
    GetVehicleParamsEx(vehicleid, Vehicle_gsParams[vehicleid][PARAMS_ENGINE], Vehicle_gsParams[vehicleid][PARAMS_LIGHTS], Vehicle_gsParams[vehicleid][PARAMS_ALARM], Vehicle_gsParams[vehicleid][PARAMS_DOORS], Vehicle_gsParams[vehicleid][PARAMS_BONNET], Vehicle_gsParams[vehicleid][PARAMS_BOOT], Vehicle_gsParams[vehicleid][PARAMS_OBJECTIVE]);

    return Vehicle_gsParams[vehicleid][PARAMS_OBJECTIVE];
}

/**
 * <summary> Sets the Objective state.</summary>
 * <param name="vehicleid"> Vehicle which we set the Objective state.</param>
 * <param name="engine_state"> Objective state to set.</param>
 * <returns> SetVehicleParamsEx values. </returns>
*/

stock Vehicle_SetObjectiveState(vehicleid, e_BONNET_STATES: bonnet_state) 
{

    GetVehicleParamsEx(vehicleid, Vehicle_gsParams[vehicleid][PARAMS_ENGINE], Vehicle_gsParams[vehicleid][PARAMS_LIGHTS], Vehicle_gsParams[vehicleid][PARAMS_ALARM], Vehicle_gsParams[vehicleid][PARAMS_DOORS], Vehicle_gsParams[vehicleid][PARAMS_BONNET], Vehicle_gsParams[vehicleid][PARAMS_BOOT], Vehicle_gsParams[vehicleid][PARAMS_OBJECTIVE]);
    Vehicle_gsParams[vehicleid][PARAMS_OBJECTIVE] = bonnet_state;
    new const ret = SetVehicleParamsEx(vehicleid, Vehicle_gsParams[vehicleid][PARAMS_ENGINE], Vehicle_gsParams[vehicleid][PARAMS_LIGHTS], Vehicle_gsParams[vehicleid][PARAMS_ALARM], Vehicle_gsParams[vehicleid][PARAMS_DOORS], Vehicle_gsParams[vehicleid][PARAMS_BONNET], Vehicle_gsParams[vehicleid][PARAMS_BOOT], Vehicle_gsParams[vehicleid][PARAMS_OBJECTIVE]);
    
    return ret;
}


// --
// Lights
// --

enum e_LIGHT_STATES 
{
    E_LIGHT_ON = 0,
    E_LIGHT_OFF
};

enum e_LIGHT_RUN_STATE 
{
    E_LIGHTS_OFF,
    E_LIGHTS_ON    
};

// internal

static stock vehicle_EncodeLights(front_left_light, front_right_light, back_lights) 
{
    return front_left_light | (front_right_light << 2) | (back_lights << 6);
}

static stock vehicle_DecodeLights(lights, &front_left_light, &front_right_light, &back_lights) 
{
    front_left_light = lights & 1;
    front_right_light = lights >> 2 & 1;
    back_lights = lights >> 6 & 1;
}

//

/**
 * <summary> Sets the lights state.</summary>
 * <param name="vehicleid"> Vehicle which we set the lights state.</param>
 * <param name="left_lights"> Left Ligth state to set.</param>
 * <param name="right_lights"> Right Ligth state to set.</param>
 * <param name="back_lights"> Back Ligth state to set.</param>
 * <returns> UpdateVehicleDamageStatus values. </returns>
*/


stock Vehicle_SetLightsState(vehicleid, e_LIGHT_STATES: left_lights, e_LIGHT_STATES: right_lights, e_LIGHT_STATES: back_lights) 
{
    new 
        lights, 
        unnecessary_doors,
        unnecessary_panels, 
        unnecessary_tires;

    GetVehicleDamageStatus(vehicleid, unnecessary_panels, unnecessary_doors, lights, unnecessary_tires);
    new const ret = UpdateVehicleDamageStatus(vehicleid, unnecessary_panels, unnecessary_doors, vehicle_EncodeLights(left_lights, right_lights, back_lights), unnecessary_tires);
    
    return ret;
}


/**
 * <summary> Gets the lights state.</summary>
 * <param name="vehicleid"> Vehicle from which we get the lights state.</param>
 * <param name="&left_lights"> Left Ligth state to get.</param>
 * <param name="&right_lights"> Right Ligth state to get.</param>
 * <param name="&back_lights"> Back Ligth state to get.</param>
*/

stock Vehicle_GetLightsState(vehicleid, &e_LIGHT_STATES: left_lights, &e_LIGHT_STATES: right_lights, &e_LIGHT_STATES: back_lights) 
{    
    new 
        lights,
        unnecessary_params;

    GetVehicleDamageStatus(vehicleid, unnecessary_params, unnecessary_params, lights, unnecessary_params);
    vehicle_DecodeLights(lights, left_lights, right_lights, back_lights);

}


/**
 * <summary> Sets the lights state.</summary>
 * <param name="vehicleid"> Vehicle which we set the lights running state.</param>
 * <param name="light_state"> Lights state to set.</param>
 * <returns> SetVehicleParamsEx values. </returns>
*/

stock Vehicle_SetLightsRunState(vehicleid, e_LIGHT_RUN_STATE: light_state) 
{
        
    GetVehicleParamsEx(vehicleid, Vehicle_gsParams[vehicleid][PARAMS_ENGINE], Vehicle_gsParams[vehicleid][PARAMS_LIGHTS], Vehicle_gsParams[vehicleid][PARAMS_ALARM], Vehicle_gsParams[vehicleid][PARAMS_DOORS], Vehicle_gsParams[vehicleid][PARAMS_BONNET], Vehicle_gsParams[vehicleid][PARAMS_BOOT], Vehicle_gsParams[vehicleid][PARAMS_OBJECTIVE]);
    Vehicle_gsParams[vehicleid][PARAMS_LIGHTS] = light_state;
    new const ret = SetVehicleParamsEx(vehicleid, Vehicle_gsParams[vehicleid][PARAMS_ENGINE], Vehicle_gsParams[vehicleid][PARAMS_LIGHTS], Vehicle_gsParams[vehicleid][PARAMS_ALARM], Vehicle_gsParams[vehicleid][PARAMS_DOORS], Vehicle_gsParams[vehicleid][PARAMS_BONNET], Vehicle_gsParams[vehicleid][PARAMS_BOOT], Vehicle_gsParams[vehicleid][PARAMS_OBJECTIVE]);
    
    return ret;
}

/**
 * <summary> Sets the lights state.</summary>
 * <param name="vehicleid"> Vehicle from which we get the lights running state.</param>
 * <returns> Lights' running state. </returns>
*/

stock e_LIGHT_RUN_STATE: Vehicle_GetLightsRunState(vehicleid) 
{
    GetVehicleParamsEx(vehicleid, Vehicle_gsParams[vehicleid][PARAMS_ENGINE], Vehicle_gsParams[vehicleid][PARAMS_LIGHTS], Vehicle_gsParams[vehicleid][PARAMS_ALARM], Vehicle_gsParams[vehicleid][PARAMS_DOORS], Vehicle_gsParams[vehicleid][PARAMS_BONNET], Vehicle_gsParams[vehicleid][PARAMS_BOOT], Vehicle_gsParams[vehicleid][PARAMS_OBJECTIVE]);
    return Vehicle_gsParams[vehicleid][PARAMS_LIGHTS];
}


// --
// Alarms
// --
enum e_ALARMS_STATE 
{
    E_ALARMS_OFF = 0,
    E_ALARMS_ON
};

/**
 * <summary> Sets the alarms state.</summary>
 * <param name="vehicleid"> Vehicle which we set the alarms state.</param>
 * <returns> SetVehicleParamsEx values. </returns>
*/

stock Vehicle_SetAlarms(vehicleid, e_ALARMS_STATE: alarm_state) 
{
    GetVehicleParamsEx(vehicleid, Vehicle_gsParams[vehicleid][PARAMS_ENGINE], Vehicle_gsParams[vehicleid][PARAMS_LIGHTS], Vehicle_gsParams[vehicleid][PARAMS_ALARM], Vehicle_gsParams[vehicleid][PARAMS_DOORS], Vehicle_gsParams[vehicleid][PARAMS_BONNET], Vehicle_gsParams[vehicleid][PARAMS_BOOT], Vehicle_gsParams[vehicleid][PARAMS_OBJECTIVE]);
    Vehicle_gsParams[vehicleid][PARAMS_ALARM] = alarm_state;
   
    new const ret = SetVehicleParamsEx(vehicleid, Vehicle_gsParams[vehicleid][PARAMS_ENGINE], Vehicle_gsParams[vehicleid][PARAMS_LIGHTS], Vehicle_gsParams[vehicleid][PARAMS_ALARM], Vehicle_gsParams[vehicleid][PARAMS_DOORS], Vehicle_gsParams[vehicleid][PARAMS_BONNET], Vehicle_gsParams[vehicleid][PARAMS_BOOT], Vehicle_gsParams[vehicleid][PARAMS_OBJECTIVE]);
    return ret;
}

/**
 * <summary> Gets the alarms state.</summary>
 * <param name="vehicleid"> Vehicle from which we set the alarms state.</param>
 * <returns> Alarms' state. </returns>
*/

stock e_ALARMS_STATE: Vehicle_GetAlarms(vehicleid) 
{
    GetVehicleParamsEx(vehicleid, Vehicle_gsParams[vehicleid][PARAMS_ENGINE], Vehicle_gsParams[vehicleid][PARAMS_LIGHTS], Vehicle_gsParams[vehicleid][PARAMS_ALARM], Vehicle_gsParams[vehicleid][PARAMS_DOORS], Vehicle_gsParams[vehicleid][PARAMS_BONNET], Vehicle_gsParams[vehicleid][PARAMS_BOOT], Vehicle_gsParams[vehicleid][PARAMS_OBJECTIVE]);
    return Vehicle_gsParams[vehicleid][PARAMS_ALARM];
}

// ---
// Doors
// ---

enum e_DOOR_LOCK_STATES 
{
    E_DOOR_STATE_OFF = 0,
    E_DOOR_STATE_ON
};

enum e_DOOR_STATES 
{
    E_DOOR_STATE_NORMAL = 0,
    E_DOOR_STATE_OPEN = 1,
    E_DOOR_STATE_DAMAGED = 2,
    E_DOOR_STATE_OPEN_DAMAGED,
    E_DOOR_STATE_REMOVED
};

//--
static stock vehicle_EncodeDoors(bonnet, boot, driver_door, passenger_door) 
{
    return bonnet | (boot << 8) | (driver_door << 16) | (passenger_door << 24);
}


static stock vehicle_DecodeDoors(doors, &bonnet, &boot, &driver_door, &passenger_door) 
{
    bonnet = doors & 7;
    boot = doors >> 8 & 7;
    driver_door = doors >> 16 & 7;
    passenger_door = doors >> 24 & 7;
}
//

/**
 * <summary> Sets the doors state.</summary>
 * <param name="vehicleid"> Vehicle which we set the doors state.</param>
 * <param name="hood"> Hood state to set. </param>
 * <param name="trunk"> Trunk state to set. </param>
 * <param name="driver"> Driver-door state to set. </param>
 * <param name="passenger"> Passenger-door state to set. </param>
 * <returns> UpdateVehicleDamageStatus values. </returns>
*/

stock Vehicle_SetDoorState(vehicleid, e_DOOR_STATES: hood, e_DOOR_STATES: trunk, e_DOOR_STATES: driver, e_DOOR_STATES: passenger) 
{
    new
        doors,
        unnecessary_panels,
        unnecessary_tires,
        unnecessary_lights,
        ret;

    GetVehicleDamageStatus(vehicleid, unnecessary_panels, doors, unnecessary_lights, unnecessary_tires);
    ret = UpdateVehicleDamageStatus(vehicleid, unnecessary_panels, vehicle_EncodeDoors(hood, trunk, driver, passenger), unnecessary_lights, unnecessary_tires);
    return ret;
}

/**
 * <summary> Gets the doors state.</summary>
 * <param name="vehicleid"> Vehicle from which we get the doors state.</param>
 * <param name="&hood"> Hood state to get.</param>
 * <param name="&trunk"> Trunk state to get.</param>
 * <param name="&driver"> Driver-door state to get.</param>
 * <param name="&passenger"> Passenger-door state to get.</param>
*/

stock Vehicle_GetDoorState(vehicleid, &e_DOOR_STATES: bonnet, &e_DOOR_STATES: boot, &e_DOOR_STATES: driver, &e_DOOR_STATES: passenger) 
{
    new 
        doors,
        unnecessary_params;

    GetVehicleDamageStatus(vehicleid, unnecessary_params, doors, unnecessary_params, unnecessary_params);
    vehicle_DecodeDoors(doors, bonnet, boot, driver, passenger);
}

/**
 * <summary> Gets the doors' lock state.</summary>
 * <param name="vehicleid"> Vehicle from which we set the doors' lock state.</param>
 * <returns> Doors' lock state. </returns>
*/

stock e_DOOR_LOCK_STATES: Vehicle_GetDoorsLockState(vehicleid) 
{

    GetVehicleParamsEx(vehicleid, Vehicle_gsParams[vehicleid][PARAMS_ENGINE], Vehicle_gsParams[vehicleid][PARAMS_LIGHTS], Vehicle_gsParams[vehicleid][PARAMS_ALARM], Vehicle_gsParams[vehicleid][PARAMS_DOORS], Vehicle_gsParams[vehicleid][PARAMS_BONNET], Vehicle_gsParams[vehicleid][PARAMS_BOOT], Vehicle_gsParams[vehicleid][PARAMS_OBJECTIVE]);
    
    return Vehicle_gsParams[vehicleid][PARAMS_DOORS];
}

/**
 * <summary> Sets the alarms state.</summary>
 * <param name="vehicleid"> Vehicle which we set the doors' lock state.</param>
 * <param name="door_state"> Door's lock state to set. </param>
 * <returns> SetVehicleParamsEx values. </returns>
*/

stock Vehicle_SetDoorsLockState(vehicleid, e_DOOR_LOCK_STATES: door_state) 
{
    GetVehicleParamsEx(vehicleid, Vehicle_gsParams[vehicleid][PARAMS_ENGINE], Vehicle_gsParams[vehicleid][PARAMS_LIGHTS], Vehicle_gsParams[vehicleid][PARAMS_ALARM], Vehicle_gsParams[vehicleid][PARAMS_DOORS], Vehicle_gsParams[vehicleid][PARAMS_BONNET], Vehicle_gsParams[vehicleid][PARAMS_BOOT], Vehicle_gsParams[vehicleid][PARAMS_OBJECTIVE]);
    Vehicle_gsParams[vehicleid][PARAMS_DOORS] = door_state;
    new const ret = SetVehicleParamsEx(vehicleid, Vehicle_gsParams[vehicleid][PARAMS_ENGINE], Vehicle_gsParams[vehicleid][PARAMS_LIGHTS], Vehicle_gsParams[vehicleid][PARAMS_ALARM], Vehicle_gsParams[vehicleid][PARAMS_DOORS], Vehicle_gsParams[vehicleid][PARAMS_BONNET], Vehicle_gsParams[vehicleid][PARAMS_BOOT], Vehicle_gsParams[vehicleid][PARAMS_OBJECTIVE]);
    return ret;
}



// ---
// Tires
// ---

enum e_TIRE_STATUS 
{
    E_TIRE_INFLATED = 0,
    E_TIRE_POPPED
};

/**
 * <summary> Gets the tires state.</summary>
 * <param name="vehicleid"> Vehicle which we set the tires state.</param>
 * <param name="rear_right_tire"> Rear Right Tire state to set.</param>
 * <param name="front_right_tire"> Front Right Tire state to set.</param>
 * <param name="rear_left_tire"> Rear Left Tire state to set.</param>
 * <param name="front_left_tire"> Front Left Tire state to set.</param>
 * <returns> UpdateVehicleDamageStatus values. </returns>
*/

stock Vehicle_SetTireState(vehicleid, e_TIRE_STATUS: rear_right_tire, e_TIRE_STATUS: front_right_tire, e_TIRE_STATUS: rear_left_tire, e_TIRE_STATUS: front_left_tire) 
{    
    new
        tires,
        unnecessary_panels,
        unnecessary_doors,
        unnecessary_lights,
        ret;

    GetVehicleDamageStatus(vehicleid, unnecessary_panels, unnecessary_doors, unnecessary_lights, tires);
    ret = UpdateVehicleDamageStatus(vehicleid, unnecessary_panels, unnecessary_doors, unnecessary_lights, vehicle_EncodeTires(rear_right_tire, front_right_tire, rear_left_tire, front_left_tire));

    return ret;
}

/**
 * <summary> Gets the tires state.</summary>
 * <param name="vehicleid"> Vehicle from which we get the tires state.</param>
 * <param name="&rear_right_tire"> Rear Right Tire state to get.</param>
 * <param name="&front_right_tire"> Front Right Tire state to get.</param>
 * <param name="&rear_left_tire"> Rear Left Tire state to get.</param>
 * <param name="&front_left_tire"> Front Left Tire state to get.</param>
*/

stock Vehicle_GetTireState(vehicleid, &e_TIRE_STATUS: rear_right_tire, &e_TIRE_STATUS: front_right_tire, &e_TIRE_STATUS: rear_left_tire, &e_TIRE_STATUS: front_left_tire) 
{    
    new 
        tires,
        unnecessary_params;

    GetVehicleDamageStatus(vehicleid, unnecessary_params, unnecessary_params, unnecessary_params, tires);
    vehicle_DecodeTires(tires, rear_right_tire, front_right_tire, rear_left_tire, front_left_tire);

}

//--

static stock vehicle_EncodeTires(rear_right_tire, front_right_tire, rear_left_tire, front_left_tire)
{
    return rear_right_tire | (front_right_tire << 1) | (rear_left_tire << 2) | (front_left_tire << 3);
}

static stock vehicle_DecodeTires(tires, &rear_right_tire, &front_right_tire, &rear_left_tire, &front_left_tire)
{
    rear_right_tire = tires & 1;
    front_right_tire = tires >> 1 & 1;
    rear_left_tire = tires >> 2 & 1;
    front_left_tire = tires >> 3 & 1;
}

// -- 
// Plate
// --

/**
 * <summary> Gets the vehicle's number plate.</summary>
 * <param name="vehicleid"> Vehicle from which we get the vehicle plate.</param>
 * <param name="plate"> Plate to get.</param>
 * <param name="len"> Size of the plate.</param>
*/

stock Vehicle_GetNumberPlate(vehicleid, plate[], len = sizeof(plate)) 
{
    #if defined YSF_Enable
        GetVehicleNumberPlate(vehicleid, plate, len);
    #else
        format(plate, len, "%s", Vehicle_gsNumberPlate[vehicleid]);
    #endif
}

/**
 * <summary> Sets the vehicle's number plate.</summary>
 * <param name="vehicleid"> Vehicle which we set the vehicle plate.</param>
 * <param name="plate"> Plate to set.</param>
*/

stock Vehicle_SetNumberPlate(vehicleid, const plate[]) 
{

	format(Vehicle_gsNumberPlate[vehicleid], MAX_NUMBER_PLATE, "%s", plate);
	new const ret = SetVehicleNumberPlate(vehicleid, Vehicle_gsNumberPlate[vehicleid]);
    return ret;
}

// --
// Windows 
// --

enum e_WINDOWS_STATES 
{
    E_WINDOW_NOT_SET = -1,
    E_WINDOW_OPENED = 0,
    E_WINDOW_CLOSED
};

/**
 * <summary> Sets the vehicle's windows states.</summary>
 * <param name="vehicleid"> Vehicle which we set the vehicle plate.</param>
 * <param name="driver"> Driver-window state to set.</param>
 * <param name="passenger"> Passenger-window state to set.</param>
 * <param name="backleft"> Backleft-window state to set.</param>
 * <param name="backright"> Backright-window state to gset.</param>
*/

stock Vehicle_SetWindows(vehicleid, e_WINDOWS_STATES: driver, e_WINDOWS_STATES: passenger, e_WINDOWS_STATES: backleft, e_WINDOWS_STATES: backright) 
{
    Vehicle_gsWindows[vehicleid][WINDOWS_DRIVER] = driver;
    Vehicle_gsWindows[vehicleid][WINDOWS_PASSENGER] = passenger;
    Vehicle_gsWindows[vehicleid][WINDOWS_REAR_LEFT] = backleft;
    Vehicle_gsWindows[vehicleid][WINDOWS_REAR_RIGHT] = backright;
    SetVehicleParamsCarWindows(vehicleid, Vehicle_gsWindows[vehicleid][WINDOWS_DRIVER], Vehicle_gsWindows[vehicleid][WINDOWS_PASSENGER], Vehicle_gsWindows[vehicleid][WINDOWS_REAR_LEFT], Vehicle_gsWindows[vehicleid][WINDOWS_REAR_RIGHT]);
}

/**
 * <summary> Gets the vehicle's windows states. </summary>
 * <param name="vehicleid"> Vehicle from which we get the vehicle plate.</param>
 * <param name="&driver"> Driver-window state to get.</param>
 * <param name="&passenger"> Passenger-window state to get.</param>
 * <param name="&backleft"> Backleft-window state to get.</param>
 * <param name="&backright"> Backright-window state to get.</param>
*/

stock Vehicle_GetWindows(vehicleid, &e_WINDOWS_STATES: driver, &e_WINDOWS_STATES: passenger, &e_WINDOWS_STATES: backleft, &e_WINDOWS_STATES: backright) 
{
    GetVehicleParamsCarWindows(vehicleid, Vehicle_gsWindows[vehicleid][WINDOWS_DRIVER], Vehicle_gsWindows[vehicleid][WINDOWS_PASSENGER], Vehicle_gsWindows[vehicleid][WINDOWS_REAR_LEFT], Vehicle_gsWindows[vehicleid][WINDOWS_REAR_RIGHT]);
    driver = Vehicle_gsWindows[vehicleid][WINDOWS_DRIVER];
    passenger = Vehicle_gsWindows[vehicleid][WINDOWS_PASSENGER];
    backleft = Vehicle_gsWindows[vehicleid][WINDOWS_REAR_LEFT];
    backright = Vehicle_gsWindows[vehicleid][WINDOWS_REAR_RIGHT];
}

// --
// Panels 
// --

/* Note: It seems that you can only read the value of the windshield. 
        Setting it does update the value on the server, 
        but it does not result 
        in any physical change on the vehicle.
*/

enum e_PANEL_STATES 
{
    E_PANEL_UNDAMAGED = 0,
    E_PANEL_CRUSHED = 1,
    E_PANEL_HANGING_LOOSE = 2,
    E_PANEL_REMOVED = 3
};

//--

static stock vehicle_DecodePanels(panels, &front_left_panel, &front_right_panel, &rear_left_panel, &rear_right_panel, &windshield, &front_bumper, &rear_bumper)
{
    front_left_panel = panels & 15;
    front_right_panel = panels >> 4 & 15;
    rear_left_panel = panels >> 8 & 15;
    rear_right_panel = panels >> 12 & 15;
    windshield = panels >> 16 & 15;
    front_bumper = panels >> 20 & 15;
    rear_bumper = panels >> 24 & 15;
}

static stock vehicle_EncodePanels(front_left_panel, front_right_panel, rear_left_panel, rear_right_panel, windshield, front_bumper, rear_bumper)
{
    return front_left_panel | (front_right_panel << 4) | (rear_left_panel << 8) | (rear_right_panel << 12) | (windshield << 16) | (front_bumper << 20) | (rear_bumper << 24);
}

//--

/**
 * <summary> Sets the vehicle's panels' states. </summary>
 * <param name="vehicleid"> Id of vehicle we are setting panel states to.</param>
 * <param name="front_left_panel"> Front left panel state to set.</param>
 * <param name="front_right_panel"> Front right panel state to set.</param>
 * <param name="back_left_panel"> Back left panel state to set.</param>
 * <param name="back_right_panel"> Back right state to set.</param>
*/

stock Vehicle_SetPanelStates(vehicleid, e_PANEL_STATES: front_left_panel, e_PANEL_STATES: front_right_panel, e_PANEL_STATES: back_left_panel, e_PANEL_STATES: back_right_panel) 
{
    new
        panels,
        unnecessary_lights,
        unnecessary_tires,
        unnecessary_doors,
        unnecessary_windshield,
        unnecessary_front_bumper,
        unnecessary_rear_bumper,
        unnecessary_decodes;

    GetVehicleDamageStatus(vehicleid, panels, unnecessary_doors, unnecessary_lights, unnecessary_tires);
    vehicle_DecodePanels(panels, unnecessary_decodes, unnecessary_decodes, unnecessary_decodes, unnecessary_decodes, unnecessary_windshield, unnecessary_front_bumper, unnecessary_rear_bumper);
    
    new ret = UpdateVehicleDamageStatus(vehicleid, vehicle_EncodePanels(front_left_panel, front_right_panel, back_left_panel, back_right_panel, unnecessary_windshield, unnecessary_front_bumper, unnecessary_rear_bumper), unnecessary_doors, unnecessary_lights, unnecessary_tires);
    return ret;
}

/**
 * <summary> Gets the vehicle's panels' states. </summary>
 * <param name="vehicleid"> Id of vehicle we are getting panel states from.</param>
 * <param name="&front_left_panel"> Front left panel state to get.</param>
 * <param name="&front_right_panel"> Front right panel state to get.</param>
 * <param name="&back_left_panel"> Back left panel state to get.</param>
 * <param name="&back_right_panel"> Back right state to get.</param>
*/

stock Vehicle_GetPanelStates(vehicleid, &e_PANEL_STATES: front_left_panel, &e_PANEL_STATES: front_right_panel, &e_PANEL_STATES: back_left_panel, &e_PANEL_STATES: back_right_panel) 
{
    new
        panels,
        unnecessary;

    GetVehicleDamageStatus(vehicleid, panels, unnecessary, unnecessary, unnecessary);
    vehicle_DecodePanels(panels, front_left_panel, front_right_panel, back_left_panel, back_right_panel, unnecessary, unnecessary, unnecessary);

}

/**
 * <summary> Sets the vehicle's bumpers' states. </summary>
 * <param name="vehicleid"> Id of vehicle we are setting bumper states to.</param>
 * <param name="front_left_panel"> Front bumper state to set.</param>
 * <param name="back_left_panel"> Rear bumper state to set.</param>
*/

stock Vehicle_SetBumperStates(vehicleid, e_PANEL_STATES: front_bumper, e_PANEL_STATES: rear_bumper) 
{
    new
        panels,
        unnecessary_front_left, 
        unnecessary_front_right, 
        unnecessary_back_left, 
        unnecessary_back_right,
        unnecessary_lights,
        unnecessary_tires,
        unnecessary_doors,
        unnecessary_decodes,
        unnecessary_windshield;

    GetVehicleDamageStatus(vehicleid, panels, unnecessary_doors, unnecessary_lights, unnecessary_tires);
    vehicle_DecodePanels(panels, unnecessary_front_left, unnecessary_front_right, unnecessary_back_left, unnecessary_back_right, unnecessary_windshield, unnecessary_decodes, unnecessary_decodes);
    
    new ret = UpdateVehicleDamageStatus(vehicleid, vehicle_EncodePanels(unnecessary_front_left, unnecessary_front_right, unnecessary_back_left, unnecessary_back_right, unnecessary_windshield, front_bumper, rear_bumper), unnecessary_doors, unnecessary_lights, unnecessary_tires);
    return ret;
}

/**
 * <summary> Sets the vehicle's bumpers states. </summary>
 * <param name="vehicleid"> Id of vehicle we are setting bumper states from.</param>
 * <param name="&front_left_panel"> Front bumper state to get.</param>
 * <param name="&back_left_panel"> Rear bumper state to get.</param>
*/


stock Vehicle_GetBumperStates(vehicleid, &e_PANEL_STATES: front_bumper, &e_PANEL_STATES: rear_bumper) 
{
    new
        panels,
        unnecessary;

    GetVehicleDamageStatus(vehicleid, panels, unnecessary, unnecessary, unnecessary);
    vehicle_DecodePanels(panels, unnecessary, unnecessary, unnecessary, unnecessary, unnecessary, front_bumper, rear_bumper);

}

/**
 * <summary> Gets the vehicle's windshield state. </summary>
 * <param name="vehicleid"> Id of vehicle we are getting winshield state from.</param>
 * <param name="&windshield"> Windshield state to get.</param>
*/

stock Vehicle_GetWindshieldState(vehicleid, &e_PANEL_STATES: windshield) 
{
    new
        panels,
        unnecessary;

    GetVehicleDamageStatus(vehicleid, panels, unnecessary, unnecessary, unnecessary);
    vehicle_DecodePanels(panels, unnecessary, unnecessary, unnecessary, unnecessary, windshield, unnecessary, unnecessary);

}

// --
// Colors
// --

// here no summary

stock Vehicle_SetColor(VEHICLE_FRAMEWORK_TAGS: vehicleid, color1, color2, tag=tagof(vehicleid))
{
    if(tag == tagof(_:))
    {
        Vehicle_gsColorInfo[vehicleid][0] = color1;
        Vehicle_gsColorInfo[vehicleid][1] = color2;
        
        ChangeVehicleColor(vehicleid, Vehicle_gsColorInfo[vehicleid][0], Vehicle_gsColorInfo[vehicleid][1]);
    }
    else if(tag == tagof(VehicleGroup:)) 
    {
        foreach(new i : Vehicle_gsGroups[VehicleGroup: vehicleid])
        {
            Vehicle_gsColorInfo[i][0] = color1;
            Vehicle_gsColorInfo[i][1] = color2;
            ChangeVehicleColor(i, Vehicle_gsColorInfo[i][0], Vehicle_gsColorInfo[i][1]);
        }
    }
}

stock Vehicle_GetColor(vehicleid, &color1, &color2) 
{
    #if !defined YSF_Enable
        color1 = Vehicle_gsColorInfo[vehicleid][0];
        color2 = Vehicle_gsColorInfo[vehicleid][1];
    #else
        GetVehicleColor(vehicleid, color1, color2);
    #endif
}

// --
// Dimensions
// --

stock Vehicle_SetInterior(vehicleid, interiorid) 
{
    Vehicle_gsDimensionInfo[vehicleid][DIMENSION_INTERIOR] = interiorid;
    LinkVehicleToInterior(vehicleid, Vehicle_gsDimensionInfo[vehicleid][DIMENSION_INTERIOR]);
}

stock Vehicle_GetInterior(vehicleid) 
{
    #if !defined YSF_Enable
        return Vehicle_gsDimensionInfo[vehicleid][DIMENSION_INTERIOR];
    #else
        GetVehicleInterior(vehicleid);
    #endif
}

stock Vehicle_SetVirtualWorld(vehicleid, virtual_world)
{
    Vehicle_gsDimensionInfo[vehicleid][DIMENSION_VIRTUAL_WORLD] = virtual_world;
    SetVehicleVirtualWorld(vehicleid, virtual_world);
}

stock Vehicle_GetVirtualWorld(vehicleid) 
{
    return Vehicle_gsDimensionInfo[vehicleid][DIMENSION_VIRTUAL_WORLD];
}

stock Vehicle_SetDimensionInfo(vehicleid, interiorid, virtual_world)
{
    Vehicle_gsDimensionInfo[vehicleid][DIMENSION_INTERIOR] = interiorid;
    Vehicle_gsDimensionInfo[vehicleid][DIMENSION_VIRTUAL_WORLD] = virtual_world;
    LinkVehicleToInterior(vehicleid, Vehicle_gsDimensionInfo[vehicleid][DIMENSION_INTERIOR]);
    SetVehicleVirtualWorld(vehicleid, Vehicle_gsDimensionInfo[vehicleid][DIMENSION_VIRTUAL_WORLD]);
}

stock Vehicle_GetDimensionInfo(vehicleid, &interiorid, &virtualworld) 
{
    if(GetVehicleVirtualWorld(vehicleid) != Vehicle_GetVirtualWorld(vehicleid)) 
    {
        return;
    }
    interiorid = Vehicle_GetInterior(vehicleid);
    virtualworld = Vehicle_GetVirtualWorld(vehicleid);
}

// --
// Paintjobs
// --

stock Vehicle_SetPaintjob(vehicleid, paintjobid)
{
    if(GetVehicleModel(vehicleid) == 483 && (paintjobid != 0 || paintjobid != 3)) 
    {
        return 0;
    }
    else if(GetVehicleModel(vehicleid) == 575 && (paintjobid != 0 || paintjobid != 1 || paintjobid != 3)) 
    {
        return 0;
    }
 
    for(new i = 0; i < sizeof(Vehicle_gscPaintjob); i++)
    {
        if(GetVehicleModel(vehicleid) != Vehicle_gscPaintjob[i])
        {
            continue;
        }
        Vehicle_gsPaintjob[vehicleid] = paintjobid;
        ChangeVehiclePaintjob(vehicleid, paintjobid);
        break;
    }
    return 1;
}

stock Vehicle_GetPaintjob(vehicleid)
{
    #if !defined YSF_Enable 
        return Vehicle_gsPaintjob[vehicleid];
    #else 
        return GetVehiclePaintjob(vehicleid);
    #endif
}

// --
// Occupied state
// --

stock Vehicle_IsOccupied(vehicleid)
{
    #if defined YSF_Enable
        return IsVehicleOccupied(vehicleid);
    #else
        if(Vehicle_gsIsOccupied[vehicleid][0] != -1 || Vehicle_gsIsOccupied[vehicleid][1] != -1 || Vehicle_gsIsOccupied[vehicleid][2] != -1 || Vehicle_gsIsOccupied[vehicleid][3] != -1)
        {
            return true;
        }
        return false;
    #endif
}

stock Vehicle_GetLastDriver(vehicleid)
{
    #if defined YSF_Enable
        return GetVehicleLastDriver(vehicleid);
    #else
        return Vehicle_gsLastDriver[vehicleid];
    #endif
}

// --
// Trailers
// --

stock Vehicle_AttachTrailer(vehicleid, trailerid)
{
    if(!IsValidVehicle(vehicleid) || !IsValidVehicle(trailerid) || trailerid == vehicleid)
    {
        return 0;
    }
    Vehicle_gsTrailer[vehicleid][0] = trailerid;
    Vehicle_gsTrailer[trailerid][1] = vehicleid;
    AttachTrailerToVehicle(trailerid, vehicleid);
    return 1;
}

stock Vehicle_DetachTrailer(vehicleid)
{
    new trailer = Vehicle_gsTrailer[vehicleid][0];
    Vehicle_gsTrailer[vehicleid][0] = 0;
    Vehicle_gsTrailer[trailer][1] = 0;
    DetachTrailerFromVehicle(vehicleid);
    return 1;
}

stock Vehicle_GetTrailer(vehicleid)
{
    if(GetVehicleTrailer(vehicleid) != Vehicle_gsTrailer[vehicleid][0]) 
    {
        return -1;
    }
    return Vehicle_gsTrailer[vehicleid][0];
}

stock Vehicle_GetTrailerCab(trailerid)
{
    return Vehicle_gsTrailer[trailerid][1];
}


// --
// Time functions
// --

#if defined YSF_Enable
    stock Vehicle_SetRespawnDelay(vehicleid, respawn_delay)
    {
        Vehicle_gsRespawnDelay[vehicleid] = respawn_delay;
        SetVehicleRespawnDelay(vehicleid, respawn_delay);
    }
#endif

stock Vehicle_GetRespawnDelay(vehicleid)
{
    #if defined YSF_Enable
        return GetVehicleRespawnDelay(vehicleid);
    #else
        return Vehicle_gsRespawnDelay[vehicleid];
    #endif
}

stock Vehicle_GetOccupiedTime(vehicleid)
{
    if(Vehicle_IsOccupied(vehicleid))
    {
        return ((gettime() - Vehicle_gsOccupiedTick[vehicleid]) * 1000);
    }
    return -1;
}

stock Vehicle_GetRespawnedTime(vehicleid)
{
    return ((gettime() - Vehicle_gsRespawnTick[vehicleid]) * 1000);
}

// --
// Sirens 
// --

stock Vehicle_GetSirenEnable(vehicleid)
{
    return Vehicle_gsSirenState[vehicleid];
}

// --
// Hooks
// --

// By default, all vehicles have engines. 
// Bypass: Hook a callback to check if player is riding a manual vehicle (pedal power)
// and then turn engines on
// When the player have exited vehicle turn offV

hook OnPlayerStateChange(playerid, newstate, oldstate) 
{
    static seat;
    if(oldstate == PLAYER_STATE_ONFOOT && (newstate == PLAYER_STATE_DRIVER || newstate == PLAYER_STATE_PASSENGER))
    {
        if(Vehicle_IsManual(GetPlayerVehicleID(playerid))) 
        {
            Vehicle_SetEngineState(GetPlayerVehicleID(playerid), E_ENGINE_STATE_ON);
        } 
        Player_LastVehicleID[playerid] = GetPlayerVehicleID(playerid);
        seat = GetPlayerVehicleSeat(playerid);
        if(seat == 0) 
        {
            Vehicle_gsLastDriver[Player_LastVehicleID[playerid]] = playerid;
        }
        Vehicle_gsIsOccupied[Player_LastVehicleID[playerid]][seat] = playerid;
        Vehicle_gsOccupiedTick[Player_LastVehicleID[playerid]] = gettime();
    }
    else if((oldstate == PLAYER_STATE_DRIVER || oldstate == PLAYER_STATE_PASSENGER) && newstate == PLAYER_STATE_ONFOOT) 
    {
        if(Vehicle_IsManual(Player_LastVehicleID[playerid]))
        {
            Vehicle_SetEngineState(Player_LastVehicleID[playerid], E_ENGINE_STATE_OFF);
        }
        Vehicle_gsIsOccupied[Player_LastVehicleID[playerid]][seat] = -1;
        Vehicle_gsOccupiedTick[Player_LastVehicleID[playerid]] = -1;
    }
    else if((oldstate == PLAYER_STATE_DRIVER || oldstate == PLAYER_STATE_PASSENGER) && newstate == PLAYER_STATE_WASTED)
    {
        Vehicle_gsIsOccupied[Player_LastVehicleID[playerid]][seat] = -1; 
        Vehicle_gsOccupiedTick[Player_LastVehicleID[playerid]] = -1;
    }
    return Y_HOOKS_CONTINUE_RETURN_1;
}

hook OnVehicleSpawn(vehicleid)
{
    Vehicle_gsParams[vehicleid][PARAMS_ENGINE] = E_ENGINE_STATE_OFF;
    Vehicle_gsParams[vehicleid][PARAMS_LIGHTS] = E_LIGHTS_OFF;
    Vehicle_gsParams[vehicleid][PARAMS_DOORS] = E_DOOR_STATE_OFF;
    Vehicle_gsParams[vehicleid][PARAMS_ALARM] = E_ALARMS_OFF;
    Vehicle_gsParams[vehicleid][PARAMS_BOOT] = E_BOOT_CLOSED;
    Vehicle_gsParams[vehicleid][PARAMS_BONNET] = E_BONNET_CLOSED;
    Vehicle_gsParams[vehicleid][PARAMS_OBJECTIVE] = E_OBJECTIVE_OFF;

    Vehicle_gsWindows[vehicleid][WINDOWS_DRIVER] = E_WINDOW_NOT_SET;
    Vehicle_gsWindows[vehicleid][WINDOWS_PASSENGER] = E_WINDOW_NOT_SET;
    Vehicle_gsWindows[vehicleid][WINDOWS_REAR_LEFT] = E_WINDOW_NOT_SET;
    Vehicle_gsWindows[vehicleid][WINDOWS_REAR_RIGHT] = E_WINDOW_NOT_SET;

    Vehicle_gsRespawnTick[vehicleid] = gettime();

    if(Vehicle_IsOccupied(vehicleid))
    {
        Vehicle_gsIsOccupied[vehicleid][0] = -1;
        Vehicle_gsIsOccupied[vehicleid][1] = -1;
        Vehicle_gsIsOccupied[vehicleid][2] = -1;
        Vehicle_gsIsOccupied[vehicleid][3] = -1;
    }
    Vehicle_gsOccupiedTick[vehicleid] = -1;
    return 1;
}

hook OnPlayerDisconnect(playerid, reason)
{
    if(GetPlayerState(playerid) == PLAYER_STATE_PASSENGER || GetPlayerState(playerid) == PLAYER_STATE_DRIVER)
    {
        Vehicle_gsIsOccupied[Player_LastVehicleID[playerid]][GetPlayerVehicleSeat(playerid)] = -1;
        Player_LastVehicleID[playerid] = -1;
    }
    return 1;
}