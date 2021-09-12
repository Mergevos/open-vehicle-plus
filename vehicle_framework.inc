#include <a_samp>
#include <fixes>
#include <YSI_Data\y_foreach>

#include <YSI_Coding\y_timers>
#include <YSI_Coding\y_hooks>
#include <YSI_Game\y_vehicledata>
#include <streamer>


#if !defined RELEASED
	#define RELEASED(%0)			(((newkeys & (%0)) != (%0)) && ((oldkeys & (%0)) == (%0)))
#endif

#if defined EMPTY_DAMAGE 
    #include <weapon-config>
#endif

#if defined YSF_Enable
    #include <YSF>
#endif

#if !defined isnull
    #define isnull(%1) ((!(%1[0])) || (((%1[0]) == '\1') && (!(%1[1]))))
#endif

native _Framework_CallLocalFunction(const function[], const format[], {VehicleGroup, Float,_}:...) = CallLocalFunction;

// -- 
// Script-Side
// --

#define BLINK_SIDES 4


#if !defined MAX_NUMBER_PLATE
    #define MAX_NUMBER_PLATE 32
#endif

#if !defined MAX_VEHICLE_GROUPS
    #define MAX_VEHICLE_GROUPS 12
#endif

/*
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
};*/

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

    Vehicle_gsBlink[MAX_VEHICLES][BLINK_SIDES],

    Float: Vehicle_gsHealth[MAX_VEHICLES],
    Float: Vehicle_gsPosition[MAX_VEHICLES][4];

static
    Player_LastVehicleID[MAX_PLAYERS];


// --
// Vehicle Info
// --

enum e_VEHICLE_INFO
{
	E_VEHICLE_NAME[27],
	E_VEHICLE_WINDOWS_NUMBER,
	Float:E_VEHICLE_MASS,
	E_VEHICLE_DRIVE_TYPE,
	E_VEHICLE_ENGINE_TYPE
};

static const Vehicle_Info[212][e_VEHICLE_INFO] = 
{
    {"Landstalker", 4, 1700.0, '4', 'D'},
    {"Bravura", 2, 1300.0, 'F', 'P'},
    {"Buffalo", 2, 1500.0, 'R', 'P'},
    {"Linerunner", 2, 3800.0, 'R', 'D'},
    {"Perennial", 4, 1200.0, 'F', 'P'},
    {"Sentinel", 4, 1600.0, 'R', 'P'},
    {"Dumper", 1, 20000.0, 'R', 'D'},
    {"Firetruck", 2, 6500.0, 'R', 'D'},
    {"Trashmaster", 2, 5500.0, 'R', 'D'},
    {"Stretch", 4, 2200.0, 'R', 'P'},
    {"Manana", 2, 1000.0, 'F', 'P'},
    {"Infernus", 2, 1400.0, '4', 'P'},
    {"Voodoo", 2, 1800.0, 'R', 'P'},
    {"Pony", 4, 2600.0, 'R', 'D'},
    {"Mule", 2, 3500.0, 'R', 'D'},
    {"Cheetah", 2, 1200.0, 'R', 'P'},
    {"Ambulance", 4, 2600.0, '4', 'D'},
    {"Leviathan", 2, 15000.0, '4', 'P'}, 
    {"Moonbeam", 4, 2000.0, 'R', 'D'},
    {"Esperanto", 2, 1800.0, 'R', 'P'},
    {"Taxi", 4, 1450.0, 'F', 'P'},
    {"Washington", 2, 1850.0, 'R', 'P'},
    {"Bobcat", 2, 1700.0, '4', 'D'},
    {"Mr. Whoopee", 2, 1700.0, 'R', 'D'},
    {"BF Injection", 0, 1200.0, 'R', 'P'},
    {"Hunter", 1, 10000.0, '4', 'P'}, 
    {"Premier", 4, 1600.0, 'R', 'P'},
    {"Enforcer", 4, 4000.0, 'R', 'D'},
    {"Securicar", 2, 7000.0, 'R', 'D'},
    {"Banshee", 2, 1400.0, 'R', 'P'},
    {"Predator", 0, 2200.0, 'R', 'P'}, 
    {"Bus", 2, 5500.0, 'R', 'D'},
    {"Rhino", 0, 25000.0, '4', 'D'},
    {"Barracks", 2, 10500.0, '4', 'D'},
    {"Hotknife", 2, 1400.0, 'R', 'P'},
    {"Article Trailer", 0, 3800.0, 'R', 'D'},
    {"Previon", 2, 1400.0, 'F', 'P'},
    {"Coach", 0, 9500.0, 'R', 'D'},
    {"Cabbie", 4, 1750.0, 'R', 'P'},
    {"Stallion", 2, 1600.0, 'R', 'P'},
    {"Rumpo", 4, 2000.0, 'F', 'P'},
    {"RC Bandit", 0, 100.0, '4', 'E'},
    {"Romero", 2, 2500.0, 'R', 'P'},
    {"Packer", 2, 8000.0, 'R', 'D'},
    {"Monster", 2, 5000.0, '4', 'P'},
    {"Admiral", 4, 1650.0, 'F', 'P'},
    {"Squallo", 0, 2200.0, 'R', 'P'}, 
    {"Seasparrow", 2, 3000.0, '4', 'P'}, 
    {"Pizzaboy", 0, 350.0, 'R', 'P'}, 
    {"Tram", 0, 1900.0, 'R', 'P'}, 
    {"Article Trailer 2", 0, 3800.0, 'R', 'D'},
    {"Turismo", 2, 1400.0, '4', 'P'},
    {"Speeder", 0, 2200.0, 'R', 'P'}, 
    {"Reefer", 0, 5000.0, 'R', 'P'},
    {"Tropic", 0, 2200.0, 'R', 'P'}, 
    {"Flatbed", 2, 8500.0, 'R', 'D'},
    {"Yankee", 2, 4500.0, 'R', 'D'},
    {"Caddy", 0, 1000.0, '4', 'E'},
    {"Solair", 4, 2000.0, 'R', 'P'},
    {"Topfun Van (Berkley's RC)", 4, 1900.0, 'R', 'D'},
    {"Skimmer", 2, 5000.0, '4', 'P'},
    {"PCJ-600", 0, 500.0, 'R', 'P'},
    {"Faggio", 0, 350.0, 'R', 'P'},
    {"Freeway", 0, 800.0, 'R', 'P'},
    {"RC Baron", 0, 100.0, 'F', 'P'},
    {"RC Raider", 0, 100.0, '4', 'P'},
    {"Glendale", 4, 1600.0, 'R', 'P'},
    {"Oceanic", 4, 1900.0, 'R', 'P'},
    {"Sanchez", 0, 500.0, 'R', 'P'},
    {"Sparrow", 2, 2500.0, '4', 'P'},
    {"Patriot", 4, 2500.0, '4', 'P'},
    {"Quad", 0, 400.0, '4', 'P'},
    {"Coastguard", 0, 1200.0, 'R', 'P'},
    {"Dinghy", 0, 800.0, 'R', 'P'},
    {"Hermes", 2, 1950.0, 'F', 'P'},
    {"Sabre", 2, 1700.0, 'R', 'P'},
    {"Rustler", 1, 5000.0, '4', 'P'},
    {"ZR-350", 2, 1400.0, 'R', 'P'},
    {"Walton", 2, 1850.0, '4', 'D'},
    {"Regina", 4, 1500.0, 'F', 'P'},
    {"Comet", 2, 1400.0, '4', 'P'},
    {"BMX", 0, 100.0, 'R', 'P'},
    {"Burrito", 4, 1900.0, 'R', 'P'},
    {"Camper", 3, 1900.0, 'R', 'P'},
    {"Marquis", 0, 5000.0, 'R', 'P'},
    {"Baggage", 0, 1000.0, 'R', 'E'},
    {"Dozer", 0, 10000.0, '4', 'D'},
    {"Maverick", 4, 5000.0, '4', 'P'}, 
    {"News Chopper", 2, 3500.0, '4', 'P'}, 
    {"Rancher", 2, 2500.0, '4', 'P'},
    {"FBI Rancher", 4, 3500.0, '4', 'P'},
    {"Virgo", 2, 1700.0, 'R', 'P'},
    {"Greenwood", 4, 1600.0, 'R', 'P'},
    {"Jetmax", 0, 3000.0, 'R', 'P'}, 
    {"Hotring Racer", 2, 1600.0, 'R', 'P'},
    {"Sandking", 2, 2000.0, '4', 'P'},
    {"Blista Compact", 2, 1000.0, 'F', 'P'},
    {"Police Maverick", 4, 4500.0, '4', 'P'}, 
    {"Boxville", 2, 5500.0, 'R', 'D'},
    {"Benson", 2, 3500.0, 'R', 'D'},
    {"Mesa", 2, 1300.0, '4', 'D'},
    {"RC Goblin", 0, 100.0, '4', 'P'},
    {"Hotring Racer A", 2, 1600.0, 'R', 'P'},
    {"Hotring Racer B", 2, 1600.0, 'R', 'P'},
    {"Bloodring Banger", 2, 2100.0, 'R', 'P'},
    {"Rancher (Lure)", 2, 2500.0, '4', 'P'},
    {"Super GT", 2, 1400.0, 'R', 'P'},
    {"Elegant", 4, 2200.0, 'R', 'P'},
    {"Journey", 3, 3500.0, 'R', 'D'},
    {"Bike", 0, 100.0, 'R', 'P'}, 
    {"Mountain Bike", 0, 100.0, 'R', 'P'}, 
    {"Beagle", 2, 10000.0, '4', 'P'}, 
    {"Cropduster", 1, 5000.0, '4', 'P'}, 
    {"Stuntplane", 1, 5000.0, '4', 'P'}, 
    {"Tanker", 2, 3800.0, 'R', 'D'},
    {"Roadtrain", 2, 5000.0, 'R', 'D'},
    {"Nebula", 4, 1400.0, 'F', 'P'},
    {"Majestic", 2, 1400.0, 'R', 'P'},
    {"Buccaneer", 2, 1700.0, 'R', 'P'},
    {"Shamal", 0, 15000.0, '4', 'P'}, 
    {"Hydra", 1, 9000.0, '4', 'P'}, 
    {"FCR-900", 0, 500.0, 'R', 'P'}, 
    {"NRG-500", 0, 400.0, 'R', 'P'}, 
    {"HPV1000", 0, 500.0, 'R', 'P'}, 
    {"Cement Truck", 2, 5500.0, 'R', 'D'},
    {"Towtruck", 2, 3500.0, 'R', 'D'},
    {"Fortune", 2, 1700.0, 'R', 'P'},
    {"Cadrona", 2, 1200.0, 'R', 'P'},
    {"FBI Truck", 2, 4000.0, '4', 'D'},
    {"Willard", 4, 1800.0, 'R', 'P'},
    {"Forklift", 0, 1000.0, 'F', 'E'},
    {"Tractor", 0, 2000.0, 'R', 'D'},
    {"Combine Harvester", 1, 8500.0, '4', 'D'},
    {"Feltzer", 2, 1600.0, 'R', 'P'},
    {"Remington", 2, 1800.0, 'R', 'P'},
    {"Slamvan", 2, 1950.0, 'R', 'P'},
    {"Blade", 2, 1500.0, 'R', 'P'},
    {"Freight", 0, 5500.0, 'R', 'D'},
    {"Brown Streak", 0, 5500.0, 'R', 'D'},
    {"Vortex", 0, 1900.0, 'R', 'P'},
    {"Vincent", 4, 1800.0, 'F', 'P'},
    {"Bullet", 2, 1200.0, 'R', 'P'},
    {"Clover", 2, 1600.0, 'R', 'P'},
    {"Sadler", 2, 1700.0, '4', 'D'},
    {"Firetruck (ladder variant)", 2, 6500.0, 'R', 'D'},
    {"Hustler", 2, 1700.0, 'R', 'P'},
    {"Intruder", 4, 1800.0, 'R', 'P'},
    {"Primo", 4, 1600.0, 'R', 'P'},
    {"Cargobob", 2, 20000.0, '4', 'P'}, 
    {"Tampa", 2, 1700.0, 'R', 'P'},
    {"Sunrise", 4, 1600.0, 'F', 'P'},
    {"Merit", 4, 1800.0, 'R', 'P'},
    {"Utility Van", 2, 2600.0, 'R', 'D'},
    {"Nevada", 0, 25000.0, '4', 'P'}, 
    {"Yosemite", 2, 3000.0, 'R', 'P'},
    {"Windsor", 2, 1500.0, 'R', 'P'},
    {"Monster A", 2, 5000.0, '4', 'P'},
    {"Monster B", 2, 5000.0, '4', 'P'},
    {"Uranus", 2, 1400.0, 'R', 'P'},
    {"Jester", 2, 1500.0, 'F', 'P'},
    {"Sultan", 4, 1400.0, '4', 'P'},
    {"Stratum", 4, 1800.0, 'R', 'P'},
    {"Elegy", 2, 1500.0, 'R', 'P'},
    {"Raindance", 2, 10000.0, '4', 'P'}, 
    {"RC Tiger", 0, 100.0, '4', 'E'},
    {"Flash", 2, 1400.0, 'F', 'P'},
    {"Tahoma", 4, 1800.0, 'R', 'P'},
    {"Savanna", 2, 1500.0, 'R', 'P'},
    {"Bandito", 0, 1000.0, 'R', 'P'},
    {"Freight Flat Trailer", 0, 5500.0, 'R', 'D'},
    {"Brown Streak Trailer", 0, 5500.0, 'R', 'D'},
    {"Kart", 0, 300.0, 'R', 'P'},
    {"Mower", 0, 800.0, 'R', 'P'},
    {"Dune", 2, 10000.0, '4', 'P'},
    {"Sweeper", 2, 800.0, 'R', 'P'},
    {"Broadway", 2, 1700.0, 'R', 'P'},
    {"Tornado", 2, 1700.0, 'R', 'P'},
    {"AT-400", 0, 60000.0, '4', 'P'}, 
    {"DFT-30", 2, 5500.0, 'R', 'D'},
    {"Huntley", 4, 2500.0, '4', 'P'},
    {"Stafford", 4, 2200.0, 'R', 'P'},
    {"BF-400", 0, 500.0, 'R', 'P'}, 
    {"Newsvan", 3, 1900.0, 'R', 'D'},
    {"Tug", 0, 800.0, 'R', 'P'},
    {"Petrol Trailer", 0, 3800.0, 'R', 'D'},
    {"Emperor", 4, 1800.0, 'R', 'P'},
    {"Wayfarer", 0, 800.0, 'R', 'P'}, 
    {"Euros", 2, 1400.0, '4', 'P'},
    {"Hotdog", 2, 5500.0, 'R', 'D'},
    {"Club", 2, 1400.0, 'F', 'P'},
    {"Freight Box Trailer", 0, 5500.0, 'R', 'D'},
    {"Article Trailer 3", 0, 3800.0, 'R', 'D'},
    {"Andromada", 0, 40000.0, '4', 'P'}, 
    {"Dodo", 2, 5000.0, '4', 'P'}, 
    {"RC Cam", 0, 100.0, '4', 'E'},
    {"Launch", 0, 2200.0, 'R', 'P'}, 
    {"Police Car (LSPD)", 4, 1600.0, 'R', 'P'},
    {"Police Car (SFPD)", 4, 1600.0, 'R', 'P'},
    {"Police Car (LVPD)", 4, 1600.0, 'R', 'P'},
    {"Ranger", 2, 2500.0, '4', 'D'},
    {"Picador", 2, 1600.0, 'R', 'D'},
    {"S.W.A.T.", 0, 5000.0, '4', 'D'},
    {"Alpha", 2, 1500.0, 'R', 'P'},
    {"Phoenix", 2, 1500.0, 'R', 'P'},
    {"Glendale (destroyed)", 4, 1600.0, 'R', 'D'},
    {"Sadler (destroyed)", 2, 1700.0, '4', 'D'},
    {"Baggage Trailer A", 0, 1000.0, 'R', 'E'},
    {"Baggage Trailer B", 0, 1000.0, 'R', 'E'},
    {"Tug Stairs Trailer", 0, 1000.0, 'R', 'E'},
    {"Boxville (burglary)", 2, 5500.0, 'R', 'D'},
    {"Farm Trailer", 0, 400.0, 'R', 'E'},
    {"Utility Trailer", 0, 1000.0, 'R', 'E'}
};

/**
 * <summary> Returns vehicle name</summary>
 */

stock Vehicle_ReturnName(vehicleid, name[], len=sizeof(name)) 
{
    return strcopy(name, Vehicle_Info[GetVehicleModel(vehicleid) - 400][E_VEHICLE_NAME], len);
}

/**
 * <summary> Gets vehicle mass</summary>
 */

stock Float: Vehicle_GetMass(vehicleid) 
{
    return Vehicle_Info[GetVehicleModel(vehicleid) - 400][E_VEHICLE_MASS];
}


// --
// Blinkers
// --

enum e_BLINKERS
{
    E_BLINK_NOT_SET = -1,
    E_BLINK_LEFT = 0,
    E_BLINK_RIGHT,
    E_BLINK_EMERGENCY
}

new 
    e_BLINKERS: Vehicle_gsBlinkSide[MAX_VEHICLES];

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

forward OnVehicleFirstSpawn(vehicleid);
forward OnVehicleDestroy(vehicleid);

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

    Vehicle_gsBlinkSide[idx] = E_BLINK_NOT_SET;
    Vehicle_gsBlink[idx][0] = -1;
    Vehicle_gsBlink[idx][1] = -1;
    Vehicle_gsBlink[idx][2] = -1;
    Vehicle_gsBlink[idx][3] = -1;

    Vehicle_gsLastDriver[idx] = -1;
    Vehicle_gsIsOccupied[idx][0] = -1;
    Vehicle_gsIsOccupied[idx][1] = -1;
    Vehicle_gsIsOccupied[idx][2] = -1;
    Vehicle_gsIsOccupied[idx][3] = -1;
    Vehicle_gsHealth[idx] = 1000;
    
    SetVehicleVirtualWorld(idx, virtual_world);
    LinkVehicleToInterior(idx, interior);
    new Timer: Veh = repeat Vehicle_HealthTrack(idx, Vehicle_gsHealth[idx]);
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
    
    Vehicle_gsBlinkSide[idx] = E_BLINK_NOT_SET;
    Vehicle_gsIsOccupied[idx][0] = -1;
    Vehicle_gsIsOccupied[idx][1] = -1;
    Vehicle_gsIsOccupied[idx][2] = -1;
    Vehicle_gsIsOccupied[idx][3] = -1;
    repeat Vehicle_HealthTrack(idx);
    strcopy(Vehicle_gsNumberPlate[idx], plate);
    
    SetVehicleHealth(idx, health);
    SetVehicleNumberPlate(idx, plate);
    SetVehicleVirtualWorld(idx, virtual_world);
    LinkVehicleToInterior(idx, interior);
    CallLocalFunction("OnVehicleFirstSpawn", "i", idx);
    return idx;
}

// --
// Groups 
// --

forward OnGroupInitialize(VehicleGroup: groupid);
forward OnGroupAdd(VehicleGroup: groupid, vehicleid);
forward OnGroupRemoveMember(VehicleGroup: groupid, vehicleid);
forward OnGroupClear(VehicleGroup: groupid);

#define VEHICLE_FRAMEWORK_TAGS \
    {_, VehicleGroup}

#define INVALID_VEHICLE_GROUP_ID 0

static
    Vehicle_gsGroupName[VehicleGroup: MAX_VEHICLE_GROUPS][24],
    Iterator: Vehicle_gsActGps<MAX_VEHICLE_GROUPS>,
    Iterator: Vehicle_gsGroups[VehicleGroup: MAX_VEHICLE_GROUPS]<MAX_VEHICLES>;

/**
 * <summary> Looks for a group with specified name.</summary>
 * <param name="name">Name to look. </param>
 * <returns> This function returns -1 if name is NULL, INVALID_VEHICLE_GROUP_ID if there're no groups to match with the name, and group id if there's a group with the name. </returns>
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
 * <summary> Checks whether the group has vehicleid.</summary>
 * <param name="vehicleid">Vehicleid to look for. </param>
 * <returns> Returns true if has, else false. </returns>
*/

stock Vehicle_GroupHas(VehicleGroup: groupid, vehicleid)
{
    if(Iter_Contains(Vehicle_gsActGps, _:groupid) && IsValidVehicle(vehicleid) && Iter_Contains(Vehicle_gsGroups[groupid], vehicleid))
    {
        return true;
    }
    return false;
}

/**
 * <summary> Initialize a group.</summary>
 * <param name="name">Name to set. </param>
 * <returns> ID of the group. </returns>
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
 * <summary> Checks whether a group is created.</summary>
 * <param name="groupid">Group ID. </param>
 * <returns> False if isn't, else true. </returns>
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
 * <summary> Adds a vehicle to a created group.</summary>
 * <param name="groupid">Group ID. </param>
 * <param name="vehicleid">Vehicleid to add. </param>
 * <returns> INVALID_VEHICLE_GROUP if vehicle or group isn't valid. </returns>
*/

stock Vehicle_AddToGroup(VehicleGroup: groupid, vehicleid)
{
    if(!Vehicle_IsValidGroup(groupid) || !IsValidVehicle(vehicleid) || Iter_Contains(Vehicle_gsGroups[groupid], vehicleid))
    {
        return INVALID_VEHICLE_GROUP_ID;
    }
    Iter_Add(Vehicle_gsGroups[groupid], vehicleid);
    _Framework_CallLocalFunction("OnGroupAddMember", "ii", groupid, vehicleid);
    return 1;
}



/**
 * <summary> Removes a vehicle from a created group.</summary>
 * <param name="groupid">Group ID. </param>
 * <param name="vehicleid">Vehicleid to remove. </param>
 * <returns> INVALID_VEHICLE_GROUP if vehicle or group isn't valid. </returns>
*/

stock Vehicle_RemoveFromGroup(VehicleGroup: groupid, vehicleid)
{
    if(!Vehicle_IsValidGroup(groupid) || !Iter_Contains(Vehicle_gsGroups[groupid], vehicleid))
    {
        return INVALID_VEHICLE_GROUP_ID;
    }
    Iter_Remove(Vehicle_gsGroups[groupid], vehicleid);
    _Framework_CallLocalFunction("OnGroupRemoveMember", "ii", groupid, vehicleid);
    return 1;
}

/**
 * <summary> Clears a group.</summary>
 * <param name="groupid">Group ID. </param>
*/

stock Vehicle_ClearGroup(VehicleGroup: groupid)
{
    if(!Vehicle_IsValidGroup(groupid) || Iter_IsEmpty(Vehicle_gsGroups[groupid]))
    {
        return;
    }
    foreach(new i : Vehicle_gsGroups[groupid])
    {
        Iter_SafeRemove(Vehicle_gsGroups[groupid], i, i);
    }
    _Framework_CallLocalFunction("OnGroupClear", "i", groupid);
}

/**
 * <summary> Checks if a group is empty.</summary>
 * <param name="groupid">True if empty, else false. </param>
*/

stock Vehicle_IsEmptyGroup(VehicleGroup: groupid)
{
    if(!Vehicle_IsValidGroup(groupid))
    {
        return -1;
    }
    else if(Iter_IsEmpty(Vehicle_gsGroups[groupid]))
    {
        return true;
    }
    return false;
}

/**
 * <summary> Destroys a group.</summary>
 * <param name="groupid">Group to destroy. </param>
*/

stock Vehicle_GroupDestroy(VehicleGroup: groupid)
{
    if(!Iter_Contains(Vehicle_gsActGps, _:groupid))
    {
        return;
    }
    foreach(new i : Vehicle_gsGroups[groupid])
    {
        Iter_SafeRemove(Vehicle_gsGroups[groupid], i, i);
    }
    Iter_Remove(Vehicle_gsActGps, _:groupid);
}

// --
// Destroy Vehicle
// --

/**
 * <summary> Destroys vehicle. </summary>
*/



stock Vehicle_Destroy(idx)
{
    if(!IsValidVehicle(idx)) 
    {
        return 0;
    }
    Vehicle_gsModel[idx] = -1;
    Vehicle_gsPosition[idx][0] = -1;
    Vehicle_gsPosition[idx][1] = -1;
    Vehicle_gsPosition[idx][2] = -1;
    Vehicle_gsPosition[idx][3] = -1;
    Vehicle_gsSirenState[idx] = -1;
    Vehicle_gsColorInfo[idx][0] = -1;
    Vehicle_gsColorInfo[idx][1] = -1;
    Vehicle_gsPaintjob[idx] = -1;
    Vehicle_gsTrailer[idx][0] = -1;
    Vehicle_gsTrailer[idx][1] = -1;
    Vehicle_gsRespawnDelay[idx] = -1;
    Vehicle_gsDimensionInfo[idx][DIMENSION_INTERIOR] = -1;
    Vehicle_gsDimensionInfo[idx][DIMENSION_VIRTUAL_WORLD] = -1;
    Vehicle_gsHealth[idx] = -1;
    Vehicle_gsOccupiedTick[idx] = -1;
    Vehicle_gsRespawnTick[idx] = -1;

    Vehicle_gsLastDriver[idx] = -1;
    
    Vehicle_gsBlinkSide[idx] = E_BLINK_NOT_SET;
    Vehicle_gsIsOccupied[idx][0] = -1;
    Vehicle_gsIsOccupied[idx][1] = -1;
    Vehicle_gsIsOccupied[idx][2] = -1;
    Vehicle_gsIsOccupied[idx][3] = -1;
    CallLocalFunction("OnVehicleDestroy", "i", idx);
    foreach(new i : Vehicle_gsActGps)
    {
        if(Vehicle_GroupHas(VehicleGroup: i, idx)) 
        {
            Vehicle_RemoveFromGroup(VehicleGroup: i, idx);
        }
    }
    DestroyVehicle(idx);
    return 1;
}

// --
// Blinkers
// --



enum e_BLINKING_ENUM 
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

/* <summary> Set vehicle's indicators.</summary>
 * <param name="vehicleid"> Vehicleid which we set indicators.</param>
 * <param name="side"> Indicators side. </param>
 */

stock Vehicle_SetBlinking(VEHICLE_FRAMEWORK_TAGS: vehicleid, e_BLINKERS: side, bool:skip=false, tag=tagof(vehicleid)) //0 - left 1 - right 2 - emergy
{

    if(tag == tagof(_:))
    {
    
        new 
            modelid = GetVehicleModel(_:vehicleid),
            id = modelid-400,
            trailer;

        

        if(Vehicle_IsBlinking(vehicleid) && !skip) 
        {
            return Vehicle_CancelBlinking(vehicleid);
        }
    
        if(!skip) 
        {
            Vehicle_gsBlinkSide[vehicleid] = side;
        }
    
        if(id >= 0)
        {
            if(Vehicle_gscBlinkOffsets[id][e_BLINKING_POSX] != 0.0)
            {
                if(side == E_BLINK_LEFT)
                {
                    Vehicle_gsBlink[vehicleid][0] = CreateDynamicObject(19294, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0); //blink yellow  front
                }
                else if(side == E_BLINK_RIGHT)
                {
                    Vehicle_gsBlink[vehicleid][2] = CreateDynamicObject(19294, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0);
                }
            }
            if((trailer = Vehicle_GetTrailer(vehicleid)) != 0)
            {
                if(Vehicle_GetCategory(trailer) == CATEGORY_TRAILER)
                {
                    if(side == E_BLINK_LEFT)
                    {
                        Vehicle_gsBlink[vehicleid][1] = CreateDynamicObject(19294, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0); //blink yellow back truck only trailer
                        
                    }
                    else if(side == E_BLINK_RIGHT)
                    {
                        Vehicle_gsBlink[vehicleid][3] = CreateDynamicObject(19294, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0);    
                    }
                }
            }
            else
            {
                if(Vehicle_gscBlinkOffsets[id][e_BLINKING_ROTX] != 0.0)
                {
                    if(side == E_BLINK_LEFT)
                    {
                        Vehicle_gsBlink[vehicleid][1] = CreateDynamicObject(19294, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0); //blink yellow back truck only trailer
                    }
                    else if(side == E_BLINK_RIGHT)
                    {
                        Vehicle_gsBlink[vehicleid][3] = CreateDynamicObject(19294, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0);
                    }
                }
            }
    
            if(side == E_BLINK_LEFT) //left
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
            else if(side == E_BLINK_RIGHT) //right
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
            else if(side == E_BLINK_EMERGENCY)  //emergency
            {
                Vehicle_SetBlinking(vehicleid, E_BLINK_LEFT, true);
                Vehicle_SetBlinking(vehicleid, E_BLINK_RIGHT, true);
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
    return 1;
}

/* <summary> Checks if vehicle's blinking.</summary>
 * <param name="vehicleid"> Vehicleid which whose indicators we check. </param>
 * <returns> 0 if not blinking, else 1.</returns>
 */

stock Vehicle_IsBlinking(vehicleid)
{
    if(Vehicle_gsBlinkSide[vehicleid] == E_BLINK_NOT_SET)
    {
        return 0;
    }

    return 1;
}

/* <summary> Cancel vehicle blinking.</summary>
 * <param name="vehicleid"> Vehicleid whose indicators we cancel. </param>
 * <returns> 0 if not blinking, else if function success 1.</returns>
 */

stock Vehicle_CancelBlinking(vehicleid)
{
    if(!Vehicle_IsBlinking(vehicleid)) 
    {
        return 0;
    }

    for(new i; i < BLINK_SIDES; i++)
    {
        if(Vehicle_gsBlink[vehicleid][i] != -1) 
        {
            DestroyDynamicObject(Vehicle_gsBlink[vehicleid][i]);
        }
        Vehicle_gsBlink[vehicleid][i] = -1;
        Vehicle_gsBlinkSide[vehicleid] = E_BLINK_NOT_SET;
        
    }
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

enum e_DRIVE_TYPE 
{
    E_DRIVE_4WD = 1,
    E_DRIVE_FWD = 2,
    E_DRIVE_RWD
};

enum e_ENGINE_TYPE
{
    E_ENGINE_PETROL = 1,
    E_ENGINE_DIESEL = 2,
    E_ENGINE_ELECTRIC
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

stock Vehicle_SetEngineState(VEHICLE_FRAMEWORK_TAGS: id, e_ENGINE_STATES: engine_state, tag=tagof(id)) 
{
    if(tag==tagof(_:))
    {
        GetVehicleParamsEx(id, Vehicle_gsParams[id][PARAMS_ENGINE], Vehicle_gsParams[id][PARAMS_LIGHTS], Vehicle_gsParams[id][PARAMS_ALARM], Vehicle_gsParams[id][PARAMS_DOORS], Vehicle_gsParams[id][PARAMS_BONNET], Vehicle_gsParams[id][PARAMS_BOOT], Vehicle_gsParams[id][PARAMS_OBJECTIVE]);
        new const ret = SetVehicleParamsEx(id, (Vehicle_gsParams[id][PARAMS_ENGINE] == E_ENGINE_STATE_OFF ? E_ENGINE_STATE_ON : E_ENGINE_STATE_ON), Vehicle_gsParams[id][PARAMS_LIGHTS], Vehicle_gsParams[id][PARAMS_ALARM], Vehicle_gsParams[id][PARAMS_DOORS], Vehicle_gsParams[id][PARAMS_BONNET], Vehicle_gsParams[id][PARAMS_BOOT], Vehicle_gsParams[id][PARAMS_OBJECTIVE]);
        if(ret == 0) 
        {
            return ret;
        } 
        Vehicle_gsParams[id][PARAMS_ENGINE] = engine_state;
        return ret;
    }
    else if(tag==tagof(VehicleGroup:))
    {
        foreach(new i : Vehicle_gsGroups[VehicleGroup: id])
        {
            GetVehicleParamsEx(i, Vehicle_gsParams[i][PARAMS_ENGINE], Vehicle_gsParams[i][PARAMS_LIGHTS], Vehicle_gsParams[i][PARAMS_ALARM], Vehicle_gsParams[i][PARAMS_DOORS], Vehicle_gsParams[i][PARAMS_BONNET], Vehicle_gsParams[i][PARAMS_BOOT], Vehicle_gsParams[i][PARAMS_OBJECTIVE]);
            Vehicle_gsParams[i][PARAMS_ENGINE] = engine_state;
            SetVehicleParamsEx(i, Vehicle_gsParams[i][PARAMS_ENGINE], Vehicle_gsParams[i][PARAMS_LIGHTS], Vehicle_gsParams[i][PARAMS_ALARM], Vehicle_gsParams[i][PARAMS_DOORS], Vehicle_gsParams[i][PARAMS_BONNET], Vehicle_gsParams[i][PARAMS_BOOT], Vehicle_gsParams[i][PARAMS_OBJECTIVE]);
        }
    }
    return 1;
}

/**
 * <summary> Gets vehicle drive type
 */

stock e_DRIVE_TYPE: Vehicle_GetDriveType(vehicleid) 
{
    new model = GetVehicleModel(vehicleid) - 400;
    if(Vehicle_Info[model][E_VEHICLE_DRIVE_TYPE] == 'R') 
    {
        return E_DRIVE_RWD;
    }
    else if(Vehicle_Info[model][E_VEHICLE_DRIVE_TYPE] == 'F') 
    {
        return E_DRIVE_FWD;
    }
    else if(Vehicle_Info[model][E_VEHICLE_DRIVE_TYPE] == '4') 
    {
        return E_DRIVE_4WD;
    }
    return e_DRIVE_TYPE:0;
}

/**
 * <summary> Gets vehicle engine type
 */

stock e_ENGINE_TYPE: Vehicle_GetEngineType(vehicleid) 
{
    new model = GetVehicleModel(vehicleid) - 400;
    if(Vehicle_Info[model][E_VEHICLE_ENGINE_TYPE] == 'D') 
    {
        return E_ENGINE_DIESEL;
    }
    else if(Vehicle_Info[model][E_VEHICLE_ENGINE_TYPE] == 'P') 
    {
        return E_ENGINE_PETROL;
    }
    else if(Vehicle_Info[model][E_VEHICLE_ENGINE_TYPE] == 'E') 
    {
        return E_ENGINE_ELECTRIC;
    }
    return e_ENGINE_TYPE: 0;
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

stock Vehicle_SetBootState(VEHICLE_FRAMEWORK_TAGS: id, e_BOOT_STATES: boot_state, tag=tagof(id)) 
{
    if(tag==tagof(_:)) 
    {        
        GetVehicleParamsEx(id, Vehicle_gsParams[id][PARAMS_ENGINE], Vehicle_gsParams[id][PARAMS_LIGHTS], Vehicle_gsParams[id][PARAMS_ALARM], Vehicle_gsParams[id][PARAMS_DOORS], Vehicle_gsParams[id][PARAMS_BONNET], Vehicle_gsParams[id][PARAMS_BOOT], Vehicle_gsParams[id][PARAMS_OBJECTIVE]);
        Vehicle_gsParams[id][PARAMS_BOOT] = boot_state;
        SetVehicleParamsEx(id, Vehicle_gsParams[id][PARAMS_ENGINE], Vehicle_gsParams[id][PARAMS_LIGHTS], Vehicle_gsParams[id][PARAMS_ALARM], Vehicle_gsParams[id][PARAMS_DOORS], Vehicle_gsParams[id][PARAMS_BONNET], Vehicle_gsParams[id][PARAMS_BOOT], Vehicle_gsParams[id][PARAMS_OBJECTIVE]);
    }
    else if(tag==tagof(VehicleGroup:))
    {
        foreach(new i : Vehicle_gsGroups[VehicleGroup: id])
        {
            GetVehicleParamsEx(i, Vehicle_gsParams[i][PARAMS_ENGINE], Vehicle_gsParams[i][PARAMS_LIGHTS], Vehicle_gsParams[i][PARAMS_ALARM], Vehicle_gsParams[i][PARAMS_DOORS], Vehicle_gsParams[i][PARAMS_BONNET], Vehicle_gsParams[i][PARAMS_BOOT], Vehicle_gsParams[i][PARAMS_OBJECTIVE]);
            Vehicle_gsParams[i][PARAMS_BOOT] = boot_state;
            SetVehicleParamsEx(i, Vehicle_gsParams[i][PARAMS_ENGINE], Vehicle_gsParams[i][PARAMS_LIGHTS], Vehicle_gsParams[i][PARAMS_ALARM], Vehicle_gsParams[i][PARAMS_DOORS], Vehicle_gsParams[i][PARAMS_BONNET], Vehicle_gsParams[i][PARAMS_BOOT], Vehicle_gsParams[i][PARAMS_OBJECTIVE]);
        }
    }
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

stock Vehicle_SetBonnetState(VEHICLE_FRAMEWORK_TAGS: id, e_BONNET_STATES: bonnet_state, tag=tagof(id)) 
{
    if(tag==tagof(_:)) 
    {        
        GetVehicleParamsEx(id, Vehicle_gsParams[id][PARAMS_ENGINE], Vehicle_gsParams[id][PARAMS_LIGHTS], Vehicle_gsParams[id][PARAMS_ALARM], Vehicle_gsParams[id][PARAMS_DOORS], Vehicle_gsParams[id][PARAMS_BONNET], Vehicle_gsParams[id][PARAMS_BOOT], Vehicle_gsParams[id][PARAMS_OBJECTIVE]);
        Vehicle_gsParams[id][PARAMS_BONNET] = bonnet_state;
        SetVehicleParamsEx(id, Vehicle_gsParams[id][PARAMS_ENGINE], Vehicle_gsParams[id][PARAMS_LIGHTS], Vehicle_gsParams[id][PARAMS_ALARM], Vehicle_gsParams[id][PARAMS_DOORS], Vehicle_gsParams[id][PARAMS_BONNET], Vehicle_gsParams[id][PARAMS_BOOT], Vehicle_gsParams[id][PARAMS_OBJECTIVE]);
    }
    else if(tag==tagof(VehicleGroup:))
    {
        foreach(new i : Vehicle_gsGroups[VehicleGroup: id])
        {
            GetVehicleParamsEx(i, Vehicle_gsParams[i][PARAMS_ENGINE], Vehicle_gsParams[i][PARAMS_LIGHTS], Vehicle_gsParams[i][PARAMS_ALARM], Vehicle_gsParams[i][PARAMS_DOORS], Vehicle_gsParams[i][PARAMS_BONNET], Vehicle_gsParams[i][PARAMS_BOOT], Vehicle_gsParams[i][PARAMS_OBJECTIVE]);
            Vehicle_gsParams[i][PARAMS_BONNET] = bonnet_state;
            SetVehicleParamsEx(i, Vehicle_gsParams[i][PARAMS_ENGINE], Vehicle_gsParams[i][PARAMS_LIGHTS], Vehicle_gsParams[i][PARAMS_ALARM], Vehicle_gsParams[i][PARAMS_DOORS], Vehicle_gsParams[i][PARAMS_BONNET], Vehicle_gsParams[i][PARAMS_BOOT], Vehicle_gsParams[i][PARAMS_OBJECTIVE]);
        }
    }
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

stock e_OBJECTIVE_STATES: Vehicle_GetObjectiveState(vehicleid) 
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

stock Vehicle_SetObjectiveState(VEHICLE_FRAMEWORK_TAGS: id, e_OBJECTIVE_STATES: objective_state, tag=tagof(id)) 
{
    if(tag==tagof(_:)) 
    {        
        GetVehicleParamsEx(id, Vehicle_gsParams[id][PARAMS_ENGINE], Vehicle_gsParams[id][PARAMS_LIGHTS], Vehicle_gsParams[id][PARAMS_ALARM], Vehicle_gsParams[id][PARAMS_DOORS], Vehicle_gsParams[id][PARAMS_BONNET], Vehicle_gsParams[id][PARAMS_BOOT], Vehicle_gsParams[id][PARAMS_OBJECTIVE]);
        Vehicle_gsParams[id][PARAMS_OBJECTIVE] = objective_state;
        SetVehicleParamsEx(id, Vehicle_gsParams[id][PARAMS_ENGINE], Vehicle_gsParams[id][PARAMS_LIGHTS], Vehicle_gsParams[id][PARAMS_ALARM], Vehicle_gsParams[id][PARAMS_DOORS], Vehicle_gsParams[id][PARAMS_BONNET], Vehicle_gsParams[id][PARAMS_BOOT], Vehicle_gsParams[id][PARAMS_OBJECTIVE]);
    }
    else if(tag==tagof(VehicleGroup:))
    {
        foreach(new i : Vehicle_gsGroups[VehicleGroup: id])
        {
            GetVehicleParamsEx(i, Vehicle_gsParams[i][PARAMS_ENGINE], Vehicle_gsParams[i][PARAMS_LIGHTS], Vehicle_gsParams[i][PARAMS_ALARM], Vehicle_gsParams[i][PARAMS_DOORS], Vehicle_gsParams[i][PARAMS_BONNET], Vehicle_gsParams[i][PARAMS_BOOT], Vehicle_gsParams[i][PARAMS_OBJECTIVE]);
            Vehicle_gsParams[i][PARAMS_OBJECTIVE] = objective_state;
            SetVehicleParamsEx(i, Vehicle_gsParams[i][PARAMS_ENGINE], Vehicle_gsParams[i][PARAMS_LIGHTS], Vehicle_gsParams[i][PARAMS_ALARM], Vehicle_gsParams[i][PARAMS_DOORS], Vehicle_gsParams[i][PARAMS_BONNET], Vehicle_gsParams[i][PARAMS_BOOT], Vehicle_gsParams[i][PARAMS_OBJECTIVE]);
        }
    }
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


stock Vehicle_SetLightsState(VEHICLE_FRAMEWORK_TAGS: id, e_LIGHT_STATES: left_lights, e_LIGHT_STATES: right_lights, e_LIGHT_STATES: back_lights, tag=tagof(id)) 
{
    new 
        lights, 
        unnecessary_doors,
        unnecessary_panels, 
        unnecessary_tires;

    if(tag == tagof(_:))
    {
        GetVehicleDamageStatus(id, unnecessary_panels, unnecessary_doors, lights, unnecessary_tires);
        new ret = UpdateVehicleDamageStatus(id, unnecessary_panels, unnecessary_doors, vehicle_EncodeLights(left_lights, right_lights, back_lights), unnecessary_tires);
        return ret;
    }
    else if(tag == tagof(VehicleGroup:))
    {
        foreach(new i : Vehicle_gsGroups[VehicleGroup: id])
        {
            GetVehicleDamageStatus(i, unnecessary_panels, unnecessary_doors, lights, unnecessary_tires);
            UpdateVehicleDamageStatus(i, unnecessary_panels, unnecessary_doors, vehicle_EncodeLights(left_lights, right_lights, back_lights), unnecessary_tires);
        }
    }
    return 1;
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
        
    new ret = GetVehicleDamageStatus(vehicleid, unnecessary_params, unnecessary_params, lights, unnecessary_params);
    if(ret == 0)
    {
        return 0;
    }
    vehicle_DecodeLights(lights, left_lights, right_lights, back_lights);
    return ret;

}


/**
 * <summary> Sets the lights state.</summary>
 * <param name="vehicleid"> Vehicle which we set the lights running state.</param>
 * <param name="light_state"> Lights state to set.</param>
 * <returns> SetVehicleParamsEx values. </returns>
*/

stock Vehicle_SetLightsRunState(VEHICLE_FRAMEWORK_TAGS: id, e_LIGHT_RUN_STATE: light_state, tag=tagof(id)) 
{
    if(tag==tagof(_:)) 
    {        
        GetVehicleParamsEx(id, Vehicle_gsParams[id][PARAMS_ENGINE], Vehicle_gsParams[id][PARAMS_LIGHTS], Vehicle_gsParams[id][PARAMS_ALARM], Vehicle_gsParams[id][PARAMS_DOORS], Vehicle_gsParams[id][PARAMS_BONNET], Vehicle_gsParams[id][PARAMS_BOOT], Vehicle_gsParams[id][PARAMS_OBJECTIVE]);
        new const ret = SetVehicleParamsEx(id, Vehicle_gsParams[id][PARAMS_ENGINE], Vehicle_gsParams[id][PARAMS_LIGHTS] == E_LIGHTS_ON ? E_LIGHTS_OFF : E_LIGHTS_ON, Vehicle_gsParams[id][PARAMS_ALARM], Vehicle_gsParams[id][PARAMS_DOORS], Vehicle_gsParams[id][PARAMS_BONNET], Vehicle_gsParams[id][PARAMS_BOOT], Vehicle_gsParams[id][PARAMS_OBJECTIVE]);
        if(ret == 0)
        {
            return ret;
        }
        Vehicle_gsParams[id][PARAMS_LIGHTS] = light_state;
        return ret;
    }
    else if(tag==tagof(VehicleGroup:))
    {
        foreach(new i : Vehicle_gsGroups[VehicleGroup: id])
        {
            GetVehicleParamsEx(i, Vehicle_gsParams[i][PARAMS_ENGINE], Vehicle_gsParams[i][PARAMS_LIGHTS], Vehicle_gsParams[i][PARAMS_ALARM], Vehicle_gsParams[i][PARAMS_DOORS], Vehicle_gsParams[i][PARAMS_BONNET], Vehicle_gsParams[i][PARAMS_BOOT], Vehicle_gsParams[i][PARAMS_OBJECTIVE]);
            Vehicle_gsParams[i][PARAMS_LIGHTS] = light_state;
            SetVehicleParamsEx(i, Vehicle_gsParams[i][PARAMS_ENGINE], Vehicle_gsParams[i][PARAMS_LIGHTS], Vehicle_gsParams[i][PARAMS_ALARM], Vehicle_gsParams[i][PARAMS_DOORS], Vehicle_gsParams[i][PARAMS_BONNET], Vehicle_gsParams[i][PARAMS_BOOT], Vehicle_gsParams[i][PARAMS_OBJECTIVE]);
        }
    }
    return 1;
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

stock Vehicle_SetAlarms(VEHICLE_FRAMEWORK_TAGS: id, e_ALARMS_STATE: alarm_state, tag=tagof(id)) 
{
    if(tag==tagof(_:)) 
    {        
        GetVehicleParamsEx(id, Vehicle_gsParams[id][PARAMS_ENGINE], Vehicle_gsParams[id][PARAMS_LIGHTS], Vehicle_gsParams[id][PARAMS_ALARM], Vehicle_gsParams[id][PARAMS_DOORS], Vehicle_gsParams[id][PARAMS_BONNET], Vehicle_gsParams[id][PARAMS_BOOT], Vehicle_gsParams[id][PARAMS_OBJECTIVE]);
        new ret = SetVehicleParamsEx(id, Vehicle_gsParams[id][PARAMS_ENGINE], Vehicle_gsParams[id][PARAMS_LIGHTS], (Vehicle_gsParams[id][PARAMS_ALARM] == E_ALARMS_ON ? E_ALARMS_OFF : E_ALARMS_ON), Vehicle_gsParams[id][PARAMS_DOORS], Vehicle_gsParams[id][PARAMS_BONNET], Vehicle_gsParams[id][PARAMS_BOOT], Vehicle_gsParams[id][PARAMS_OBJECTIVE]);
        if(ret == 0)
        {
            return ret;
        }
        Vehicle_gsParams[id][PARAMS_ALARM] = alarm_state;
        return ret;
    }
    else if(tag==tagof(VehicleGroup:))
    {
        foreach(new i : Vehicle_gsGroups[VehicleGroup: id])
        {
            GetVehicleParamsEx(i, Vehicle_gsParams[i][PARAMS_ENGINE], Vehicle_gsParams[i][PARAMS_LIGHTS], Vehicle_gsParams[i][PARAMS_ALARM], Vehicle_gsParams[i][PARAMS_DOORS], Vehicle_gsParams[i][PARAMS_BONNET], Vehicle_gsParams[i][PARAMS_BOOT], Vehicle_gsParams[i][PARAMS_OBJECTIVE]);
            Vehicle_gsParams[i][PARAMS_ALARM] = alarm_state;
            SetVehicleParamsEx(i, Vehicle_gsParams[i][PARAMS_ENGINE], Vehicle_gsParams[i][PARAMS_LIGHTS], Vehicle_gsParams[i][PARAMS_ALARM], Vehicle_gsParams[i][PARAMS_DOORS], Vehicle_gsParams[i][PARAMS_BONNET], Vehicle_gsParams[i][PARAMS_BOOT], Vehicle_gsParams[i][PARAMS_OBJECTIVE]);
        }
    }
    return 1;
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

stock Vehicle_SetDoorState(VEHICLE_FRAMEWORK_TAGS: id, e_DOOR_STATES: hood, e_DOOR_STATES: trunk, e_DOOR_STATES: driver, e_DOOR_STATES: passenger, tag=tagof(id)) 
{
    new
        doors,
        unnecessary_panels,
        unnecessary_tires,
        unnecessary_lights;
    
    if(tag == tagof(_:)) 
    {
        GetVehicleDamageStatus(id, unnecessary_panels, doors, unnecessary_lights, unnecessary_tires);
        UpdateVehicleDamageStatus(id, unnecessary_panels, vehicle_EncodeDoors(hood, trunk, driver, passenger), unnecessary_lights, unnecessary_tires);
    }
    else if(tag == tagof(VehicleGroup:))
    {
        foreach(new i : Vehicle_gsGroups[VehicleGroup: id])
        {
            GetVehicleDamageStatus(i, unnecessary_panels, doors, unnecessary_lights, unnecessary_tires);
            UpdateVehicleDamageStatus(i, unnecessary_panels, vehicle_EncodeDoors(hood, trunk, driver, passenger), unnecessary_lights, unnecessary_tires);
        } 
    }
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

stock Vehicle_SetDoorsLockState(VEHICLE_FRAMEWORK_TAGS: id, e_DOOR_LOCK_STATES: door_state, tag=tagof(id)) 
{
    if(tag==tagof(_:)) 
    {        
        GetVehicleParamsEx(id, Vehicle_gsParams[id][PARAMS_ENGINE], Vehicle_gsParams[id][PARAMS_LIGHTS], Vehicle_gsParams[id][PARAMS_ALARM], Vehicle_gsParams[id][PARAMS_DOORS], Vehicle_gsParams[id][PARAMS_BONNET], Vehicle_gsParams[id][PARAMS_BOOT], Vehicle_gsParams[id][PARAMS_OBJECTIVE]);
        Vehicle_gsParams[id][PARAMS_DOORS] = door_state;
        SetVehicleParamsEx(id, Vehicle_gsParams[id][PARAMS_ENGINE], Vehicle_gsParams[id][PARAMS_LIGHTS], Vehicle_gsParams[id][PARAMS_ALARM], Vehicle_gsParams[id][PARAMS_DOORS], Vehicle_gsParams[id][PARAMS_BONNET], Vehicle_gsParams[id][PARAMS_BOOT], Vehicle_gsParams[id][PARAMS_OBJECTIVE]);
    }
    else if(tag==tagof(VehicleGroup:))
    {
        foreach(new i : Vehicle_gsGroups[VehicleGroup: id])
        {
            GetVehicleParamsEx(i, Vehicle_gsParams[i][PARAMS_ENGINE], Vehicle_gsParams[i][PARAMS_LIGHTS], Vehicle_gsParams[i][PARAMS_ALARM], Vehicle_gsParams[i][PARAMS_DOORS], Vehicle_gsParams[i][PARAMS_BONNET], Vehicle_gsParams[i][PARAMS_BOOT], Vehicle_gsParams[i][PARAMS_OBJECTIVE]);
            Vehicle_gsParams[i][PARAMS_DOORS] = door_state;
            SetVehicleParamsEx(i, Vehicle_gsParams[i][PARAMS_ENGINE], Vehicle_gsParams[i][PARAMS_LIGHTS], Vehicle_gsParams[i][PARAMS_ALARM], Vehicle_gsParams[i][PARAMS_DOORS], Vehicle_gsParams[i][PARAMS_BONNET], Vehicle_gsParams[i][PARAMS_BOOT], Vehicle_gsParams[i][PARAMS_OBJECTIVE]);
        }
    }
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

stock Vehicle_SetTireState(VEHICLE_FRAMEWORK_TAGS: id, e_TIRE_STATUS: rear_right_tire, e_TIRE_STATUS: front_right_tire, e_TIRE_STATUS: rear_left_tire, e_TIRE_STATUS: front_left_tire, tag=tagof(id)) 
{    
    new
        tires,
        unnecessary_panels,
        unnecessary_doors,
        unnecessary_lights;

    if(tag == tagof(_:)) 
    {
        GetVehicleDamageStatus(id, unnecessary_panels, unnecessary_doors, unnecessary_lights, tires);
        UpdateVehicleDamageStatus(id, unnecessary_panels, unnecessary_doors, unnecessary_lights, vehicle_EncodeTires(rear_right_tire, front_right_tire, rear_left_tire, front_left_tire));
    }
    else if(tag == tagof(VehicleGroup:))
    {
        foreach(new i : Vehicle_gsGroups[VehicleGroup: id])
        {
            GetVehicleDamageStatus(i, unnecessary_panels, unnecessary_doors, unnecessary_lights, tires);
            UpdateVehicleDamageStatus(i, unnecessary_panels, unnecessary_doors, unnecessary_lights, vehicle_EncodeTires(rear_right_tire, front_right_tire, rear_left_tire, front_left_tire));
        }
    }
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
    if(!IsValidVehicle(vehicleid)) 
    {
        return 0;
    }
    #if defined YSF_Enable
        GetVehicleNumberPlate(vehicleid, plate, len);
    #else
        strcopy(plate, Vehicle_gsNumberPlate[vehicleid], len);
    #endif
    return 1;
}

/**
 * <summary> Sets the vehicle's number plate.</summary>
 * <param name="vehicleid"> Vehicle which we set the vehicle plate.</param>
 * <param name="plate"> Plate to set.</param>
*/

stock Vehicle_SetNumberPlate(VEHICLE_FRAMEWORK_TAGS: id, const plate[], tag=tagof(id)) 
{
    if(tag == tagof(_:))
    {
        strcopy(Vehicle_gsNumberPlate[id], plate);
        SetVehicleNumberPlate(id, Vehicle_gsNumberPlate[id]);
    }
    else if(tag == tagof(VehicleGroup:))
    {   
        foreach(new i : Vehicle_gsGroups[VehicleGroup: id])
        {
            strcopy(Vehicle_gsNumberPlate[id], plate);
            SetVehicleNumberPlate(i, Vehicle_gsNumberPlate[i]);
        } 
    }
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
 * <summary> Gets the vehicle's windows number.</summary>
 */

stock Vehicle_GetWindowsNumber(vehicleid) 
{
    return Vehicle_Info[GetVehicleModel(vehicleid) - 400][E_VEHICLE_WINDOWS_NUMBER];
}

/**
 * <summary> Sets the vehicle's windows states.</summary>
 * <param name="vehicleid"> Vehicle which we set the vehicle plate.</param>
 * <param name="driver"> Driver-window state to set.</param>
 * <param name="passenger"> Passenger-window state to set.</param>
 * <param name="backleft"> Backleft-window state to set.</param>
 * <param name="backright"> Backright-window state to gset.</param>
*/

stock Vehicle_SetWindows(VEHICLE_FRAMEWORK_TAGS: id, e_WINDOWS_STATES: driver, e_WINDOWS_STATES: passenger, e_WINDOWS_STATES: backleft, e_WINDOWS_STATES: backright, tag=tagof(id)) 
{
    if(tag == tagof(_:))
    {
        Vehicle_gsWindows[id][WINDOWS_DRIVER] = driver;
        Vehicle_gsWindows[id][WINDOWS_PASSENGER] = passenger;
        Vehicle_gsWindows[id][WINDOWS_REAR_LEFT] = backleft;
        Vehicle_gsWindows[id][WINDOWS_REAR_RIGHT] = backright;
        SetVehicleParamsCarWindows(id, Vehicle_gsWindows[id][WINDOWS_DRIVER], Vehicle_gsWindows[id][WINDOWS_PASSENGER], Vehicle_gsWindows[id][WINDOWS_REAR_LEFT], Vehicle_gsWindows[id][WINDOWS_REAR_RIGHT]);
    }
    else if(tag == tagof(VehicleGroup:))
    {
        foreach(new i : Vehicle_gsGroups[VehicleGroup: id])
        {
            Vehicle_gsWindows[i][WINDOWS_DRIVER] = driver;
            Vehicle_gsWindows[i][WINDOWS_PASSENGER] = passenger;
            Vehicle_gsWindows[i][WINDOWS_REAR_LEFT] = backleft;
            Vehicle_gsWindows[i][WINDOWS_REAR_RIGHT] = backright;
            SetVehicleParamsCarWindows(i, Vehicle_gsWindows[i][WINDOWS_DRIVER], Vehicle_gsWindows[i][WINDOWS_PASSENGER], Vehicle_gsWindows[i][WINDOWS_REAR_LEFT], Vehicle_gsWindows[i][WINDOWS_REAR_RIGHT]);
        }
    }
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

stock Vehicle_SetPanelStates(VEHICLE_FRAMEWORK_TAGS: id, e_PANEL_STATES: front_left_panel, e_PANEL_STATES: front_right_panel, e_PANEL_STATES: back_left_panel, e_PANEL_STATES: back_right_panel, tag=tagof(id)) 
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

    if(tag == tagof(_:))
    {
        GetVehicleDamageStatus(id, panels, unnecessary_doors, unnecessary_lights, unnecessary_tires);
        vehicle_DecodePanels(panels, unnecessary_decodes, unnecessary_decodes, unnecessary_decodes, unnecessary_decodes, unnecessary_windshield, unnecessary_front_bumper, unnecessary_rear_bumper);
        UpdateVehicleDamageStatus(id, vehicle_EncodePanels(front_left_panel, front_right_panel, back_left_panel, back_right_panel, unnecessary_windshield, unnecessary_front_bumper, unnecessary_rear_bumper), unnecessary_doors, unnecessary_lights, unnecessary_tires);
    }
    else if(tag == tagof(VehicleGroup:))
    {
        foreach(new i : Vehicle_gsGroups[VehicleGroup: id])
        {
            GetVehicleDamageStatus(i, panels, unnecessary_doors, unnecessary_lights, unnecessary_tires);
            vehicle_DecodePanels(panels, unnecessary_decodes, unnecessary_decodes, unnecessary_decodes, unnecessary_decodes, unnecessary_windshield, unnecessary_front_bumper, unnecessary_rear_bumper);
            UpdateVehicleDamageStatus(i, vehicle_EncodePanels(front_left_panel, front_right_panel, back_left_panel, back_right_panel, unnecessary_windshield, unnecessary_front_bumper, unnecessary_rear_bumper), unnecessary_doors, unnecessary_lights, unnecessary_tires);
        }
    }
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

stock Vehicle_SetBumperStates(VEHICLE_FRAMEWORK_TAGS: id, e_PANEL_STATES: front_bumper, e_PANEL_STATES: rear_bumper, tag=tagof(id)) 
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

    if(tag == tagof(_:))
    {
        GetVehicleDamageStatus(id, panels, unnecessary_doors, unnecessary_lights, unnecessary_tires);
        vehicle_DecodePanels(panels, unnecessary_front_left, unnecessary_front_right, unnecessary_back_left, unnecessary_back_right, unnecessary_windshield, unnecessary_decodes, unnecessary_decodes);
        UpdateVehicleDamageStatus(id, vehicle_EncodePanels(unnecessary_front_left, unnecessary_front_right, unnecessary_back_left, unnecessary_back_right, unnecessary_windshield, front_bumper, rear_bumper), unnecessary_doors, unnecessary_lights, unnecessary_tires);
    }
    else if(tag == tagof(VehicleGroup:))
    {
        foreach(new i : Vehicle_gsGroups[VehicleGroup: id])
        {
            GetVehicleDamageStatus(i, panels, unnecessary_doors, unnecessary_lights, unnecessary_tires);
            vehicle_DecodePanels(panels, unnecessary_front_left, unnecessary_front_right, unnecessary_back_left, unnecessary_back_right, unnecessary_windshield, unnecessary_decodes, unnecessary_decodes);
            UpdateVehicleDamageStatus(i, vehicle_EncodePanels(unnecessary_front_left, unnecessary_front_right, unnecessary_back_left, unnecessary_back_right, unnecessary_windshield, front_bumper, rear_bumper), unnecessary_doors, unnecessary_lights, unnecessary_tires);
        }
    }
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

/**
 * <summary> Sets the vehicle or group color.</summary>
 * <param name="id"> Vehicle or group which we set the color to.</param>
 * <param name="color1"> First color to set.</param>
 * <param name="color2"> Second color to set.</param>
*/

stock Vehicle_SetColor(VEHICLE_FRAMEWORK_TAGS: id, color1, color2, tag=tagof(id))
{
    if(tag == tagof(_:))
    {
        Vehicle_gsColorInfo[id][0] = (color1 == -1 ? random(255) : color1); 
        Vehicle_gsColorInfo[id][1] = (color2 == -1 ? random(255) : color2);
        
        ChangeVehicleColor(id, Vehicle_gsColorInfo[id][0], Vehicle_gsColorInfo[id][1]);
    }
    else if(tag == tagof(VehicleGroup:)) 
    {
        foreach(new i : Vehicle_gsGroups[VehicleGroup: id])
        {
            Vehicle_gsColorInfo[i][0] = (color1 == -1 ? random(255) : color1);
            Vehicle_gsColorInfo[i][1] = (color2 == -1 ? random(255) : color2);
            ChangeVehicleColor(i, Vehicle_gsColorInfo[i][0], Vehicle_gsColorInfo[i][1]);
        }
    }
}

/**
 * <summary> Gets the vehicle color.</summary>
 * <param name="vehicleid"> Vehicle which we get the color from.</param>
 * <param name="color1"> First color to get.</param>
 * <param name="color2"> Second color to get.</param>
*/

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

/**
 * <summary> Sets the vehicle or group interior.</summary>
 * <param name="id"> Vehicle or group which we set the interior to.</param>
 * <param name="interior"> Interior to set.</param>
*/

stock Vehicle_SetInterior(VEHICLE_FRAMEWORK_TAGS: id, interiorid, tag=tagof(id)) 
{
    if(tag == tagof(_:))
    {
        Vehicle_gsDimensionInfo[id][DIMENSION_INTERIOR] = interiorid;
        LinkVehicleToInterior(id, Vehicle_gsDimensionInfo[id][DIMENSION_INTERIOR]);
    }
    else if(tag == tagof(VehicleGroup:))
    {
        foreach(new i : Vehicle_gsGroups[VehicleGroup: id])
        {
            Vehicle_gsDimensionInfo[i][DIMENSION_INTERIOR] = interiorid;
            LinkVehicleToInterior(i, Vehicle_gsDimensionInfo[i][DIMENSION_INTERIOR]);
        }
    }
}

/**
 * <summary> Gets the vehicle or group interior.</summary>
 * <param name="vehicleid"> Vehicle or group which we set the interior to.</param>
 * <returns> Vehicle interior. </returns>
*/

stock Vehicle_GetInterior(vehicleid) 
{
    #if !defined YSF_Enable
        return Vehicle_gsDimensionInfo[vehicleid][DIMENSION_INTERIOR];
    #else
        return GetVehicleInterior(vehicleid);
    #endif
}

/**
 * <summary> Sets the vehicle or group virtual world.</summary>
 * <param name="id"> Vehicle or group which we set the virtual world to.</param>
 * <param name="virtual_world"> Virtual World to set.</param>
*/

stock Vehicle_SetVirtualWorld(VEHICLE_FRAMEWORK_TAGS: id, virtual_world, tag=tagof(id))
{
    if(tag == tagof(_:))
    {
        Vehicle_gsDimensionInfo[id][DIMENSION_VIRTUAL_WORLD] = virtual_world;
        SetVehicleVirtualWorld(id, Vehicle_gsDimensionInfo[id][DIMENSION_VIRTUAL_WORLD]);
    }
    else if(tag == tagof(VehicleGroup:))
    {
        foreach(new i : Vehicle_gsGroups[VehicleGroup: id])
        {
            Vehicle_gsDimensionInfo[i][DIMENSION_VIRTUAL_WORLD] = virtual_world;
            SetVehicleVirtualWorld(i, Vehicle_gsDimensionInfo[i][DIMENSION_VIRTUAL_WORLD]);
        }
    }
}


/**
 * <summary> Gets the vehicle virtual world.</summary>
 * <param name="vehicleid"> Vehicle or group which we set the interior to.</param>
 * <returns> Vehicle virtual world. </returns>
*/

stock Vehicle_GetVirtualWorld(vehicleid) 
{
    return Vehicle_gsDimensionInfo[vehicleid][DIMENSION_VIRTUAL_WORLD];
}

/**
 * <summary> Sets the vehicle or group dimensions.</summary>
 * <param name="id"> Vehicle or group which we set the dimensions to.</param>
 * <param name="interior"> Interior to set.</param>
 * <param name="virtual_world"> Virtual World to set.</param>
*/

stock Vehicle_SetDimensionInfo(VEHICLE_FRAMEWORK_TAGS: id, interiorid, virtual_world, tag=tagof(id))
{
    if(tag == tagof(_:))
    {
        Vehicle_gsDimensionInfo[id][DIMENSION_INTERIOR] = interiorid;
        Vehicle_gsDimensionInfo[id][DIMENSION_VIRTUAL_WORLD] = virtual_world;
        LinkVehicleToInterior(id, Vehicle_gsDimensionInfo[id][DIMENSION_INTERIOR]);
        SetVehicleVirtualWorld(id, Vehicle_gsDimensionInfo[id][DIMENSION_VIRTUAL_WORLD]);
    }
    else if(tag == tagof(VehicleGroup:))
    {
        foreach(new i : Vehicle_gsGroups[VehicleGroup: id])
        {
            Vehicle_gsDimensionInfo[i][DIMENSION_INTERIOR] = interiorid;
            Vehicle_gsDimensionInfo[i][DIMENSION_VIRTUAL_WORLD] = virtual_world;
            LinkVehicleToInterior(i, Vehicle_gsDimensionInfo[i][DIMENSION_INTERIOR]);
            SetVehicleVirtualWorld(i, Vehicle_gsDimensionInfo[i][DIMENSION_VIRTUAL_WORLD]);
        }
    }
}

/**
 * <summary> Sets the vehicle or group dimensions.</summary>
 * <param name="id"> Vehicle or group which we get the dimensions from.</param>
 * <param name="&interior"> Interior to get.</param>
 * <param name="&virtual_world"> Virtual World to get.</param>
*/

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

forward OnVehiclePaintjobChange(vehicleid, paintjobid);

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
            Vehicle_gsPaintjob[id] = paintjobid;
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
                Vehicle_gsPaintjob[i] = paintjobid;
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
    #if !defined YSF_Enable 
        return Vehicle_gsPaintjob[vehicleid];
    #else 
        return GetVehiclePaintjob(vehicleid);
    #endif
}

// --
// Occupied state
// --

stock Vehicle_GetSeatsNumber(vehicleid)
{
    return GetVehicleSeats(GetVehicleModel(vehicleid));
}

/**
 * <summary> Check whether the vehicle is occupied.</summary>
 * <param name="vehicleid"> Vehicle we check.</param>
 * <returns> True if occupied, else false.</returns>
*/

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

/**
 * <summary> Sets the vehicle or group dimensions.</summary>
 * <param name="vehicleid"> Vehicle which we get the last driver from.</param>
 * <returns> Playerid of vehicle last driver. </returns>
*/

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

forward OnTrailerHook(vehicleid, trailerid);
forward OnTrailerUnhook(vehicleid, trailerid);

/**
 * <summary> Attach a trailer to vehicle.</summary>
 * <param name="vehicleid"> Vehicle which we set the trailer to.</param>
 * <param name="trailerid"> Trailer to set.</param>
 * <returns> 0 if function fails, eg. trailerid is equal to vehicleid or vehicle and trailer aren't created, else 1.</param>
*/

stock Vehicle_AttachTrailer(vehicleid, trailerid)
{
    if(!IsValidVehicle(vehicleid) || !IsValidVehicle(trailerid) || trailerid == vehicleid)
    {
        return 0;
    }
    Vehicle_gsTrailer[vehicleid][0] = trailerid;
    Vehicle_gsTrailer[trailerid][1] = vehicleid;
    AttachTrailerToVehicle(trailerid, vehicleid);
    if(Vehicle_IsBlinking(vehicleid))
    {
        new e_BLINKERS: blinkers = Vehicle_gsBlinkSide[vehicleid];
        Vehicle_CancelBlinking(vehicleid);
        Vehicle_SetBlinking(vehicleid, blinkers);
    }
    return 1;
}

/**
 * <summary> Detach a trailer from vehicle.</summary>
 * <param name="vehicleid"> Vehicle we are detaching the trailer from.</param>
*/

stock Vehicle_DetachTrailer(vehicleid)
{
    new trailer = Vehicle_gsTrailer[vehicleid][0];
    Vehicle_gsTrailer[vehicleid][0] = 0;
    Vehicle_gsTrailer[trailer][1] = 0;
    DetachTrailerFromVehicle(vehicleid); 
    if(Vehicle_IsBlinking(vehicleid))
    {
        new e_BLINKERS: blinkers = Vehicle_gsBlinkSide[vehicleid];
        Vehicle_CancelBlinking(vehicleid);
        Vehicle_SetBlinking(vehicleid, blinkers);
    }
}

/**
 * <summary> Gets the vehicle trailer.</summary>
 * <param name="vehicleid"> Vehicle from which we get the trailerid from.</param>
 * <returns> Trailerid. </returns>
*/



stock Vehicle_GetTrailer(vehicleid)
{
    return Vehicle_gsTrailer[vehicleid][0];
}

/**
 * <summary> Gets the trailer's vehicle.</summary>
 * <param name="trailerid"> Trailer which we get the cab from.</param>
 * <returns> Vehicleid. </returns>
*/

stock Vehicle_GetTrailerCab(trailerid)
{
    return Vehicle_gsTrailer[trailerid][1];
}


// --
// Time functions
// --

#if defined YSF_Enable

    /**
    * <summary> Sets the vehicle or groups respawn delay.</summary>
    * <param name="id"> Vehicle or group which we set the delay to.</param>
    * <param name="respawn_delay"> Delay to set.</param>
    */

    stock Vehicle_SetRespawnDelay(VEHICLE_FRAMEWORK_TAGS: id, respawn_delay, tag=tagof(id))
    {
        if(tag == tagof(_:))
        {
            Vehicle_gsRespawnDelay[id] = respawn_delay;
            SetVehicleRespawnDelay(id, respawn_delay);
        }
        else if(tag == tagof(VehicleGroup:))
        {
            foreach(new i : Vehicle_gsGroups[VehicleGroup: id])
            {
                Vehicle_gsRespawnDelay[i] = respawn_delay;
                SetVehicleRespawnDelay(i, respawn_delay);
            }
        }
    }
#endif

/**
 * <summary> Gets the vehicle respawn delay.</summary>
 * <param name="vehicleid"> Vehicle we get the delay from.</param>
 * <returns> Respawn delay.</returns>
 */

stock Vehicle_GetRespawnDelay(vehicleid)
{
    #if defined YSF_Enable
        return GetVehicleRespawnDelay(vehicleid);
    #else
        return Vehicle_gsRespawnDelay[vehicleid];
    #endif
}

/**
 * <summary> Gets the vehicle occupied time.</summary>
 * <param name="vehicleid"> Vehicle we get the occupied time from.</param>
 * <returns> -1 if vehicle is occupied. Else occupied time.</returns>
 */

stock Vehicle_GetOccupiedTime(vehicleid)
{
    if(Vehicle_IsOccupied(vehicleid))
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
    return ((gettime() - Vehicle_gsRespawnTick[vehicleid]) * 1000);
}

// --
// Sirens 
// --

/**
 * <summary> Gets the vehicle siren's state. </summary>
 * <param name="vehicleid"> Vehicle which we get the siren state from.</param>
 * <returns> Siren state, 1 if enabled, else 0. </returns>
*/

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

hook OnEnterExitModShop(playerid, enterexit, interiorid) 
{
    if(enterexit) 
    {
        SetPVarInt(playerid, "Player_IsInModShop", 1);
    }
    else
    {
        SetPVarInt(playerid, "Player_IsInModShop", 0);
    }
    return Y_HOOKS_CONTINUE_RETURN_1;
}
/*
hook OnVehicleRespray(playerid, vehicleid, color1, color2)
{
    if(GetPlayerMoney(playerid) < 100 || GetPVarInt(playerid, "Player_IsInModShop") == 1 || (color1 == Vehicle_gsColorInfo[vehicleid][0] && color2 == Vehicle_gsColorInfo[vehicleid][1] && !IsPlayerInsidePaySpray(playerid)))
	{
		return Y_HOOKS_BREAK_RETURN_0;
	}

    Vehicle_gsColorInfo[vehicleid][0] = color1;
    Vehicle_gsColorInfo[vehicleid][1] = color2;
    return Y_HOOKS_CONTINUE_RETURN_1;
}*/


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

    return Y_HOOKS_CONTINUE_RETURN_1;
}

hook OnVehicleDeath(vehicleid, killerid)
{
    if(Vehicle_GetCategory(vehicleid) == CATEGORY_TRAILER)
    {
        new 
            vehicle = Vehicle_gsTrailer[vehicleid][0];

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
    return Y_HOOKS_CONTINUE_RETURN_1;
}


hook OnPlayerConnect(playerid)
{
    SetPVarInt(playerid, "FRAMEWORK_PaintJob", -1);
    return Y_HOOKS_CONTINUE_RETURN_1;
}

hook OnPlayerDisconnect(playerid, reason)
{
    if(GetPlayerState(playerid) == PLAYER_STATE_PASSENGER || GetPlayerState(playerid) == PLAYER_STATE_DRIVER)
    {
        Vehicle_gsIsOccupied[Player_LastVehicleID[playerid]][GetPlayerVehicleSeat(playerid)] = -1;
        Player_LastVehicleID[playerid] = -1;
    }
    return Y_HOOKS_CONTINUE_RETURN_1;
}

hook OnTrailerUpdate(playerid, vehicleid)
{
    new 
        cabid = GetPlayerVehicleID(playerid);
    if(IsTrailerAttachedToVehicle(cabid) && Vehicle_GetTrailer(cabid) == 0) 
    {
        Vehicle_gsTrailer[cabid][0] = vehicleid;
        Vehicle_gsTrailer[vehicleid][1] = cabid;
        if(Vehicle_IsBlinking(cabid))
        {
            new e_BLINKERS: blinkers = Vehicle_gsBlinkSide[cabid];
            Vehicle_CancelBlinking(cabid);
            Vehicle_SetBlinking(cabid, blinkers);
        }
        CallLocalFunction("OnTrailerHook", "ii", cabid, vehicleid);
    }


    return Y_HOOKS_CONTINUE_RETURN_1;
}

hook OnUnoccupiedVehicleUpd(vehicleid, playerid, passenger_seat, Float:new_x, Float:new_y, Float:new_z, Float:vel_x, Float:vel_y, Float:vel_z)
{

    if(Vehicle_GetCategory(vehicleid) == CATEGORY_TRAILER && Vehicle_GetTrailerCab(vehicleid) != 0)
    {
        new cabid = Vehicle_GetTrailerCab(vehicleid);
        Vehicle_gsTrailer[cabid][0] = 0;
        Vehicle_gsTrailer[vehicleid][1] = 0;
        new e_BLINKERS: blinkers = Vehicle_gsBlinkSide[cabid];
        if(Vehicle_IsBlinking(cabid))
        {
            Vehicle_CancelBlinking(cabid);
            Vehicle_SetBlinking(cabid, blinkers);
        }
        CallLocalFunction("OnTrailerUnhook", "ii", cabid, vehicleid);
    }
    
    return Y_HOOKS_CONTINUE_RETURN_1;
}

// OnVehiclePaintjob gets called when player previews paintjob
// We need a callback to detect if paintjob has changed
// Solution: call OnVehiclePaintjobChange

hook OnVehiclePaintjob(playerid, vehicleid, paintjobid)
{
    if(paintjobid != Vehicle_gsPaintjob[vehicleid]) 
    {
        SetPVarInt(playerid, "FRAMEWORK_PaintJobLevel", 1);
        SetPVarInt(playerid, "FRAMEWORK_PaintJob", paintjobid);
    }
    return Y_HOOKS_CONTINUE_RETURN_1;
}

hook OnPlayerKeyStateChange(playerid, newkeys, oldkeys)
{ // vSync 
    if(RELEASED(KEY_SPRINT) && GetPlayerMoney(playerid) >= 500)
    {
        if(GetPVarInt(playerid, "FRAMEWORK_PaintJobLevel") == 1)
        {
            new vehicle = GetPlayerVehicleID(playerid);
            Vehicle_gsPaintjob[vehicle] = GetPVarInt(playerid, "FRAMEWORK_PaintJob");
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

// --
// Components
// --

enum e_VEHICLE_COMPONENT_SLOT
{
    E_COMPONENT_SPOILER,
    E_COMPONENT_HOOD,
    E_COMPONENT_ROOF,
    E_COMPONENT_SIDESKIRT,
    E_COMPONENT_LAMPS,
    E_COMPONENT_NITRO,
    E_COMPONENT_EXHAUST,
    E_COMPONENT_WHEELS,
    E_COMPONENT_STEREO,
    E_COMPONENT_HYDRAULICS,
    E_COMPONENT_FRONT_BUMPER,
    E_COMPONENT_REAR_BUMPER,
    E_COMPONENT_VENT_RIGHT,
    E_COMPONENT_VENT_LEFT,
    E_COMPONENT_FRONT_BULLBAR,
    E_COMPONENT_REAR_BULLBAR
};

/**
 * <summary> Adds a component to vehicle.</summary>
 * <param name="id"> Vehicle or a group which we set the component to.</param>
 * <param name="componentid"> componentid to set.</param>
 * <returns> 0 if function fails, eg. vehicle is not created or vehicle can't have the component, else 1.</param>
*/

stock Vehicle_AddComponent(VEHICLE_FRAMEWORK_TAGS: id, componentid, tag=tagof(id)) 
{
    if(tag == tagof(_:))
    {
        if(!Vehicle_CanHaveComponent(id, componentid))
        {
            return 0;
        }
        new ret = AddVehicleComponent(id, componentid);
        return ret;
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
    return 1;
}


/**
 * <summary> Removes a component from vehicle.</summary>
 * <param name="vehicleid"> Vehicle we remove component from.</param>
 * <param name="componentid"> Component to remove.</param>
 * <returns> 0 if function fails, eg. vehicle is not created, else 1.</param>
*/

stock Vehicle_RemoveComponent(vehicleid, componentid)
{
    return RemoveVehicleComponent(vehicleid, componentid);
}

/**
 * <summary> Checks whether vehicle supports the component.</summary>
 * <param name="vehicleid"> Vehicle we check.</param>
 * <param name="componentid"> Component to check.</param>
 * <returns> 0 if doesn't support, else 1.</param>
*/

stock Vehicle_CanHaveComponent(vehicleid, componentid)
{
    return VehicleCanHaveComponent(GetVehicleModel(vehicleid), componentid);
}

/**
 * <summary> Checks if vehicle has the component in specified slot.</summary>
 * <param name="vehicleid"> Vehicle we check.</param>
 * <param name="slot"> Slot to check.</param>
 * <returns> ID of the component in slot.</param>
*/

stock Vehicle_GetComponentInSlot(vehicleid, e_VEHICLE_COMPONENT_SLOT: slot) 
{
    return GetVehicleComponentInSlot(vehicleid, _:slot);
}

/**
 * <summary> Gets vehicle component's slot.</summary>
 * <param name="vehicleid"> Vehicle we check.</param>
 * <param name="componentid"> Component to check.</param>
 * <returns> Component slot.</param>
*/

stock e_VEHICLE_COMPONENT_SLOT: Vehicle_GetComponentType(vehicleid, componentid)
{
    return e_VEHICLE_COMPONENT_SLOT: GetVehicleComponentType(componentid);
}
// --
// Health
// --

forward OnVehicleHealthChange(vehicleid, Float:health);

/**
 * <summary> Sets vehicle health.</summary>
 * <param name="vehicleid"> Vehicle we set health to.</param>
 * <param name="health"> Health to set.</param>
 * <returns> 1 on success, 0 otherwise.</param>
*/

stock Vehicle_SetHealth(vehicleid, Float:health)
{
    if(health < 0) 
    {
        return 0;
    }
    new const ret = SetVehicleHealth(vehicleid, health);
    if(ret == 1)
    {    
        Vehicle_gsHealth[vehicleid] = health;
    }
    return ret;
    
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

stock Vehicle_Repair(vehicleid) 
{
    new const ret = RepairVehicle(vehicleid);
    if(ret == 0)
    {
        return 0;
    }
    Vehicle_gsHealth[vehicleid] = 1000;
}

timer Vehicle_HealthTrack[500](vehicleid)
{
    new Float:healthveh;
    GetVehicleHealth(vehicleid, healthveh);
    if(healthveh != Vehicle_gsHealth[vehicleid]) 
    {
        CallLocalFunction("OnVehicleHealthChange", "if", vehicleid, healthveh);
        Vehicle_gsHealth[vehicleid] = healthveh;
    }
}

/*

ptask Vehicle_TrackPayNSpray[700](playerid) 
{
    
    if(GetPlayerState(playerid) == PLAYER_STATE_DRIVER)
    {
        new vehicleid = GetPlayerVehicleID(playerid);
        static bool: inside;
        if(GetPlayerMoney(playerid) >= 100 && Vehicle_Speed(vehicleid) == 0)
        {
            for(new i = sizeof(Vehicle_gscPayNSprayLocations) - 1; i != -1; --i)
            {
                if(IsPlayerInsidePaySpray(playerid) && !inside)
                {
                    Vehicle_SetColor(vehicleid, -1, -1);
                    CallRemoteFunction("OnVehicleRespray", "iiii", playerid, vehicleid, Vehicle_gsColorInfo[vehicleid][0], Vehicle_gsColorInfo[vehicleid][1]);
                    inside = true;
                }
                else if(!IsPlayerInsidePaySpray(playerid) && inside)
                {
                    inside = false;
                }
            }
        }
    }
}

// --
// Developer functions
// --

static stock IsPlayerInsidePaySpray(playerid)
{
	for(new i = 0; i < sizeof(Vehicle_gscPayNSprayLocations); i++)
    {
        if(IsPlayerInRangeOfPoint(playerid, 5.0, Vehicle_gscPayNSprayLocations[i][0], Vehicle_gscPayNSprayLocations[i][1], Vehicle_gscPayNSprayLocations[i][2]))
        {
            return 1;
        }
    }
    return 0;
}*/