#if defined _VEHICLE_PLUS_included
	#endinput
#endif
#define _VEHICLE_PLUS_included

#include <open.mp>

#include <YSI_Data\y_foreach>
#include <YSI_Coding\y_timers>
#include <YSI_Coding\y_hooks>
#include <YSI_Game\y_vehicledata>

#include <streamer>

/*
    Since I don't know how to fix the tag mismatch warning,
    where it happens on every line which performs tag checking,
    for whether it is VehicleGroup: or _: a.k.a. no tag. Including 
    indirection before all YSI includes would literally "fix" all warning
    including those which it shouldn't, or even sometimes, nothing would happen.
    Warning is now disabled for this file.
    Including a file after this library won't be affected by this.
    If you encounter any bugs, I please you to open an issue on gihub, also
    not to forget, documentation and wiki will be updated every new release.
    > https://github.com/Mergevos/open-vehicle-plus
*/

#pragma warning push
#pragma warning disable 213

/**
 * <library>omp_vehicle</library>
 * <summary>Check if a vehicle ID is valid.</summary>
 * <param name="vehicleid">The ID of the vehicle to check</param>
 * <seealso name="CreateVehicle" />
 * <seealso name="DestroyVehicle" />
 * <returns>true or false.</returns>
 */
forward bool: IsPlayerInsidePaySpray(playerid);

#if !defined RELEASED
	#define RELEASED(%0)			(((newkeys & (%0)) != (%0)) && ((oldkeys & (%0)) == (%0)))
#endif

#if defined EMPTY_DAMAGE 
    #include <weapon-config>
#endif

#define VEHICLE_FRAMEWORK_TAGS \
    {_, VehicleGroup}

#if !defined isnull
    #define isnull(%1) ((!(%1[0])) || (((%1[0]) == '\1') && (!(%1[1]))))
#endif

native _Framework_CallLocalFunction(const function[], const format[], {VehicleGroup, Float,_}:...) = CallLocalFunction;


// -- 
// Script-Side
// --

#define MAX_VEHICLES_COMPONENTS         194
#define MAX_VEHICLE_COMPONENT_NAME      64
#define MAX_VEHICLES_MODELS             212
#define MAX_VEHICLE_COMPONENTS          48
#define INVALID_VEHICLE_COMPONENT_ID    -1

#define BLINK_SIDES 4

static
    Vehicle_gsOccupiedTick[MAX_VEHICLES],
    Vehicle_gsRespawnTick[MAX_VEHICLES],

    Vehicle_gsBlink[MAX_VEHICLES][BLINK_SIDES],

    Timer: Vehicle_gsDistanceTimer[MAX_VEHICLES],

    Float: Vehicle_gsPositionX[MAX_VEHICLES],
    Float: Vehicle_gsPositionY[MAX_VEHICLES],
    Float: Vehicle_gsPositionZ[MAX_VEHICLES],

    Float: Vehicle_gsHealth[MAX_VEHICLES]; 


#if !defined MAX_NUMBER_PLATE
    #define MAX_NUMBER_PLATE 32
#endif

#if !defined MAX_VEHICLE_GROUPS
    #define MAX_VEHICLE_GROUPS 12
#endif

static const Float:Vehicle_gscPayNSprayLocations[10][3] = {
    {1025.05, -1024.23, 32.1}, 	// LS Temple
    {487.68, -1740.87, 11.13}, 	// LS Santa Maria
    {-1420.73, 2583.37, 55.56}, // El Quebrados
    {-1904.39, 284.97, 40.75}, 	// Wang Cars
    {-2425.91, 1022.33, 50.10}, // Juniper Hill
    {1975.60, 2162.16, 10.77}, 	// LV Redsands
    {2065.38, -1831.51, 13.25}, // Idlewood
    {-99.55, 1118.36, 19.44}, 	// Fort Carson
    {721.07, -455.94, 16.04}, 	// Dillimore
    {2393.74, 1493.01, 10.52} 	// LV Unused (Pyramid)
};

// --
// Vehicle Info
// --

#define DRIVE_TYPE: __TAG(DRIVE_TYPE):
enum DRIVE_TYPE:__DRIVE_TYPE
{
    DRIVE_INVALID = 0,
    DRIVE_4WD = 1,
    DRIVE_FWD = 2,
    DRIVE_RWD
}
static stock DRIVE_TYPE:_@DRIVE_TYPE() { return __DRIVE_TYPE; }

#define ENGINE_TYPE: __TAG(ENGINE_TYPE):
enum ENGINE_TYPE:__ENGINE_TYPE
{
    ENGINE_INVALID = 0,
    ENGINE_PETROL = 1,
    ENGINE_DIESEL = 2,
    ENGINE_ELECTRIC
}
static stock ENGINE_TYPE:_@ENGINE_TYPE() { return __ENGINE_TYPE; }

static enum e_VEHICLE_INFO
{
	E_VEHICLE_WINDOWS_NUMBER,
	Float:E_VEHICLE_MASS,
	DRIVE_TYPE: E_VEHICLE_DRIVE_TYPE,
	ENGINE_TYPE: E_VEHICLE_ENGINE_TYPE,
    E_VEHICLE_LIGHTS_NUMBER // rears are considered as one, while front ones are separated, bikes have 2 lights and 4-light vehs have 3
};

static const Vehicle_Info[212][e_VEHICLE_INFO] = 
{
    {4, 1700.0, DRIVE_4WD, ENGINE_DIESEL, 3},
    {2, 1300.0, DRIVE_FWD, ENGINE_PETROL, 3},
    {2, 1500.0, DRIVE_RWD, ENGINE_PETROL, 3},
    {2, 3800.0, DRIVE_RWD, ENGINE_DIESEL, 3},
    {4, 1200.0, DRIVE_FWD, ENGINE_PETROL, 3},
    {4, 1600.0, DRIVE_RWD, ENGINE_PETROL, 3},
    {1, 20000.0, DRIVE_RWD, ENGINE_DIESEL, 3},
    {2, 6500.0, DRIVE_RWD, ENGINE_DIESEL, 3},
    {2, 5500.0, DRIVE_RWD, ENGINE_DIESEL, 3},
    {4, 2200.0, DRIVE_RWD, ENGINE_PETROL, 3},
    {2, 1000.0, DRIVE_FWD, ENGINE_PETROL, 3},
    {2, 1400.0, DRIVE_4WD, ENGINE_PETROL, 3},
    {2, 1800.0, DRIVE_RWD, ENGINE_PETROL, 3},
    {4, 2600.0, DRIVE_RWD, ENGINE_DIESEL, 3},
    {2, 3500.0, DRIVE_RWD, ENGINE_DIESEL, 3},
    {2, 1200.0, DRIVE_RWD, ENGINE_PETROL, 3},
    {4, 2600.0, DRIVE_4WD, ENGINE_DIESEL, 3},
    {2, 15000.0, DRIVE_4WD, ENGINE_PETROL, 0}, 
    {4, 2000.0, DRIVE_RWD, ENGINE_DIESEL, 3},
    {2, 1800.0, DRIVE_RWD, ENGINE_PETROL, 3},
    {4, 1450.0, DRIVE_FWD, ENGINE_PETROL, 3},
    {2, 1850.0, DRIVE_RWD, ENGINE_PETROL, 3},
    {2, 1700.0, DRIVE_4WD, ENGINE_DIESEL, 3},
    {2, 1700.0, DRIVE_RWD, ENGINE_DIESEL, 3},
    {0, 1200.0, DRIVE_RWD, ENGINE_PETROL, 3},
    {1, 10000.0, DRIVE_4WD, ENGINE_PETROL, 0}, 
    {4, 1600.0, DRIVE_RWD, ENGINE_PETROL, 3},
    {4, 4000.0, DRIVE_RWD, ENGINE_DIESEL, 3},
    {2, 7000.0, DRIVE_RWD, ENGINE_DIESEL, 3},
    {2, 1400.0, DRIVE_RWD, ENGINE_PETROL, 3},
    {0, 2200.0, DRIVE_RWD, ENGINE_PETROL, 0}, 
    {2, 5500.0, DRIVE_RWD, ENGINE_DIESEL, 3},
    {0, 25000.0, DRIVE_4WD, ENGINE_DIESEL, 0},
    {2, 10500.0, DRIVE_4WD, ENGINE_DIESEL, 3},
    {2, 1400.0, DRIVE_RWD, ENGINE_PETROL, 3},
    {0, 3800.0, DRIVE_RWD, ENGINE_DIESEL, 1},
    {2, 1400.0, DRIVE_FWD, ENGINE_PETROL, 3},
    {0, 9500.0, DRIVE_RWD, ENGINE_DIESEL, 3},
    {4, 1750.0, DRIVE_RWD, ENGINE_PETROL, 3},
    {2, 1600.0, DRIVE_RWD, ENGINE_PETROL, 3},
    {4, 2000.0, DRIVE_FWD, ENGINE_PETROL, 3},
    {0, 100.0, DRIVE_4WD, ENGINE_ELECTRIC, 0},
    {2, 2500.0, DRIVE_RWD, ENGINE_PETROL, 3},
    {2, 8000.0, DRIVE_RWD, ENGINE_DIESEL, 3},
    {2, 5000.0, DRIVE_4WD, ENGINE_PETROL, 3},
    {4, 1650.0, DRIVE_FWD, ENGINE_PETROL, 3},
    {0, 2200.0, DRIVE_RWD, ENGINE_PETROL, 0}, 
    {2, 3000.0, DRIVE_4WD, ENGINE_PETROL, 0}, 
    {0, 350.0, DRIVE_RWD, ENGINE_PETROL, 2}, 
    {0, 1900.0, DRIVE_RWD, ENGINE_PETROL, 3}, 
    {0, 3800.0, DRIVE_RWD, ENGINE_DIESEL, 1},
    {2, 1400.0, DRIVE_4WD, ENGINE_PETROL, 3},
    {0, 2200.0, DRIVE_RWD, ENGINE_PETROL, 0}, 
    {0, 5000.0, DRIVE_RWD, ENGINE_PETROL, 0},
    {0, 2200.0, DRIVE_RWD, ENGINE_PETROL, 0}, 
    {2, 8500.0, DRIVE_RWD, ENGINE_DIESEL, 3},
    {2, 4500.0, DRIVE_RWD, ENGINE_DIESEL, 3},
    {0, 1000.0, DRIVE_4WD, ENGINE_ELECTRIC, 3},
    {4, 2000.0, DRIVE_RWD, ENGINE_PETROL, 3},
    {4, 1900.0, DRIVE_RWD, ENGINE_DIESEL, 3},
    {2, 5000.0, DRIVE_4WD, ENGINE_PETROL, 0},
    {0, 500.0, DRIVE_RWD, ENGINE_PETROL, 2},
    {0, 350.0, DRIVE_RWD, ENGINE_PETROL, 2},
    {0, 800.0, DRIVE_RWD, ENGINE_PETROL, 2},
    {0, 100.0, DRIVE_FWD, ENGINE_PETROL, 0},
    {0, 100.0, DRIVE_4WD, ENGINE_PETROL, 0},
    {4, 1600.0, DRIVE_RWD, ENGINE_PETROL, 3},
    {4, 1900.0, DRIVE_RWD, ENGINE_PETROL, 3},
    {0, 500.0, DRIVE_RWD, ENGINE_PETROL, 2},
    {2, 2500.0, DRIVE_4WD, ENGINE_PETROL, 0},
    {4, 2500.0, DRIVE_4WD, ENGINE_PETROL, 3},
    {0, 400.0, DRIVE_4WD, ENGINE_PETROL, 2},
    {0, 1200.0, DRIVE_RWD, ENGINE_PETROL, 0},
    {0, 800.0, DRIVE_RWD, ENGINE_PETROL, 0},
    {2, 1950.0, DRIVE_FWD, ENGINE_PETROL, 3},
    {2, 1700.0, DRIVE_RWD, ENGINE_PETROL, 3},
    {1, 5000.0, DRIVE_4WD, ENGINE_PETROL, 0},
    {2, 1400.0, DRIVE_RWD, ENGINE_PETROL, 3},
    {2, 1850.0, DRIVE_4WD, ENGINE_DIESEL, 3},
    {4, 1500.0, DRIVE_FWD, ENGINE_PETROL, 3},
    {2, 1400.0, DRIVE_4WD, ENGINE_PETROL, 3},
    {0, 100.0, DRIVE_RWD, ENGINE_PETROL, 0},
    {4, 1900.0, DRIVE_RWD, ENGINE_PETROL, 3},
    {3, 1900.0, DRIVE_RWD, ENGINE_PETROL, 3},
    {0, 5000.0, DRIVE_RWD, ENGINE_PETROL, 0},
    {0, 1000.0, DRIVE_RWD, ENGINE_ELECTRIC, 3},
    {0, 10000.0, DRIVE_4WD, ENGINE_DIESEL, 3},
    {4, 5000.0, DRIVE_4WD, ENGINE_PETROL, 0}, 
    {2, 3500.0, DRIVE_4WD, ENGINE_PETROL, 0}, 
    {2, 2500.0, DRIVE_4WD, ENGINE_PETROL, 3},
    {4, 3500.0, DRIVE_4WD, ENGINE_PETROL, 3},
    {2, 1700.0, DRIVE_RWD, ENGINE_PETROL, 3},
    {4, 1600.0, DRIVE_RWD, ENGINE_PETROL, 3},
    {0, 3000.0, DRIVE_RWD, ENGINE_PETROL, 0}, 
    {2, 1600.0, DRIVE_RWD, ENGINE_PETROL, 3},
    {2, 2000.0, DRIVE_4WD, ENGINE_PETROL, 3},
    {2, 1000.0, DRIVE_FWD, ENGINE_PETROL, 3},
    {4, 4500.0, DRIVE_4WD, ENGINE_PETROL, 0}, 
    {2, 5500.0, DRIVE_RWD, ENGINE_DIESEL, 3},
    {2, 3500.0, DRIVE_RWD, ENGINE_DIESEL, 3},
    {2, 1300.0, DRIVE_4WD, ENGINE_DIESEL, 3},
    {0, 100.0, DRIVE_4WD, ENGINE_PETROL, 0},
    {2, 1600.0, DRIVE_RWD, ENGINE_PETROL, 3},
    {2, 1600.0, DRIVE_RWD, ENGINE_PETROL, 3},
    {2, 2100.0, DRIVE_RWD, ENGINE_PETROL, 3},
    {2, 2500.0, DRIVE_4WD, ENGINE_PETROL, 3},
    {2, 1400.0, DRIVE_RWD, ENGINE_PETROL, 3},
    {4, 2200.0, DRIVE_RWD, ENGINE_PETROL, 3},
    {3, 3500.0, DRIVE_RWD, ENGINE_DIESEL, 3},
    {0, 100.0, DRIVE_RWD, ENGINE_PETROL, 0}, 
    {0, 100.0, DRIVE_RWD, ENGINE_PETROL, 0}, 
    {2, 10000.0, DRIVE_4WD, ENGINE_PETROL, 0}, 
    {1, 5000.0, DRIVE_4WD, ENGINE_PETROL, 0}, 
    {1, 5000.0, DRIVE_4WD, ENGINE_PETROL, 0}, 
    {2, 3800.0, DRIVE_RWD, ENGINE_DIESEL, 3},
    {2, 5000.0, DRIVE_RWD, ENGINE_DIESEL, 3},
    {4, 1400.0, DRIVE_FWD, ENGINE_PETROL, 3},
    {2, 1400.0, DRIVE_RWD, ENGINE_PETROL, 3},
    {2, 1700.0, DRIVE_RWD, ENGINE_PETROL, 3},
    {0, 15000.0, DRIVE_4WD, ENGINE_PETROL, 0}, 
    {1, 9000.0, DRIVE_4WD, ENGINE_PETROL, 0}, 
    {0, 500.0, DRIVE_RWD, ENGINE_PETROL, 2}, 
    {0, 400.0, DRIVE_RWD, ENGINE_PETROL, 2}, 
    {0, 500.0, DRIVE_RWD, ENGINE_PETROL, 2}, 
    {2, 5500.0, DRIVE_RWD, ENGINE_DIESEL, 3},
    {2, 3500.0, DRIVE_RWD, ENGINE_DIESEL, 3},
    {2, 1700.0, DRIVE_RWD, ENGINE_PETROL, 3},
    {2, 1200.0, DRIVE_RWD, ENGINE_PETROL, 3},
    {2, 4000.0, DRIVE_4WD, ENGINE_DIESEL, 3},
    {4, 1800.0, DRIVE_RWD, ENGINE_PETROL, 3},
    {0, 1000.0, DRIVE_FWD, ENGINE_ELECTRIC, 3},
    {0, 2000.0, DRIVE_RWD, ENGINE_DIESEL, 3},
    {1, 8500.0, DRIVE_4WD, ENGINE_DIESEL, 3},
    {2, 1600.0, DRIVE_RWD, ENGINE_PETROL, 3},
    {2, 1800.0, DRIVE_RWD, ENGINE_PETROL, 3},
    {2, 1950.0, DRIVE_RWD, ENGINE_PETROL, 3},
    {2, 1500.0, DRIVE_RWD, ENGINE_PETROL, 3},
    {0, 5500.0, DRIVE_RWD, ENGINE_DIESEL, 3},
    {0, 5500.0, DRIVE_RWD, ENGINE_DIESEL, 3},
    {0, 1900.0, DRIVE_RWD, ENGINE_PETROL, 0},
    {4, 1800.0, DRIVE_FWD, ENGINE_PETROL, 3},
    {2, 1200.0, DRIVE_RWD, ENGINE_PETROL, 3},
    {2, 1600.0, DRIVE_RWD, ENGINE_PETROL, 3},
    {2, 1700.0, DRIVE_4WD, ENGINE_DIESEL, 3},
    {2, 6500.0, DRIVE_RWD, ENGINE_DIESEL, 3},
    {2, 1700.0, DRIVE_RWD, ENGINE_PETROL, 3},
    {4, 1800.0, DRIVE_RWD, ENGINE_PETROL, 3},
    {4, 1600.0, DRIVE_RWD, ENGINE_PETROL, 3},
    {2, 20000.0, DRIVE_4WD, ENGINE_PETROL, 0}, 
    {2, 1700.0, DRIVE_RWD, ENGINE_PETROL, 3},
    {4, 1600.0, DRIVE_FWD, ENGINE_PETROL, 3},
    {4, 1800.0, DRIVE_RWD, ENGINE_PETROL, 3},
    {2, 2600.0, DRIVE_RWD, ENGINE_DIESEL, 3},
    {0, 25000.0, DRIVE_4WD, ENGINE_PETROL, 0}, 
    {2, 3000.0, DRIVE_RWD, ENGINE_PETROL, 3},
    {2, 1500.0, DRIVE_RWD, ENGINE_PETROL, 3},
    {2, 5000.0, DRIVE_4WD, ENGINE_PETROL, 3},
    {2, 5000.0, DRIVE_4WD, ENGINE_PETROL, 3},
    {2, 1400.0, DRIVE_RWD, ENGINE_PETROL, 3},
    {2, 1500.0, DRIVE_FWD, ENGINE_PETROL, 3},
    {4, 1400.0, DRIVE_4WD, ENGINE_PETROL, 3},
    {4, 1800.0, DRIVE_RWD, ENGINE_PETROL, 3},
    {2, 1500.0, DRIVE_RWD, ENGINE_PETROL, 3},
    {2, 10000.0, DRIVE_4WD, ENGINE_PETROL, 0}, 
    {0, 100.0, DRIVE_4WD, ENGINE_ELECTRIC, 0},
    {2, 1400.0, DRIVE_FWD, ENGINE_PETROL, 3},
    {4, 1800.0, DRIVE_RWD, ENGINE_PETROL, 3},
    {2, 1500.0, DRIVE_RWD, ENGINE_PETROL, 3},
    {0, 1000.0, DRIVE_RWD, ENGINE_PETROL, 3},
    {0, 5500.0, DRIVE_RWD, ENGINE_DIESEL, 0},
    {0, 5500.0, DRIVE_RWD, ENGINE_DIESEL, 0},
    {0, 300.0, DRIVE_RWD, ENGINE_PETROL, 0},
    {0, 800.0, DRIVE_RWD, ENGINE_PETROL, 0},
    {2, 10000.0, DRIVE_4WD, ENGINE_PETROL, 3},
    {2, 800.0, DRIVE_RWD, ENGINE_PETROL, 3},
    {2, 1700.0, DRIVE_RWD, ENGINE_PETROL, 3},
    {2, 1700.0, DRIVE_RWD, ENGINE_PETROL, 3},
    {0, 60000.0, DRIVE_4WD, ENGINE_PETROL, 0}, 
    {2, 5500.0, DRIVE_RWD, ENGINE_DIESEL, 3},
    {4, 2500.0, DRIVE_4WD, ENGINE_PETROL, 3},
    {4, 2200.0, DRIVE_RWD, ENGINE_PETROL, 3},
    {0, 500.0, DRIVE_RWD, ENGINE_PETROL, 2}, 
    {3, 1900.0, DRIVE_RWD, ENGINE_DIESEL, 3},
    {2, 800.0, DRIVE_RWD, ENGINE_PETROL, 3},
    {0, 3800.0, DRIVE_RWD, ENGINE_DIESEL, 0},
    {4, 1800.0, DRIVE_RWD, ENGINE_PETROL, 3},
    {0, 800.0, DRIVE_RWD, ENGINE_PETROL, 2}, 
    {2, 1400.0, DRIVE_4WD, ENGINE_PETROL, 3},
    {2, 5500.0, DRIVE_RWD, ENGINE_DIESEL, 3},
    {2, 1400.0, DRIVE_FWD, ENGINE_PETROL, 3},
    {0, 5500.0, DRIVE_RWD, ENGINE_DIESEL, 0},
    {0, 3800.0, DRIVE_RWD, ENGINE_DIESEL, 1},
    {0, 40000.0, DRIVE_4WD, ENGINE_PETROL, 0}, 
    {2, 5000.0, DRIVE_4WD, ENGINE_PETROL, 0}, 
    {0, 100.0, DRIVE_4WD, ENGINE_ELECTRIC, 0},
    {0, 2200.0, DRIVE_RWD, ENGINE_PETROL, 0}, 
    {4, 1600.0, DRIVE_RWD, ENGINE_PETROL, 3},
    {4, 1600.0, DRIVE_RWD, ENGINE_PETROL, 3},
    {4, 1600.0, DRIVE_RWD, ENGINE_PETROL, 3},
    {2, 2500.0, DRIVE_4WD, ENGINE_DIESEL, 3},
    {2, 1600.0, DRIVE_RWD, ENGINE_DIESEL, 3},
    {0, 5000.0, DRIVE_4WD, ENGINE_DIESEL, 3},
    {2, 1500.0, DRIVE_RWD, ENGINE_PETROL, 3},
    {2, 1500.0, DRIVE_RWD, ENGINE_PETROL, 3},
    {4, 1600.0, DRIVE_RWD, ENGINE_DIESEL, 3},
    {2, 1700.0, DRIVE_4WD, ENGINE_DIESEL, 3},
    {0, 1000.0, DRIVE_RWD, ENGINE_ELECTRIC, 1},
    {0, 1000.0, DRIVE_RWD, ENGINE_ELECTRIC, 1},
    {0, 1000.0, DRIVE_RWD, ENGINE_ELECTRIC, 0},
    {2, 5500.0, DRIVE_RWD, ENGINE_DIESEL, 3},
    {0, 400.0, DRIVE_RWD, ENGINE_ELECTRIC, 0},
    {0, 1000.0, DRIVE_RWD, ENGINE_ELECTRIC, 1}
};

/**
 * <summary> Gets vehicle mass</summary>
 */
stock Float: Vehicle_GetMass(vehicleid) 
{
    return Vehicle_Info[GetVehicleModel(vehicleid) - 400][E_VEHICLE_MASS];
}

// --
// Params
// --

static enum PARAMS
{
    VEHICLE_PARAMS: PARAMS_ENGINE,
    VEHICLE_PARAMS: PARAMS_LIGHTS,
    VEHICLE_PARAMS: PARAMS_ALARM, 
    VEHICLE_PARAMS: PARAMS_DOORS,
    VEHICLE_PARAMS: PARAMS_BONNET,
    VEHICLE_PARAMS: PARAMS_BOOT,
    VEHICLE_PARAMS: PARAMS_OBJECTIVE
}

static
    VEHICLE_PARAMS: Vehicle_gsParams[MAX_VEHICLES][PARAMS];

/**
 * <library>vehicle_plus</library>
 * <summary>Called when vehicle has been created.</summary>
 * <param name="vehicleid">The ID of created vehicle.</param>
 * <seealso name="Vehicle_Create" />
 * <seealso name="Vehicle_CreateEx" />
 * <seealso name="Vehicle_Destroy" />
 * <seealso name="OnVehicleDestroy" />
 * <returns>Doesn't return any specific value.</returns>
 */
forward OnVehicleFirstSpawn(vehicleid);

/**
 * <library>vehicle_plus</library>
 * <summary>Called when vehicle has been destroyed.</summary>
 * <param name="vehicleid">The ID of destroyed vehicle.</param>
 * <seealso name="Vehicle_Destroy" />
 * <seealso name="Vehicle_Create" />
 * <seealso name="Vehicle_CreateEx" />
 * <seealso name="OnVehicleFirstSpawn" />
 * <returns>Doesn't return any specific value.</returns>
 */
forward OnVehicleDestroy(vehicleid);

// --
// Groups 
// --

/**
 * <library>vehicle_plus</library>
 * <summary>Called when group has been initialized.</summary>
 * <param name="VehicleGroup: groupid">The ID of initialized group.</param>
 * <seealso name="Vehicle_GetGroupByName" />
 * <seealso name="Vehicle_GroupHas" />
 * <seealso name="Vehicle_GroupInit" />
 * <seealso name="Vehicle_IsValidGroup" />
 * <seealso name="Vehicle_AddToGroup" />
 * <seealso name="Vehicle_RemoveFromGroup" />
 * <seealso name="Vehicle_IsEmptyGroup" />
 * <seealso name="Vehicle_GroupDestroy" />
 * <seealso name="Vehicle_ClearGroup" />
 * <seealso name="OnGroupAdd" />
 * <seealso name="OnGroupRemoveMember" />
 * <seealso name="OnGroupClear" />
 * <returns>Doesn't return any specific value.</returns>
 */
forward OnGroupInitialize(VehicleGroup: groupid);

/**
 * <library>vehicle_plus</library>
 * <summary>Called when group has a member added.</summary>
 * <param name="VehicleGroup: groupid">The ID of group that has member added.</param>
 * <param name="vehicleid">The ID vehicle that has been added.</param>
 * <seealso name="Vehicle_GetGroupByName" />
 * <seealso name="Vehicle_GroupHas" />
 * <seealso name="Vehicle_GroupInit" />
 * <seealso name="Vehicle_IsValidGroup" />
 * <seealso name="Vehicle_AddToGroup" />
 * <seealso name="Vehicle_RemoveFromGroup" />
 * <seealso name="Vehicle_IsEmptyGroup" />
 * <seealso name="Vehicle_GroupDestroy" />
 * <seealso name="Vehicle_ClearGroup" />
 * <seealso name="OnGroupInitialize" />
 * <seealso name="OnGroupRemoveMember" />
 * <seealso name="OnGroupClear" />
 * <returns>Doesn't return any specific value.</returns>
 */
forward OnGroupAdd(VehicleGroup: groupid, vehicleid);

/**
 * <library>vehicle_plus</library>
 * <summary>Called when group has a member removed.</summary>
 * <param name="VehicleGroup: groupid">The ID of group that has member removed.</param>
 * <param name="vehicleid">The ID vehicle that has been removed.</param>
 * <seealso name="Vehicle_GetGroupByName" />
 * <seealso name="Vehicle_GroupHas" />
 * <seealso name="Vehicle_GroupInit" />
 * <seealso name="Vehicle_IsValidGroup" />
 * <seealso name="Vehicle_AddToGroup" />
 * <seealso name="Vehicle_RemoveFromGroup" />
 * <seealso name="Vehicle_IsEmptyGroup" />
 * <seealso name="Vehicle_GroupDestroy" />
 * <seealso name="Vehicle_ClearGroup" />
 * <seealso name="OnGroupInitialize" />
 * <seealso name="OnGroupAdd" />
 * <seealso name="OnGroupClear" />
 * <returns>Doesn't return any specific value.</returns>
 */
forward OnGroupRemoveMember(VehicleGroup: groupid, vehicleid);

/**
 * <library>vehicle_plus</library>
 * <summary>Called when group has been cleared.</summary>
 * <param name="VehicleGroup: groupid">The ID of group that has been cleared.</param>
 * <seealso name="Vehicle_GetGroupByName" />
 * <seealso name="Vehicle_GroupHas" />
 * <seealso name="Vehicle_GroupInit" />
 * <seealso name="Vehicle_IsValidGroup" />
 * <seealso name="Vehicle_AddToGroup" />
 * <seealso name="Vehicle_RemoveFromGroup" />
 * <seealso name="Vehicle_IsEmptyGroup" />
 * <seealso name="Vehicle_GroupDestroy" />
 * <seealso name="Vehicle_ClearGroup" />
 * <seealso name="OnGroupInitialize" />
 * <seealso name="OnGroupRemoveMember" />
 * <seealso name="OnGroupAdd" />
 * <returns>Doesn't return any specific value.</returns>
 */
forward OnGroupClear(VehicleGroup: groupid);


#define INVALID_VEHICLE_GROUP_ID 0

static
    Vehicle_gsGroupName[VehicleGroup: MAX_VEHICLE_GROUPS][24],
    Iterator: Vehicle_gsActGps<MAX_VEHICLE_GROUPS>,
    Iterator: Vehicle_gsGroups[VehicleGroup: MAX_VEHICLE_GROUPS]<MAX_VEHICLES>;


/**
 * <library>vehicle_plus</library>
 * <summary>Looks for a group with specified name.</summary>
 * <param name="name">Name to look.</param>
 * <seealso name="Vehicle_GroupHas" />
 * <seealso name="Vehicle_GroupInit" />
 * <seealso name="Vehicle_IsValidGroup" />
 * <seealso name="Vehicle_AddToGroup" />
 * <seealso name="Vehicle_RemoveFromGroup" />
 * <seealso name="Vehicle_IsEmptyGroup" />
 * <seealso name="Vehicle_GroupDestroy" />
 * <seealso name="Vehicle_ClearGroup" />
 * <seealso name="OnGroupInitialize" />
 * <seealso name="OnGroupRemoveMember" />
 * <seealso name="OnGroupAdd" />
 * <seealso name="OnGroupClear" />
 * <returns>Returns -1 if name is NULL, INVALID_VEHICLE_GROUP_ID if there are no groups to match with the name, and group id if there's a group with the specified name.</returns>
*/
stock VehicleGroup: Vehicle_GetGroupByName(const name[])
{
    foreach(new i : Vehicle_gsActGps) 
    {
        if(i == 0)
        {
            continue;
        }
        if(isnull(name))
        {
            return VehicleGroup: -1;
        }
        if(!strcmp(name, Vehicle_gsGroupName[VehicleGroup: i])) 
        {
            return VehicleGroup: i;
        }
    }
    return VehicleGroup: INVALID_VEHICLE_GROUP_ID;
}

/**
 * <library>vehicle_plus</library>
 * <summary>Checks whether the group has specified vehicle.</summary>
 * <param name="vehicleid">ID of vehicle to seek for.</param>
 * <seealso name="Vehicle_GetGroupByName" />
 * <seealso name="Vehicle_GroupInit" />
 * <seealso name="Vehicle_IsValidGroup" />
 * <seealso name="Vehicle_AddToGroup" />
 * <seealso name="Vehicle_RemoveFromGroup" />
 * <seealso name="Vehicle_IsEmptyGroup" />
 * <seealso name="Vehicle_GroupDestroy" />
 * <seealso name="Vehicle_ClearGroup" />
 * <seealso name="OnGroupInitialize" />
 * <seealso name="OnGroupRemoveMember" />
 * <seealso name="OnGroupAdd" />
 * <seealso name="OnGroupClear" />
 * <returns>Returns true if has, false otherwise.</returns>
*/
stock bool: Vehicle_GroupHas(VehicleGroup: groupid, vehicleid)
{
    if(Iter_Contains(Vehicle_gsActGps, _:groupid) && IsValidVehicle(vehicleid) && Iter_Contains(Vehicle_gsGroups[groupid], vehicleid))
    {
        return true;
    }
    return false;
}

/**
 * <library>vehicle_plus</library>
 * <summary>Initialize a group.</summary>
 * <param name="name">Name to set.</param>
 * <seealso name="Vehicle_GetGroupByName" />
 * <seealso name="Vehicle_GroupHas" />
 * <seealso name="Vehicle_IsValidGroup" />
 * <seealso name="Vehicle_AddToGroup" />
 * <seealso name="Vehicle_RemoveFromGroup" />
 * <seealso name="Vehicle_IsEmptyGroup" />
 * <seealso name="Vehicle_GroupDestroy" />
 * <seealso name="Vehicle_ClearGroup" />
 * <seealso name="OnGroupInitialize" />
 * <seealso name="OnGroupRemoveMember" />
 * <seealso name="OnGroupAdd" />
 * <seealso name="OnGroupClear" />
 * <returns>ID of the group that has been initialized.</returns>
*/
stock VehicleGroup: Vehicle_GroupInit(const name[]) 
{
    
    if(Iter_IsEmpty(Vehicle_gsActGps))
    {
        Iter_Add(Vehicle_gsActGps, 0);
    }
    new VehicleGroup: idx = VehicleGroup: Iter_Free(Vehicle_gsActGps);
    if(Vehicle_GetGroupByName(name) != VehicleGroup: INVALID_VEHICLE_GROUP_ID)
    {
        return VehicleGroup: INVALID_VEHICLE_GROUP_ID;
    }
    strcopy(Vehicle_gsGroupName[idx], name);
    Iter_Add(Vehicle_gsActGps, _:idx); 
    _Framework_CallLocalFunction("OnGroupInitialize", "i", idx);

    return idx;
}

/**
 * <library>vehicle_plus</library>
 * <summary>Checks whether a specified group is valid.</summary>
 * <param name="groupid">The group we are checking. </param>
 * <seealso name="Vehicle_GetGroupByName" />
 * <seealso name="Vehicle_GroupHas" />
 * <seealso name="Vehicle_GroupInit" />
 * <seealso name="Vehicle_AddToGroup" />
 * <seealso name="Vehicle_RemoveFromGroup" />
 * <seealso name="Vehicle_IsEmptyGroup" />
 * <seealso name="Vehicle_GroupDestroy" />
 * <seealso name="Vehicle_ClearGroup" />
 * <seealso name="OnGroupInitialize" />
 * <seealso name="OnGroupRemoveMember" />
 * <seealso name="OnGroupAdd" />
 * <seealso name="OnGroupClear" />
 * <returns> Returns true if the group is valid, otherwise false. </returns>
*/
stock bool: Vehicle_IsValidGroup(VehicleGroup: groupid)
{
    if(groupid == VehicleGroup: 0 || !Iter_Contains(Vehicle_gsActGps, _:groupid))
    {
        return false;
    }
    return true;
}

/**
 * <library>vehicle_plus</library>
 * <summary>Adds a vehicle to a created group.</summary>
 * <param name="groupid">The group we are adding to.</param>
 * <param name="vehicleid">Vehicle to add.</param>
 * <seealso name="Vehicle_GetGroupByName" />
 * <seealso name="Vehicle_GroupHas" />
 * <seealso name="Vehicle_GroupInit" />
 * <seealso name="Vehicle_IsValidGroup" />
 * <seealso name="Vehicle_RemoveFromGroup" />
 * <seealso name="Vehicle_IsEmptyGroup" />
 * <seealso name="Vehicle_GroupDestroy" />
 * <seealso name="Vehicle_ClearGroup" />
 * <seealso name="OnGroupInitialize" />
 * <seealso name="OnGroupRemoveMember" />
 * <seealso name="OnGroupAdd" />
 * <seealso name="OnGroupClear" />
 * <returns>Returns INVALID_VEHICLE_GROUP_ID if vehicle or group isn't valid or group already contains the vehicle.</returns>
*/
stock Vehicle_AddToGroup(VehicleGroup: groupid, vehicleid)
{
    if(!Vehicle_IsValidGroup(groupid) || !IsValidVehicle(vehicleid) || Iter_Contains(Vehicle_gsGroups[groupid], vehicleid))
    {
        return INVALID_VEHICLE_GROUP_ID;
    }
    Iter_Add(Vehicle_gsGroups[groupid], vehicleid);
    _Framework_CallLocalFunction("OnGroupAddMember", "ii", groupid, vehicleid);
    return true;
}

/**
 * <library>vehicle_plus</library>
 * <summary>Removes a vehicle from a created group.</summary>
 * <param name="groupid">The group we are removing member from.</param>
 * <param name="vehicleid">Vehicle to remove.s</param>
 * <seealso name="Vehicle_GetGroupByName" />
 * <seealso name="Vehicle_GroupHas" />
 * <seealso name="Vehicle_GroupInit" />
 * <seealso name="Vehicle_IsValidGroup" />
 * <seealso name="Vehicle_AddToGroup" />
 * <seealso name="Vehicle_IsEmptyGroup" />
 * <seealso name="Vehicle_GroupDestroy" />
 * <seealso name="Vehicle_ClearGroup" />
 * <seealso name="OnGroupInitialize" />
 * <seealso name="OnGroupRemoveMember" />
 * <seealso name="OnGroupAdd" />
 * <seealso name="OnGroupClear" />
 * <returns>False if vehicle or group isn't valid, true otherwise</returns>
*/
stock bool: Vehicle_RemoveFromGroup(VehicleGroup: groupid, vehicleid)
{
    if(!Vehicle_IsValidGroup(groupid) || !Iter_Contains(Vehicle_gsGroups[groupid], vehicleid))
    {
        return false;
    }
    Iter_Remove(Vehicle_gsGroups[groupid], vehicleid);
    _Framework_CallLocalFunction("OnGroupRemoveMember", "ii", groupid, vehicleid);
    return true;
}

/**
 * <library>vehicle_plus</library>
 * <summary>Clears a group.</summary>
 * <param name="groupid">The group .</param>
 * <seealso name="Vehicle_GetGroupByName" />
 * <seealso name="Vehicle_GroupHas" />
 * <seealso name="Vehicle_GroupInit" />
 * <seealso name="Vehicle_IsValidGroup" />
 * <seealso name="Vehicle_RemoveFromGroup" />
 * <seealso name="Vehicle_IsEmptyGroup" />
 * <seealso name="Vehicle_GroupDestroy" />
 * <seealso name="Vehicle_AddToGroup" />
 * <seealso name="OnGroupInitialize" />
 * <seealso name="OnGroupRemoveMember" />
 * <seealso name="OnGroupAdd" />
 * <seealso name="OnGroupClear" />
 * <returns>False if vehicle or group isn't valid, true otherwise</returns>
*/
stock bool: Vehicle_ClearGroup(VehicleGroup: groupid)
{
    if(!Vehicle_IsValidGroup(groupid) || Iter_IsEmpty(Vehicle_gsGroups[groupid]))
    {
        return false;
    }
    foreach(new i : Vehicle_gsGroups[groupid])
    {
        Iter_SafeRemove(Vehicle_gsGroups[groupid], i, i);
    }
    _Framework_CallLocalFunction("OnGroupClear", "i", groupid);
    return true;
}

/**
 * <library>vehicle_plus</library>
 * <summary> Checks if a group is empty.</summary>
 * <param name="groupid">True if empty, otherwise false. </param>
 * <seealso name="Vehicle_GetGroupByName" />
 * <seealso name="Vehicle_GroupHas" />
 * <seealso name="Vehicle_GroupInit" />
 * <seealso name="Vehicle_IsValidGroup" />
 * <seealso name="Vehicle_RemoveFromGroup" />
 * <seealso name="Vehicle_ClearGroup" />
 * <seealso name="Vehicle_GroupDestroy" />
 * <seealso name="Vehicle_AddToGroup" />
 * <seealso name="OnGroupInitialize" />
 * <seealso name="OnGroupRemoveMember" />
 * <seealso name="OnGroupAdd" />
 * <seealso name="OnGroupClear" />
 * <returns>False if vehicle or group isn't valid, true otherwise</returns>
*/
stock bool: Vehicle_IsEmptyGroup(VehicleGroup: groupid)
{
    if(!Vehicle_IsValidGroup(groupid))
    {
        return false;
    }
    else if(Iter_IsEmpty(Vehicle_gsGroups[groupid]))
    {
        return true;
    }
    return false;
}

/**
 * <library>vehicle_plus</library>
 * <summary>Destroys a group.</summary>
 * <param name="groupid">Group to destroy.</param>
 * <seealso name="Vehicle_GetGroupByName" />
 * <seealso name="Vehicle_GroupHas" />
 * <seealso name="Vehicle_GroupInit" />
 * <seealso name="Vehicle_IsValidGroup" />
 * <seealso name="Vehicle_RemoveFromGroup" />
 * <seealso name="Vehicle_IsEmptyGroup" />
 * <seealso name="Vehicle_ClearGroup" />
 * <seealso name="Vehicle_AddToGroup" />
 * <seealso name="OnGroupInitialize" />
 * <seealso name="OnGroupRemoveMember" />
 * <seealso name="OnGroupAdd" />
 * <seealso name="OnGroupClear" />
 * <returns>False if vehicle or group isn't valid, true otherwise</returns>
*/
stock bool: Vehicle_GroupDestroy(VehicleGroup: groupid)
{
    if(!Iter_Contains(Vehicle_gsActGps, _:groupid))
    {
        return false;
    }
    foreach(new i : Vehicle_gsGroups[groupid])
    {
        Iter_SafeRemove(Vehicle_gsGroups[groupid], i, i);
    }
    Iter_Remove(Vehicle_gsActGps, _:groupid);
    return true;
} 


// --
// Blinkers
// --


#define VEHICLE_BLINKERS: __TAG(VEHICLE_BLINKERS):
enum VEHICLE_BLINKERS:__VEHICLE_BLINKERS
{
    VEHICLE_BLINKERS_NOT_SET = -1,
    VEHICLE_BLINKERS_LEFT = 0,
    VEHICLE_BLINKERS_RIGHT,
    VEHICLE_BLINKERS_EMERGENCY
}
static stock VEHICLE_BLINKERS:_@VEHICLE_BLINKERS() { return __VEHICLE_BLINKERS; }


static 
    VEHICLE_BLINKERS: Vehicle_gsBlinkSide[MAX_VEHICLES];


static enum e_BLINKING_ENUM 
{
    e_BLINKING_MODEL, 
    Float:e_BLINKING_POSX, 
    Float:e_BLINKING_POSY, 
    Float:e_BLINKING_POSZ, 
    Float:e_BLINKING_ROTX, 
    Float:e_BLINKING_ROTY, 
    Float:e_BLINKING_ROTZ
}

static const Vehicle_gscBlinkOffsets[][e_BLINKING_ENUM] = 
{
    {400, -0.9001, 2.0490, -0.0965, -0.9605, -2.2404, -0.0965},
    {401, -0.9686, 2.5393, 0.0235, -0.9970, -2.2863, 0.0235},
    {402, -0.8788, 2.5160, -0.0565, -0.9208, -2.5936, 0.0435},
    {403, -1.3897, 3.6007, -0.2194, -1.1904, -0.9415, 1.8389},
    {404, -0.7236, 2.1715, -0.0365, -0.7906, -2.7052, 0.0635},
    {405, -0.9127, 2.2766, -0.0565, -0.8723, -2.6526, -0.0565},
    {406, -2.2165, 4.9242, -0.0332, -2.0785, -5.2054, 0.5468},
    {407, -0.9887, 4.0622, -0.0913, -1.0141, -3.4034, -0.0913},
    {408, -0.9659, 4.7615, -0.2513, -0.8430, -3.9722, -0.5113},
    {409, -0.7859, 3.5522, -0.0313, -0.8027, -3.8228, -0.0313},
    {410, -0.825, 2.105, 0.0635, -0.8723, -2.1926, 0.1235},
    {411, -0.9405, 2.6710, -0.1825, -0.9636, -2.4525, 0.0825},
    {412, -0.8965, 2.6478, -0.0575, -0.9592, -3.4870, -0.1575},
    {413, -0.8669, 2.5464, -0.0913, -0.8209, -2.5829, 0.0687},
    {414, -0.8069, 2.7473, 0.0087, -0.9058, -3.2050, 0.1094},
    {415, -0.8738, 2.4866, -0.2357, -0.7792, -2.3501, 0.0450},
    {416, -0.8469, 2.9073, 0.0087, -1.0352, -3.5937, 1.4522},
    {417, 0.0000, 0.0000, 0.0000, 0.0000, 0.0000, 0.0000},
    {418, -0.9511, 2.3820, -0.2357, -1.0081, -2.4904, -0.1557},
    {419, -1.0742, 2.3978, -0.1757, -0.8127, -2.8620, -0.1557},
    {420, -1.0142, 2.2978, -0.0157, -0.9637, -2.6744, 0.0043},
    {421, -0.9013, 2.5343, -0.1357, -0.5921, -2.9228, -0.1957},
    {422, -0.7813, 2.1543, -0.2657, -0.8985, -2.4349, -0.1057},
    {423, -0.7817, 2.2093, -0.0313, -0.8340, -2.1794, 0.0596},
    {424, -0.7350, 1.4505, 0.1087, -0.7062, -1.4778, 0.2996},
    {425, 0.0000, 0.0000, 0.0000, 0.0000, 0.0000, 0.0000},
    {426, -1.0006, 2.3164, -0.0100, -0.9627, -2.6930, -0.0100},
    {427, -0.8728, 2.5856, 0.1887, -0.9831, -3.8383, -0.3495},
    {428, -0.8728, 2.5856, -0.2103, -0.9045, -2.8871, 1.2466},
    {429, -0.7942, 2.2846, -0.2500, -0.8397, -2.3849, -0.0070},
    {430, 0.0000, 0.0000, 0.0000, 0.0000, 0.0000, 0.0000},
    {431, -0.9771, 5.8479, 0.3497, -1.0300, -5.7043, -0.1041},
    {432, -1.7168, 4.0330, 0.4497, -1.5830, -4.3414, 0.7039},
    {433, -1.2619, 3.7708, -0.2163, -1.2966, -4.6534, -0.1051},
    {434, 0.0, 0.0, 0.0, -0.4811, -1.9838, -0.0670},
    {435, 0.0000, 0.0000, 0.0000, -1.0697, -3.9690, -1.1053}, //trailer
    {436, -0.8495, 2.2519, -0.0070, -0.8948, -2.4838, 0.1130},
    {437, -1.1156, 5.5395, -0.2163, -0.9856, -5.3099, 1.6458},
    {438, -1.1128, 2.3675, -0.2870, -1.0992, -2.4601, -0.2870},
    {439, -0.8381, 2.3101, -0.1470, -0.8221, -2.6534, -0.1070},
    {440, -0.8270, 2.5573, -0.3173, -0.8676, -2.5376, -0.0149},
    {441, 0.0000, 0.0000, 0.0000, 0.0000, 0.0000, 0.0000},
    {442, -1.0354, 2.8278, -0.1870, -1.0226, -3.0877, -0.1270},
    {443, -1.0610, 5.7528, -0.9573, -1.2388, -7.1392, -0.8250},
    {444, -0.9880, 2.7188, 0.6140, -1.0722, -3.0184, 0.7248},
    {445, -0.9590, 2.3460, -0.0840, -0.9773, -2.8049, -0.0840},
    {446, 0.0000, 0.0000, 0.0000, 0.0000, 0.0000, 0.0000},
    {447, 0.0000, 0.0000, 0.0000, 0.0000, 0.0000, 0.0000},
    {448,  -0.1992, -0.9229, -0.1270},
    {449, 0.0000, 0.0000, 0.0000, 0.0000, 0.0000, 0.0000},
    {450,  0.0000, 0.0000, 0.0000, -1.0697, -3.9690, -1.1053}, //trailer
    {451, -0.9826, 1.9642, -0.1399, -0.8894, -2.3991, -0.0199},
    {452, 0.0000, 0.0000, 0.0000, 0.0000, 0.0000, 0.0000},
    {453, 0.0000, 0.0000, 0.0000, 0.0000, 0.0000, 0.0000},
    {454, 0.0000, 0.0000, 0.0000, 0.0000, 0.0000, 0.0000},
    {455, 0.0000, 0.0000, 0.0000, -1.3866, -4.5162, -0.7399},
    {456, 0.0000, 0.0000, 0.0000, -1.3045, -4.6123, 0.4601},
    {457, 0.0000, 0.0000, 0.0000, 0.0000, 0.0000, 0.0000},
    {458, -0.9513, 2.4197, -0.1399, -0.9703, -2.7779, 0.0201},
    {459, -0.8214, 2.5716, -0.1274, -0.8760, -2.5775, 0.1127},
    {460, 0.0000, 0.0000, 0.0000, 0.0000, 0.0000, 0.0000},
    {461, 0.0000, 0.0000, 0.0000, 0.0000, 0.0000, 0.0000},
    {462, 0.0000, 0.0000, 0.0000, -0.2054, -0.9359, -0.1399},
    {463, -0.2276, 0.7185, 0.3201, -0.1952, -1.0037, 0.1601},
    {464, 0.0000, 0.0000, 0.0000, 0.0000, 0.0000, 0.0000},
    {465, 0.0000, 0.0000, 0.0000, 0.0000, 0.0000, 0.0000},
    {466, 0.0000, 0.0000, 0.0000, -1.0201, -2.7994, 0.0401},
    {467, 0.0000, 0.0000, 0.0000, -0.9401, -3.0594, 0.1601},
    {468, 0.0000, 0.0000, 0.0000, 0.0000, 0.0000, 0.0000},
    {469, 0.0000, 0.0000, 0.0000, 0.0000, 0.0000, 0.0000},
    {470, -1.0259, 2.1382, 0.2001, -1.0433, -2.5463, 0.1201},
    {471, 0.0000, 0.0000, 0.0000, 0.0000, 0.0000, 0.0000},
    {472, 0.0000, 0.0000, 0.0000, 0.0000, 0.0000, 0.0000},
    {473, 0.0000, 0.0000, 0.0000, 0.0000, 0.0000, 0.0000},
    {474, 0.0000, 0.0000, 0.0000, -0.9833, -2.7663, -0.0599},
    {475, -0.8681, 2.4086, -0.3399, -0.8632, -2.7629, -0.2399},
    {476, 0.0000, 0.0000, 0.0000, 0.0000, 0.0000, 0.0000},
    {477, -0.8081, 2.6686, -0.1999, -0.9945, -2.6987, 0.0801},
    {478, -1.0938, 2.0255, 0.1001, -1.0579, -2.5378, -0.2799},
    {479, -0.9671, 2.4844, 0.0201, -0.9578, -2.7556, 0.0601},
    {480, -0.5897, 2.2607, -0.4399, -0.9183, -2.3388, -0.1399},
    {481, 0.0000, 0.0000, 0.0000, 0.0000, 0.0000, 0.0000},
    {482, -0.8841, 2.4094, -0.3399, -0.8685, -2.5676, 0.0201},
    {483, -0.7770, 2.6235, -0.0199, -0.6709, -2.7712, -0.1999},
    {484,  0.0000, 0.0000, 0.0000, 0.0000, 0.0000, 0.0000},
    {485, -0.6104, 1.7180, 0.2201, -0.7074, -1.3681, 0.1201},
    {486, 0.0000, 0.0000, 0.0000, -0.5919, -3.2353, 0.9601},
    {487, 0.0000, 0.0000, 0.0000, 0.0000, 0.0000, 0.0000},
    {488, 0.0000, 0.0000, 0.0000, 0.0000, 0.0000, 0.0000},
    {489, -1.1024, 2.5796, 0.0401, -1.1261, -2.6473, 0.1601},
    {490, -1.0971, 3.1462, 0.0401, -1.1327, -3.1055, 0.1601},
    {491, -0.8495, 2.5284, -0.0799, -0.8870, -2.7776, 0.0001},
    {492, -0.7905, 2.4656, -0.0599, -0.9075, -2.8130, 0.0001},
    {493, 0.0000, 0.0000, 0.0000, 0.0000, 0.0000, 0.0000},
    {494, -0.8513, 2.3630, -0.2199, -0.8275, -2.8485, 0.1401},
    {495, -1.0909, 2.3818, 0.0001, -1.1540, -2.1156, -0.0399},
    {496, -0.9219, 2.1787, 0.0401, -0.7095, -2.1087, 0.0801},
    {497, 0.0000, 0.0000, 0.0000, 0.0000, 0.0000, 0.0000},
    {498, -0.8915, 3.0670, 0.2201, -0.9740, -3.0460, 0.3601},
    {499, -0.7725, 2.4576, -0.1999, -1.1318, -3.4186, 0.1401},
    {500, -0.9152, 1.9097, -0.0355, -0.7552, -1.9266, -0.0355},
    {501, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0},
    {502, -0.8723, 2.5986, -0.1770, 0.0, 0.0, 0.0},
    {503, -0.8579, 2.3582, -0.0213, 0.0, 0.0, 0.0},
    {504, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0},
    {505, -1.1068, 2.5744, 0.0086, -1.1365, -2.6250, 0.1679},
    {506, -0.7672, 2.2106, -0.3185, -0.9113, -2.3474, -0.0273},
    {507, -1.1143, 2.6057, -0.0456, -1.1410, -2.9714, -0.0456},
    {508, -0.7012, 3.0199, -0.6678, -1.3807, -3.3356, 1.2628},
    {509, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0},
    {510, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0},
    {511, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0},
    {512, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0},
    {513, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0},
    {514, -1.2341, 4.2204, 0.0806, 0.0, 0.0, 0.0},
    {515, -1.4241, 4.4811, -0.7354, -1.2973, -4.8274, -0.8974},
    {516, -0.9512, 2.7208, -0.0543, -0.9926, -2.7809, -0.0348},
    {517, -0.9401, 2.7868, -0.0726, -1.0208, -2.7461, -0.1324},
    {518, -0.9166, 2.6548, -0.0487, -1.0124, -2.8219, -0.0487}, //fake
    {519, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0},
    {520, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0},
    {521, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0},
    {522, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0},
    {523, -0.2607, 0.6038, 0.2881, -0.1973, -0.9958, 0.0638},
    {524, -0.9279, 3.6538, -0.0473, -1.3003, -3.9309, -1.0661}, //fake back
    {525, -0.8576, 2.9431, 0.1425, -1.0428, -2.9851, -0.1486}, //fake back
    {526, -0.9421, 2.3087, -0.1128, -0.9559, -2.2882, -0.0411},
    {527, -0.9351, 2.4078, -0.0242, -0.9825, -2.2344, 0.0844},
    {528, -0.8783, 2.5102, -0.1526, -0.8133, -2.5965, -0.3237}, //fake back
    {529, -1.0177, 2.5256, 0.0698, -1.0505, -2.5713, 0.2047},
    {530, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0},
    {531, -0.4216, 1.5030, -0.1191, -0.5225, -1.0517, 0.4484}, //fake
    {532, -4.0331, 3.9619, -0.9118, -1.5976, -0.3739, 0.4484}, //fake!
    {533, -0.9410, 2.3774, 0.0985, -0.9852, -2.4663, -0.0020},
    {534, -1.0159, 2.9278, -0.1698, -0.8236, -2.7548, -0.0574},
    {535, -0.8265, 2.5014, -0.2084, -0.9267, -2.6058, -0.1784},
    {536, -0.8577, 2.3943, -0.1248, -0.8736, -3.0999, -0.0900},
    {537, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0},
    {538, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0},
    {539, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0},
    {540, -0.9827, 2.5810, -0.1248, -1.0597, -2.7121, -0.0842},
    {541, -0.6055, 2.3378, -0.2108, -0.7826, -2.1992, 0.0913},
    {542, -0.9712, 2.6252, -0.0064, -0.8281, -2.9729, -0.0577},
    {543, -0.8414, 2.2904, 0.0719, -0.9831, -2.6214, -0.0116}, //fake front
    {544, -0.7346, 3.5720, 0.0200, -0.8597, -3.2242, -0.5918},
    {545, -0.4372, 1.6776, -0.0400, -0.7955, -2.0453, -0.2888},
    {546, -1.1294, 1.0702, 0.0719, -1.0580, -2.6948, 0.0719},
    {547, -0.9582, 2.5222, 0.0271, -0.9937, -2.5915, 0.0719},
    {548, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0},
    {549, -0.9264, 2.4914, 0.0271, -0.9070, -2.5802, 0.0271},
    {550, -1.0477, 2.5642, -0.1807, -1.0302, -2.6549, -0.1297},
    {551, -0.9334, 2.6344, -0.0448, -0.9785, -3.0524, 0.0366},
    {552, -0.8757, 3.0634, 0.3600, -0.9933, -2.9119, 0.4752},
    {553, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0},
    {554, -0.9409, 2.5345, 0.0600, -1.1112, -2.8095, 0.0651},
    {555, -0.9236, 1.1282, -0.0462, 0.0, 0.0, 0.0},
    {556, -0.9686, 2.5396, 0.5800, -1.1128, -2.8895, 0.7691},
    {557, -0.9886, 2.4796, 0.7000, -1.0904, -2.7699, 0.6871},
    {558, -0.9452, 2.0854, 0.0836, -0.9187, -2.4047, 0.3185},
    {559, -0.9037, 2.4333, -0.0168, -0.8819, -2.2910, 0.1482},
    {560, -0.9347, 2.5097, -0.2644, -0.9639, -2.2337, 0.1259},
    {561, -0.9157, 2.5754, -0.0857, -0.9317, -2.5444, -0.0191},
    {562, -0.9299, 2.3759, 0.0201, -0.8575, -2.2773, 0.1659},
    {563, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0},
    {564, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0},
    {565, -0.8503, 2.0545, 0.0346, -0.9249, -1.9224, 0.0816},
    {566, -0.9539, 2.7226, 0.0346, -0.9506, -2.9199, 0.0346},
    {567, -0.9942, 2.9212, -0.1543, -1.0077, -2.9164, -0.1543},
    {568, -0.2962, 2.0729, -0.0171, -0.2384, -1.2180, 0.0720},
    {569, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0},
    {570, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0},
    {571, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0},
    {572, -0.3095, 0.7291, -0.0971, -0.4318, -0.8940, -0.0510},
    {573, -0.7949, 3.0624, -0.3371, -0.8727, -3.0917, -0.4540},
    {574, -0.4348, 1.6912, -0.2171, -0.4277, -1.1358, -0.2069},
    {575, -0.9483, 2.3252, 0.1334, -0.9173, -2.7359, -0.0291},
    {576, -0.9948, 2.4505, 0.1883, -0.9983, -3.1357, -0.0291},
    {577, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0},
    {578, -1.0478, 4.3687, -0.1800, -1.1188, -5.3472, -0.5099},
    {579, -1.0598, 2.3634, 0.1013, -1.0873, -2.7596, 0.1013},
    {580, -0.7970, 2.6313, -0.2429, -1.0776, -2.8302, 0.1013},
    {581, 0.0, 0.0, 0.0, -0.1975, -1.0375, 0.3448},
    {582, -0.8583, 2.4404, -0.0571, -0.9214, -3.3625, 0.0521},
    {583, -0.5225, 1.3799, 0.2429, -0.5486, -1.5684, 0.2462},
    {584, 0.0, 0.0, 0.0, -1.0452, -4.3338, -0.7298}, //trailer
    {585, -1.0068, 2.7905, 0.0907, -0.9760, -3.0160, 0.2088},
    {586, -0.2872, 0.5383, 0.2009, -0.1703, -1.3533, 0.2784},
    {587, -1.0236, 2.1415, -0.2834, -1.0832, -2.4851, 0.1047},
    {588, -1.0116, 3.3590, 0.1029, -1.0679, -3.9639, -0.3029},
    {589, -0.8107, 2.3905, 0.1834, -0.9605, -2.2799, 0.3761},
    {590, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0},
    {591, 0.0, 0.0, 0.0, -1.0590, -3.9902, -1.0809}, //trailer
    {592, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0},
    {593, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0},
    {594, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0},
    {595, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0},
    {596, -1.0419, 2.2929, -0.0255, -1.0332, -2.6951, 0.0005},
    {597, -1.0419, 2.2929, -0.0255, -1.0332, -2.6951, 0.0005},
    {598, -1.0423, 2.3495, 0.0600, -1.0326, -2.6930, 0.0600},
    {599, -1.0837, 2.5663, 0.0301, -1.1267, -2.6290, 0.1451},
    {600, -0.8782, 2.6525, -0.0344, -1.0586, -2.6696, 0.0596},
    {601, -0.8094, 3.0084, 0.5429, -0.9321, -3.0808, 0.9071},
    {602, -1.0297, 2.0487, -0.0543, -0.8907, -2.5334, -0.3228},
    {603, -0.9069, 2.6672, -0.1335, -0.8647, -2.5991, -0.1335},
    {604, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0},
    {605, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0},
    {606, 0.0, 0.0, 0.0, -0.5702, -1.5842, 0.7956}, //trailer
    {607, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0},
    {608, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0},
    {609, -0.8972, 3.0244, 0.2629, -0.9349, -2.9809, 0.2791},
    {610, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0},
    {611, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0}
};


/**
 * <library>vehicle_plus</library>
 * <summary>Checks if vehicle's blinking.</summary>
 * <param name="vehicleid">Vehicle whose indicators we check.</param>
 * <seealso name="Vehicle_SetBlinking" />
 * <seealso name="Vehicle_CancelBlinking" />
 * <returns>Returns false if not blinking, else true.</returns>
 */
stock bool: Vehicle_IsBlinking(vehicleid)
{
    if(Vehicle_gsBlinkSide[vehicleid] == VEHICLE_BLINKERS_NOT_SET)
    {
        return false;
    }

    return true;
}

/** 
 * <library>vehicle_plus</library>
 * <summary>Set vehicle's indicators.</summary>
 * <param name="vehicleid">Vehicle or group of vehicles whose indicators we set.</param>
 * <param name="side">Indicators side.</param>
 * <seealso name="Vehicle_IsBlinking" />
 * <seealso name="Vehicle_CancelBlinking" />
 * <returns> Returns false if vehicle is blinking already, otherwise true.</returns>
 */
stock bool: Vehicle_SetBlinking(VEHICLE_FRAMEWORK_TAGS: vehicleid, VEHICLE_BLINKERS: side, bool:skip=false, tag=tagof(vehicleid)) //0 - left 1 - right 2 - emergy
{
    if(tag == tagof(_:))
    {
    
        new 
            modelid = GetVehicleModel(_:vehicleid),
            id = modelid-400,
            trailer;

        if(Vehicle_IsBlinking(vehicleid) && !skip) 
        {
            return false;
        }
    
        if(!skip) 
        {
            Vehicle_gsBlinkSide[vehicleid] = side;
        }
    
        if(id >= 0)
        {
            if(Vehicle_gscBlinkOffsets[id][e_BLINKING_POSX] != 0.0)
            {
                if(side == VEHICLE_BLINKERS_LEFT)
                {
                    Vehicle_gsBlink[vehicleid][0] = CreateDynamicObject(19294, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0); //blink yellow  front
                }
                else if(side == VEHICLE_BLINKERS_RIGHT)
                {
                    Vehicle_gsBlink[vehicleid][2] = CreateDynamicObject(19294, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0);
                }
            }
            if((trailer = Vehicle_GetTrailer(vehicleid)) != 0)
            {
                if(Vehicle_GetCategory(trailer) == CATEGORY_TRAILER)
                {
                    if(side == VEHICLE_BLINKERS_LEFT)
                    {
                        Vehicle_gsBlink[vehicleid][1] = CreateDynamicObject(19294, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0); //blink yellow back truck only trailer
                        
                    }
                    else if(side == VEHICLE_BLINKERS_RIGHT)
                    {
                        Vehicle_gsBlink[vehicleid][3] = CreateDynamicObject(19294, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0);    
                    }
                }
            }
            else
            {
                if(Vehicle_gscBlinkOffsets[id][e_BLINKING_ROTX] != 0.0)
                {
                    if(side == VEHICLE_BLINKERS_LEFT)
                    {
                        Vehicle_gsBlink[vehicleid][1] = CreateDynamicObject(19294, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0); //blink yellow back truck only trailer
                    }
                    else if(side == VEHICLE_BLINKERS_RIGHT)
                    {
                        Vehicle_gsBlink[vehicleid][3] = CreateDynamicObject(19294, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0);
                    }
                }
            }
    
            if(side == VEHICLE_BLINKERS_LEFT) //left
            {
                if(Vehicle_gsBlink[vehicleid][0] != 0) 
                {
                    AttachDynamicObjectToVehicle(Vehicle_gsBlink[vehicleid][0], vehicleid, Vehicle_gscBlinkOffsets[id][e_BLINKING_POSX], Vehicle_gscBlinkOffsets[id][e_BLINKING_POSY], Vehicle_gscBlinkOffsets[id][e_BLINKING_POSZ], 0.0, 0.0, 0.0);
                }
                if(trailer != 0)
                {
                    id = GetVehicleModel(trailer)-400;
                    AttachDynamicObjectToVehicle(Vehicle_gsBlink[vehicleid][1], trailer, Vehicle_gscBlinkOffsets[id][e_BLINKING_ROTX], Vehicle_gscBlinkOffsets[id][e_BLINKING_ROTY], Vehicle_gscBlinkOffsets[id][e_BLINKING_ROTZ], 0.0, 0.0, 0.0);
                }
                else
                {
                    if(Vehicle_gsBlink[vehicleid][1] != 0) 
                    {
                        AttachDynamicObjectToVehicle(Vehicle_gsBlink[vehicleid][1], vehicleid, Vehicle_gscBlinkOffsets[id][e_BLINKING_ROTX], Vehicle_gscBlinkOffsets[id][e_BLINKING_ROTY], Vehicle_gscBlinkOffsets[id][e_BLINKING_ROTZ], 0.0, 0.0, 0.0);
                    }
                }
            }
            else if(side == VEHICLE_BLINKERS_RIGHT) //right
            {
                if(Vehicle_gsBlink[vehicleid][2] != 0) 
                {
                    AttachDynamicObjectToVehicle(Vehicle_gsBlink[vehicleid][2], vehicleid, -Vehicle_gscBlinkOffsets[id][e_BLINKING_POSX], Vehicle_gscBlinkOffsets[id][e_BLINKING_POSY], Vehicle_gscBlinkOffsets[id][e_BLINKING_POSZ], 0.0, 0.0, 0.0);
                }
                if(trailer != 0)
                {
                    id = GetVehicleModel(trailer)-400;
                    AttachDynamicObjectToVehicle(Vehicle_gsBlink[vehicleid][3], trailer, -Vehicle_gscBlinkOffsets[id][e_BLINKING_ROTX], Vehicle_gscBlinkOffsets[id][e_BLINKING_ROTY], Vehicle_gscBlinkOffsets[id][e_BLINKING_ROTZ], 0.0, 0.0, 0.0);
                }
                else
                {
                    if(Vehicle_gsBlink[vehicleid][3] != 0) 
                    {
                        AttachDynamicObjectToVehicle(Vehicle_gsBlink[vehicleid][3], vehicleid, -Vehicle_gscBlinkOffsets[id][e_BLINKING_ROTX], Vehicle_gscBlinkOffsets[id][e_BLINKING_ROTY], Vehicle_gscBlinkOffsets[id][e_BLINKING_ROTZ], 0.0, 0.0, 0.0);
                    }
                }
            }
            else if(side == VEHICLE_BLINKERS_EMERGENCY)  //emergency
            {
                Vehicle_SetBlinking(vehicleid, VEHICLE_BLINKERS_LEFT, true);
                Vehicle_SetBlinking(vehicleid, VEHICLE_BLINKERS_RIGHT, true);
            }
        }
    }
    else if(tag == tagof(VehicleGroup:))
    {
        foreach(new i : Vehicle_gsGroups[VehicleGroup: vehicleid])
        {
            Vehicle_SetBlinking(i, side, skip);
        }
    }
    return true;
}


/**
 * <library>vehicle_plus</library>
 * <summary> Cancel vehicle blinking.</summary>
 * <param name="vehicleid"> Vehicle or group of vehicles whose indicators we cancel. </param>
 * <seealso name="Vehicle_IsBlinking" />
 * <seealso name="Vehicle_SetBlinking" />
 * <returns> False if hasn't been blinkig, true otherwise.</returns>
 */
stock bool: Vehicle_CancelBlinking(VEHICLE_FRAMEWORK_TAGS: id, tag=(tagof(id)))
{
    if(tag == tagof(_:))
    {
        if(!Vehicle_IsBlinking(id)) 
        {
            return false;
        }

        for(new i; i < BLINK_SIDES; i++)
        {
            if(Vehicle_gsBlink[id][i] != -1) 
            {
                DestroyDynamicObject(Vehicle_gsBlink[id][i]);
            }
            Vehicle_gsBlink[id][i] = -1;
            Vehicle_gsBlinkSide[id] = VEHICLE_BLINKERS_NOT_SET;
            
        }
    }
    else if(tag == tagof(_:))
    {
        foreach(new idx : Vehicle_gsGroups[VehicleGroup: id])
        {
            if(!Vehicle_IsBlinking(idx)) 
            {
                continue;
            }

            for(new i; i < BLINK_SIDES; i++)
            {
                if(Vehicle_gsBlink[idx][i] != -1) 
                {
                    DestroyDynamicObject(Vehicle_gsBlink[idx][i]);
                }
                Vehicle_gsBlink[idx][i] = -1;
                Vehicle_gsBlinkSide[idx] = VEHICLE_BLINKERS_NOT_SET; 
            }
        }
    }
    return true;
}

// -- 
// Engine 
// --

/**
 * <library>vehicle_plus</library>
 * <summary> Gets the engine state.</summary>
 * <param name="vehicleid"> Vehicle which we get engine state from.</param>
 * <seealso name="Vehicle_SetEngineState" />
 * <seealso name="Vehicle_GetDriveType" />
 * <seealso name="Vehicle_GetEngineType" />
 * <returns> Returns VEHICLE_PARAMS_ON if engine is turned on, otherwise VEHICLE_PARAMS_OFF including invalid vehicle. </returns>
 */
stock VEHICLE_PARAMS: Vehicle_GetEngineState(vehicleid) 
{
    return Vehicle_gsParams[vehicleid][PARAMS_ENGINE];
}

/**
 * <library>vehicle_plus</library>
 * <summary> Sets the engine state.</summary>
 * <param name="vehicleid"> Vehicle or group of vehicles which we set the engine state.</param>
 * <param name="engine_state"> Engine state to set.</param>
 * <seealso name="Vehicle_GetEngineState" />
 * <seealso name="Vehicle_GetDriveType" />
 * <seealso name="Vehicle_GetEngineType" />
 * <returns> SetVehicleParamsEx values. </returns>
*/
stock bool: Vehicle_SetEngineState(VEHICLE_FRAMEWORK_TAGS: id, VEHICLE_PARAMS: engine_state, tag=tagof(id)) 
{
    if(tag==tagof(_:))
    {
        if(!IsValidVehicle(id))
        {
            return false;
        }
        Vehicle_gsParams[id][PARAMS_ENGINE] = engine_state;
        SetVehicleParamsEx(id, Vehicle_gsParams[id][PARAMS_ENGINE], Vehicle_gsParams[id][PARAMS_LIGHTS], Vehicle_gsParams[id][PARAMS_ALARM], Vehicle_gsParams[id][PARAMS_DOORS], Vehicle_gsParams[id][PARAMS_BONNET], Vehicle_gsParams[id][PARAMS_BOOT], Vehicle_gsParams[id][PARAMS_OBJECTIVE]);
    }
    else if(tag==tagof(VehicleGroup:))
    {
        foreach(new i : Vehicle_gsGroups[VehicleGroup: id])
        {
            Vehicle_gsParams[i][PARAMS_ENGINE] = engine_state;
            SetVehicleParamsEx(i, Vehicle_gsParams[i][PARAMS_ENGINE], Vehicle_gsParams[i][PARAMS_LIGHTS], Vehicle_gsParams[i][PARAMS_ALARM], Vehicle_gsParams[i][PARAMS_DOORS], Vehicle_gsParams[i][PARAMS_BONNET], Vehicle_gsParams[i][PARAMS_BOOT], Vehicle_gsParams[i][PARAMS_OBJECTIVE]);
        }
    }
    return true;
}

/**
 * <library>vehicle_plus</library>
 * <summary>Gets vehicle drive type.</summary>
 * <param name="vehicleid">Vehicle which we get the drive type from.</param>
 * <seealso name="Vehicle_GetEngineState" />
 * <seealso name="Vehicle_SetEngineState" />
 * <seealso name="Vehicle_GetEngineType" />
 * <returns>Vehicle drive type. 0 if vehicle is not created.</returns>
 */
stock DRIVE_TYPE: Vehicle_GetDriveType(vehicleid) 
{
    new model = GetVehicleModel(vehicleid) - 400;
    return (IsValidVehicle(vehicleid)) ? (Vehicle_Info[model][E_VEHICLE_DRIVE_TYPE]) : (DRIVE_INVALID);
}

/**
 * <library>vehicle_plus</library>
 * <summary> Gets vehicle engine type
 * <param name="vehicleid">Vehicle which we get the engine type from.</param>
 * <seealso name="Vehicle_GetEngineState" />
 * <seealso name="Vehicle_SetEngineState" />
 * <seealso name="Vehicle_GetDriveType" />
 * <returns>Vehicle engine type. 0 if vehicle is not created.</returns>
 */
stock ENGINE_TYPE: Vehicle_GetEngineType(vehicleid) 
{
    new model = GetVehicleModel(vehicleid) - 400;
    return ((IsValidVehicle(vehicleid)) ? (Vehicle_Info[model][E_VEHICLE_ENGINE_TYPE]) : (ENGINE_INVALID));
}

// --
// Boot
// --

/**
 * <summary> Gets the engine state.</summary>
 * <param name="vehicleid"> Vehicle from which we get engine state.</param>

 * <returns> Engine state. </returns>
 */

stock VEHICLE_PARAMS: Vehicle_GetBootState(vehicleid) 
{
    return Vehicle_gsParams[vehicleid][PARAMS_BOOT];
}

/**
 * <summary> Sets the boot state.</summary>
 * <param name="vehicleid"> Vehicle which we set the boot state.</param>
 * <param name="engine_state"> Boot state to set.</param>
 * <returns> SetVehicleParamsEx values. </returns>
*/

stock bool: Vehicle_SetBootState(VEHICLE_FRAMEWORK_TAGS: id, VEHICLE_PARAMS: boot_state, tag=tagof(id)) 
{
    if(tag==tagof(_:)) 
    {        
        if(!IsValidVehicle(id))
        {
            return false;
        }
        Vehicle_gsParams[id][PARAMS_BOOT] = boot_state;
        SetVehicleParamsEx(id, Vehicle_gsParams[id][PARAMS_ENGINE], Vehicle_gsParams[id][PARAMS_LIGHTS], Vehicle_gsParams[id][PARAMS_ALARM], Vehicle_gsParams[id][PARAMS_DOORS], Vehicle_gsParams[id][PARAMS_BONNET], Vehicle_gsParams[id][PARAMS_BOOT], Vehicle_gsParams[id][PARAMS_OBJECTIVE]);
    }
    else if(tag==tagof(VehicleGroup:))
    {
        foreach(new i : Vehicle_gsGroups[VehicleGroup: id])
        {
            Vehicle_gsParams[i][PARAMS_BOOT] = boot_state;
            SetVehicleParamsEx(i, Vehicle_gsParams[i][PARAMS_ENGINE], Vehicle_gsParams[i][PARAMS_LIGHTS], Vehicle_gsParams[i][PARAMS_ALARM], Vehicle_gsParams[i][PARAMS_DOORS], Vehicle_gsParams[i][PARAMS_BONNET], Vehicle_gsParams[i][PARAMS_BOOT], Vehicle_gsParams[i][PARAMS_OBJECTIVE]);
        }
    }
    return true;
}

// --
// Bonnet
// --

/**
 * <summary> Gets the bonnet state.</summary>
 * <param name="vehicleid"> Vehicle from which we get bonnet state.</param>
 * <returns> Bonnet state. </returns>
 */

stock VEHICLE_PARAMS: Vehicle_GetBonnetState(vehicleid) 
{
    return Vehicle_gsParams[vehicleid][PARAMS_BONNET];
}

/**
 * <summary> Sets the bonnet state.</summary>
 * <param name="vehicleid"> Vehicle which we set the bonnet state.</param>
 * <param name="engine_state"> Bonnet state to set.</param>
 * <returns> SetVehicleParamsEx values. </returns>
*/

stock bool: Vehicle_SetBonnetState(VEHICLE_FRAMEWORK_TAGS: id, VEHICLE_PARAMS: bonnet_state, tag=tagof(id)) 
{
    if(tag==tagof(_:)) 
    {        
        if(!IsValidVehicle(id))
        {
            return false;
        }
        Vehicle_gsParams[id][PARAMS_BONNET] = bonnet_state;
        SetVehicleParamsEx(id, Vehicle_gsParams[id][PARAMS_ENGINE], Vehicle_gsParams[id][PARAMS_LIGHTS], Vehicle_gsParams[id][PARAMS_ALARM], Vehicle_gsParams[id][PARAMS_DOORS], Vehicle_gsParams[id][PARAMS_BONNET], Vehicle_gsParams[id][PARAMS_BOOT], Vehicle_gsParams[id][PARAMS_OBJECTIVE]);
    }
    else if(tag==tagof(VehicleGroup:))
    {
        foreach(new i : Vehicle_gsGroups[VehicleGroup: id])
        {
            Vehicle_gsParams[i][PARAMS_BONNET] = bonnet_state;
            SetVehicleParamsEx(i, Vehicle_gsParams[i][PARAMS_ENGINE], Vehicle_gsParams[i][PARAMS_LIGHTS], Vehicle_gsParams[i][PARAMS_ALARM], Vehicle_gsParams[i][PARAMS_DOORS], Vehicle_gsParams[i][PARAMS_BONNET], Vehicle_gsParams[i][PARAMS_BOOT], Vehicle_gsParams[i][PARAMS_OBJECTIVE]);
        }
    }
    return true;
}


// --
// Objective
// --

/**
 * <summary> Gets the objective state.</summary>
 * <param name="vehicleid"> Vehicle from which we get Objective state.</param>
 * <returns> Objective state. </returns>
 */

stock VEHICLE_PARAMS: Vehicle_GetObjectiveState(vehicleid) 
{
    return Vehicle_gsParams[vehicleid][PARAMS_OBJECTIVE];
}

/**
 * <summary> Sets the Objective state.</summary>
 * <param name="vehicleid"> Vehicle which we set the Objective state.</param>
 * <param name="engine_state"> Objective state to set.</param>
 * <returns> SetVehicleParamsEx values. </returns>
*/

stock bool: Vehicle_SetObjectiveState(VEHICLE_FRAMEWORK_TAGS: id, VEHICLE_PARAMS: objective_state, tag=tagof(id)) 
{
    if(tag==tagof(_:)) 
    {        
        if(!IsValidVehicle(vehicleid))
        {
            return false;
        }
        Vehicle_gsParams[id][PARAMS_OBJECTIVE] = objective_state;
        SetVehicleParamsEx(id, Vehicle_gsParams[id][PARAMS_ENGINE], Vehicle_gsParams[id][PARAMS_LIGHTS], Vehicle_gsParams[id][PARAMS_ALARM], Vehicle_gsParams[id][PARAMS_DOORS], Vehicle_gsParams[id][PARAMS_BONNET], Vehicle_gsParams[id][PARAMS_BOOT], Vehicle_gsParams[id][PARAMS_OBJECTIVE]);
    }
    else if(tag==tagof(VehicleGroup:))
    {
        foreach(new i : Vehicle_gsGroups[VehicleGroup: id])
        {
            Vehicle_gsParams[i][PARAMS_OBJECTIVE] = objective_state;
            SetVehicleParamsEx(i, Vehicle_gsParams[i][PARAMS_ENGINE], Vehicle_gsParams[i][PARAMS_LIGHTS], Vehicle_gsParams[i][PARAMS_ALARM], Vehicle_gsParams[i][PARAMS_DOORS], Vehicle_gsParams[i][PARAMS_BONNET], Vehicle_gsParams[i][PARAMS_BOOT], Vehicle_gsParams[i][PARAMS_OBJECTIVE]);
        }
    }
    return true;
}

// --
// Lights
// --

static stock decode_lights(VEHICLE_LIGHT_STATUS: lights, &front_left_light, &front_right_light, &back_lights)
{
    front_left_light = _:lights & 1;
    front_right_light = _:lights >> 2 & 1;
    back_lights = _:lights >> 6 & 1;
}

static stock encode_lights(front_left_light, front_right_light, back_lights)
{
    return front_left_light | (front_right_light << 2) | (back_lights << 6);
}

#define VEHICLE_LIGHT: __TAG(VEHICLE_LIGHT):
enum VEHICLE_LIGHT:__VEHICLE_LIGHT
{
    VEHICLE_LIGHT_FRONT_LEFT, 
    VEHICLE_LIGHT_FRONT_RIGHT,
    VEHICLE_LIGHT_BACK
}
static stock VEHICLE_LIGHT:_@VEHICLE_LIGHT() { return __VEHICLE_LIGHT; }

#define VEHICLE_LIGHT_CONDITION: bool:

const 
    bool: VEHICLE_LIGHT_CONDITION_DAMAGED = true,
    bool: VEHICLE_LIGHT_CONDITION_FIXED = false;

/** 
 * <library>vehicle_plus</library>
 * <summary>Gets the number of lights of specified vehicle.</summary>
 * <param name="vehicleid">Vehicle which we get number of lights from.</param>
 * <seealso name="Vehicle_SetLightsCondition" />
 * <seealso name="Vehicle_GetLightsCondition" />
 * <seealso name="Vehicle_SetLightsConditionEx" />
 * <seealso name="Vehicle_GetLightsConditionEx" />
 * <seealso name="Vehicle_SetLightsState" />
 * <seealso name="Vehicle_GetLightsState" />
 * <returns>Number of lights. 0 if specified vehicle is invalid.</returns>
 * <remarks>Returns number of lights which can be modified. </remarks>
 */
stock Vehicle_GetLightsNumber(vehicleid)
{
    return ((IsValidVehicle(vehicleid)) ? (Vehicle_Info[GetVehicleModel(vehicleid) - 400][E_VEHICLE_LIGHTS_NUMBER]) : (INVALID_VEHICLE_ID));
}

/** 
 * <library>vehicle_plus</library>
 * <summary>Sets the condition of all lights.</summary>
 * <param name="vehicleid">Vehicle which we set the lights' condition.</param>
 * <param name="front_left">Left Light condition to set.</param>
 * <param name="front_right">Right Light condition to set.</param>
 * <param name="back">Back Ligtht condition to set.</param>
 * <seealso name="Vehicle_SetLightsCondition" />
 * <seealso name="Vehicle_GetLightsCondition" />
 * <seealso name="Vehicle_GetLightsNumber" />
 * <seealso name="Vehicle_GetLightsConditionEx" />
 * <seealso name="Vehicle_SetLightsState" />
 * <seealso name="Vehicle_GetLightsState" />
 * <returns> UpdateVehicleDamageStatus values. </returns>
 */
stock bool: Vehicle_SetLightsConditionEx(VEHICLE_FRAMEWORK_TAGS: id, VEHICLE_LIGHT_CONDITION: front_left, VEHICLE_LIGHT_CONDITION: front_right, VEHICLE_LIGHT_CONDITION: back, tag=tagof(id))
{
    new 
        VEHICLE_LIGHT_STATUS: lights, 
        VEHICLE_DOOR_STATUS: unnecessary_doors,
        VEHICLE_PANEL_STATUS: unnecessary_panels, 
        VEHICLE_TYRE_STATUS: unnecessary_tires;
    
    if(tag == tagof(_:))
    {
        if(!IsValidVehicle(id))
        {
            return false;
        }
        GetVehicleDamageStatus(id, unnecessary_panels, unnecessary_doors, lights, unnecessary_tires);
        UpdateVehicleDamageStatus(id, unnecessary_panels, unnecessary_doors, VEHICLE_LIGHT_STATUS: encode_lights(front_left, front_right, back), unnecessary_tires);
    }
    else if(tag==tagof(VehicleGroup:))
    {
        foreach(new i : Vehicle_gsGroups[VehicleGroup: id])
        {
            GetVehicleDamageStatus(id, unnecessary_panels, unnecessary_doors, lights, unnecessary_tires);
            UpdateVehicleDamageStatus(id, unnecessary_panels, unnecessary_doors, VEHICLE_LIGHT_STATUS: encode_lights(front_left, front_right, back), unnecessary_tires);
        }
    }
    return true;
}

/**
 * <library>vehicle_plus</library>
 * <summary>Sets the light condition.</summary>
 * <param name="vehicleid">Vehicle which we set the light condition.</param>
 * <param name="light">Light we modify.</param>
 * <param name="condition">Condition to set.</param>
 * <seealso name="Vehicle_GetLightsNumber" />
 * <seealso name="Vehicle_GetLightsCondition" />
 * <seealso name="Vehicle_SetLightsConditionEx" />
 * <seealso name="Vehicle_GetLightsConditionEx" />
 * <seealso name="Vehicle_SetLightsState" />
 * <seealso name="Vehicle_GetLightsState" />
 * <returns> UpdateVehicleDamageStatus values. </returns>
*/
stock Vehicle_SetLightsCondition(VEHICLE_FRAMEWORK_TAGS: id, VEHICLE_LIGHT: light, VEHICLE_LIGHT_CONDITION: condition, tag=tagof(id))
{
    new 
        VEHICLE_LIGHT_STATUS: lights, 
        VEHICLE_DOOR_STATUS: unnecessary_doors,
        VEHICLE_PANEL_STATUS: unnecessary_panels, 
        VEHICLE_TYRE_STATUS: unnecessary_tires;
    
    if(tag == tagof(_:))
    {
        if(!IsValidVehicle(id))
        {
            return false;
        }
        GetVehicleDamageStatus(id, unnecessary_panels, unnecessary_doors, lights, unnecessary_tires);
        switch(light)
        {
            case VEHICLE_LIGHT_FRONT_LEFT:
            {
                if(condition)
                {
                    lights |= CARLIGHT_DRIVER_BROKEN;
                }
                else
                {
                    lights &= ~CARLIGHT_DRIVER_BROKEN;
                }
                UpdateVehicleDamageStatus(id, unnecessary_panels, unnecessary_doors, lights, unnecessary_tires);
            }
            case VEHICLE_LIGHT_FRONT_RIGHT:
            {
                if(condition)
                {
                    lights |= CARLIGHT_PASSENGER_BROKEN; 
                }
                else
                {
                    lights &= ~CARLIGHT_PASSENGER_BROKEN;
                }
                UpdateVehicleDamageStatus(id, unnecessary_panels, unnecessary_doors, lights, unnecessary_tires);          
            } 
            case VEHICLE_LIGHT_BACK:
            {
                if(condition)
                {
                    lights |= CARLIGHT_REAR_BROKEN;
                }
                else
                {
                    lights &= ~CARLIGHT_REAR_BROKEN;
                }
                UpdateVehicleDamageStatus(id, unnecessary_panels, unnecessary_doors, lights, unnecessary_tires);
            }
        }
    }
    else if(tag==tagof(VehicleGroup:))
    {
        foreach(new i : Vehicle_gsGroups[VehicleGroup: id])
        {
            GetVehicleDamageStatus(i, unnecessary_panels, unnecessary_doors, lights, unnecessary_tires);
            switch(light)
            {
                case VEHICLE_LIGHT_FRONT_LEFT:
                {
                    if(condition)
                    {
                        lights |= CARLIGHT_DRIVER_BROKEN;
                    }
                    else
                    {
                        lights &= ~CARLIGHT_DRIVER_BROKEN;
                    }
                    UpdateVehicleDamageStatus(i, unnecessary_panels, unnecessary_doors, lights, unnecessary_tires);
                }
                case VEHICLE_LIGHT_FRONT_RIGHT:
                {
                    if(condition)
                    {
                        lights |= CARLIGHT_PASSENGER_BROKEN;
                    }
                    else
                    {
                        lights &= ~CARLIGHT_PASSENGER_BROKEN;
                    }
                    UpdateVehicleDamageStatus(i, unnecessary_panels, unnecessary_doors, lights, unnecessary_tires);          
                }
                case VEHICLE_LIGHT_BACK:
                {
                    if(condition)
                    {
                        lights |= CARLIGHT_REAR_BROKEN;
                    }
                    else
                    {
                        lights &= ~CARLIGHT_REAR_BROKEN;
                    }
                    UpdateVehicleDamageStatus(i, unnecessary_panels, unnecessary_doors, lights, unnecessary_tires);
                }
            }
        }
    }
    return true;
}

/**
 * <library>vehicle_plus</library>
 * <summary> Gets the lights condition.</summary>
 * <param name="vehicleid"> Vehicle from which we get the lights condition.</param>
 * <param name="light">Light condition to get.</param>
 * <seealso name="Vehicle_GetLightsNumber" />
 * <seealso name="Vehicle_GetLightsConditionEx" />
 * <seealso name="Vehicle_SetLightsConditionEx" />
 * <seealso name="Vehicle_SetLightsCondition" />
 * <seealso name="Vehicle_SetLightsState" />
 * <seealso name="Vehicle_GetLightsState" />
 * <returns>Returns -1 if vehicle doesn't exist, otherwise light state.</returns>
*/
stock VEHICLE_LIGHT_CONDITION: Vehicle_GetLightsCondition(vehicleid, VEHICLE_LIGHT: light)
{    
    if(!IsValidVehicle(vehicleid))
    {
        return VEHICLE_LIGHT_CONDITION: -1;
    }
    new 
        VEHICLE_LIGHT_STATUS: lights,
        VEHICLE_DOOR_STATUS: unnecessary_doors,
        VEHICLE_PANEL_STATUS: unnecessary_panels, 
        VEHICLE_TYRE_STATUS: unnecessary_tires;

    new 
        front_left,
        front_right,
        back;


    GetVehicleDamageStatus(vehicleid, unnecessary_panels, unnecessary_doors, lights, unnecessary_tires);

    decode_lights(lights, front_left, front_right, back);

    switch(light)
    {
        case VEHICLE_LIGHT_FRONT_LEFT:
        {
            return front_left;
        }
        case VEHICLE_LIGHT_FRONT_RIGHT:
        {
            return front_right;
        }
        case VEHICLE_LIGHT_BACK:
        {
            return back;
        }
    }
    return VEHICLE_LIGHT_CONDITION: -1;
}

/**
 * <library>vehicle_plus</library>
 * <summary> Gets the lights condition.</summary>
 * <param name="vehicleid"> Vehicle from which we get the lights condition.</param>
 * <param name="&left_lights"> Left Light condition to get.</param>
 * <param name="&right_lights"> Right Light condition to get.</param>
 * <param name="&back_lights"> Back Light condition to get.</param>
 * <seealso name="Vehicle_GetLightsNumber" />
 * <seealso name="Vehicle_GetLightsCondition" />
 * <seealso name="Vehicle_SetLightsConditionEx" />
 * <seealso name="Vehicle_SetLightsCondition" />
 * <returns> GetVehicleDamageStatus values. </returns>
*/
stock bool: Vehicle_GetLightsConditionEx(vehicleid, &VEHICLE_LIGHT_CONDITION: front_left, &VEHICLE_LIGHT_CONDITION: front_right, &VEHICLE_LIGHT_CONDITION: back) 
{    
    if(!IsValidVehicle(vehicleid))
    {
        return false;
    }
    new 
        VEHICLE_LIGHT_STATUS: lights,
        VEHICLE_DOOR_STATUS: unnecessary_doors,
        VEHICLE_PANEL_STATUS: unnecessary_panels, 
        VEHICLE_TYRE_STATUS: unnecessary_tires;

    GetVehicleDamageStatus(vehicleid, unnecessary_panels, unnecessary_doors, lights, unnecessary_tires);
    decode_lights(lights, front_left, front_right, back);
    return true;
}

/**
 * <library>vehicle_plus</library>
 * <summary> Sets the lights state.</summary>
 * <param name="vehicleid"> Vehicle which we set the lights running state.</param>
 * <param name="light_state"> Lights state to set.</param>
 * <seealso name="Vehicle_GetLightsNumber" />
 * <seealso name="Vehicle_GetLightsCondition" />
 * <seealso name="Vehicle_SetLightsConditionEx" />
 * <seealso name="Vehicle_GetLightsConditionEx" />
 * <seealso name="Vehicle_SetLightsCondition" />
 * <seealso name="Vehicle_GetLightsState" />
 * <returns> SetVehicleParamsEx values. </returns>
*/
stock bool: Vehicle_SetLightsState(VEHICLE_FRAMEWORK_TAGS: id, VEHICLE_PARAMS: light_state, tag=tagof(id)) 
{
    if(tag==tagof(_:)) 
    {        
        if(!IsValidVehicle(id))
        {
            return false;
        }
        Vehicle_gsParams[id][PARAMS_LIGHTS] = light_state;
        SetVehicleParamsEx(id, Vehicle_gsParams[id][PARAMS_ENGINE], Vehicle_gsParams[id][PARAMS_LIGHTS], Vehicle_gsParams[id][PARAMS_ALARM], Vehicle_gsParams[id][PARAMS_DOORS], Vehicle_gsParams[id][PARAMS_BONNET], Vehicle_gsParams[id][PARAMS_BOOT], Vehicle_gsParams[id][PARAMS_OBJECTIVE]);
    
    }
    else if(tag==tagof(VehicleGroup:))
    {
        foreach(new i : Vehicle_gsGroups[VehicleGroup: id])
        {
            if(Vehicle_GetLightsNumber(id) == 0)
            {
                continue;
            }
            Vehicle_gsParams[id][PARAMS_LIGHTS] = light_state;
            SetVehicleParamsEx(id, Vehicle_gsParams[id][PARAMS_ENGINE], Vehicle_gsParams[id][PARAMS_LIGHTS], Vehicle_gsParams[id][PARAMS_ALARM], Vehicle_gsParams[id][PARAMS_DOORS], Vehicle_gsParams[id][PARAMS_BONNET], Vehicle_gsParams[id][PARAMS_BOOT], Vehicle_gsParams[id][PARAMS_OBJECTIVE]);
        }
    }
    return true;
}

/**
 * <library>vehicle_plus</library>
 * <summary> Gets the lights state.</summary>
 * <param name="vehicleid"> Vehicle which we get the lights running state.</param>
 * <seealso name="Vehicle_GetLightsNumber" />
 * <seealso name="Vehicle_GetLightsCondition" />
 * <seealso name="Vehicle_SetLightsConditionEx" />
 * <seealso name="Vehicle_GetLightsConditionEx" />
 * <seealso name="Vehicle_SetLightsCondition" />
 * <seealso name="Vehicle_SetLightsState" />
 * <returns> VEHICLE_PARAMS_OFF and VEHICLE_PARAMS_ON for their light states respectively. </returns>
*/
stock VEHICLE_PARAMS: Vehicle_GetLightsState(vehicleid) 
{
    if(!IsValidVehicle(id))
    {
        return false;
    }
    return Vehicle_gsParams[vehicleid][PARAMS_LIGHTS];
}

// --
// Alarms
// --

/**
 * <library>vehicle_plus</library>
 * <summary> Sets the alarms state.</summary>
 * <param name="id"> Vehicle or group of vehicles which we set the alarms state.</param>
 * <param name="alarm_state"> State we set.</param>
 * <seealso name="Vehicle_GetAlarms" />
 * <returns> SetVehicleParamsEx values. </returns>
*/
stock bool: Vehicle_SetAlarms(VEHICLE_FRAMEWORK_TAGS: id, VEHICLE_PARAMS: alarm_state, tag=tagof(id)) 
{
    if(tag==tagof(_:)) 
    {      
        if(!IsValidVehicle(id))
        {
            return false;
        }
        Vehicle_gsParams[id][PARAMS_ALARM] = alarm_state;
        SetVehicleParamsEx(id, Vehicle_gsParams[id][PARAMS_ENGINE], Vehicle_gsParams[id][PARAMS_LIGHTS], Vehicle_gsParams[id][PARAMS_ALARM], Vehicle_gsParams[id][PARAMS_DOORS], Vehicle_gsParams[id][PARAMS_BONNET], Vehicle_gsParams[id][PARAMS_BOOT], Vehicle_gsParams[id][PARAMS_OBJECTIVE]);
    }
    else if(tag==tagof(VehicleGroup:))
    {
        foreach(new i : Vehicle_gsGroups[VehicleGroup: id])
        {
            Vehicle_gsParams[i][PARAMS_ALARM] = alarm_state;
            SetVehicleParamsEx(i, Vehicle_gsParams[i][PARAMS_ENGINE], Vehicle_gsParams[i][PARAMS_LIGHTS], Vehicle_gsParams[i][PARAMS_ALARM], Vehicle_gsParams[i][PARAMS_DOORS], Vehicle_gsParams[i][PARAMS_BONNET], Vehicle_gsParams[i][PARAMS_BOOT], Vehicle_gsParams[i][PARAMS_OBJECTIVE]);
        }
    }
    return true;
}

/**
 * <library>vehicle_plus</library>
 * <summary> Gets the alarms state.</summary>
 * <param name="vehicleid"> Vehicle from which we set the alarms state.</param>
 * <seealso name="Vehicle_GetAlarms" />
 * <returns> VEHICLE_PARAMS_OFF and VEHICLE_PARAMS_ON for their alarm states respectively. </returns>
*/
stock VEHICLE_PARAMS: Vehicle_GetAlarms(vehicleid) 
{
    return Vehicle_gsParams[vehicleid][PARAMS_ALARM];
}

// --
// Windows 
// --

#define VEHICLE_WINDOWS: bool:
#define VEHICLE_WINDOW_OPEN false
#define VEHICLE_WINDOW_CLOSED true

#define VEHICLE_WINDOW: __TAG(VEHICLE_WINDOW):
enum VEHICLE_WINDOW:__VEHICLE_WINDOW
{
    VEHICLE_WINDOWS: VEHICLE_WINDOW_FRONT_LEFT, 
    VEHICLE_WINDOWS: VEHICLE_WINDOW_FRONT_RIGHT,
    VEHICLE_WINDOWS: VEHICLE_WINDOW_REAR_LEFT,
    VEHICLE_WINDOWS: VEHICLE_WINDOW_REAR_RIGHT,
    VEHICLE_WINDOWS: VEHICLE_WINDOW_DRIVER = VEHICLE_WINDOWS: VEHICLE_WINDOW_FRONT_LEFT,
    VEHICLE_WINDOWS: VEHICLE_WINDOW_PASSENGER = VEHICLE_WINDOWS: VEHICLE_WINDOW_FRONT_RIGHT
}
static stock VEHICLE_WINDOW:_@VEHICLE_WINDOW() { return __VEHICLE_WINDOW; }

static
    Vehicle_gsWindows[MAX_VEHICLES][4];

/**
 * <library>vehicle_plus</library>
 * <summary> Gets the vehicle's number of windows.</summary>
 * <param name="vehicleid"> Vehicle which we get the number from.</param>
 * <seealso name="Vehicle_SetWindows" />
 * <seealso name="Vehicle_GetWindows" />
 * <seealso name="Vehicle_SetWindowsEx" />
 * <seealso name="Vehicle_GetWindowsEx" />
 * <returns>Number of windows specified vehicle has. INVALID_VEHICLE_ID if specified vehicle is invalid.</returns>
*/
stock Vehicle_GetWindowsNumber(vehicleid) 
{
    return (IsValidVehicle(vehicleid)) ? (Vehicle_Info[GetVehicleModel(vehicleid) - 400][E_VEHICLE_WINDOWS_NUMBER]) : (INVALID_VEHICLE_ID);
}

/**
 * <library>vehicle_plus</library>
 * <summary>Sets the window state.</summary>
 * <param name="id">Vehicle which we set the windows to.</param>
 * <param name="windows">Window we modify.</param>
 * <param name="window_status">Status to set.</param>
 * <seealso name="Vehicle_GetWindowsNumber" />
 * <seealso name="Vehicle_GetWindows" />
 * <seealso name="Vehicle_SetWindowsEx" />
 * <seealso name="Vehicle_GetWindowsEx" />
 * <returns>SetVehicleParamsCarWindows values.</returns>
*/
stock bool: Vehicle_SetWindows(VEHICLE_FRAMEWORK_TAGS: id, VEHICLE_WINDOW: window, VEHICLE_WINDOWS: window_status, tag=tagof(id))
{
    if(tag == tagof(_:))
    {
        if(!IsValidVehicle(id))
        {
            return false;
        }
        Vehicle_gsWindows[id][window] = window_status;
        SetVehicleParamsCarWindows(id, Vehicle_gsWindows[id][VEHICLE_WINDOW_DRIVER], Vehicle_gsWindows[id][VEHICLE_WINDOW_PASSENGER], Vehicle_gsWindows[id][VEHICLE_WINDOW_REAR_LEFT], Vehicle_gsWindows[id][VEHICLE_WINDOW_REAR_RIGHT]);
        return true;
    }
    else if(tag == tagof(VehicleGroup:))
    {
        foreach(new i : Vehicle_gsGroups[VehicleGroup: id])
        {
            Vehicle_gsWindows[i][window] = window_status;
            SetVehicleParamsCarWindows(i, Vehicle_gsWindows[i][VEHICLE_WINDOW_DRIVER], Vehicle_gsWindows[i][VEHICLE_WINDOW_PASSENGER], Vehicle_gsWindows[i][VEHICLE_WINDOW_REAR_LEFT], Vehicle_gsWindows[i][VEHICLE_WINDOW_REAR_RIGHT]);
        }
    }
    return true;
}

/**
 * <library>vehicle_plus</library>
 * <summary>Sets the window state.</summary>
 * <param name="vehicleid">Vehicle which we set the windows to.</param>
 * <param name="vehicleid">Vehicle which we set the windows to.</param>
 * <seealso name="Vehicle_GetWindowsNumber" />
 * <seealso name="Vehicle_SetWindows" />
 * <seealso name="Vehicle_SetWindowsEx" />
 * <seealso name="Vehicle_GetWindowsEx" />
 * <returns>Window state for specified window.</returns>
*/
stock VEHICLE_WINDOWS: Vehicle_GetWindows(vehicleid, VEHICLE_WINDOW: window)
{
    return Vehicle_gsWindows[vehicleid][window];
}

/**
 * <library>vehicle_plus</library>
 * <summary> Sets the vehicle's windows states.</summary>
 * <param name="id"> Vehicle or group which we set the windows' states.</param>
 * <param name="driver"> Driver-window state to set.</param>
 * <param name="passenger"> Passenger-window state to set.</param>
 * <param name="backleft"> Backleft-window state to set.</param>
 * <param name="backright"> Backright-window state to gset.</param>
 * <seealso name="Vehicle_GetWindowsNumber" />
 * <seealso name="Vehicle_SetWindows" />
 * <seealso name="Vehicle_GetWindows" />
 * <seealso name="Vehicle_GetWindowsEx" />
 * <returns>SetVehicleParamsCarWindows values.</returns>
*/
stock bool: Vehicle_SetWindowsEx(VEHICLE_FRAMEWORK_TAGS: id, VEHICLE_WINDOWS: driver, VEHICLE_WINDOWS: passenger, VEHICLE_WINDOWS: backleft, VEHICLE_WINDOWS: backright, tag=tagof(id)) 
{
    if(tag == tagof(_:))
    {
        if(!IsValidVehicle(id))
        {
            return false;
        }
        Vehicle_gsWindows[id][VEHICLE_WINDOW_DRIVER] = driver;
        Vehicle_gsWindows[id][VEHICLE_WINDOW_PASSENGER] = passenger;
        Vehicle_gsWindows[id][VEHICLE_WINDOW_REAR_LEFT] = backleft;
        Vehicle_gsWindows[id][VEHICLE_WINDOW_REAR_RIGHT] = backright;
        SetVehicleParamsCarWindows(id, Vehicle_gsWindows[id][VEHICLE_WINDOW_DRIVER], Vehicle_gsWindows[id][VEHICLE_WINDOW_PASSENGER],Vehicle_gsWindows[id][VEHICLE_WINDOW_REAR_LEFT], Vehicle_gsWindows[id][VEHICLE_WINDOW_REAR_RIGHT]);
        return true;
    }
    else if(tag == tagof(VehicleGroup:))
    {
        foreach(new i : Vehicle_gsGroups[VehicleGroup: id])
        {
            Vehicle_gsWindows[i][VEHICLE_WINDOW_DRIVER] = driver;
            Vehicle_gsWindows[i][VEHICLE_WINDOW_PASSENGER] = passenger;
            Vehicle_gsWindows[i][VEHICLE_WINDOW_REAR_LEFT] = backleft;
            Vehicle_gsWindows[i][VEHICLE_WINDOW_REAR_RIGHT] = backright;
            SetVehicleParamsCarWindows(i, Vehicle_gsWindows[i][VEHICLE_WINDOW_DRIVER], Vehicle_gsWindows[i][VEHICLE_WINDOW_PASSENGER], Vehicle_gsWindows[i][VEHICLE_WINDOW_REAR_LEFT], Vehicle_gsWindows[i][VEHICLE_WINDOW_REAR_RIGHT]);
        }
    }
    return true;
}

/**
 * <library>vehicle_plus</library>
 * <summary> Gets the vehicle's windows states. </summary>
 * <param name="vehicleid"> Vehicle from which we get the vehicle plate.</param>
 * <param name="&driver"> Driver-window state to get.</param>
 * <param name="&passenger"> Passenger-window state to get.</param>
 * <param name="&backleft"> Backleft-window state to get.</param>
 * <param name="&backright"> Backright-window state to get.</param>
 * <seealso name="Vehicle_GetWindowsNumber" />
 * <seealso name="Vehicle_SetWindows" />
 * <seealso name="Vehicle_GetWindows" />
 * <seealso name="Vehicle_GetWindowsEx" />
 * <returns>GetVehicleParamsCarWindows values.</returns>
*/
stock bool: Vehicle_GetWindowsEx(vehicleid, &VEHICLE_WINDOWS: driver, &VEHICLE_WINDOWS: passenger, &VEHICLE_WINDOWS: backleft, &VEHICLE_WINDOWS: backright) 
{
    if(!IsValidVehicle(vehicleid))
    {
        return false;
    }

    driver = Vehicle_gsWindows[vehicleid][VEHICLE_WINDOW_DRIVER];
    passenger = Vehicle_gsWindows[vehicleid][VEHICLE_WINDOW_PASSENGER];
    backleft = Vehicle_gsWindows[vehicleid][VEHICLE_WINDOW_REAR_LEFT];
    backright = Vehicle_gsWindows[vehicleid][VEHICLE_WINDOW_REAR_RIGHT];

    return true;
}

// --
// Tires
// ---

#define VEHICLE_TYRE: __TAG(VEHICLE_TYRE):
enum VEHICLE_TYRE:__VEHICLE_TYRE
{
    VEHICLE_TYRE_FRONT_LEFT,
    VEHICLE_TYRE_FRONT_RIGHT,
    VEHICLE_TYRE_BACK_LEFT,
    VEHICLE_TYRE_BACK_RIGHT,
    VEHICLE_TYRE_BIKE_FRONT = VEHICLE_TYRE_FRONT_RIGHT,
    VEHICLE_TYRE_BIKE_BACK = VEHICLE_TYRE_BACK_RIGHT
}
static stock VEHICLE_TYRE:_@VEHICLE_TYRE() { return __VEHICLE_TYRE; }

#define VEHICLE_TYRE_CONDITION: bool:
#define VEHICLE_TYRE_POPPED true
#define VEHICLE_TYRE_INFLATED false

static 
    bool: Vehicle_gsTyrePoppingGlobal = true;

/**
 * <library>vehicle_plus</library>
 * <summary>Sets tyre popping.</summary>
 * <param name="set">Popping, true or false, yes or no.</param>
 * <seealso name="Vehicle_SetTyreConditionEx" />
 * <seealso name="Vehicle_GetTyreCondition" />
 * <seealso name="Vehicle_GetTyreConditionEx" />
 * <seealso name="Vehicle_GetTyrePoppingGlobal" />
 * <returns>No specific value.</returns>
*/
stock void: Vehicle_SetTyrePoppingGlobal(bool: set)
{
    Vehicle_gsTyrePoppingGlobal = set;
}

/**
 * <library>vehicle_plus</library>
 * <summary>Sets tyre popping.</summary>
 * <seealso name="Vehicle_SetTyreConditionEx" />
 * <seealso name="Vehicle_GetTyreCondition" />
 * <seealso name="Vehicle_GetTyreConditionEx" />
 * <seealso name="Vehicle_SetTyrePoppingGlobal" />
 * <returns>True if enabled, otherwise false.</returns>
*/
stock bool: Vehicle_GetTyrePoppingGlobal()
{
    return Vehicle_gsTyrePoppingGlobal;
}

/**
 * <library>vehicle_plus</library>
 * <summary> Sets the state for a tyre.</summary>
 * <param name="id"> Vehicle or group which we set the tires state.</param>
 * <param name="tyre">Tyre to modify.</param>
 * <param name="condition">Condition to set.</param>
 * <seealso name="Vehicle_SetTyreConditionEx" />
 * <seealso name="Vehicle_GetTyreCondition" />
 * <seealso name="Vehicle_GetTyreConditionEx" />
 * <seealso name="Vehicle_SetTyrePoppingGlobal" />
 * <seealso name="Vehicle_GetTyrePoppingGlobal" />
 * <returns>UpdateVehicleDamageStatus values. </returns>
*/
stock bool: Vehicle_SetTyreCondition(VEHICLE_FRAMEWORK_TAGS: id, VEHICLE_TYRE: tyre, VEHICLE_TYRE_CONDITION: condition, tag=tagof(id)) 
{    
    new
        VEHICLE_DOOR_STATUS: unnecessary_doors,
        VEHICLE_LIGHT_STATUS: unnecessary_lights,
        VEHICLE_PANEL_STATUS: unnecessary_panels, 
        VEHICLE_TYRE_STATUS: tires;

    new 
        front_left,
        front_right,
        back_left,
        back_right;

    if(tag == tagof(_:)) 
    {
        if(!IsValidVehicle(id))
        {
            return false;
        }
        GetVehicleDamageStatus(id, unnecessary_panels, unnecessary_doors, unnecessary_lights, tires);
        decode_tires(tires, back_right, front_right, back_left, front_left);
        switch(tyre)
        {
            case VEHICLE_TYRE_FRONT_LEFT:
            {
                front_left = condition;
            }
            case VEHICLE_TYRE_FRONT_RIGHT:
            {
                front_right = condition;
            }
            case VEHICLE_TYRE_BACK_LEFT:
            {
                back_left = condition;
            }
            case VEHICLE_TYRE_BACK_RIGHT:
            {
                back_right = condition;
            }
        }
        UpdateVehicleDamageStatus(id, unnecessary_panels, unnecessary_doors, unnecessary_lights, VEHICLE_TYRE_STATUS: encode_tires(back_right, front_right, back_left, front_left));
    }
    else if(tag == tagof(VehicleGroup:))
    {
        foreach(new i : Vehicle_gsGroups[VehicleGroup: id])
        {
            GetVehicleDamageStatus(id, unnecessary_panels, unnecessary_doors, unnecessary_lights, tires);
            decode_tires(tires, back_right, front_right, back_left, front_left);
            switch(tyre)
            {
                case VEHICLE_TYRE_FRONT_LEFT:
                {
                    front_left = condition;
                }
                case VEHICLE_TYRE_FRONT_RIGHT:
                {
                    front_right = condition;
                }
                case VEHICLE_TYRE_BACK_LEFT:
                {
                    back_left = condition;
                }
                case VEHICLE_TYRE_BACK_RIGHT:
                {
                    back_right = condition;
                }
            }
            UpdateVehicleDamageStatus(id, unnecessary_panels, unnecessary_doors, unnecessary_lights, VEHICLE_TYRE_STATUS: encode_tires(back_right, front_right, back_left, front_left));
        }
    }
    return true;
}

/**
 * <library>vehicle_plus</library>
 * <summary>Sets the tires state.</summary>
 * <param name="vehicleid">Vehicle which we set the tires state.</param>
 * <param name="rear_right_tire">Rear Right Tire state to set.</param>
 * <param name="front_right_tire">Front Right Tire state to set.</param>
 * <param name="rear_left_tire">Rear Left Tire state to set.</param>
 * <param name="front_left_tire">Front Left Tire state to set.</param>
 * <seealso name="Vehicle_SetTyreCondition" />
 * <seealso name="Vehicle_GetTyreCondition" />
 * <seealso name="Vehicle_GetTyreConditionEx" />
 * <seealso name="Vehicle_SetTyrePoppingGlobal" />
 * <seealso name="Vehicle_GetTyrePoppingGlobal" />
 * <returns>UpdateVehicleDamageStatus values. </returns>
*/
stock bool: Vehicle_SetTyreConditionEx(VEHICLE_FRAMEWORK_TAGS: id, VEHICLE_TYRE_CONDITION: rear_right, VEHICLE_TYRE_CONDITION: front_right, VEHICLE_TYRE_CONDITION: rear_left, VEHICLE_TYRE_CONDITION: front_left, tag=tagof(id)) 
{    
    new
        VEHICLE_DOOR_STATUS: unnecessary_doors,
        VEHICLE_LIGHT_STATUS: unnecessary_lights,
        VEHICLE_PANEL_STATUS: unnecessary_panels, 
        VEHICLE_TYRE_STATUS: tires;

    if(tag == tagof(_:)) 
    {
        if(!IsValidVehicle(id))
        {
            return false;
        }
        GetVehicleDamageStatus(id, unnecessary_panels, unnecessary_doors, unnecessary_lights, tires);
        UpdateVehicleDamageStatus(id, unnecessary_panels, unnecessary_doors, unnecessary_lights, VEHICLE_TYRE_STATUS: encode_tires(rear_right, front_right, rear_left, front_left));
    }
    else if(tag == tagof(VehicleGroup:))
    {
        foreach(new i : Vehicle_gsGroups[VehicleGroup: id])
        {
            GetVehicleDamageStatus(id, unnecessary_panels, unnecessary_doors, unnecessary_lights, tires);
            UpdateVehicleDamageStatus(id, unnecessary_panels, unnecessary_doors, unnecessary_lights, VEHICLE_TYRE_STATUS: encode_tires(rear_right, front_right, rear_left, front_left));
        }
    }
    return true;
}

/**
 * <library>vehicle_plus</library>
 * <summary>Gets the state of a tire.</summary>
 * <param name="vehicleid"> Vehicle from which we get the tire state.</param>
 * <param name="tyre">Tyre we get state of.</param>
 * <seealso name="Vehicle_SetTyreConditionEx" />
 * <seealso name="Vehicle_SetTyreCondition" />
 * <seealso name="Vehicle_GetTyreConditionEx" />
 * <seealso name="Vehicle_SetTyrePoppingGlobal" />
 * <seealso name="Vehicle_GetTyrePoppingGlobal" />
 * <returns>-1 if vehicle is invalid, otherwise true.</returns>
*/
stock VEHICLE_TYRE_CONDITION: Vehicle_GetTyreCondition(vehicleid, VEHICLE_TYRE: tyre) 
{    
    if(!IsValidVehicle(vehicleid))
    {
        return VEHICLE_TYRE_CONDITION: -1;
    }
    new 
        VEHICLE_DOOR_STATUS: unnecessary_doors,
        VEHICLE_LIGHT_STATUS: unnecessary_lights,
        VEHICLE_PANEL_STATUS: unnecessary_panels, 
        VEHICLE_TYRE_STATUS: tires,

        rear_right_tire,
        front_right_tire,
        rear_left_tire,
        front_left_tire;

    GetVehicleDamageStatus(vehicleid, unnecessary_panels, unnecessary_doors, unnecessary_lights, tires);
    decode_tires(tires, rear_right_tire, front_right_tire, rear_left_tire, front_left_tire);

    switch(tyre)
    {
        case VEHICLE_TYRE_FRONT_LEFT:
        {
            return front_left_tire;
        }
        case VEHICLE_TYRE_FRONT_RIGHT:
        {
            return front_right_tire;
        }
        case VEHICLE_TYRE_BACK_LEFT:
        {
            return rear_left_tire;
        }
        case VEHICLE_TYRE_BACK_RIGHT:
        {
            return rear_right_tire;
        }
    }
    return VEHICLE_TYRE_CONDITION: -1;
}

/**
 * <library>vehicle_plus</library>
 * <summary>Gets the tires state.</summary>
 * <param name="vehicleid"> Vehicle from which we get the tires state.</param>
 * <param name="&rear_right_tire"> Rear Right Tire state to get.</param>
 * <param name="&front_right_tire"> Front Right Tire state to get.</param>
 * <param name="&rear_left_tire"> Rear Left Tire state to get.</param>
 * <param name="&front_left_tire"> Front Left Tire state to get.</param>
 * <seealso name="Vehicle_SetTyreConditionEx" />
 * <seealso name="Vehicle_GetTyreCondition" />
 * <seealso name="Vehicle_SetTyreCondition" />
 * <seealso name="Vehicle_SetTyrePoppingGlobal" />
 * <seealso name="Vehicle_GetTyrePoppingGlobal" />
 * <returns>GetVehicleDamageStatus values.</returns>
*/
stock bool: Vehicle_GetTyreConditionEx(vehicleid, &VEHICLE_TYRE_CONDITION: rear_right_tire, &VEHICLE_TYRE_CONDITION: front_right_tire, &VEHICLE_TYRE_CONDITION: rear_left_tire, &VEHICLE_TYRE_CONDITION: front_left_tire) 
{    
    if(!IsValidVehicle(vehicleid))
    {
        return false;
    }
    new 
        VEHICLE_DOOR_STATUS: unnecessary_doors,
        VEHICLE_LIGHT_STATUS: unnecessary_lights,
        VEHICLE_PANEL_STATUS: unnecessary_panels, 
        VEHICLE_TYRE_STATUS: tires;

    GetVehicleDamageStatus(vehicleid, unnecessary_panels, unnecessary_doors, unnecessary_lights, tires);
    decode_tires(tires, rear_right_tire, front_right_tire, rear_left_tire, front_left_tire);
    

    return true;
}

static stock decode_tires(tires, &rear_right_tire, &front_right_tire, &rear_left_tire, &front_left_tire)
{
    rear_right_tire = tires & 1;
    front_right_tire = tires >> 1 & 1;
    rear_left_tire = tires >> 2 & 1;
    front_left_tire = tires >> 3 & 1;
}

static stock encode_tires(rear_right_tire, front_right_tire, rear_left_tire, front_left_tire)
{
    return rear_right_tire | (front_right_tire << 1) | (rear_left_tire << 2) | (front_left_tire << 3);
}

hook OnVehicleDamageStatusUpdate(vehicleid, playerid)
{
    if(!Vehicle_gsTyrePoppingGlobal)
    {
        new 
            VEHICLE_TYRE_CONDITION: front_left,
            VEHICLE_TYRE_CONDITION: back_left,
            VEHICLE_TYRE_CONDITION: front_right,
            VEHICLE_TYRE_CONDITION: back_right;

        Vehicle_GetTyreConditionEx(vehicleid, back_right, front_right, back_left, front_left);

        if((back_right, front_right, back_left, front_left) == VEHICLE_TYRE_POPPED)
        {
            back_right = VEHICLE_TYRE_INFLATED,
            front_right = VEHICLE_TYRE_INFLATED, 
            back_left = VEHICLE_TYRE_INFLATED, 
            front_left = VEHICLE_TYRE_INFLATED;

            Vehicle_SetTyreConditionEx(vehicleid, back_right, front_right, back_left, front_left);
        }
    }
    return Y_HOOKS_CONTINUE_RETURN_1;
}

// ---
// Doors
// ---

#define VEHICLE_DOORS: __TAG(VEHICLE_DOORS):
enum VEHICLE_DOORS:__VEHICLE_DOORS
{
    VEHICLE_DOOR_HOOD = 1,
    VEHICLE_DOOR_TRUNK,
    VEHICLE_DOOR_DRIVER,
    VEHICLE_DOOR_PASSENGER
}
static stock VEHICLE_DOORS:_@VEHICLE_DOORS() { return __VEHICLE_DOORS; }

#define VEHICLE_DOOR_CONDITION: __TAG(VEHICLE_DOOR_CONDITION):
enum VEHICLE_DOOR_CONDITION:__VEHICLE_DOOR_CONDITION
{
    VEHICLE_DOOR_CONDITION_NORMAL = 0,
    VEHICLE_DOOR_CONDITION_OPEN = 1,
    VEHICLE_DOOR_CONDITION_DAMAGED = 2,
    VEHICLE_DOOR_CONDITION_OPEN_DAMAGED,
    VEHICLE_DOOR_CONDITION_REMOVED
}
static stock VEHICLE_DOOR_CONDITION:_@VEHICLE_DOOR_CONDITION() { return __VEHICLE_DOOR_CONDITION; }

//--
static stock encode_doors(bonnet, boot, driver_door, passenger_door)
{
    return bonnet | (boot << 8) | (driver_door << 16) | (passenger_door << 24);
}


static stock decode_doors(VEHICLE_DOOR_STATUS: doors, &bonnet, &boot, &driver_door, &passenger_door) 
{
    bonnet = _:doors & 7;
    boot = _:doors >> 8 & 7;
    driver_door = _:doors >> 16 & 7;
    passenger_door = _:doors >> 24 & 7;
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

stock bool: Vehicle_SetDoorCondition(VEHICLE_FRAMEWORK_TAGS: id, VEHICLE_DOORS: door, VEHICLE_DOOR_CONDITION: condition, tag=tagof(id)) 
{

    new
        VEHICLE_DOOR_STATUS: doors,
        VEHICLE_LIGHT_STATUS: unnecessary_lights,
        VEHICLE_PANEL_STATUS: unnecessary_panels, 
        VEHICLE_TYRE_STATUS: unnecessary_tires;

    new hood,
        trunk, 
        left_door,
        right_door;
    
    if(tag == tagof(_:)) 
    {
        if(!IsValidVehicle(id))
        {
            return false;
        }
        GetVehicleDamageStatus(id, unnecessary_panels, doors, unnecessary_lights, unnecessary_tires);
        decode_doors(doors, hood, trunk, left_door, right_door);
        switch(door)
        {
            case VEHICLE_DOOR_HOOD:
            {
                hood = condition;
            }
            case VEHICLE_DOOR_TRUNK:
            {
                trunk = condition;
            }
            case VEHICLE_DOOR_DRIVER:
            {
                left_door = condition;
            }
            case VEHICLE_DOOR_PASSENGER:
            {
                right_door = condition;
            }
        }
        UpdateVehicleDamageStatus(id, unnecessary_panels, VEHICLE_DOOR_STATUS: encode_doors(hood, trunk, left_door, right_door), unnecessary_lights, unnecessary_tires);
    }
    else if(tag == tagof(VehicleGroup:))
    {
        foreach(new i : Vehicle_gsGroups[VehicleGroup: id])
        {
            GetVehicleDamageStatus(i, unnecessary_panels, doors, unnecessary_lights, unnecessary_tires);
            decode_doors(doors, hood, trunk, left_door, right_door);
            switch(door)
            {
                case VEHICLE_DOOR_HOOD:
                {
                    hood = condition;
                }
                case VEHICLE_DOOR_TRUNK:
                {
                    trunk = condition;
                }
                case VEHICLE_DOOR_DRIVER:
                {
                    left_door = condition;
                }
                case VEHICLE_DOOR_PASSENGER:
                {
                    right_door = condition;
                }
            }
            UpdateVehicleDamageStatus(i, unnecessary_panels, VEHICLE_DOOR_STATUS: encode_doors(hood, trunk, left_door, right_door), unnecessary_lights, unnecessary_tires);
        } 
    }
    return true;
}

/**
 * <summary> Gets the doors state.</summary>
 * <param name="vehicleid"> Vehicle from which we get the doors state.</param>
 * <param name="&hood"> Hood state to get.</param>
 * <param name="&trunk"> Trunk state to get.</param>
 * <param name="&driver"> Driver-door state to get.</param>
 * <param name="&passenger"> Passenger-door state to get.</param>
*/

stock Vehicle_GetDoorCondition(vehicleid, &VEHICLE_DOOR_CONDITION: bonnet, &VEHICLE_DOOR_CONDITION: boot, &VEHICLE_DOOR_CONDITION: driver, &VEHICLE_DOOR_CONDITION: passenger) 
{
    if(!IsValidVehicle(vehicleid))
    {
        return false;
    }
    new 
        VEHICLE_DOOR_STATUS: doors,
        VEHICLE_LIGHT_STATUS: unnecessary_lights,
        VEHICLE_PANEL_STATUS: unnecessary_panels, 
        VEHICLE_TYRE_STATUS: unnecessary_tires;
    
    GetVehicleDamageStatus(vehicleid, unnecessary_panels, doors, unnecessary_lights, unnecessary_tires);
    decode_doors(doors, bonnet, boot, driver, passenger);
    return true;
}

/**
 * <summary> Gets the doors' lock state.</summary>
 * <param name="vehicleid"> Vehicle from which we set the doors' lock state.</param>
 * <returns> Doors' lock state. </returns>
*/

stock VEHICLE_PARAMS: Vehicle_GetDoorsLocked(vehicleid) 
{
    return Vehicle_gsParams[vehicleid][PARAMS_DOORS];
}

/**
 * <summary> Sets the alarms state.</summary>
 * <param name="vehicleid"> Vehicle which we set the doors' lock state.</param>
 * <param name="door_state"> Door's lock state to set. </param>
 * <returns> SetVehicleParamsEx values. </returns>
*/

stock Vehicle_SetDoorsLocked(VEHICLE_FRAMEWORK_TAGS: id, VEHICLE_PARAMS: door_state, tag=tagof(id)) 
{
    if(tag==tagof(_:)) 
    {        
        if(!IsValidVehicle(id))
        {
            return false;
        }
        Vehicle_gsParams[id][PARAMS_DOORS] = door_state;
        SetVehicleParamsEx(id, Vehicle_gsParams[id][PARAMS_ENGINE], Vehicle_gsParams[id][PARAMS_LIGHTS], Vehicle_gsParams[id][PARAMS_ALARM], Vehicle_gsParams[id][PARAMS_DOORS], Vehicle_gsParams[id][PARAMS_BONNET], Vehicle_gsParams[id][PARAMS_BOOT], Vehicle_gsParams[id][PARAMS_OBJECTIVE]);
    }
    else if(tag==tagof(VehicleGroup:))
    {
        foreach(new i : Vehicle_gsGroups[VehicleGroup: id])
        {
            Vehicle_gsParams[i][PARAMS_DOORS] = door_state;
            SetVehicleParamsEx(i, Vehicle_gsParams[i][PARAMS_ENGINE], Vehicle_gsParams[i][PARAMS_LIGHTS], Vehicle_gsParams[i][PARAMS_ALARM], Vehicle_gsParams[i][PARAMS_DOORS], Vehicle_gsParams[i][PARAMS_BONNET], Vehicle_gsParams[i][PARAMS_BOOT], Vehicle_gsParams[i][PARAMS_OBJECTIVE]);
        }
    }
    return true;
}

// --
// Panels 
// --

/* Note: It seems that you can only read the value of the windshield. 
        Setting it does update the value on the server, 
        but it does not result 
        in any physical change on the vehicle.
*/

#define VEHICLE_PANEL: __TAG(VEHICLE_PANEL):
enum VEHICLE_PANEL:__VEHICLE_PANEL
{
    VEHICLE_PANEL_FRONT_LEFT = 1,
    VEHICLE_PANEL_FRONT_RIGHT,
    VEHICLE_PANEL_BACK_LEFT,
    VEHICLE_PANEL_BACK_RIGHT,
    VEHICLE_PANEL_WINDSHIELD,
    VEHICLE_PANEL_FRONT_BUMPER,
    VEHICLE_PANEL_BACK_BUMPER
}
static stock VEHICLE_PANEL:_@VEHICLE_PANEL() { return __VEHICLE_PANEL; }

#define VEHICLE_PANELS_CONDITION: __TAG(VEHICLE_PANELS_CONDITION):
enum VEHICLE_PANELS_CONDITION:__VEHICLE_PANELS_CONDITION
{
    VEHICLE_PANEL_UNDAMAGEDL = 0,
    VEHICLE_PANEL_CRUSHED = 1,
    VEHICLE_PANEL_HANGING_LOOSE = 2,
    VEHICLE_PANEL_REMOVED
}
static stock VEHICLE_PANELS_CONDITION:_@VEHICLE_PANELS_CONDITION() { return __VEHICLE_PANELS_CONDITION; }

static stock decode_panels(panels, &front_left_panel, &front_right_panel, &rear_left_panel, &rear_right_panel, &windshield, &front_bumper, &rear_bumper)
{
    front_left_panel = panels & 15;
    front_right_panel = panels >> 4 & 15;
    rear_left_panel = panels >> 8 & 15;
    rear_right_panel = panels >> 12 & 15;
    windshield = panels >> 16 & 15;
    front_bumper = panels >> 20 & 15;
    rear_bumper = panels >> 24 & 15;
}

static stock encode_panels(front_left_panel, front_right_panel, rear_left_panel, rear_right_panel, windshield, front_bumper, rear_bumper)
{
    return front_left_panel | (front_right_panel << 4) | (rear_left_panel << 8) | (rear_right_panel << 12) | (windshield << 16) | (front_bumper << 20) | (rear_bumper << 24);
}


/**
 * <summary> Sets the vehicle's panels' states. </summary>
 * <param name="vehicleid"> Id of vehicle we are setting panel states to.</param>
 * <param name="front_left_panel"> Front left panel state to set.</param>
 * <param name="front_right_panel"> Front right panel state to set.</param>
 * <param name="back_left_panel"> Back left panel state to set.</param>
 * <param name="back_right_panel"> Back right state to set.</param>
*/
stock bool: Vehicle_SetPanelsCondition(VEHICLE_FRAMEWORK_TAGS: id, VEHICLE_PANELS_CONDITION: front_left_panel, VEHICLE_PANELS_CONDITION: front_right_panel, VEHICLE_PANELS_CONDITION: back_left_panel, VEHICLE_PANELS_CONDITION: back_right_panel, VEHICLE_PANELS_CONDITION: front_bumper, VEHICLE_PANELS_CONDITION: back_bumper, tag=tagof(id)) 
{
    new
        VEHICLE_DOOR_STATUS: unnecessary_doors,
        VEHICLE_LIGHT_STATUS: unnecessary_lights,
        VEHICLE_PANEL_STATUS: panels, 
        VEHICLE_TYRE_STATUS: unnecessary_tires,
        
        unnecessary_windshield,
        unnecessary_decodes;

    if(tag == tagof(_:))
    {
        if(!IsValidVehicle(id))
        {
            return false;
        }
        GetVehicleDamageStatus(id, panels, unnecessary_doors, unnecessary_lights, unnecessary_tires);
        decode_panels(panels, unnecessary_decodes, unnecessary_decodes, unnecessary_decodes, unnecessary_decodes, unnecessary_windshield, unnecessary_decodes, unnecessary_decodes);
        UpdateVehicleDamageStatus(id, VEHICLE_PANEL_STATUS: encode_panels(front_left_panel, front_right_panel, back_left_panel, back_right_panel, unnecessary_windshield, front_bumper, back_bumper), unnecessary_doors, unnecessary_lights, unnecessary_tires);
    }
    else if(tag == tagof(VehicleGroup:))
    {
        foreach(new i : Vehicle_gsGroups[VehicleGroup: id])
        {
            GetVehicleDamageStatus(i, panels, unnecessary_doors, unnecessary_lights, unnecessary_tires);
            decode_panels(panels, unnecessary_decodes, unnecessary_decodes, unnecessary_decodes, unnecessary_decodes, unnecessary_windshield, unnecessary_decodes, unnecessary_decodes);
            UpdateVehicleDamageStatus(i, VEHICLE_PANEL_STATUS: encode_panels(front_left_panel, front_right_panel, back_left_panel, back_right_panel, unnecessary_windshield, front_bumper, back_bumper), unnecessary_doors, unnecessary_lights, unnecessary_tires);
        }
    }
    return true;
}

/**
 * <summary> Gets the vehicle's panels' states. </summary>
 * <param name="vehicleid"> Id of vehicle we are getting panel states from.</param>
 * <param name="&front_left_panel"> Front left panel state to get.</param>
 * <param name="&front_right_panel"> Front right panel state to get.</param>
 * <param name="&back_left_panel"> Back left panel state to get.</param>
 * <param name="&back_right_panel"> Back right state to get.</param>
*/
stock bool: Vehicle_GetPanelsCondition(vehicleid, &VEHICLE_PANELS_CONDITION: front_left_panel, &VEHICLE_PANELS_CONDITION: front_right_panel, &VEHICLE_PANELS_CONDITION: back_left_panel, &VEHICLE_PANELS_CONDITION: back_right_panel, &VEHICLE_PANELS_CONDITION: front_bumper, &VEHICLE_PANELS_CONDITION: back_bumper, &VEHICLE_PANELS_CONDITION: windshield) 
{
    new
        VEHICLE_DOOR_STATUS: unnecessary_doors,
        VEHICLE_LIGHT_STATUS: unnecessary_lights,
        VEHICLE_PANEL_STATUS: panels, 
        VEHICLE_TYRE_STATUS: unnecessary_tires;

    if(!IsValidVehicle(vehicleid))
    {
        return false;
    }
    GetVehicleDamageStatus(vehicleid, panels, unnecessary_doors, unnecessary_lights, unnecessary_tires);
    decode_panels(panels, front_left_panel, front_right_panel, back_left_panel, back_right_panel, front_bumper, back_bumper, windshield);
    return true;
}


// --
// Health
// --

forward OnVehicleHealthChange(vehicleid, Float: oldhealth, Float:newhealth);

timer __Vehicle_HealthTrack[500](vehicleid)
{
    new Float: healthveh;
    GetVehicleHealth(vehicleid, healthveh);
    if(healthveh != Vehicle_gsHealth[vehicleid]) 
    {
        CallLocalFunction("OnVehicleHealthChange", "iff", vehicleid, Vehicle_gsHealth[vehicleid], healthveh);
        Vehicle_gsHealth[vehicleid] = healthveh;
    }
}

/**
 * <summary> Sets vehicle health.</summary>
 * <param name="vehicleid"> Vehicle we set health to.</param>
 * <param name="health"> Health to set.</param>
 * <returns> 1 on success, 0 otherwise.</param>
*/

stock bool: Vehicle_SetHealth(vehicleid, Float:health)
{
    if(health < 0 || !IsValidVehicle(vehicleid)) 
    {
        return false;
    }
    Vehicle_gsHealth[vehicleid] = health;
    SetVehicleHealth(vehicleid, health);
    return true;
}

/**
 * <summary> Gets vehicle health.</summary>
 * <param name="vehicleid"> Vehicle we get health from.</param>
 * <param name="&health"> Optional variable to store value into.</param>
 * <returns> 0.0 on fail, else vehicle health or stores health in referred variable.</param>
*/

stock Float:Vehicle_GetHealth(vehicleid, &Float:health = 0.0)
{
    if(!IsValidVehicle(vehicleid)) 
    {
        return 0.0;
    }
    if(numargs() == 2)
    {
        return health = Vehicle_gsHealth[vehicleid];
    }
    else
    {
        return Vehicle_gsHealth[vehicleid];
    }
}

/**
 * <summary> Repairs vehicle. </summary>
 */

stock bool: Vehicle_Repair(vehicleid) 
{
    return RepairVehicle(vehicleid);
}

// --
// Plate
// --

/**
 * <library>vehicle_plus</library>
 * <summary> Gets the vehicle's number plate.</summary>
 * <param name="vehicleid"> Vehicle from which we get the vehicle plate.</param>
 * <param name="plate"> Plate to get.</param>
 * <param name="len"> Size of the plate.</param>
 * <seealso name="Vehicle_SetNumberPlate" />
 * <returns>INVALID_VEHICLE_ID if vehicle is invalid.</returns>
*/
stock Vehicle_GetNumberPlate(vehicleid, string: plate[], len = sizeof(plate)) 
{
    return IsValidVehicle(vehicleid) ? GetVehicleNumberPlate(vehicleid, plate, len) : INVALID_VEHICLE_ID;
}

/**
 * <library>vehicle_plus</library>
 * <summary> Sets the vehicle's number plate.</summary>
 * <param name="vehicleid"> Vehicle which we set the vehicle plate.</param>
 * <param name="plate"> Plate to set.</param>
 * <seealso name="Vehicle_GetNumberPlate" />
 * <returns>False if vehicle is invalid, otherwise true.</returns>
*/
stock bool: Vehicle_SetNumberPlate(VEHICLE_FRAMEWORK_TAGS: id, const string: plate[], tag=tagof(id)) 
{
    if(tag == tagof(_:))
    {
        if(!IsValidVehicle(id))
        {
            return false;
        }
        SetVehicleNumberPlate(id, plate);
    }
    else if(tag == tagof(VehicleGroup:))
    {
        foreach(new i : Vehicle_gsGroups[VehicleGroup: id])
        {
            SetVehicleNumberPlate(i, plate);
        } 
    }
    return true;
}

// --
// Components
// --

static const Vehicle_gscComponentName[MAX_VEHICLES_COMPONENTS][MAX_VEHICLE_COMPONENT_NAME] = 
{
	!"Pro Spoiler",
	!"Win Spoiler",
	!"Drag Spoiler",
	!"Alpha Spoiler",
	!"Champ Scoop Hood",
	!"Fury Scoop Hood",
	!"Roof Scoop",
	!"Right Sideskirt",
	!"5x Nitro",
	!"2x Nitro",
	!"10x Nitro",
	!"Race Scoop Hood",
	!"Worx Scoop Hood",
	!"Round Fog Lamps",
	!"Champ Spoiler",
	!"Race Spoiler",
	!"Worx Spoiler",
	!"Left Sideskirt",
	!"Upswept Exhaust",
	!"Twin Exhaust",
	!"Large Exhaust",
	!"Medium Exhaust",
	!"Small Exhaust",
	!"Fury Spoiler",
	!"Square Fog Lamps",
	!"Offroad Wheels",
	!"Right Alien Sideskirt \"Sultan\"",
	!"Left Alien Sideskirt \"Sultan\"",
	!"Alien Exhaust \"Sultan\"",
	!"X-Flow Exhaust \"Sultan\"",
	!"Left X-Flow Sideskirt \"Sultan\"",
	!"Right X-Flow Sideskirt \"Sultan\"",
	!"Alien Roof Vent \"Sultan\"",
	!"X-Flow Roof Vent \"Sultan\"",
	!"Alien Exhaust \"Elegy\"",
	!"X-Flow Roof Vent \"Elegy\"",
	!"Right Alien Sideskirt \"Elegy\"",
	!"X-Flow Exhaust \"Elegy\"",
	!"Alien Roof Vent \"Elegy\"",
	!"Right X-Flow Sideskirt \"Elegy\"",
	!"Left Alien Sideskirt \"Elegy\"",
	!"Right X-Flow Sideskirt \"Elegy\"",
	!"Right Chrome Sideskirt \"Broadway\"",
	!"Slamin Exhaust \"Broadway\"",
	!"Chrome Exhaust \"Broadway\"",
	!"X-Flow Exhaust \"Flash\"",
	!"Alien Exhaust \"Flash\"",
	!"Right Alien Sideskirt \"Flash\"",
	!"Right X-Flow Sideskirt \"Flash\"",
	!"Alien Spoiler \"Flash\"",
	!"X-Flow Spoiler \"Flash\"",
	!"Left Alien Sideskirt \"Flash\"",
	!"Left X-Flow Sideskirt \"Flash\"",
	!"X-Flow Roof \"Flash\"",
	!"Alien Roof \"Flash\"",
	!"Alien Roof \"Stratum\"",
	!"Right Alien Sideskirt \"Stratum\"",
	!"Right X-Flow Sideskirt \"Stratum\"",
	!"Alien Spoiler \"Stratum\"",
	!"X-Flow Exhaust \"Stratum\"",
	!"X-Flow Spoiler \"Stratum\"",
	!"X-Flow Roof \"Stratum\"",
	!"Left Alien Sideskirt \"Stratum\"",
	!"Left X-Flow Sideskirt \"Stratum\"",
	!"Alien Exhaust \"Stratum\"",
	!"Alien Exhaust \"Jester\"",
	!"X-Flow Exhaust \"Jester\"",
	!"Alien Roof \"Jester\"",
	!"X-Flow Roof \"Jester\"",
	!"Right Alien Sideskirt \"Jester\"",
	!"Right X-Flow Sideskirt \"Jester\"",
	!"Left Alien Sideskirt \"Jester\"",
	!"Left X-Flow Sideskirt \"Jester\"",
	!"Shadow Wheels",
	!"Mega Wheels",
	!"Rimshine Wheels",
	!"Wires Wheels",
	!"Classic Wheels",
	!"Twist Wheels",
	!"Cutter Wheels",
	!"Switch Wheels",
	!"Grove Wheels",
	!"Import Wheels",
	!"Dollar Wheels",
	!"Trance Wheels",
	!"Atomic Wheels",
	!"Stereo Sound",
	!"Hydraulics",
	!"Alien Roof \"Uranus\"",
	!"X-Flow Exhaust \"Uranus\"",
	!"Right Alien Sideskirt \"Uranus\"",
	!"X-Flow Roof \"Uranus\"",
	!"Alien Exhaust \"Uranus\"",
	!"Right X-Flow Sideskirt \"Uranus\"",
	!"Left Alien Sideskirt \"Uranus\"",
	!"Right X-Flow Sideskirt \"Uranus\"",
	!"Ahab Wheels",
	!"Virtual Wheels",
	!"Access Wheels",
	!"Left Chrome Sideskirt \"Broadway\"",
	!"Chrome Grill Bullbar \"Remington\"",
	!"Left \"Chrome Flames\" Sideskirt \"Remington\"",
	!"Left \"Chrome Strip\" Sideskirt \"Savanna\"",
	!"Corvetible Roof \"Blade\"",
	!"Chrome Exhaust \"Blade\"",
	!"Slamin Exhaust \"Blade\"",
	!"Right \"Chrome Arches\" Sideskirt \"Remington\"",
	!"Left \"Chrome Strip\" Sideskirt \"Blade\"",
	!"Right \"Chrome Strip\" Sideskirt \"Blade\"",
	!"Chrome Rear Bullbars \"Slamvan\"",
	!"Slamin Rear Bullbars \"Slamvan\"",
	!"Little Front Sign? \"Slamvan\"",
	!"Little Front Sign? \"Slamvan\"",
	!"Chrome Exhaust \"Slamvan\"",
	!"Slamin Exhaust \"Slamvan\"",
	!"Chrome Front Bullbars \"Slamvan\"",
	!"Slamin Front Bullbars \"Slamvan\"",
	!"Chrome Front Bumper \"Slamvan\"",
	!"Right \"Chrome Trim\" Sideskirt \"Slamvan\"",
	!"Right \"Wheelcovers\" Sideskirt \"Slamvan\"",
	!"Left \"Chrome Trim\" Sideskirt \"Slamvan\"",
	!"Left \"Wheelcovers\" Sideskirt \"Slamvan\"",
	!"Right \"Chrome Flames\" Sideskirt \"Remington\"",
	!"Bullbar Chrome Bars \"Remington\"",
	!"Left \"Chrome Arches\" Sideskirt \"Remington\"",
	!"Bullbar Chrome Lights \"Remongton\"",
	!"Chrome Exhaust \"Remington\"",
	!"Slamin Exhaust \"Remington\"",
	!"Vinyl Hardtop \"Blade\"",
	!"Chrome Exhaust \"Savanna\"",
	!"Hardtop Roof \"Savanna\"",
	!"Softtop Roof \"Savanna\"",
	!"Slamin Exhaust \"Savanna\"",
	!"Right \"Chrome Strip\" Sideskirt \"Savanna\"",
	!"Right \"Chrome Strip\" Sideskirt \"Blade\"",
	!"Slamin Exhaust \"Blade\"",
	!"Chrome Exhaust \"Blade\"",
	!"Left \"Chrome Strip\" Sideskirt \"Blade\"",
	!"Alien Spoiler \"Sultan\"",
	!"X-Flow Spoiler \"Sultan\"",
	!"X-Flow Rear Bumper \"Sultan\"",
	!"Alien Rear Bumper \"Sultan\"",
	!"Left Oval Vents",
	!"Right Oval Vents",
	!"Left Square Vents",
	!"Right Square Vents",
	!"X-Flow Spoiler \"Elegy\"",
	!"Alien Spoiler \"Elegy\"",
	!"X-Flow Rear Bumper \"Elegy\"",
	!"Alien Rear Bumper \"Elegy\"",
	!"Alien Rear Bumper \"Flash\"",
	!"X-Flow Rear Bumper \"Flash\"",
	!"X-Flow Front Bumper \"Flash\"",
	!"Alien Front Bumper \"Flash\"",
	!"Alien Rear Bumper \"Stratum\"",
	!"Alien Front Bumper \"Stratum\"",
	!"X-Flow Rear Bumper \"Stratum\"",
	!"X-Flow Front Bumper \"Stratum\"",
	!"X-Flow Spoiler \"Jester\"",
	!"Alien Rear Bumper \"Jester\"",
	!"Alien Front Bumper \"Jester\"",
	!"X-Flow Rear Bumper \"Jester\"",
	!"Alien Spoiler \"Jester\"",
	!"X-Flow Spoiler \"Uranus\"",
	!"Alien Spoiler \"Uranus\"",
	!"X-Flow Front Bumper \"Uranus\"",
	!"Alien Front Bumper \"Uranus\"",
	!"X-Flow Rear Bumper \"Uranus\"",
	!"Alien Rear Bumper \"Uranus\"",
	!"Alien Front Bumper \"Sultan\"",
	!"X-Flow Front Bumper \"Sultan\"",
	!"Alien Front Bumper \"Elegy\"",
	!"X-Flow Front Bumper \"Elegy\"",
	!"X-Flow Front Bumper \"Jester\"",
	!"Chrome Front Bumper \"Broadway\"",
	!"Slamin Front Bumper \"Broadway\"",
	!"Chrome Rear Bumper \"Broadway\"",
	!"Slamin Rear Bumper \"Broadway\"",
	!"Slamin Rear Bumper \"Remington\"",
	!"Chrome Front Bumper \"Remington\"",
	!"Chrome Rear Bumper \"Remington\"",
	!"Slamin Front Bumper \"Blade\"",
	!"Chrome Front Bumper \"Blade\"",
	!"Slamin Rear Bumper \"Blade\"",
	!"Chrome Rear Bumper \"Blade\"",
	!"Slamin Front Bumper \"Remington\"",
	!"Slamin Rear Bumper \"Savanna\"",
	!"Chrome Rear Bumper \"Savanna\"",
	!"Slamin Front Bumper \"Savanna\"",
	!"Chrome Front Bumper \"Savanna\"",
	!"Slamin Front Bumper \"Blade\"",
	!"Chrome Front Bumper \"Blade\"",
	!"Chrome Rear Bumper \"Blade\"",
	!"Slamin Rear Bumper \"Blade\""
};

static const Vehicle_gscComponents[MAX_VEHICLES_MODELS][MAX_VEHICLE_COMPONENTS] = 
{
	{1008, 1009, 1010, 1013, 1018, 1019, 1020, 1021, 1024, 1025, 1073, 1074, 1075, 1076, 1077, 1078, 1079, 1080, 1081, 1082, 1083, 1084, 1085, 1086, 1087, 1096, 1097, 1098, INVALID_VEHICLE_COMPONENT_ID, INVALID_VEHICLE_COMPONENT_ID, ...},
	{1001, 1003, 1004, 1005, 1006, 1007, 1008, 1009, 1010, 1013, 1017, 1019, 1020, 1025, 1073, 1074, 1075, 1076, 1077, 1078, 1079, 1080, 1081, 1082, 1083, 1084, 1085, 1086, 1087, 1096, 1097, 1098, 1142, 1143, 1144, 1145, INVALID_VEHICLE_COMPONENT_ID, INVALID_VEHICLE_COMPONENT_ID, ...},
	{1008, 1009, 1010, 1025, 1073, 1074, 1075, 1076, 1077, 1078, 1079, 1080, 1081, 1082, 1083, 1084, 1085, 1086, 1087, 1096, 1097, 1098, INVALID_VEHICLE_COMPONENT_ID, INVALID_VEHICLE_COMPONENT_ID, ...},
	{1008, 1009, 1010, 1025, 1073, 1074, 1075, 1076, 1077, 1078, 1079, 1080, 1081, 1082, 1083, 1084, 1085, 1086, 1087, 1096, 1097, 1098, INVALID_VEHICLE_COMPONENT_ID, INVALID_VEHICLE_COMPONENT_ID, ...},
	{1000, 1002, 1007, 1008, 1009, 1010, 1013, 1016, 1017, 1019, 1020, 1021, 1025, 1073, 1074, 1075, 1076, 1077, 1078, 1079, 1080, 1081, 1082, 1083, 1084, 1085, 1086, 1087, 1096, 1097, 1098, INVALID_VEHICLE_COMPONENT_ID, INVALID_VEHICLE_COMPONENT_ID, ...},
	{1000, 1001, 1008, 1009, 1010, 1014, 1018, 1019, 1020, 1021, 1023, 1025, 1073, 1074, 1075, 1076, 1077, 1078, 1079, 1080, 1081, 1082, 1083, 1084, 1085, 1086, 1087, 1096, 1097, 1098, INVALID_VEHICLE_COMPONENT_ID, INVALID_VEHICLE_COMPONENT_ID, ...},
	{1008, 1009, 1010, 1025, 1073, 1074, 1075, 1076, 1077, 1078, 1079, 1080, 1081, 1082, 1083, 1084, 1085, 1086, 1087, 1096, 1097, 1098, INVALID_VEHICLE_COMPONENT_ID, INVALID_VEHICLE_COMPONENT_ID, ...},
	{1008, 1009, 1010, 1025, 1073, 1074, 1075, 1076, 1077, 1078, 1079, 1080, 1081, 1082, 1083, 1084, 1085, 1086, 1087, 1096, 1097, 1098, INVALID_VEHICLE_COMPONENT_ID, INVALID_VEHICLE_COMPONENT_ID, ...},
	{1008, 1009, 1010, 1025, 1073, 1074, 1075, 1076, 1077, 1078, 1079, 1080, 1081, 1082, 1083, 1084, 1085, 1086, 1087, 1096, 1097, 1098, INVALID_VEHICLE_COMPONENT_ID, INVALID_VEHICLE_COMPONENT_ID, ...},
	{1008, 1009, 1010, 1025, 1073, 1074, 1075, 1076, 1077, 1078, 1079, 1080, 1081, 1082, 1083, 1084, 1085, 1086, 1087, 1096, 1097, 1098, INVALID_VEHICLE_COMPONENT_ID, INVALID_VEHICLE_COMPONENT_ID, ...},
	{1001, 1003, 1007, 1008, 1009, 1010, 1013, 1017, 1019, 1020, 1021, 1023, 1024, 1025, 1073, 1074, 1075, 1076, 1077, 1078, 1079, 1080, 1081, 1082, 1083, 1084, 1085, 1086, 1087, 1096, 1097, 1098, INVALID_VEHICLE_COMPONENT_ID, INVALID_VEHICLE_COMPONENT_ID, ...},
	{1008, 1009, 1010, 1025, 1073, 1074, 1075, 1076, 1077, 1078, 1079, 1080, 1081, 1082, 1083, 1084, 1085, 1086, 1087, 1096, 1097, 1098, INVALID_VEHICLE_COMPONENT_ID, INVALID_VEHICLE_COMPONENT_ID, ...},
	{1008, 1009, 1010, 1025, 1073, 1074, 1075, 1076, 1077, 1078, 1079, 1080, 1081, 1082, 1083, 1084, 1085, 1086, 1087, 1096, 1097, 1098, INVALID_VEHICLE_COMPONENT_ID, INVALID_VEHICLE_COMPONENT_ID, ...},
	{1008, 1009, 1010, 1025, 1073, 1074, 1075, 1076, 1077, 1078, 1079, 1080, 1081, 1082, 1083, 1084, 1085, 1086, 1087, 1096, 1097, 1098, INVALID_VEHICLE_COMPONENT_ID, INVALID_VEHICLE_COMPONENT_ID, ...},
	{1008, 1009, 1010, 1025, 1073, 1074, 1075, 1076, 1077, 1078, 1079, 1080, 1081, 1082, 1083, 1084, 1085, 1086, 1087, 1096, 1097, 1098, INVALID_VEHICLE_COMPONENT_ID, INVALID_VEHICLE_COMPONENT_ID, ...},
	{1001, 1003, 1007, 1008, 1009, 1010, 1017, 1018, 1019, 1023, 1025, 1073, 1074, 1075, 1076, 1077, 1078, 1079, 1080, 1081, 1082, 1083, 1084, 1085, 1086, 1087, 1096, 1097, 1098, INVALID_VEHICLE_COMPONENT_ID, INVALID_VEHICLE_COMPONENT_ID, ...},
	{1008, 1009, 1010, 1025, 1073, 1074, 1075, 1076, 1077, 1078, 1079, 1080, 1081, 1082, 1083, 1084, 1085, 1086, 1087, 1096, 1097, 1098, INVALID_VEHICLE_COMPONENT_ID, INVALID_VEHICLE_COMPONENT_ID, ...},
	{1008, 1009, 1010, 1025, 1073, 1074, 1075, 1076, 1077, 1078, 1079, 1080, 1081, 1082, 1083, 1084, 1085, 1086, 1087, 1096, 1097, 1098, INVALID_VEHICLE_COMPONENT_ID, INVALID_VEHICLE_COMPONENT_ID, ...},
	{1002, 1006, 1008, 1009, 1010, 1016, 1020, 1021, 1025, 1073, 1074, 1075, 1076, 1077, 1078, 1079, 1080, 1081, 1082, 1083, 1084, 1085, 1086, 1087, 1096, 1097, 1098, INVALID_VEHICLE_COMPONENT_ID, INVALID_VEHICLE_COMPONENT_ID, ...},
	{1008, 1009, 1010, 1025, 1073, 1074, 1075, 1076, 1077, 1078, 1079, 1080, 1081, 1082, 1083, 1084, 1085, 1086, 1087, 1096, 1097, 1098, INVALID_VEHICLE_COMPONENT_ID, INVALID_VEHICLE_COMPONENT_ID, ...},
	{1001, 1003, 1004, 1005, 1008, 1009, 1010, 1019, 1021, 1025, 1073, 1074, 1075, 1076, 1077, 1078, 1079, 1080, 1081, 1082, 1083, 1084, 1085, 1086, 1087, 1096, 1097, 1098, INVALID_VEHICLE_COMPONENT_ID, INVALID_VEHICLE_COMPONENT_ID, ...},
	{1000, 1008, 1009, 1010, 1014, 1016, 1018, 1019, 1020, 1021, 1023, 1025, 1073, 1074, 1075, 1076, 1077, 1078, 1079, 1080, 1081, 1082, 1083, 1084, 1085, 1086, 1087, 1096, 1097, 1098, INVALID_VEHICLE_COMPONENT_ID, INVALID_VEHICLE_COMPONENT_ID, ...},
	{1007, 1008, 1009, 1010, 1013, 1017, 1019, 1020, 1021, 1025, 1073, 1074, 1075, 1076, 1077, 1078, 1079, 1080, 1081, 1082, 1083, 1084, 1085, 1086, 1087, 1096, 1097, 1098, INVALID_VEHICLE_COMPONENT_ID, INVALID_VEHICLE_COMPONENT_ID, ...},
	{1008, 1009, 1010, 1025, 1073, 1074, 1075, 1076, 1077, 1078, 1079, 1080, 1081, 1082, 1083, 1084, 1085, 1086, 1087, 1096, 1097, 1098, INVALID_VEHICLE_COMPONENT_ID, INVALID_VEHICLE_COMPONENT_ID, ...},
	{1008, 1009, 1010, 1025, 1073, 1074, 1075, 1076, 1077, 1078, 1079, 1080, 1081, 1082, 1083, 1084, 1085, 1086, 1087, 1096, 1097, 1098, INVALID_VEHICLE_COMPONENT_ID, INVALID_VEHICLE_COMPONENT_ID, ...},
	{1008, 1009, 1010, 1025, 1073, 1074, 1075, 1076, 1077, 1078, 1079, 1080, 1081, 1082, 1083, 1084, 1085, 1086, 1087, 1096, 1097, 1098, INVALID_VEHICLE_COMPONENT_ID, INVALID_VEHICLE_COMPONENT_ID, ...},
	{1001, 1003, 1004, 1005, 1006, 1008, 1009, 1010, 1019, 1021, 1025, 1073, 1074, 1075, 1076, 1077, 1078, 1079, 1080, 1081, 1082, 1083, 1084, 1085, 1086, 1087, 1096, 1097, 1098, INVALID_VEHICLE_COMPONENT_ID, INVALID_VEHICLE_COMPONENT_ID, ...},
	{1008, 1009, 1010, 1025, 1073, 1074, 1075, 1076, 1077, 1078, 1079, 1080, 1081, 1082, 1083, 1084, 1085, 1086, 1087, 1096, 1097, 1098, INVALID_VEHICLE_COMPONENT_ID, INVALID_VEHICLE_COMPONENT_ID, ...},
	{1008, 1009, 1010, 1025, 1073, 1074, 1075, 1076, 1077, 1078, 1079, 1080, 1081, 1082, 1083, 1084, 1085, 1086, 1087, 1096, 1097, 1098, INVALID_VEHICLE_COMPONENT_ID, INVALID_VEHICLE_COMPONENT_ID, ...},
	{1008, 1009, 1010, 1025, 1073, 1074, 1075, 1076, 1077, 1078, 1079, 1080, 1081, 1082, 1083, 1084, 1085, 1086, 1087, 1096, 1097, 1098, INVALID_VEHICLE_COMPONENT_ID, INVALID_VEHICLE_COMPONENT_ID, ...},
	{INVALID_VEHICLE_COMPONENT_ID, ...},
	{1008, 1009, 1010, 1025, 1073, 1074, 1075, 1076, 1077, 1078, 1079, 1080, 1081, 1082, 1083, 1084, 1085, 1086, 1087, 1096, 1097, 1098, INVALID_VEHICLE_COMPONENT_ID, INVALID_VEHICLE_COMPONENT_ID, ...},
	{1008, 1009, 1010, 1025, 1073, 1074, 1075, 1076, 1077, 1078, 1079, 1080, 1081, 1082, 1083, 1084, 1085, 1086, 1087, 1096, 1097, 1098, INVALID_VEHICLE_COMPONENT_ID, INVALID_VEHICLE_COMPONENT_ID, ...},
	{1008, 1009, 1010, 1025, 1073, 1074, 1075, 1076, 1077, 1078, 1079, 1080, 1081, 1082, 1083, 1084, 1085, 1086, 1087, 1096, 1097, 1098, INVALID_VEHICLE_COMPONENT_ID, INVALID_VEHICLE_COMPONENT_ID, ...},
	{1008, 1009, 1010, 1025, 1073, 1074, 1075, 1076, 1077, 1078, 1079, 1080, 1081, 1082, 1083, 1084, 1085, 1086, 1087, 1096, 1097, 1098, INVALID_VEHICLE_COMPONENT_ID, INVALID_VEHICLE_COMPONENT_ID, ...},
	{1008, 1009, 1010, 1025, 1073, 1074, 1075, 1076, 1077, 1078, 1079, 1080, 1081, 1082, 1083, 1084, 1085, 1086, 1087, 1096, 1097, 1098, INVALID_VEHICLE_COMPONENT_ID, INVALID_VEHICLE_COMPONENT_ID, ...},
	{1001, 1003, 1006, 1007, 1008, 1009, 1010, 1013, 1017, 1019, 1020, 1021, 1022, 1025, 1073, 1074, 1075, 1076, 1077, 1078, 1079, 1080, 1081, 1082, 1083, 1084, 1085, 1086, 1087, 1096, 1097, 1098, INVALID_VEHICLE_COMPONENT_ID, INVALID_VEHICLE_COMPONENT_ID, ...},
	{1008, 1009, 1010, 1025, 1073, 1074, 1075, 1076, 1077, 1078, 1079, 1080, 1081, 1082, 1083, 1084, 1085, 1086, 1087, 1096, 1097, 1098, INVALID_VEHICLE_COMPONENT_ID, INVALID_VEHICLE_COMPONENT_ID, ...},
	{1008, 1009, 1010, 1025, 1073, 1074, 1075, 1076, 1077, 1078, 1079, 1080, 1081, 1082, 1083, 1084, 1085, 1086, 1087, 1096, 1097, 1098, INVALID_VEHICLE_COMPONENT_ID, INVALID_VEHICLE_COMPONENT_ID, ...},
	{1001, 1003, 1007, 1008, 1009, 1010, 1013, 1017, 1023, 1025, 1073, 1074, 1075, 1076, 1077, 1078, 1079, 1080, 1081, 1082, 1083, 1084, 1085, 1086, 1087, 1096, 1097, 1098, 1142, 1143, 1144, 1145, INVALID_VEHICLE_COMPONENT_ID, INVALID_VEHICLE_COMPONENT_ID, ...},
	{1008, 1009, 1010, 1025, 1073, 1074, 1075, 1076, 1077, 1078, 1079, 1080, 1081, 1082, 1083, 1084, 1085, 1086, 1087, 1096, 1097, 1098, INVALID_VEHICLE_COMPONENT_ID, INVALID_VEHICLE_COMPONENT_ID, ...},
	{1008, 1009, 1010, 1025, 1073, 1074, 1075, 1076, 1077, 1078, 1079, 1080, 1081, 1082, 1083, 1084, 1085, 1086, 1087, 1096, 1097, 1098, INVALID_VEHICLE_COMPONENT_ID, INVALID_VEHICLE_COMPONENT_ID, ...},
	{1008, 1009, 1010, 1025, 1073, 1074, 1075, 1076, 1077, 1078, 1079, 1080, 1081, 1082, 1083, 1084, 1085, 1086, 1087, 1096, 1097, 1098, INVALID_VEHICLE_COMPONENT_ID, INVALID_VEHICLE_COMPONENT_ID, ...},
	{1008, 1009, 1010, 1025, 1073, 1074, 1075, 1076, 1077, 1078, 1079, 1080, 1081, 1082, 1083, 1084, 1085, 1086, 1087, 1096, 1097, 1098, INVALID_VEHICLE_COMPONENT_ID, INVALID_VEHICLE_COMPONENT_ID, ...},
	{1008, 1009, 1010, 1025, 1073, 1074, 1075, 1076, 1077, 1078, 1079, 1080, 1081, 1082, 1083, 1084, 1085, 1086, 1087, 1096, 1097, 1098, INVALID_VEHICLE_COMPONENT_ID, INVALID_VEHICLE_COMPONENT_ID, ...},
	{1008, 1009, 1010, 1025, 1073, 1074, 1075, 1076, 1077, 1078, 1079, 1080, 1081, 1082, 1083, 1084, 1085, 1086, 1087, 1096, 1097, 1098, INVALID_VEHICLE_COMPONENT_ID, INVALID_VEHICLE_COMPONENT_ID, ...},
	{INVALID_VEHICLE_COMPONENT_ID, ...},
	{1008, 1009, 1010, 1025, 1073, 1074, 1075, 1076, 1077, 1078, 1079, 1080, 1081, 1082, 1083, 1084, 1085, 1086, 1087, 1096, 1097, 1098, INVALID_VEHICLE_COMPONENT_ID, INVALID_VEHICLE_COMPONENT_ID, ...},
	{INVALID_VEHICLE_COMPONENT_ID, ...},
	{INVALID_VEHICLE_COMPONENT_ID, ...},
	{1008, 1009, 1010, 1025, 1073, 1074, 1075, 1076, 1077, 1078, 1079, 1080, 1081, 1082, 1083, 1084, 1085, 1086, 1087, 1096, 1097, 1098, INVALID_VEHICLE_COMPONENT_ID, INVALID_VEHICLE_COMPONENT_ID, ...},
	{1008, 1009, 1010, 1025, 1073, 1074, 1075, 1076, 1077, 1078, 1079, 1080, 1081, 1082, 1083, 1084, 1085, 1086, 1087, 1096, 1097, 1098, INVALID_VEHICLE_COMPONENT_ID, INVALID_VEHICLE_COMPONENT_ID, ...},
	{INVALID_VEHICLE_COMPONENT_ID, ...},
	{INVALID_VEHICLE_COMPONENT_ID, ...},
	{INVALID_VEHICLE_COMPONENT_ID, ...},
	{1008, 1009, 1010, 1025, 1073, 1074, 1075, 1076, 1077, 1078, 1079, 1080, 1081, 1082, 1083, 1084, 1085, 1086, 1087, 1096, 1097, 1098, INVALID_VEHICLE_COMPONENT_ID, INVALID_VEHICLE_COMPONENT_ID, ...},
	{1008, 1009, 1010, 1025, 1073, 1074, 1075, 1076, 1077, 1078, 1079, 1080, 1081, 1082, 1083, 1084, 1085, 1086, 1087, 1096, 1097, 1098, INVALID_VEHICLE_COMPONENT_ID, INVALID_VEHICLE_COMPONENT_ID, ...},
	{1008, 1009, 1010, 1025, 1073, 1074, 1075, 1076, 1077, 1078, 1079, 1080, 1081, 1082, 1083, 1084, 1085, 1086, 1087, 1096, 1097, 1098, INVALID_VEHICLE_COMPONENT_ID, INVALID_VEHICLE_COMPONENT_ID, ...},
	{1008, 1009, 1010, 1025, 1073, 1074, 1075, 1076, 1077, 1078, 1079, 1080, 1081, 1082, 1083, 1084, 1085, 1086, 1087, 1096, 1097, 1098, INVALID_VEHICLE_COMPONENT_ID, INVALID_VEHICLE_COMPONENT_ID, ...},
	{1008, 1009, 1010, 1025, 1073, 1074, 1075, 1076, 1077, 1078, 1079, 1080, 1081, 1082, 1083, 1084, 1085, 1086, 1087, 1096, 1097, 1098, INVALID_VEHICLE_COMPONENT_ID, INVALID_VEHICLE_COMPONENT_ID, ...},
	{1008, 1009, 1010, 1025, 1073, 1074, 1075, 1076, 1077, 1078, 1079, 1080, 1081, 1082, 1083, 1084, 1085, 1086, 1087, 1096, 1097, 1098, INVALID_VEHICLE_COMPONENT_ID, INVALID_VEHICLE_COMPONENT_ID, ...},
	{INVALID_VEHICLE_COMPONENT_ID, ...},
	{INVALID_VEHICLE_COMPONENT_ID, ...},
	{INVALID_VEHICLE_COMPONENT_ID, ...},
	{1008, 1009, 1010, 1025, 1073, 1074, 1075, 1076, 1077, 1078, 1079, 1080, 1081, 1082, 1083, 1084, 1085, 1086, 1087, 1096, 1097, 1098, INVALID_VEHICLE_COMPONENT_ID, INVALID_VEHICLE_COMPONENT_ID, ...},
	{1008, 1009, 1010, 1025, 1073, 1074, 1075, 1076, 1077, 1078, 1079, 1080, 1081, 1082, 1083, 1084, 1085, 1086, 1087, 1096, 1097, 1098, INVALID_VEHICLE_COMPONENT_ID, INVALID_VEHICLE_COMPONENT_ID, ...},
	{1008, 1009, 1010, 1025, 1073, 1074, 1075, 1076, 1077, 1078, 1079, 1080, 1081, 1082, 1083, 1084, 1085, 1086, 1087, 1096, 1097, 1098, INVALID_VEHICLE_COMPONENT_ID, INVALID_VEHICLE_COMPONENT_ID, ...},
	{1008, 1009, 1010, 1025, 1073, 1074, 1075, 1076, 1077, 1078, 1079, 1080, 1081, 1082, 1083, 1084, 1085, 1086, 1087, 1096, 1097, 1098, INVALID_VEHICLE_COMPONENT_ID, INVALID_VEHICLE_COMPONENT_ID, ...},
	{INVALID_VEHICLE_COMPONENT_ID, ...},
	{1008, 1009, 1010, 1025, 1073, 1074, 1075, 1076, 1077, 1078, 1079, 1080, 1081, 1082, 1083, 1084, 1085, 1086, 1087, 1096, 1097, 1098, INVALID_VEHICLE_COMPONENT_ID, INVALID_VEHICLE_COMPONENT_ID, ...},
	{1008, 1009, 1010, 1025, 1073, 1074, 1075, 1076, 1077, 1078, 1079, 1080, 1081, 1082, 1083, 1084, 1085, 1086, 1087, 1096, 1097, 1098, INVALID_VEHICLE_COMPONENT_ID, INVALID_VEHICLE_COMPONENT_ID, ...},
	{1008, 1009, 1010, 1025, 1073, 1074, 1075, 1076, 1077, 1078, 1079, 1080, 1081, 1082, 1083, 1084, 1085, 1086, 1087, 1096, 1097, 1098, INVALID_VEHICLE_COMPONENT_ID, INVALID_VEHICLE_COMPONENT_ID, ...},
	{INVALID_VEHICLE_COMPONENT_ID, ...},
	{INVALID_VEHICLE_COMPONENT_ID, ...},
	{1008, 1009, 1010, 1025, 1073, 1074, 1075, 1076, 1077, 1078, 1079, 1080, 1081, 1082, 1083, 1084, 1085, 1086, 1087, 1096, 1097, 1098, INVALID_VEHICLE_COMPONENT_ID, INVALID_VEHICLE_COMPONENT_ID, ...},
	{1008, 1009, 1010, 1025, 1073, 1074, 1075, 1076, 1077, 1078, 1079, 1080, 1081, 1082, 1083, 1084, 1085, 1086, 1087, 1096, 1097, 1098, INVALID_VEHICLE_COMPONENT_ID, INVALID_VEHICLE_COMPONENT_ID, ...},
	{1008, 1009, 1010, 1025, 1073, 1074, 1075, 1076, 1077, 1078, 1079, 1080, 1081, 1082, 1083, 1084, 1085, 1086, 1087, 1096, 1097, 1098, INVALID_VEHICLE_COMPONENT_ID, INVALID_VEHICLE_COMPONENT_ID, ...},
	{1006, 1007, 1008, 1009, 1010, 1017, 1018, 1019, 1020, 1021, 1025, 1073, 1074, 1075, 1076, 1077, 1078, 1079, 1080, 1081, 1082, 1083, 1084, 1085, 1086, 1087, 1096, 1097, 1098, INVALID_VEHICLE_COMPONENT_ID, INVALID_VEHICLE_COMPONENT_ID, ...},
	{1004, 1005, 1008, 1009, 1010, 1012, 1013, 1020, 1021, 1022, 1024, 1025, 1073, 1074, 1075, 1076, 1077, 1078, 1079, 1080, 1081, 1082, 1083, 1084, 1085, 1086, 1087, 1096, 1097, 1098, INVALID_VEHICLE_COMPONENT_ID, INVALID_VEHICLE_COMPONENT_ID, ...},
	{1008, 1009, 1010, 1025, 1073, 1074, 1075, 1076, 1077, 1078, 1079, 1080, 1081, 1082, 1083, 1084, 1085, 1086, 1087, 1096, 1097, 1098, INVALID_VEHICLE_COMPONENT_ID, INVALID_VEHICLE_COMPONENT_ID, ...},
	{1008, 1009, 1010, 1025, 1073, 1074, 1075, 1076, 1077, 1078, 1079, 1080, 1081, 1082, 1083, 1084, 1085, 1086, 1087, 1096, 1097, 1098, INVALID_VEHICLE_COMPONENT_ID, INVALID_VEHICLE_COMPONENT_ID, ...},
	{INVALID_VEHICLE_COMPONENT_ID, ...},
	{1008, 1009, 1010, 1025, 1073, 1074, 1075, 1076, 1077, 1078, 1079, 1080, 1081, 1082, 1083, 1084, 1085, 1086, 1087, 1096, 1097, 1098, INVALID_VEHICLE_COMPONENT_ID, INVALID_VEHICLE_COMPONENT_ID, ...},
	{1008, 1009, 1010, 1025, 1073, 1074, 1075, 1076, 1077, 1078, 1079, 1080, 1081, 1082, 1083, 1084, 1085, 1086, 1087, 1096, 1097, 1098, INVALID_VEHICLE_COMPONENT_ID, INVALID_VEHICLE_COMPONENT_ID, ...},
	{INVALID_VEHICLE_COMPONENT_ID, ...},
	{1008, 1009, 1010, 1025, 1073, 1074, 1075, 1076, 1077, 1078, 1079, 1080, 1081, 1082, 1083, 1084, 1085, 1086, 1087, 1096, 1097, 1098, INVALID_VEHICLE_COMPONENT_ID, INVALID_VEHICLE_COMPONENT_ID, ...},
	{1008, 1009, 1010, 1025, 1073, 1074, 1075, 1076, 1077, 1078, 1079, 1080, 1081, 1082, 1083, 1084, 1085, 1086, 1087, 1096, 1097, 1098, INVALID_VEHICLE_COMPONENT_ID, INVALID_VEHICLE_COMPONENT_ID, ...},
	{1008, 1009, 1010, 1025, 1073, 1074, 1075, 1076, 1077, 1078, 1079, 1080, 1081, 1082, 1083, 1084, 1085, 1086, 1087, 1096, 1097, 1098, INVALID_VEHICLE_COMPONENT_ID, INVALID_VEHICLE_COMPONENT_ID, ...},
	{1008, 1009, 1010, 1025, 1073, 1074, 1075, 1076, 1077, 1078, 1079, 1080, 1081, 1082, 1083, 1084, 1085, 1086, 1087, 1096, 1097, 1098, INVALID_VEHICLE_COMPONENT_ID, INVALID_VEHICLE_COMPONENT_ID, ...},
	{1000, 1002, 1004, 1005, 1006, 1008, 1009, 1010, 1013, 1016, 1018, 1019, 1020, 1024, 1025, 1073, 1074, 1075, 1076, 1077, 1078, 1079, 1080, 1081, 1082, 1083, 1084, 1085, 1086, 1087, 1096, 1097, 1098, INVALID_VEHICLE_COMPONENT_ID, INVALID_VEHICLE_COMPONENT_ID, ...},
	{1008, 1009, 1010, 1025, 1073, 1074, 1075, 1076, 1077, 1078, 1079, 1080, 1081, 1082, 1083, 1084, 1085, 1086, 1087, 1096, 1097, 1098, INVALID_VEHICLE_COMPONENT_ID, INVALID_VEHICLE_COMPONENT_ID, ...},
	{1003, 1007, 1008, 1009, 1010, 1014, 1017, 1018, 1019, 1020, 1021, 1023, 1025, 1073, 1074, 1075, 1076, 1077, 1078, 1079, 1080, 1081, 1082, 1083, 1084, 1085, 1086, 1087, 1096, 1097, 1098, 1142, 1143, 1144, 1145, INVALID_VEHICLE_COMPONENT_ID, INVALID_VEHICLE_COMPONENT_ID, ...},
	{1000, 1004, 1005, 1006, 1008, 1009, 1010, 1016, 1025, 1073, 1074, 1075, 1076, 1077, 1078, 1079, 1080, 1081, 1082, 1083, 1084, 1085, 1086, 1087, 1096, 1097, 1098, INVALID_VEHICLE_COMPONENT_ID, INVALID_VEHICLE_COMPONENT_ID, ...},
	{INVALID_VEHICLE_COMPONENT_ID, ...},
	{1008, 1009, 1010, 1025, 1073, 1074, 1075, 1076, 1077, 1078, 1079, 1080, 1081, 1082, 1083, 1084, 1085, 1086, 1087, 1096, 1097, 1098, INVALID_VEHICLE_COMPONENT_ID, INVALID_VEHICLE_COMPONENT_ID, ...},
	{1008, 1009, 1010, 1025, 1073, 1074, 1075, 1076, 1077, 1078, 1079, 1080, 1081, 1082, 1083, 1084, 1085, 1086, 1087, 1096, 1097, 1098, INVALID_VEHICLE_COMPONENT_ID, INVALID_VEHICLE_COMPONENT_ID, ...},
	{1001, 1002, 1003, 1006, 1007, 1008, 1009, 1010, 1011, 1017, 1019, 1020, 1023, 1025, 1073, 1074, 1075, 1076, 1077, 1078, 1079, 1080, 1081, 1082, 1083, 1084, 1085, 1086, 1087, 1096, 1097, 1098, 1142, 1143, INVALID_VEHICLE_COMPONENT_ID, INVALID_VEHICLE_COMPONENT_ID, ...},
	{1008, 1009, 1010, 1025, 1073, 1074, 1075, 1076, 1077, 1078, 1079, 1080, 1081, 1082, 1083, 1084, 1085, 1086, 1087, 1096, 1097, 1098, INVALID_VEHICLE_COMPONENT_ID, INVALID_VEHICLE_COMPONENT_ID, ...},
	{1008, 1009, 1010, 1025, 1073, 1074, 1075, 1076, 1077, 1078, 1079, 1080, 1081, 1082, 1083, 1084, 1085, 1086, 1087, 1096, 1097, 1098, INVALID_VEHICLE_COMPONENT_ID, INVALID_VEHICLE_COMPONENT_ID, ...},
	{1008, 1009, 1010, 1025, 1073, 1074, 1075, 1076, 1077, 1078, 1079, 1080, 1081, 1082, 1083, 1084, 1085, 1086, 1087, 1096, 1097, 1098, INVALID_VEHICLE_COMPONENT_ID, INVALID_VEHICLE_COMPONENT_ID, ...},
	{1008, 1009, 1010, 1013, 1019, 1020, 1021, 1024, 1025, 1073, 1074, 1075, 1076, 1077, 1078, 1079, 1080, 1081, 1082, 1083, 1084, 1085, 1086, 1087, 1096, 1097, 1098, INVALID_VEHICLE_COMPONENT_ID, INVALID_VEHICLE_COMPONENT_ID, ...},
	{1008, 1009, 1010, 1025, 1073, 1074, 1075, 1076, 1077, 1078, 1079, 1080, 1081, 1082, 1083, 1084, 1085, 1086, 1087, 1096, 1097, 1098, INVALID_VEHICLE_COMPONENT_ID, INVALID_VEHICLE_COMPONENT_ID, ...},
	{1008, 1009, 1010, 1025, 1073, 1074, 1075, 1076, 1077, 1078, 1079, 1080, 1081, 1082, 1083, 1084, 1085, 1086, 1087, 1096, 1097, 1098, INVALID_VEHICLE_COMPONENT_ID, INVALID_VEHICLE_COMPONENT_ID, ...},
	{1008, 1009, 1010, 1025, 1073, 1074, 1075, 1076, 1077, 1078, 1079, 1080, 1081, 1082, 1083, 1084, 1085, 1086, 1087, 1096, 1097, 1098, INVALID_VEHICLE_COMPONENT_ID, INVALID_VEHICLE_COMPONENT_ID, ...},
	{1008, 1009, 1010, 1025, 1073, 1074, 1075, 1076, 1077, 1078, 1079, 1080, 1081, 1082, 1083, 1084, 1085, 1086, 1087, 1096, 1097, 1098, INVALID_VEHICLE_COMPONENT_ID, INVALID_VEHICLE_COMPONENT_ID, ...},
	{1008, 1009, 1010, 1025, 1073, 1074, 1075, 1076, 1077, 1078, 1079, 1080, 1081, 1082, 1083, 1084, 1085, 1086, 1087, 1096, 1097, 1098, INVALID_VEHICLE_COMPONENT_ID, INVALID_VEHICLE_COMPONENT_ID, ...},
	{1008, 1009, 1010, 1025, 1073, 1074, 1075, 1076, 1077, 1078, 1079, 1080, 1081, 1082, 1083, 1084, 1085, 1086, 1087, 1096, 1097, 1098, INVALID_VEHICLE_COMPONENT_ID, INVALID_VEHICLE_COMPONENT_ID, ...},
	{1008, 1009, 1010, 1025, 1073, 1074, 1075, 1076, 1077, 1078, 1079, 1080, 1081, 1082, 1083, 1084, 1085, 1086, 1087, 1096, 1097, 1098, INVALID_VEHICLE_COMPONENT_ID, INVALID_VEHICLE_COMPONENT_ID, ...},
	{1008, 1009, 1010, 1025, 1073, 1074, 1075, 1076, 1077, 1078, 1079, 1080, 1081, 1082, 1083, 1084, 1085, 1086, 1087, 1096, 1097, 1098, INVALID_VEHICLE_COMPONENT_ID, INVALID_VEHICLE_COMPONENT_ID, ...},
	{INVALID_VEHICLE_COMPONENT_ID, ...},
	{INVALID_VEHICLE_COMPONENT_ID, ...},
	{1008, 1009, 1010, 1025, 1073, 1074, 1075, 1076, 1077, 1078, 1079, 1080, 1081, 1082, 1083, 1084, 1085, 1086, 1087, 1096, 1097, 1098, INVALID_VEHICLE_COMPONENT_ID, INVALID_VEHICLE_COMPONENT_ID, ...},
	{1008, 1009, 1010, 1025, 1073, 1074, 1075, 1076, 1077, 1078, 1079, 1080, 1081, 1082, 1083, 1084, 1085, 1086, 1087, 1096, 1097, 1098, INVALID_VEHICLE_COMPONENT_ID, INVALID_VEHICLE_COMPONENT_ID, ...},
	{1008, 1009, 1010, 1025, 1073, 1074, 1075, 1076, 1077, 1078, 1079, 1080, 1081, 1082, 1083, 1084, 1085, 1086, 1087, 1096, 1097, 1098, INVALID_VEHICLE_COMPONENT_ID, INVALID_VEHICLE_COMPONENT_ID, ...},
	{1008, 1009, 1010, 1025, 1073, 1074, 1075, 1076, 1077, 1078, 1079, 1080, 1081, 1082, 1083, 1084, 1085, 1086, 1087, 1096, 1097, 1098, INVALID_VEHICLE_COMPONENT_ID, INVALID_VEHICLE_COMPONENT_ID, ...},
	{1008, 1009, 1010, 1025, 1073, 1074, 1075, 1076, 1077, 1078, 1079, 1080, 1081, 1082, 1083, 1084, 1085, 1086, 1087, 1096, 1097, 1098, INVALID_VEHICLE_COMPONENT_ID, INVALID_VEHICLE_COMPONENT_ID, ...},
	{1000, 1002, 1004, 1007, 1008, 1009, 1010, 1015, 1016, 1017, 1018, 1019, 1020, 1021, 1025, 1073, 1074, 1075, 1076, 1077, 1078, 1079, 1080, 1081, 1082, 1083, 1084, 1085, 1086, 1087, 1096, 1097, 1098, INVALID_VEHICLE_COMPONENT_ID, INVALID_VEHICLE_COMPONENT_ID, ...},
	{1002, 1003, 1007, 1008, 1009, 1010, 1016, 1017, 1018, 1019, 1020, 1023, 1025, 1073, 1074, 1075, 1076, 1077, 1078, 1079, 1080, 1081, 1082, 1083, 1084, 1085, 1086, 1087, 1096, 1097, 1098, 1142, 1143, 1144, 1145, INVALID_VEHICLE_COMPONENT_ID, INVALID_VEHICLE_COMPONENT_ID, ...},
	{1001, 1003, 1005, 1006, 1007, 1008, 1009, 1010, 1013, 1017, 1018, 1020, 1023, 1025, 1073, 1074, 1075, 1076, 1077, 1078, 1079, 1080, 1081, 1082, 1083, 1084, 1085, 1086, 1087, 1096, 1097, 1098, 1142, 1143, 1144, 1145, INVALID_VEHICLE_COMPONENT_ID, INVALID_VEHICLE_COMPONENT_ID, ...},
	{1008, 1009, 1010, 1025, 1073, 1074, 1075, 1076, 1077, 1078, 1079, 1080, 1081, 1082, 1083, 1084, 1085, 1086, 1087, 1096, 1097, 1098, INVALID_VEHICLE_COMPONENT_ID, INVALID_VEHICLE_COMPONENT_ID, ...},
	{1008, 1009, 1010, 1025, 1073, 1074, 1075, 1076, 1077, 1078, 1079, 1080, 1081, 1082, 1083, 1084, 1085, 1086, 1087, 1096, 1097, 1098, INVALID_VEHICLE_COMPONENT_ID, INVALID_VEHICLE_COMPONENT_ID, ...},
	{INVALID_VEHICLE_COMPONENT_ID, ...},
	{INVALID_VEHICLE_COMPONENT_ID, ...},
	{INVALID_VEHICLE_COMPONENT_ID, ...},
	{1008, 1009, 1010, 1025, 1073, 1074, 1075, 1076, 1077, 1078, 1079, 1080, 1081, 1082, 1083, 1084, 1085, 1086, 1087, 1096, 1097, 1098, INVALID_VEHICLE_COMPONENT_ID, INVALID_VEHICLE_COMPONENT_ID, ...},
	{1008, 1009, 1010, 1025, 1073, 1074, 1075, 1076, 1077, 1078, 1079, 1080, 1081, 1082, 1083, 1084, 1085, 1086, 1087, 1096, 1097, 1098, INVALID_VEHICLE_COMPONENT_ID, INVALID_VEHICLE_COMPONENT_ID, ...},
	{1008, 1009, 1010, 1025, 1073, 1074, 1075, 1076, 1077, 1078, 1079, 1080, 1081, 1082, 1083, 1084, 1085, 1086, 1087, 1096, 1097, 1098, INVALID_VEHICLE_COMPONENT_ID, INVALID_VEHICLE_COMPONENT_ID, ...},
	{1001, 1007, 1008, 1009, 1010, 1014, 1015, 1017, 1018, 1020, 1021, 1025, 1073, 1074, 1075, 1076, 1077, 1078, 1079, 1080, 1081, 1082, 1083, 1084, 1085, 1086, 1087, 1096, 1097, 1098, INVALID_VEHICLE_COMPONENT_ID, INVALID_VEHICLE_COMPONENT_ID, ...},
	{1008, 1009, 1010, 1025, 1073, 1074, 1075, 1076, 1077, 1078, 1079, 1080, 1081, 1082, 1083, 1084, 1085, 1086, 1087, 1096, 1097, 1098, INVALID_VEHICLE_COMPONENT_ID, INVALID_VEHICLE_COMPONENT_ID, ...},
	{1001, 1003, 1006, 1007, 1008, 1009, 1010, 1011, 1012, 1017, 1018, 1019, 1020, 1023, 1025, 1073, 1074, 1075, 1076, 1077, 1078, 1079, 1080, 1081, 1082, 1083, 1084, 1085, 1086, 1087, 1096, 1097, 1098, INVALID_VEHICLE_COMPONENT_ID, INVALID_VEHICLE_COMPONENT_ID, ...},
	{1008, 1009, 1010, 1025, 1073, 1074, 1075, 1076, 1077, 1078, 1079, 1080, 1081, 1082, 1083, 1084, 1085, 1086, 1087, 1096, 1097, 1098, INVALID_VEHICLE_COMPONENT_ID, INVALID_VEHICLE_COMPONENT_ID, ...},
	{1008, 1009, 1010, 1025, 1073, 1074, 1075, 1076, 1077, 1078, 1079, 1080, 1081, 1082, 1083, 1084, 1085, 1086, 1087, 1096, 1097, 1098, INVALID_VEHICLE_COMPONENT_ID, INVALID_VEHICLE_COMPONENT_ID, ...},
	{1008, 1009, 1010, 1025, 1073, 1074, 1075, 1076, 1077, 1078, 1079, 1080, 1081, 1082, 1083, 1084, 1085, 1086, 1087, 1096, 1097, 1098, INVALID_VEHICLE_COMPONENT_ID, INVALID_VEHICLE_COMPONENT_ID, ...},
	{1008, 1009, 1010, 1025, 1073, 1074, 1075, 1076, 1077, 1078, 1079, 1080, 1081, 1082, 1083, 1084, 1085, 1086, 1087, 1096, 1097, 1098, INVALID_VEHICLE_COMPONENT_ID, INVALID_VEHICLE_COMPONENT_ID, ...},
	{1008, 1009, 1010, 1025, 1073, 1074, 1075, 1076, 1077, 1078, 1079, 1080, 1081, 1082, 1083, 1084, 1085, 1086, 1087, 1096, 1097, 1098, 1100, 1101, 1106, 1122, 1123, 1124, 1125, 1126, 1127, 1178, 1179, 1180, 1185, INVALID_VEHICLE_COMPONENT_ID, INVALID_VEHICLE_COMPONENT_ID, ...},
	{1008, 1009, 1010, 1025, 1073, 1074, 1075, 1076, 1077, 1078, 1079, 1080, 1081, 1082, 1083, 1084, 1085, 1086, 1087, 1096, 1097, 1098, 1109, 1110, 1113, 1114, 1115, 1116, 1117, 1118, 1119, 1120, 1121, INVALID_VEHICLE_COMPONENT_ID, INVALID_VEHICLE_COMPONENT_ID, ...},
	{1008, 1009, 1010, 1025, 1073, 1074, 1075, 1076, 1077, 1078, 1079, 1080, 1081, 1082, 1083, 1084, 1085, 1086, 1087, 1096, 1097, 1098, 1103, 1104, 1105, 1107, 1108, 1128, 1181, 1182, 1183, 1184, INVALID_VEHICLE_COMPONENT_ID, INVALID_VEHICLE_COMPONENT_ID, ...},
	{INVALID_VEHICLE_COMPONENT_ID, ...},
	{INVALID_VEHICLE_COMPONENT_ID, ...},
	{1008, 1009, 1010, 1025, 1073, 1074, 1075, 1076, 1077, 1078, 1079, 1080, 1081, 1082, 1083, 1084, 1085, 1086, 1087, 1096, 1097, 1098, INVALID_VEHICLE_COMPONENT_ID, INVALID_VEHICLE_COMPONENT_ID, ...},
	{1001, 1004, 1006, 1007, 1008, 1009, 1010, 1017, 1018, 1019, 1020, 1023, 1024, 1025, 1073, 1074, 1075, 1076, 1077, 1078, 1079, 1080, 1081, 1082, 1083, 1084, 1085, 1086, 1087, 1096, 1097, 1098, 1142, 1143, 1144, 1145, INVALID_VEHICLE_COMPONENT_ID, INVALID_VEHICLE_COMPONENT_ID, ...},
	{1008, 1009, 1010, 1025, 1073, 1074, 1075, 1076, 1077, 1078, 1079, 1080, 1081, 1082, 1083, 1084, 1085, 1086, 1087, 1096, 1097, 1098, INVALID_VEHICLE_COMPONENT_ID, INVALID_VEHICLE_COMPONENT_ID, ...},
	{1008, 1009, 1010, 1014, 1015, 1018, 1019, 1020, 1021, 1025, 1073, 1074, 1075, 1076, 1077, 1078, 1079, 1080, 1081, 1082, 1083, 1084, 1085, 1086, 1087, 1096, 1097, 1098, 1144, 1145, INVALID_VEHICLE_COMPONENT_ID, INVALID_VEHICLE_COMPONENT_ID, ...},
	{1008, 1009, 1010, 1025, 1073, 1074, 1075, 1076, 1077, 1078, 1079, 1080, 1081, 1082, 1083, 1084, 1085, 1086, 1087, 1096, 1097, 1098, INVALID_VEHICLE_COMPONENT_ID, INVALID_VEHICLE_COMPONENT_ID, ...},
	{1008, 1009, 1010, 1025, 1073, 1074, 1075, 1076, 1077, 1078, 1079, 1080, 1081, 1082, 1083, 1084, 1085, 1086, 1087, 1096, 1097, 1098, INVALID_VEHICLE_COMPONENT_ID, INVALID_VEHICLE_COMPONENT_ID, ...},
	{1008, 1009, 1010, 1025, 1073, 1074, 1075, 1076, 1077, 1078, 1079, 1080, 1081, 1082, 1083, 1084, 1085, 1086, 1087, 1096, 1097, 1098, INVALID_VEHICLE_COMPONENT_ID, INVALID_VEHICLE_COMPONENT_ID, ...},
	{1001, 1002, 1004, 1006, 1007, 1008, 1009, 1010, 1017, 1018, 1019, 1023, 1024, 1025, 1073, 1074, 1075, 1076, 1077, 1078, 1079, 1080, 1081, 1082, 1083, 1084, 1085, 1086, 1087, 1096, 1097, 1098, 1142, 1143, 1144, 1145, INVALID_VEHICLE_COMPONENT_ID, INVALID_VEHICLE_COMPONENT_ID, ...},
	{1000, 1003, 1008, 1009, 1010, 1016, 1018, 1019, 1020, 1021, 1025, 1073, 1074, 1075, 1076, 1077, 1078, 1079, 1080, 1081, 1082, 1083, 1084, 1085, 1086, 1087, 1096, 1097, 1098, 1142, 1143, INVALID_VEHICLE_COMPONENT_ID, INVALID_VEHICLE_COMPONENT_ID, ...},
	{1008, 1009, 1010, 1025, 1073, 1074, 1075, 1076, 1077, 1078, 1079, 1080, 1081, 1082, 1083, 1084, 1085, 1086, 1087, 1096, 1097, 1098, INVALID_VEHICLE_COMPONENT_ID, INVALID_VEHICLE_COMPONENT_ID, ...},
	{1001, 1003, 1007, 1008, 1009, 1010, 1011, 1012, 1017, 1018, 1019, 1020, 1023, 1025, 1073, 1074, 1075, 1076, 1077, 1078, 1079, 1080, 1081, 1082, 1083, 1084, 1085, 1086, 1087, 1096, 1097, 1098, 1142, 1143, 1144, 1145, INVALID_VEHICLE_COMPONENT_ID, INVALID_VEHICLE_COMPONENT_ID, ...},
	{1001, 1003, 1004, 1005, 1006, 1008, 1009, 1010, 1018, 1019, 1020, 1023, 1025, 1073, 1074, 1075, 1076, 1077, 1078, 1079, 1080, 1081, 1082, 1083, 1084, 1085, 1086, 1087, 1096, 1097, 1098, 1142, 1143, 1144, 1145, INVALID_VEHICLE_COMPONENT_ID, INVALID_VEHICLE_COMPONENT_ID, ...},
	{1002, 1003, 1005, 1006, 1008, 1009, 1010, 1016, 1018, 1019, 1020, 1021, 1023, 1025, 1073, 1074, 1075, 1076, 1077, 1078, 1079, 1080, 1081, 1082, 1083, 1084, 1085, 1086, 1087, 1096, 1097, 1098, INVALID_VEHICLE_COMPONENT_ID, INVALID_VEHICLE_COMPONENT_ID, ...},
	{1008, 1009, 1010, 1025, 1073, 1074, 1075, 1076, 1077, 1078, 1079, 1080, 1081, 1082, 1083, 1084, 1085, 1086, 1087, 1096, 1097, 1098, INVALID_VEHICLE_COMPONENT_ID, INVALID_VEHICLE_COMPONENT_ID, ...},
	{1008, 1009, 1010, 1025, 1073, 1074, 1075, 1076, 1077, 1078, 1079, 1080, 1081, 1082, 1083, 1084, 1085, 1086, 1087, 1096, 1097, 1098, INVALID_VEHICLE_COMPONENT_ID, INVALID_VEHICLE_COMPONENT_ID, ...},
	{1008, 1009, 1010, 1025, 1073, 1074, 1075, 1076, 1077, 1078, 1079, 1080, 1081, 1082, 1083, 1084, 1085, 1086, 1087, 1096, 1097, 1098, INVALID_VEHICLE_COMPONENT_ID, INVALID_VEHICLE_COMPONENT_ID, ...},
	{1008, 1009, 1010, 1025, 1073, 1074, 1075, 1076, 1077, 1078, 1079, 1080, 1081, 1082, 1083, 1084, 1085, 1086, 1087, 1096, 1097, 1098, INVALID_VEHICLE_COMPONENT_ID, INVALID_VEHICLE_COMPONENT_ID, ...},
	{1008, 1009, 1010, 1025, 1073, 1074, 1075, 1076, 1077, 1078, 1079, 1080, 1081, 1082, 1083, 1084, 1085, 1086, 1087, 1096, 1097, 1098, INVALID_VEHICLE_COMPONENT_ID, INVALID_VEHICLE_COMPONENT_ID, ...},
	{1008, 1009, 1010, 1025, 1073, 1074, 1075, 1076, 1077, 1078, 1079, 1080, 1081, 1082, 1083, 1084, 1085, 1086, 1087, 1096, 1097, 1098, INVALID_VEHICLE_COMPONENT_ID, INVALID_VEHICLE_COMPONENT_ID, ...},
	{1008, 1009, 1010, 1025, 1073, 1074, 1075, 1076, 1077, 1078, 1079, 1080, 1081, 1082, 1083, 1084, 1085, 1086, 1087, 1088, 1089, 1090, 1091, 1092, 1093, 1094, 1095, 1096, 1097, 1098, 1163, 1164, 1165, 1166, 1167, 1168, INVALID_VEHICLE_COMPONENT_ID, INVALID_VEHICLE_COMPONENT_ID, ...},
	{1008, 1009, 1010, 1025, 1065, 1066, 1067, 1068, 1069, 1070, 1071, 1072, 1073, 1074, 1075, 1076, 1077, 1078, 1079, 1080, 1081, 1082, 1083, 1084, 1085, 1086, 1087, 1096, 1097, 1098, 1158, 1159, 1160, 1161, 1162, 1173, INVALID_VEHICLE_COMPONENT_ID, INVALID_VEHICLE_COMPONENT_ID, ...},
	{1008, 1009, 1010, 1025, 1026, 1027, 1028, 1029, 1030, 1031, 1032, 1033, 1073, 1074, 1075, 1076, 1077, 1078, 1079, 1080, 1081, 1082, 1083, 1084, 1085, 1086, 1087, 1096, 1097, 1098, 1138, 1139, 1140, 1141, 1169, 1170, INVALID_VEHICLE_COMPONENT_ID, INVALID_VEHICLE_COMPONENT_ID, ...},
	{1008, 1009, 1010, 1025, 1026, 1027, 1030, 1031, 1055, 1056, 1057, 1058, 1059, 1060, 1061, 1062, 1063, 1064, 1073, 1074, 1075, 1076, 1077, 1078, 1079, 1080, 1081, 1082, 1083, 1084, 1085, 1086, 1087, 1096, 1097, 1098, 1154, 1155, 1156, 1157, INVALID_VEHICLE_COMPONENT_ID, INVALID_VEHICLE_COMPONENT_ID, ...},
	{1008, 1009, 1010, 1025, 1034, 1035, 1036, 1037, 1038, 1039, 1040, 1041, 1073, 1074, 1075, 1076, 1077, 1078, 1079, 1080, 1081, 1082, 1083, 1084, 1085, 1086, 1087, 1096, 1097, 1098, 1146, 1147, 1148, 1149, 1171, 1172, INVALID_VEHICLE_COMPONENT_ID, INVALID_VEHICLE_COMPONENT_ID, ...},
	{1008, 1009, 1010, 1025, 1073, 1074, 1075, 1076, 1077, 1078, 1079, 1080, 1081, 1082, 1083, 1084, 1085, 1086, 1087, 1096, 1097, 1098, INVALID_VEHICLE_COMPONENT_ID, INVALID_VEHICLE_COMPONENT_ID, ...},
	{1008, 1009, 1010, 1025, 1073, 1074, 1075, 1076, 1077, 1078, 1079, 1080, 1081, 1082, 1083, 1084, 1085, 1086, 1087, 1096, 1097, 1098, INVALID_VEHICLE_COMPONENT_ID, INVALID_VEHICLE_COMPONENT_ID, ...},
	{1008, 1009, 1010, 1025, 1045, 1046, 1047, 1048, 1049, 1050, 1051, 1052, 1053, 1054, 1073, 1074, 1075, 1076, 1077, 1078, 1079, 1080, 1081, 1082, 1083, 1084, 1085, 1086, 1087, 1096, 1097, 1098, 1150, 1151, 1152, 1153, INVALID_VEHICLE_COMPONENT_ID, INVALID_VEHICLE_COMPONENT_ID, ...},
	{1008, 1009, 1010, 1025, 1073, 1074, 1075, 1076, 1077, 1078, 1079, 1080, 1081, 1082, 1083, 1084, 1085, 1086, 1087, 1096, 1097, 1098, INVALID_VEHICLE_COMPONENT_ID, INVALID_VEHICLE_COMPONENT_ID, ...},
	{1008, 1009, 1010, 1025, 1073, 1074, 1075, 1076, 1077, 1078, 1079, 1080, 1081, 1082, 1083, 1084, 1085, 1086, 1087, 1096, 1097, 1098, 1102, 1129, 1130, 1131, 1132, 1133, 1186, 1187, 1188, 1189, INVALID_VEHICLE_COMPONENT_ID, INVALID_VEHICLE_COMPONENT_ID, ...},
	{1008, 1009, 1010, 1025, 1073, 1074, 1075, 1076, 1077, 1078, 1079, 1080, 1081, 1082, 1083, 1084, 1085, 1086, 1087, 1096, 1097, 1098, INVALID_VEHICLE_COMPONENT_ID, INVALID_VEHICLE_COMPONENT_ID, ...},
	{INVALID_VEHICLE_COMPONENT_ID, ...},
	{INVALID_VEHICLE_COMPONENT_ID, ...},
	{1008, 1009, 1010, 1025, 1073, 1074, 1075, 1076, 1077, 1078, 1079, 1080, 1081, 1082, 1083, 1084, 1085, 1086, 1087, 1096, 1097, 1098, INVALID_VEHICLE_COMPONENT_ID, INVALID_VEHICLE_COMPONENT_ID, ...},
	{1008, 1009, 1010, 1025, 1073, 1074, 1075, 1076, 1077, 1078, 1079, 1080, 1081, 1082, 1083, 1084, 1085, 1086, 1087, 1096, 1097, 1098, INVALID_VEHICLE_COMPONENT_ID, INVALID_VEHICLE_COMPONENT_ID, ...},
	{1008, 1009, 1010, 1025, 1073, 1074, 1075, 1076, 1077, 1078, 1079, 1080, 1081, 1082, 1083, 1084, 1085, 1086, 1087, 1096, 1097, 1098, INVALID_VEHICLE_COMPONENT_ID, INVALID_VEHICLE_COMPONENT_ID, ...},
	{1008, 1009, 1010, 1025, 1073, 1074, 1075, 1076, 1077, 1078, 1079, 1080, 1081, 1082, 1083, 1084, 1085, 1086, 1087, 1096, 1097, 1098, INVALID_VEHICLE_COMPONENT_ID, INVALID_VEHICLE_COMPONENT_ID, ...},
	{1008, 1009, 1010, 1025, 1042, 1043, 1044, 1073, 1074, 1075, 1076, 1077, 1078, 1079, 1080, 1081, 1082, 1083, 1084, 1085, 1086, 1087, 1096, 1097, 1098, 1099, 1174, 1175, 1176, 1177, INVALID_VEHICLE_COMPONENT_ID, INVALID_VEHICLE_COMPONENT_ID, ...},
	{1008, 1009, 1010, 1025, 1073, 1074, 1075, 1076, 1077, 1078, 1079, 1080, 1081, 1082, 1083, 1084, 1085, 1086, 1087, 1096, 1097, 1098, 1134, 1135, 1136, 1137, 1190, 1191, 1192, 1193, INVALID_VEHICLE_COMPONENT_ID, INVALID_VEHICLE_COMPONENT_ID, ...},
	{1008, 1009, 1010, 1025, 1073, 1074, 1075, 1076, 1077, 1078, 1079, 1080, 1081, 1082, 1083, 1084, 1085, 1086, 1087, 1096, 1097, 1098, INVALID_VEHICLE_COMPONENT_ID, INVALID_VEHICLE_COMPONENT_ID, ...},
	{1008, 1009, 1010, 1025, 1073, 1074, 1075, 1076, 1077, 1078, 1079, 1080, 1081, 1082, 1083, 1084, 1085, 1086, 1087, 1096, 1097, 1098, INVALID_VEHICLE_COMPONENT_ID, INVALID_VEHICLE_COMPONENT_ID, ...},
	{1008, 1009, 1010, 1025, 1073, 1074, 1075, 1076, 1077, 1078, 1079, 1080, 1081, 1082, 1083, 1084, 1085, 1086, 1087, 1096, 1097, 1098, INVALID_VEHICLE_COMPONENT_ID, INVALID_VEHICLE_COMPONENT_ID, ...},
	{1001, 1006, 1007, 1008, 1009, 1010, 1017, 1018, 1020, 1023, 1025, 1073, 1074, 1075, 1076, 1077, 1078, 1079, 1080, 1081, 1082, 1083, 1084, 1085, 1086, 1087, 1096, 1097, 1098, INVALID_VEHICLE_COMPONENT_ID, INVALID_VEHICLE_COMPONENT_ID, ...},
	{INVALID_VEHICLE_COMPONENT_ID, ...},
	{1008, 1009, 1010, 1025, 1073, 1074, 1075, 1076, 1077, 1078, 1079, 1080, 1081, 1082, 1083, 1084, 1085, 1086, 1087, 1096, 1097, 1098, INVALID_VEHICLE_COMPONENT_ID, INVALID_VEHICLE_COMPONENT_ID, ...},
	{1008, 1009, 1010, 1025, 1073, 1074, 1075, 1076, 1077, 1078, 1079, 1080, 1081, 1082, 1083, 1084, 1085, 1086, 1087, 1096, 1097, 1098, INVALID_VEHICLE_COMPONENT_ID, INVALID_VEHICLE_COMPONENT_ID, ...},
	{1008, 1009, 1010, 1025, 1073, 1074, 1075, 1076, 1077, 1078, 1079, 1080, 1081, 1082, 1083, 1084, 1085, 1086, 1087, 1096, 1097, 1098, INVALID_VEHICLE_COMPONENT_ID, INVALID_VEHICLE_COMPONENT_ID, ...},
	{1000, 1001, 1002, 1003, 1006, 1007, 1008, 1009, 1010, 1013, 1014, 1015, 1016, 1017, 1018, 1019, 1020, 1021, 1022, 1023, 1024, 1025, 1073, 1074, 1075, 1076, 1077, 1078, 1079, 1080, 1081, 1082, 1083, 1084, 1085, 1086, 1087, 1096, 1097, 1098, 1142, 1143, 1144, 1145, INVALID_VEHICLE_COMPONENT_ID, INVALID_VEHICLE_COMPONENT_ID, ...},
	{INVALID_VEHICLE_COMPONENT_ID, ...},
	{1008, 1009, 1010, 1025, 1073, 1074, 1075, 1076, 1077, 1078, 1079, 1080, 1081, 1082, 1083, 1084, 1085, 1086, 1087, 1096, 1097, 1098, INVALID_VEHICLE_COMPONENT_ID, INVALID_VEHICLE_COMPONENT_ID, ...},
	{1008, 1009, 1010, 1025, 1073, 1074, 1075, 1076, 1077, 1078, 1079, 1080, 1081, 1082, 1083, 1084, 1085, 1086, 1087, 1096, 1097, 1098, INVALID_VEHICLE_COMPONENT_ID, INVALID_VEHICLE_COMPONENT_ID, ...},
	{1000, 1004, 1005, 1006, 1007, 1008, 1009, 1010, 1013, 1016, 1017, 1018, 1020, 1024, 1025, 1073, 1074, 1075, 1076, 1077, 1078, 1079, 1080, 1081, 1082, 1083, 1084, 1085, 1086, 1087, 1096, 1097, 1098, 1144, 1145, INVALID_VEHICLE_COMPONENT_ID, INVALID_VEHICLE_COMPONENT_ID, ...},
	{INVALID_VEHICLE_COMPONENT_ID, ...},
	{1008, 1009, 1010, 1025, 1073, 1074, 1075, 1076, 1077, 1078, 1079, 1080, 1081, 1082, 1083, 1084, 1085, 1086, 1087, 1096, 1097, 1098, INVALID_VEHICLE_COMPONENT_ID, INVALID_VEHICLE_COMPONENT_ID, ...},
	{1008, 1009, 1010, 1025, 1073, 1074, 1075, 1076, 1077, 1078, 1079, 1080, 1081, 1082, 1083, 1084, 1085, 1086, 1087, 1096, 1097, 1098, INVALID_VEHICLE_COMPONENT_ID, INVALID_VEHICLE_COMPONENT_ID, ...},
	{1008, 1009, 1010, 1025, 1073, 1074, 1075, 1076, 1077, 1078, 1079, 1080, 1081, 1082, 1083, 1084, 1085, 1086, 1087, 1096, 1097, 1098, INVALID_VEHICLE_COMPONENT_ID, INVALID_VEHICLE_COMPONENT_ID, ...},
	{1008, 1009, 1010, 1025, 1073, 1074, 1075, 1076, 1077, 1078, 1079, 1080, 1081, 1082, 1083, 1084, 1085, 1086, 1087, 1096, 1097, 1098, INVALID_VEHICLE_COMPONENT_ID, INVALID_VEHICLE_COMPONENT_ID, ...},
	{INVALID_VEHICLE_COMPONENT_ID, ...},
	{1008, 1009, 1010, 1025, 1073, 1074, 1075, 1076, 1077, 1078, 1079, 1080, 1081, 1082, 1083, 1084, 1085, 1086, 1087, 1096, 1097, 1098, INVALID_VEHICLE_COMPONENT_ID, INVALID_VEHICLE_COMPONENT_ID, ...},
	{1008, 1009, 1010, 1025, 1073, 1074, 1075, 1076, 1077, 1078, 1079, 1080, 1081, 1082, 1083, 1084, 1085, 1086, 1087, 1096, 1097, 1098, INVALID_VEHICLE_COMPONENT_ID, INVALID_VEHICLE_COMPONENT_ID, ...},
	{1008, 1009, 1010, 1025, 1073, 1074, 1075, 1076, 1077, 1078, 1079, 1080, 1081, 1082, 1083, 1084, 1085, 1086, 1087, 1096, 1097, 1098, INVALID_VEHICLE_COMPONENT_ID, INVALID_VEHICLE_COMPONENT_ID, ...},
	{1008, 1009, 1010, 1025, 1073, 1074, 1075, 1076, 1077, 1078, 1079, 1080, 1081, 1082, 1083, 1084, 1085, 1086, 1087, 1096, 1097, 1098, INVALID_VEHICLE_COMPONENT_ID, INVALID_VEHICLE_COMPONENT_ID, ...},
	{1004, 1005, 1006, 1007, 1008, 1009, 1010, 1013, 1017, 1018, 1020, 1022, 1025, 1073, 1074, 1075, 1076, 1077, 1078, 1079, 1080, 1081, 1082, 1083, 1084, 1085, 1086, 1087, 1096, 1097, 1098, INVALID_VEHICLE_COMPONENT_ID, INVALID_VEHICLE_COMPONENT_ID, ...},
	{1008, 1009, 1010, 1025, 1073, 1074, 1075, 1076, 1077, 1078, 1079, 1080, 1081, 1082, 1083, 1084, 1085, 1086, 1087, 1096, 1097, 1098, INVALID_VEHICLE_COMPONENT_ID, INVALID_VEHICLE_COMPONENT_ID, ...},
	{1008, 1009, 1010, 1025, 1073, 1074, 1075, 1076, 1077, 1078, 1079, 1080, 1081, 1082, 1083, 1084, 1085, 1086, 1087, 1096, 1097, 1098, INVALID_VEHICLE_COMPONENT_ID, INVALID_VEHICLE_COMPONENT_ID, ...},
	{1001, 1006, 1007, 1008, 1009, 1010, 1017, 1018, 1019, 1020, 1023, 1024, 1025, 1073, 1074, 1075, 1076, 1077, 1078, 1079, 1080, 1081, 1082, 1083, 1084, 1085, 1086, 1087, 1096, 1097, 1098, 1142, 1143, 1144, 1145, INVALID_VEHICLE_COMPONENT_ID, INVALID_VEHICLE_COMPONENT_ID, ...},
	{1008, 1009, 1010, 1025, 1073, 1074, 1075, 1076, 1077, 1078, 1079, 1080, 1081, 1082, 1083, 1084, 1085, 1086, 1087, 1096, 1097, 1098, INVALID_VEHICLE_COMPONENT_ID, INVALID_VEHICLE_COMPONENT_ID, ...},
	{1008, 1009, 1010, 1025, 1073, 1074, 1075, 1076, 1077, 1078, 1079, 1080, 1081, 1082, 1083, 1084, 1085, 1086, 1087, 1096, 1097, 1098, INVALID_VEHICLE_COMPONENT_ID, INVALID_VEHICLE_COMPONENT_ID, ...},
	{1008, 1009, 1010, 1025, 1073, 1074, 1075, 1076, 1077, 1078, 1079, 1080, 1081, 1082, 1083, 1084, 1085, 1086, 1087, 1096, 1097, 1098, INVALID_VEHICLE_COMPONENT_ID, INVALID_VEHICLE_COMPONENT_ID, ...},
	{1008, 1009, 1010, 1025, 1073, 1074, 1075, 1076, 1077, 1078, 1079, 1080, 1081, 1082, 1083, 1084, 1085, 1086, 1087, 1096, 1097, 1098, INVALID_VEHICLE_COMPONENT_ID, INVALID_VEHICLE_COMPONENT_ID, ...},
	{1008, 1009, 1010, 1025, 1073, 1074, 1075, 1076, 1077, 1078, 1079, 1080, 1081, 1082, 1083, 1084, 1085, 1086, 1087, 1096, 1097, 1098, INVALID_VEHICLE_COMPONENT_ID, INVALID_VEHICLE_COMPONENT_ID, ...},
	{1008, 1009, 1010, 1025, 1073, 1074, 1075, 1076, 1077, 1078, 1079, 1080, 1081, 1082, 1083, 1084, 1085, 1086, 1087, 1096, 1097, 1098, INVALID_VEHICLE_COMPONENT_ID, INVALID_VEHICLE_COMPONENT_ID, ...},
	{1008, 1009, 1010, 1025, 1073, 1074, 1075, 1076, 1077, 1078, 1079, 1080, 1081, 1082, 1083, 1084, 1085, 1086, 1087, 1096, 1097, 1098, INVALID_VEHICLE_COMPONENT_ID, INVALID_VEHICLE_COMPONENT_ID, ...},
	{1008, 1009, 1010, 1025, 1073, 1074, 1075, 1076, 1077, 1078, 1079, 1080, 1081, 1082, 1083, 1084, 1085, 1086, 1087, 1096, 1097, 1098, INVALID_VEHICLE_COMPONENT_ID, INVALID_VEHICLE_COMPONENT_ID, ...}
};

/**
summary to write */

stock Vehicle_GetComponentName(componentid, name[], size = sizeof(name))
{
	new idx = componentid - 1000;

	if (!(0 <= idx <= MAX_VEHICLES_COMPONENTS)) 
    {
		return 0;
	}
	return strunpack(name, Vehicle_gscComponentName[idx], size);
}

stock bool: Vehicle_CanHaveComponent(vehicleid, componentid)
{
	new idx = GetVehicleModel(vehicleid) - 400;

	if(!(0 <= idx <= MAX_VEHICLES_MODELS) || !IsValidVehicle(vehicleid)) 
    {
		return false;
	}
	for(new i; i < MAX_VEHICLE_COMPONENTS; i++)
	{
		if (Vehicle_gscComponents[idx][i] != INVALID_VEHICLE_COMPONENT_ID && Vehicle_gscComponents[idx][i] == componentid) 
        {
			return true;
		}
	}
	return false;
}

/**
 * <summary> Adds a component to vehicle.</summary>
 * <param name="id"> Vehicle or a group which we set the component to.</param>
 * <param name="componentid"> componentid to set.</param>
 * <returns> 0 if function fails, eg. vehicle is not created or vehicle can't have the component, else 1.</param>
*/

stock bool: Vehicle_AddComponent(VEHICLE_FRAMEWORK_TAGS: id, componentid, tag=tagof(id)) 
{
    if(tag == tagof(_:))
    {
        if(!Vehicle_CanHaveComponent(id, componentid) || !IsValidVehicle(id))
        {
            return false;
        }
        AddVehicleComponent(id, componentid);
    }
    else if(tag == tagof(VehicleGroup:))
    {
        foreach(new i : Vehicle_gsGroups[VehicleGroup: id])
        {
            if(!Vehicle_CanHaveComponent(id, componentid))
            {
                continue;
            }
            AddVehicleComponent(id, componentid);
        }
    }
    return true;
}

/**
 * <summary> Removes a component from vehicle.</summary>
 * <param name="vehicleid"> Vehicle we remove component from.</param>
 * <param name="componentid"> Component to remove.</param>
 * <returns> 0 if function fails, eg. vehicle is not created, else 1.</param>
*/

stock bool: Vehicle_RemoveComponent(vehicleid, componentid)
{
    return RemoveVehicleComponent(vehicleid, componentid);
}

/**
 * <summary> Checks if vehicle has the component in specified slot.</summary>
 * <param name="vehicleid"> Vehicle we check.</param>
 * <param name="slot"> Slot to check.</param>
 * <returns> ID of the component in slot.</param>
*/

stock Vehicle_GetComponentInSlot(vehicleid, CARMODTYPE: slot) 
{
    return GetVehicleComponentInSlot(vehicleid, slot);
}

/**
 * <summary> Gets vehicle component's slot.</summary>
 * <param name="vehicleid"> Vehicle we check.</param>
 * <param name="componentid"> Component to check.</param>
 * <returns> Component slot.</param>
*/

stock CARMODTYPE: Vehicle_GetComponentType(vehicleid, componentid)
{
    return CARMODTYPE: GetVehicleComponentType(componentid);
}

// --
// Mileage
// --

#define VEHICLE_UNIT: __TAG(VEHICLE_UNIT):
enum VEHICLE_UNIT:__VEHICLE_UNIT
{
    VEHICLE_UNIT_METRIC,
    VEHICLE_UNIT_IMPERIAL
}
static stock VEHICLE_UNIT:_@VEHICLE_UNIT() { return __VEHICLE_UNIT; }

static
    Float: Vehicle_gsDistanceTravelledTotalMiles[MAX_VEHICLES],
    Float: Vehicle_gsDistanceTravelledTotalKilometres[MAX_VEHICLES];

forward OnVehicleDrivenDistanceUpdate(vehicleid);

// If we don't hook this, vehicle distance travelled will go skyhigh when teleported far away

hook native SetVehiclePos(vehicleid, Float:x, Float:y, Float:z)
{
    Vehicle_gsPositionX[vehicleid] = x;
    Vehicle_gsPositionY[vehicleid] = y;
    Vehicle_gsPositionZ[vehicleid] = z;
    return continue(vehicleid, x, y, z);
}

timer __Vehicle_DistanceTravelled[1000](vehicleid)
{
    if(GetVehicleDistanceFromPoint(vehicleid, Vehicle_gsPositionX[vehicleid], Vehicle_gsPositionY[vehicleid], Vehicle_gsPositionZ[vehicleid]) >= 0.005)
    {
        Vehicle_gsDistanceTravelledTotalKilometres[vehicleid] += (GetVehicleDistanceFromPoint(vehicleid, Vehicle_gsPositionX[vehicleid], Vehicle_gsPositionY[vehicleid], Vehicle_gsPositionZ[vehicleid]) / 1000);
        Vehicle_gsDistanceTravelledTotalMiles[vehicleid] += ((GetVehicleDistanceFromPoint(vehicleid, Vehicle_gsPositionX[vehicleid], Vehicle_gsPositionY[vehicleid], Vehicle_gsPositionZ[vehicleid]) / 1000) * 0.621371);

        CallLocalFunction("OnVehicleDrivenDistanceUpdate", "i", vehicleid);
        GetVehiclePos(vehicleid, Vehicle_gsPositionX[vehicleid], Vehicle_gsPositionY[vehicleid], Vehicle_gsPositionZ[vehicleid]);
    }
}

/**
 * <library>vehicle_plus</library>
 * <summary>Gets vehicle distance travelled.</summary>
 * <param name="vehicleid">Vehicle to obtain travelled distance from.</param>
 * <param name="unit">Unit we return distance in.</param>
 * <seealso name="Vehicle_SetDistanceTravelled" />
 * <returns>Distance travelled.</returns> 
 */
stock Float: Vehicle_GetDistanceTravelled(vehicleid, VEHICLE_UNIT: unit)
{
    if(!IsValidVehicle(vehicleid))
    {
        return INVALID_VEHICLE_ID;
    }
    if(unit == VEHICLE_UNIT_IMPERIAL)
    {
	    return Vehicle_gsDistanceTravelledTotalMiles[vehicleid];
    }
    else if(unit == VEHICLE_UNIT_METRIC)
    {
        return Vehicle_gsDistanceTravelledTotalKilometres[vehicleid];
    }
    return INVALID_VEHICLE_ID;
}

/**
 * <library>vehicle_plus</library>
 * <summary>Sets vehicle distance travelled.</summary>
 * <param name="vehicleid">Vehicle to set travelled distance to.</param>
 * <param name="unit">Unit we set distance in.</param>
 * <param name="distance">Distance we set.</param>
 * <seealso name="Vehicle_GetDistanceTravelled" />
 * <returns>False if vehicle is invalid, otherwise true.</returns> 
 */
stock bool: Vehicle_SetDistanceTravelled(vehicleid, VEHICLE_UNIT: unit, Float:distance)
{
	if(IsValidVehicle(vehicleid))
    {
        if(unit == VEHICLE_UNIT_METRIC)
        {
            Vehicle_gsDistanceTravelledTotalKilometres[vehicleid] = distance;
            Vehicle_gsDistanceTravelledTotalMiles[vehicleid] = distance * 0.621371;
        }
        else if(unit == VEHICLE_UNIT_IMPERIAL)
        {
            Vehicle_gsDistanceTravelledTotalMiles[vehicleid] = distance;
            Vehicle_gsDistanceTravelledTotalKilometres[vehicleid] = distance * 1.609344;
        }
        return true;
    }
	return false;
}

// --
// Dimensions
// --

/**
 * <library>vehicle_plus</library>
 * <summary>Sets the vehicle or group of vehicles interior.</summary>
 * <param name="id">Vehicle or group which we set the interior to.</param>
 * <param name="interior">Interior to set.</param>
 * <seealso name="Vehicle_GetInterior" />
 * <seealso name="Vehicle_SetVirtualWorld" />
 * <seealso name="Vehicle_GetVirtualWorld" />
 * <seealso name="Vehicle_SetDimensionsInfo" />
 * <seealso name="Vehicle_GetDimensionsInfo" />
 * <returns>LinkVehicleToInterior values.</returns>
 */
stock bool: Vehicle_SetInterior(VEHICLE_FRAMEWORK_TAGS: id, interiorid, tag=tagof(id)) 
{
    if(tag == tagof(_:))
    {
        return LinkVehicleToInterior(id, interiorid);
    }
    else if(tag == tagof(VehicleGroup:))
    {
        foreach(new i : Vehicle_gsGroups[VehicleGroup: id])
        {
            LinkVehicleToInterior(i, interiorid);
        }
    }
    return true;
}

/**
 * <library>vehicle_plus</library>
 * <summary>Gets the vehicle or group of vehicles interior.</summary>
 * <param name="vehicleid">Vehicle we get the interior from.</param>
 * <seealso name="Vehicle_SetInterior" />
 * <seealso name="Vehicle_SetVirtualWorld" />
 * <seealso name="Vehicle_GetVirtualWorld" />
 * <seealso name="Vehicle_SetDimensionsInfo" />
 * <seealso name="Vehicle_GetDimensionsInfo" />
 * <returns>INVALID_VEHICLE_ID if vehicle is invalid, otherwise true.</returns>
 */
stock Vehicle_GetInterior(vehicleid) 
{
    return ((IsValidVehicle(vehicleid)) ? (GetVehicleInterior(vehicleid)) : (INVALID_VEHICLE_ID));
}

/**
 * <library>vehicle_plus</library>
 * <summary>Sets the vehicle or group of vehicles Virtual World.</summary>
 * <param name="id">Vehicle or group which we set the Virtual World to.</param>
 * <param name="virtual_world">Virtual World to set.</param>
 * <seealso name="Vehicle_GetInterior" />
 * <seealso name="Vehicle_GetVirtualWorld" />
 * <seealso name="Vehicle_SetDimensionsInfo" />
 * <seealso name="Vehicle_GetDimensionsInfo" />
 * <returns>SetVehicleVirtualWorld values.</returns>
 */
stock bool: Vehicle_SetVirtualWorld(VEHICLE_FRAMEWORK_TAGS: id, virtual_world, tag=tagof(id))
{
    if(tag == tagof(_:))
    {
        if(!IsValidVehicle(id))
        {
            return false;
        }
        SetVehicleVirtualWorld(id, virtual_world);
        return true;
    }
    else if(tag == tagof(VehicleGroup:))
    {
        foreach(new i : Vehicle_gsGroups[VehicleGroup: id])
        {
            SetVehicleVirtualWorld(i, virtual_world);
        }
    }
    return true;
}

/**
 * <library>vehicle_plus</library>
 * <summary>Gets the vehicle virtual world.</summary>
 * <param name="vehicleid">Vehicle which we get the virtual world from.</param>
 * <seealso name="Vehicle_GetInterior" />
 * <seealso name="Vehicle_SetVirtualWorld" />
 * <seealso name="Vehicle_SetDimensionsInfo" />
 * <seealso name="Vehicle_GetDimensionsInfo" />
 * <returns>Vehicle virtual world or INVALID_VEHICLE_ID if vehicle is invalid.</returns>
*/
stock Vehicle_GetVirtualWorld(vehicleid) 
{
    return ((IsValidVehicle(vehicleid)) ? (GetVehicleVirtualWorld(vehicleid)) : (INVALID_VEHICLE_ID));
}

/**
 * <library>vehicle_plus</library>
 * <summary>Sets the vehicle virtual world and interior.</summary>
 * <param name="vehicleid">Vehicle which we set the virtual world and interior to.</param>
 * <param name="inteiorid">Interior to set.</param>
 * <param name="virtual_world">Virtual world to set.</param>
 * <seealso name="Vehicle_GetInterior" />
 * <seealso name="Vehicle_SetVirtualWorld" />
 * <seealso name="Vehicle_GetVirtualWorld" />
 * <seealso name="Vehicle_GetDimensionsInfo" />
 * <returns>SetVehicleVirtualWorld values.</returns>
*/
stock bool: Vehicle_SetDimensionInfo(VEHICLE_FRAMEWORK_TAGS: id, interiorid, virtual_world, tag=tagof(id))
{
    if(tag == tagof(_:))
    {
        LinkVehicleToInterior(id, interiorid);
        return SetVehicleVirtualWorld(id, virtual_world);
    }
    else if(tag == tagof(VehicleGroup:))
    {
        foreach(new i : Vehicle_gsGroups[VehicleGroup: id])
        {
            LinkVehicleToInterior(i, interiorid);
            SetVehicleVirtualWorld(i, virtual_world);
        }
    }
    return true;
}

/**
 * <library>vehicle_plus</library>
 * <summary>Gets the vehicle virtual world and interior.</summary>
 * <param name="vehicleid">Vehicle which we get the virtual world and interior from.</param>
 * <param name="&interiorid">Variable to store interior into.</param>
 * <param name="&virtualworld">Variable to store virtual world into.</param>
 * <seealso name="Vehicle_GetInterior" />
 * <seealso name="Vehicle_SetVirtualWorld" />
 * <seealso name="Vehicle_GetVirtualWorld" />
 * <seealso name="Vehicle_SetDimensionsInfo" />
 * <returns>SetVehicleVirtualWorld values.</returns>
*/
stock void: Vehicle_GetDimensionInfo(vehicleid, &interiorid, &virtualworld) 
{
    interiorid = Vehicle_GetInterior(vehicleid);
    virtualworld = Vehicle_GetVirtualWorld(vehicleid);
}

// --
// Occupied state
// --

stock Vehicle_GetSeatsNumber(vehicleid)
{
    return ((IsValidVehicle(vehicleid)) ? (GetVehicleSeats(GetVehicleModel(vehicleid))) : -(1));
}

/**
 * <summary> Check whether the vehicle is occupied.</summary>
 * <param name="vehicleid"> Vehicle we check.</param>
 * <returns> True if occupied, else false.</returns>
*/

stock bool: Vehicle_IsOccupied(vehicleid)
{
    return IsVehicleOccupied(vehicleid);
}

/**
 * <summary> Sets the vehicle or group dimensions.</summary>
 * <param name="vehicleid"> Vehicle which we get the last driver from.</param>
 * <returns> Playerid of vehicle last driver. </returns>
*/

stock Vehicle_GetLastDriver(vehicleid)
{
    return ((IsValidVehicle(vehicleid)) ? (GetVehicleLastDriver(vehicleid)) : -(1));
}

// --
// Time functions
// --


/**
* <summary> Sets the vehicle or groups respawn delay.</summary>
* <param name="id"> Vehicle or group which we set the delay to.</param>
* <param name="respawn_delay"> Delay to set.</param>
*/

stock bool: Vehicle_SetRespawnDelay(VEHICLE_FRAMEWORK_TAGS: id, respawn_delay, tag=tagof(id))
{
    if(tag == tagof(_:))
    {
        return SetVehicleRespawnDelay(id, respawn_delay)
    }
    else if(tag == tagof(VehicleGroup:))
    {
        foreach(new i : Vehicle_gsGroups[VehicleGroup: id])
        {
            SetVehicleRespawnDelay(i, respawn_delay)
        }
    }
    return true;
}

/**
 * <summary> Gets the vehicle respawn delay.</summary>
 * <param name="vehicleid"> Vehicle we get the delay from.</param>
 * <returns> Respawn delay. -2 if vehicle is invalid</returns>
 */

stock Vehicle_GetRespawnDelay(vehicleid)
{
    return ((IsValidVehicle(vehicleid)) ? (GetVehicleRespawnDelay(vehicleid)) : -(2));
}

/**
 * <summary> Gets the vehicle occupied time.</summary>
 * <param name="vehicleid"> Vehicle we get the occupied time from.</param>
 * <returns> -1 if vehicle is not occupied. Else occupied time.</returns>
 */

stock Vehicle_GetOccupiedTime(vehicleid)
{
    if(Vehicle_IsOccupied(vehicleid) && IsValidVehicle(vehicleid))
    {
        return ((gettime() - Vehicle_gsOccupiedTick[vehicleid]) * 1000);
    }
    return -1;
}

/**
 * <summary> Gets the vehicle (re)spawned time.</summary>
 * <param name="vehicleid"> Vehicle we get the respawned time from.</param>
 * <returns> Respawned time.</returns>
 */

stock Vehicle_GetRespawnedTime(vehicleid)
{
    if(IsValidVehicle(vehicleid))
    {
        return ((gettime() - Vehicle_gsRespawnTick[vehicleid]) * 1000);
    }
    return -1;
}

// By default, all vehicles have engines. 
// Bypass: Hook a callback to check if player is riding a manual vehicle (pedal power)
// and then turn engines on
// When the player have exited vehicle turn offV

hook OnPlayerStateChange(playerid, PLAYER_STATE:newstate, PLAYER_STATE:oldstate)
{
    if(oldstate == PLAYER_STATE_ONFOOT && (newstate == PLAYER_STATE_DRIVER || newstate == PLAYER_STATE_PASSENGER))
    {
        new vehicle = GetPlayerVehicleID(playerid);
        Vehicle_gsOccupiedTick[vehicle] = gettime();
        SetPVarInt(playerid, "FRAMEWORK_PlayerLastVehicle", vehicle);
    }
    else if((oldstate == PLAYER_STATE_DRIVER || oldstate == PLAYER_STATE_PASSENGER) && (newstate == PLAYER_STATE_ONFOOT || newstate == PLAYER_STATE_WASTED)) 
    {
        new vehicle = GetPVarInt(playerid, "FRAMEWORK_PlayerLastVehicle");
        Vehicle_gsOccupiedTick[vehicle] = -1;
        SetPVarInt(playerid, "FRAMEWORK_PlayerLastVehicle", 0);
    }
    return Y_HOOKS_CONTINUE_RETURN_1;
}

// --
// Respawn
// --

/**
 * <summary>(Re)spawns a vehicle or group of vehicles.</summary>
 * <param name="id">Vehicle or group of vehicles we respawn.</param>
 * <returns>False if function fails, eg. invalid vehicle, else true.</param>
*/
stock bool: Vehicle_Respawn(VEHICLE_FRAMEWORK_TAGS: id, tag=tagof(id))
{
    if(tag == tagof(_:))
    {
        if(!IsValidVehicle(id))
        {
            return false;
        }
        SetVehicleToRespawn(id);
    }
    else if(tag == tagof(VehicleGroup:))
    {
        foreach(new i : Vehicle_gsGroups[VehicleGroup: id])
        {
            SetVehicleToRespawn(i);
        }
    }
    return true;
}

// --
// Trailers
// --

forward OnTrailerHook(vehicleid, trailerid);
forward OnTrailerUnhook(vehicleid, trailerid);

/**
 * <summary> Attach a trailer to vehicle.</summary>
 * <param name="vehicleid"> Vehicle which we set the trailer to.</param>
 * <param name="trailerid"> Trailer to set.</param>
 * <returns> 0 if function fails, eg. trailerid is equal to vehicleid or vehicle and trailer aren't created, else 1.</param>
*/

stock bool: Vehicle_AttachTrailer(vehicleid, trailerid)
{
    if(!IsValidVehicle(vehicleid) || !IsValidVehicle(trailerid) || trailerid == vehicleid)
    {
        return false;
    }
    AttachTrailerToVehicle(trailerid, vehicleid);
    return true;
}

/**
 * <summary> Detach a trailer from vehicle.</summary>
 * <param name="vehicleid"> Vehicle we are detaching the trailer from.</param>
*/

stock bool: Vehicle_DetachTrailer(vehicleid)
{
    if(!IsValidVehicle(vehicleid))
    {
        return false;
    } 
    DetachTrailerFromVehicle(vehicleid);
    return true;
}

/**
 * <summary> Gets the vehicle trailer.</summary>
 * <param name="vehicleid"> Vehicle from which we get the trailerid from.</param>
 * <returns> Trailerid. </returns>
*/

stock Vehicle_GetTrailer(vehicleid)
{
    return GetVehicleTrailer(vehicleid);
}

/**
 * <summary> Gets the trailer's vehicle.</summary>
 * <param name="trailerid"> Trailer which we get the cab from.</param>
 * <returns> Vehicleid. </returns>
*/

stock Vehicle_GetTrailerCab(trailerid)
{
    return GetVehicleCab(trailerid);
}

hook OnVehicleSpawn(vehicleid)
{
    Vehicle_gsParams[vehicleid][PARAMS_ENGINE] = VEHICLE_PARAMS_OFF;
    Vehicle_gsParams[vehicleid][PARAMS_LIGHTS] = VEHICLE_PARAMS_OFF;
    Vehicle_gsParams[vehicleid][PARAMS_DOORS] = VEHICLE_PARAMS_OFF;
    Vehicle_gsParams[vehicleid][PARAMS_ALARM] = VEHICLE_PARAMS_OFF;
    Vehicle_gsParams[vehicleid][PARAMS_BOOT] = VEHICLE_PARAMS_OFF;
    Vehicle_gsParams[vehicleid][PARAMS_BONNET] = VEHICLE_PARAMS_OFF;
    Vehicle_gsParams[vehicleid][PARAMS_OBJECTIVE] = VEHICLE_PARAMS_OFF;

    Vehicle_gsWindows[vehicleid][VEHICLE_WINDOW_DRIVER] = VEHICLE_WINDOW_CLOSED;
    Vehicle_gsWindows[vehicleid][VEHICLE_WINDOW_PASSENGER] = VEHICLE_WINDOW_CLOSED;
    Vehicle_gsWindows[vehicleid][VEHICLE_WINDOW_REAR_LEFT] = VEHICLE_WINDOW_CLOSED;
    Vehicle_gsWindows[vehicleid][VEHICLE_WINDOW_REAR_RIGHT] = VEHICLE_WINDOW_CLOSED;

    Vehicle_gsRespawnTick[vehicleid] = gettime();
    Vehicle_gsOccupiedTick[vehicleid] = -1;

    return Y_HOOKS_CONTINUE_RETURN_1;
}

hook OnVehicleDeath(vehicleid, killerid)
{
    if(Vehicle_GetCategory(vehicleid) == CATEGORY_TRAILER)
    {
        new 
            vehicle = GetVehicleCab(vehicleid);

        if(vehicle != 0)
        {
            if(Vehicle_IsBlinking(vehicle))
            {
                DestroyDynamicObject(Vehicle_gsBlink[vehicle][1]);
                DestroyDynamicObject(Vehicle_gsBlink[vehicle][3]);
                Vehicle_gsBlink[vehicle][1] = -1;
                Vehicle_gsBlink[vehicle][3] = -1;
            }
        }
        return 1;
    }
    Vehicle_CancelBlinking(vehicleid);
    return 1;
}

static Vehicle_TrailerID[MAX_VEHICLES];

hook OnPlayerUpdate(playerid)
{
    if(GetPlayerState(playerid) == PLAYER_STATE_DRIVER)
    {
        new vehicleid = GetPlayerVehicleID(playerid),
            trailerid = GetVehicleTrailer(vehicleid);

        if(Vehicle_TrailerID[vehicleid] != trailerid)
        {
			if(trailerid != 0)
            {
                if(Vehicle_IsBlinking(vehicleid))
                {
                    new VEHICLE_BLINKERS: blinkers = Vehicle_gsBlinkSide[vehicleid];
                    Vehicle_CancelBlinking(vehicleid);
                    Vehicle_SetBlinking(vehicleid, blinkers);
                }
                CallLocalFunction("OnTrailerHook", "ii", vehicleid, trailerid);
			} 
            else 
            {
				if(Vehicle_TrailerID[vehicleid] != INVALID_VEHICLE_ID)
                {
                    new VEHICLE_BLINKERS: blinkers = Vehicle_gsBlinkSide[vehicleid];
                    if(Vehicle_IsBlinking(vehicleid))
                    {
                        Vehicle_CancelBlinking(vehicleid);
                        Vehicle_SetBlinking(vehicleid, blinkers);
                    }
					CallRemoteFunction("OnTrailerUnhook", "ii", vehicleid,Vehicle_TrailerID[vehicleid]);
				}
			}
			Vehicle_TrailerID[vehicleid] = trailerid;
		}
    }
    return 1;
}

// --
// Colors
// --

static Player_Money[MAX_PLAYERS];
ptask Vehicle_TrackPayNSpray[700](playerid) 
{
    new
        money = GetPlayerMoney(playerid);
        
    if(Player_Money[playerid] != money)
    {
        if(GetPlayerCameraMode(playerid) == CAM_MODE_FIXED)
        {
            switch(Player_Money[playerid] - money)
            {
                case 100:
                {
                    if(IsPlayerInsidePaySpray(playerid))
                    {
                        new color1, color2, model;
                        model = GetVehicleModel(GetPlayerVehicleID(playerid));
                        GetRandomVehicleColourPair(model, color1, color2);
                        Vehicle_SetColour(GetPlayerVehicleID(playerid), color1, color2);
                        CallLocalFunction("OnVehicleRespray", "iiii", playerid, GetPlayerVehicleID(playerid), color1, color2);
                    }
                }
            }
        }
    }
    Player_Money[playerid] = money;
}

// Developer functions
static stock bool: IsPlayerInsidePaySpray(playerid)
{
	for(new i = 0; i < sizeof(Vehicle_gscPayNSprayLocations); i++)
    {
        if(GetPlayerCameraMode(playerid) == CAM_MODE_FIXED && IsPlayerInRangeOfPoint(playerid, 6, Vehicle_gscPayNSprayLocations[i][0], Vehicle_gscPayNSprayLocations[i][1], Vehicle_gscPayNSprayLocations[i][2]))
        {
            return true;
        }
    }
    return false;
}

/**
 * <summary> Sets the vehicle or group color.</summary>
 * <param name="id"> Vehicle or group which we set the color to.</param>
 * <param name="color1"> First color to set.</param>
 * <param name="color2"> Second color to set.</param>
*/
stock Vehicle_SetColour(VEHICLE_FRAMEWORK_TAGS: id, color1, color2, tag=tagof(id))
{
    if(tag == tagof(_:))
    {
        return ChangeVehicleColours(id, color1, color2);
    }
    else if(tag == tagof(VehicleGroup:)) 
    {
        foreach(new i : Vehicle_gsGroups[VehicleGroup: id])
        {
            ChangeVehicleColours(i, color1, color2);
        }
    }
    return 1;
}

/**
 * <summary> Gets the vehicle color.</summary>
 * <param name="vehicleid"> Vehicle which we get the color from.</param>
 * <param name="color1"> First color to get.</param>
 * <param name="color2"> Second color to get.</param>
*/

stock void: Vehicle_GetColour(vehicleid, &color1, &color2) 
{
    GetVehicleColours(vehicleid, color1, color2);
}

// --
// Paintjob
// --

forward OnVehiclePaintjobChange(vehicleid, paintjobid);

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

/**
 * <summary> Sets the vehicle or group paintjob.</summary>
 * <param name="id"> Vehicle or group which we set the paintjob to.</param>
 * <param name="paintjob"> Paintjob to set.</param>
*/

stock Vehicle_SetPaintjob(VEHICLE_FRAMEWORK_TAGS: id, paintjobid, tag=tagof(id))
{
    if(tag == tagof(_:))
    {
        if((GetVehicleModel(id) == 483 && (paintjobid != 0 && paintjobid != 3)) || !IsValidVehicle(id)) 
        {
            return 0;
        }
        if(GetVehicleModel(id) == 575 && (paintjobid != 0 && paintjobid != 1 && paintjobid != 3)) 
        {
            return 0;
        }
    
        for(new i = 0; i < sizeof(Vehicle_gscPaintjob); i++)
        {
            if(GetVehicleModel(id) != Vehicle_gscPaintjob[i])
            {
                continue;
            }
            ChangeVehiclePaintjob(id, paintjobid);
            CallLocalFunction("OnVehiclePaintjobChange", "dd", id, paintjobid);
            break;
        }
    }
    else if(tag == tagof(VehicleGroup:))
    {
        foreach(new i : Vehicle_gsGroups[VehicleGroup: id])
        {
            if(GetVehicleModel(i) == 483 && (paintjobid != 0 && paintjobid != 3))
            {
                continue;
            }
            if(GetVehicleModel(i) == 575 && (paintjobid != 0 && paintjobid != 1 && paintjobid != 3)) 
            {
                continue;
            }

            for(new modelid = 0; modelid < sizeof(Vehicle_gscPaintjob); modelid++)
            {
                if(GetVehicleModel(i) != Vehicle_gscPaintjob[modelid])
                {
                    continue;
                }
                ChangeVehiclePaintjob(i, paintjobid);
                CallLocalFunction("OnVehiclePaintjobChange", "dd", i, paintjobid);
                break;
            }
        }
    }
    return 1;
}

/**
 * <summary> Gets the vehicle paintjob.</summary>
 * <param name="vehicleid"> Vehicle which we get the dimensions from.</param>
 * <returns> Vehicle paintjob.</returns>
*/

stock Vehicle_GetPaintjob(vehicleid)
{
    return ((IsValidVehicle(vehicleid)) ? (GetVehiclePaintjob(vehicleid)) : -(1));
}

// OnVehiclePaintjob gets called when player previews paintjob
// We need a callback to detect if paintjob has changed
// Solution: call OnVehiclePaintjobChange

hook OnVehiclePaintjob(playerid, vehicleid, paintjobid)
{
    if(paintjobid != GetVehiclePaintjob(vehicleid)) 
    {
        SetPVarInt(playerid, "FRAMEWORK_PaintJobLevel", 1);
        SetPVarInt(playerid, "FRAMEWORK_PaintJob", paintjobid);
    }
    return Y_HOOKS_CONTINUE_RETURN_1;
}

hook OnPlayerKeyStateChange(playerid, KEY: newkeys, KEY: oldkeys)
{ // vSync 
    if(RELEASED(KEY_SPRINT) && GetPlayerMoney(playerid) >= 500)
    {
        if(GetPVarInt(playerid, "FRAMEWORK_PaintJobLevel") == 1)
        {
            new vehicle = GetPlayerVehicleID(playerid);
            CallLocalFunction("OnVehiclePaintjobChange", "dd", vehicle, GetPVarInt(playerid, "FRAMEWORK_PaintJob"));
            SetPVarInt(playerid, "FRAMEWORK_PaintJob", -1);
            SetPVarInt(playerid, "FRAMEWORK_PaintJobLevel", 0);
        }
  
    }
    if(RELEASED(KEY_SECONDARY_ATTACK) && GetPVarInt(playerid, "FRAMEWORK_PaintJobLevel") != 0)
    {
        SetPVarInt(playerid, "FRAMEWORK_PaintJob", -1);
        SetPVarInt(playerid, "FRAMEWORK_PaintJobLevel", 0);
    }
    return Y_HOOKS_CONTINUE_RETURN_1;
}

hook OnPlayerConnect(playerid)
{
    SetPVarInt(playerid, "FRAMEWORK_PaintJob", -1);
    return Y_HOOKS_CONTINUE_RETURN_1;
}

// --
// Sirens 
// --

/**
 * <summary> Gets the vehicle siren's state. </summary>
 * <param name="vehicleid"> Vehicle which we get the siren state from.</param>
 * <returns> Siren state, 1 if enabled, else 0. </returns>
*/

#define VEHICLE_SIREN: bool:
#define VEHICLE_SIREN_ON true
#define VEHICLE_SIREN_OFF false

stock VEHICLE_SIREN: Vehicle_HasSirenGet(vehicleid)
{
    return GetVehicleParamsSirenState(vehicleid);
}

// -- 
// Create destroy
// --

static Timer: Vehicle_gsHealthTrackTimer[MAX_VEHICLES];

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
stock Vehicle_Create(modelid, Float: x, Float: y, Float: z, Float: rotation, color1, color2, respawn_delay, interior, virtual_world, bool:addsiren=false) 
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

    Vehicle_gsOccupiedTick[idx] = -1;
    Vehicle_gsRespawnTick[idx] = gettime();

    Vehicle_gsBlinkSide[idx] = VEHICLE_BLINKERS_NOT_SET;
    Vehicle_gsBlink[idx][0] = -1;
    Vehicle_gsBlink[idx][1] = -1;
    Vehicle_gsBlink[idx][2] = -1;
    Vehicle_gsBlink[idx][3] = -1;

    Vehicle_gsPositionX[idx] = x;
    Vehicle_gsPositionY[idx] = y;
    Vehicle_gsPositionZ[idx] = z;

    Vehicle_gsWindows[idx][0] = VEHICLE_WINDOW_CLOSED;
    Vehicle_gsWindows[idx][1] = VEHICLE_WINDOW_CLOSED;
    Vehicle_gsWindows[idx][2] = VEHICLE_WINDOW_CLOSED;
    Vehicle_gsWindows[idx][3] = VEHICLE_WINDOW_CLOSED;

    Vehicle_gsHealth[idx] = 1000;
    Vehicle_gsDistanceTimer[idx] = repeat __Vehicle_DistanceTravelled(idx);
    
    SetVehicleVirtualWorld(idx, virtual_world);
    LinkVehicleToInterior(idx, interior);
    Vehicle_gsHealthTrackTimer[idx] = repeat __Vehicle_HealthTrack(idx);
    CallLocalFunction("OnVehicleFirstSpawn", "i", idx);
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
stock Vehicle_CreateEx(modelid, Float: x, Float: y, Float: z, Float: rotation, color1, color2, respawn_delay, interior, virtual_world, Float: health, const string: plate[MAX_NUMBER_PLATE], bool:addsiren=false) 
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

    Vehicle_gsHealth[idx] = health;
    Vehicle_gsOccupiedTick[idx] = -1;
    Vehicle_gsRespawnTick[idx] = gettime();
    
    Vehicle_gsBlinkSide[idx] = VEHICLE_BLINKERS_NOT_SET;
    Vehicle_gsBlink[idx][0] = -1;
    Vehicle_gsBlink[idx][1] = -1;
    Vehicle_gsBlink[idx][2] = -1;
    Vehicle_gsBlink[idx][3] = -1;

    Vehicle_gsWindows[idx][0] = VEHICLE_WINDOW_CLOSED;
    Vehicle_gsWindows[idx][1] = VEHICLE_WINDOW_CLOSED;
    Vehicle_gsWindows[idx][2] = VEHICLE_WINDOW_CLOSED;
    Vehicle_gsWindows[idx][3] = VEHICLE_WINDOW_CLOSED;

    Vehicle_gsPositionX[idx] = x;
    Vehicle_gsPositionY[idx] = y;
    Vehicle_gsPositionZ[idx] = z;

    Vehicle_gsHealthTrackTimer[idx] = repeat __Vehicle_HealthTrack(idx);
    SetVehicleNumberPlate(idx, plate);
    Vehicle_gsDistanceTimer[idx] = repeat __Vehicle_DistanceTravelled(idx);
    
    SetVehicleHealth(idx, health);
    SetVehicleNumberPlate(idx, plate);
    SetVehicleVirtualWorld(idx, virtual_world);
    LinkVehicleToInterior(idx, interior);
    CallLocalFunction("OnVehicleFirstSpawn", "i", idx);
    return idx;
}

/**
 * <summary> Destroys vehicle. </summary>
*/

stock bool: Vehicle_Destroy(idx)
{
    if(!IsValidVehicle(idx)) 
    {
        return false;
    }
    Vehicle_gsHealth[idx] = 0;
    Vehicle_gsOccupiedTick[idx] = -1;

    Vehicle_gsPositionX[idx] = 0.0;
    Vehicle_gsPositionY[idx] = 0.0;
    Vehicle_gsPositionZ[idx] = 0.0;

    Vehicle_gsWindows[idx][0] = VEHICLE_WINDOW_CLOSED;
    Vehicle_gsWindows[idx][1] = VEHICLE_WINDOW_CLOSED;
    Vehicle_gsWindows[idx][2] = VEHICLE_WINDOW_CLOSED;
    Vehicle_gsWindows[idx][3] = VEHICLE_WINDOW_CLOSED;
    
    Vehicle_gsBlinkSide[idx] = VEHICLE_BLINKERS_NOT_SET;
    foreach(new i : Vehicle_gsActGps)
    {
        if(Vehicle_GroupHas(VehicleGroup: i, idx)) 
        {
            Vehicle_RemoveFromGroup(VehicleGroup: i, idx);
        }
    }
    stop Vehicle_gsHealthTrackTimer[idx];
    stop Vehicle_gsDistanceTimer[idx];
    CallLocalFunction("OnVehicleDestroy", "i", idx);
    DestroyVehicle(idx);
    return true;
}

#pragma warning pop
