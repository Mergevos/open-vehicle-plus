#include <a_samp>
#include <YSI_Data\y_foreach>
#include <YSI_Game\y_vehicledata>

#if defined UGMP_Enable
    #include <ugmp>
#endif

// -- 
// Script-Side
// --


#if !defined MAX_NUMBER_PLATE
    #define MAX_NUMBER_PLATE 32
#endif

static const 
    DIMENSION_INTERIOR = 0,
    DIMENSION_VIRTUAL_WORLD = 1;

static 
    Vehicle_Model[MAX_VEHICLES],
    Vehicle_NumberPlate[MAX_VEHICLES][MAX_NUMBER_PLATE],
    Vehicle_RespawnDelay[MAX_VEHICLES],
    Vehicle_DimensionInfo[MAX_VEHICLES][2],
    Vehicle_ColorInfo[MAX_VEHICLES][2],
    Vehicle_Handler[MAX_VEHICLES],
    
    Iterator: Vehicle_Iter<MAX_VEHICLES>,
    Float: Vehicle_Health[MAX_VEHICLES],
    Float: Vehicle_Position[MAX_VEHICLES][4];
    

// --
// Params
// --

new const
    PARAMS_ENGINE = 0,
    PARAMS_LIGHTS = 1,
    PARAMS_ALARM = 2, 
    PARAMS_DOORS = 3,
    PARAMS_BONNET = 4,
    PARAMS_BOOT = 5,
    PARAMS_OBJECTIVE = 6;

new
    Vehicle_Params[MAX_VEHICLES][7];



/**
  * <param name="modelid">vehicle model (400-611)</param>
  * <param name="Float: x">x vehicle rotation</param>
  * <param name="Float: y">y vehicle rotation</param>
  * <param name="Float: z">z vehicle rotation</param>
  * <param name="Float: rotation">vehicle angle</param>
  * <param name="respawn_delay">The time(ms) when the vehicle will be respawned again.</param>
  * <param name="interior">interior id, if you want to create a vehicle inside the building</param>
*/
stock Vehicle_Create(modelid, Float: x, Float: y, Float: z, Float: rotation, color1, color2, respawn_delay, interior, virtual_world) {
    new idx = Iter_Alloc(Vehicle_Iter);
    
    Vehicle_Model[idx] = modelid;
    Vehicle_Position[idx][0] = x;
    Vehicle_Position[idx][1] = y;
    Vehicle_Position[idx][2] = z;
    Vehicle_Position[idx][3] = rotation;
    Vehicle_ColorInfo[idx][0] = color1;
    Vehicle_ColorInfo[idx][1] = color2;
    Vehicle_RespawnDelay[idx] = respawn_delay;
    Vehicle_DimensionInfo[idx][DIMENSION_INTERIOR] = interior;
    Vehicle_DimensionInfo[idx][DIMENSION_VIRTUAL_WORLD] = virtual_world;
    Vehicle_Handler[idx] = CreateVehicle(modelid, x, y, z, rotation, color1, color2, respawn_delay);

    SetVehicleVirtualWorld(Vehicle_Handler[idx], virtual_world);
    LinkVehicleToInterior(Vehicle_Handler[idx], interior);
}

/**
  * <param name="Float: health">vehicle health (damage)</param>
  * <param name="plate">the text that will appear on the vehicle plates</param>
*/
stock Vehicle_CreateEx(modelid, Float: x, Float: y, Float: z, Float: rotation, color1, color2, respawn_delay, interior, virtual_world, Float: health, const plate[MAX_NUMBER_PLATE]) {
    new idx = Iter_Alloc(Vehicle_Iter);
    
    Vehicle_Model[idx] = modelid;
    Vehicle_Position[idx][0] = x;
    Vehicle_Position[idx][1] = y;
    Vehicle_Position[idx][2] = z;
    Vehicle_Position[idx][3] = rotation;
    Vehicle_ColorInfo[idx][0] = color1;
    Vehicle_ColorInfo[idx][1] = color2;
    Vehicle_RespawnDelay[idx] = respawn_delay;
    Vehicle_DimensionInfo[idx][DIMENSION_INTERIOR] = interior;
    Vehicle_DimensionInfo[idx][DIMENSION_VIRTUAL_WORLD] = virtual_world;
    Vehicle_Health[idx] = health;
    
    strcopy(Vehicle_NumberPlate[idx], plate);
    Vehicle_Handler[idx] = CreateVehicle(modelid, x, y, z, rotation, color1, color2, respawn_delay);
    
    SetVehicleHealth(Vehicle_Handler[idx], health);
    SetVehicleNumberPlate(Vehicle_Handler[idx], plate);
    SetVehicleVirtualWorld(Vehicle_Handler[idx], virtual_world);
    LinkVehicleToInterior(Vehicle_Handler[idx], interior);
}

// -- 
// Engine 
// --

enum e_ENGINE_STATES {
    E_ENGINE_STATE_OFF = 0,
    E_ENGINE_STATE_ON
};

stock e_ENGINE_STATES: Vehicle_GetEngineState(vehicleid) {
    GetVehicleParamsEx(vehicleid, Vehicle_Params[vehicleid][PARAMS_ENGINE], Vehicle_Params[vehicleid][PARAMS_LIGHTS], Vehicle_Params[vehicleid][PARAMS_ALARM], Vehicle_Params[vehicleid][PARAMS_DOORS], Vehicle_Params[vehicleid][PARAMS_BONNET], Vehicle_Params[vehicleid][PARAMS_BOOT], Vehicle_Params[vehicleid][PARAMS_OBJECTIVE]);

    return e_ENGINE_STATES: Vehicle_Params[vehicleid][PARAMS_ENGINE];
}

stock Vehicle_SetEngineState(vehicleid, e_ENGINE_STATES: engine_state) {

    GetVehicleParamsEx(vehicleid, Vehicle_Params[vehicleid][PARAMS_ENGINE], Vehicle_Params[vehicleid][PARAMS_LIGHTS], Vehicle_Params[vehicleid][PARAMS_ALARM], Vehicle_Params[vehicleid][PARAMS_DOORS], Vehicle_Params[vehicleid][PARAMS_BONNET], Vehicle_Params[vehicleid][PARAMS_BOOT], Vehicle_Params[vehicleid][PARAMS_OBJECTIVE]);
    Vehicle_Params[vehicleid][PARAMS_ENGINE] = _:engine_state;
    new const ret = SetVehicleParamsEx(vehicleid, Vehicle_Params[vehicleid][PARAMS_ENGINE], Vehicle_Params[vehicleid][PARAMS_LIGHTS], Vehicle_Params[vehicleid][PARAMS_ALARM], Vehicle_Params[vehicleid][PARAMS_DOORS], Vehicle_Params[vehicleid][PARAMS_BONNET], Vehicle_Params[vehicleid][PARAMS_BOOT], Vehicle_Params[vehicleid][PARAMS_OBJECTIVE]);
    
    return ret;
}

// --
// Lights
// --

enum e_LIGHT_STATES {
    E_LIGHT_ON = 0,
    E_LIGHT_OFF
};

enum e_LIGHT_RUN_STATE {
    E_LIGHTS_OFF,
    E_LIGHTS_ON    
};

// internal

static stock encode_lights(front_left_light, front_right_light, back_lights) {
    return front_left_light | (front_right_light << 2) | (back_lights << 6);
}

static stock decode_lights(lights, &front_left_light, &front_right_light, &back_lights)
{
    front_left_light = lights & 1;
    front_right_light = lights >> 2 & 1;
    back_lights = lights >> 6 & 1;
}

//

stock Vehicle_SetLightsState(vehicleid, e_LIGHT_STATES: left_lights, e_LIGHT_STATES: right_lights, e_LIGHT_STATES: back_lights) {
    new 
        lights, 
        unnecessary_params;

    GetVehicleDamageStatus(vehicleid, unnecessary_params, unnecessary_params, lights, unnecessary_params);
    new const ret = UpdateVehicleDamageStatus(vehicleid, unnecessary_params, unnecessary_params, encode_lights(_:left_lights, _:right_lights, _:back_lights), unnecessary_params);
    
    return ret;
}

stock Vehicle_GetLightsState(vehicleid, &e_LIGHT_STATES: left_lights, &e_LIGHT_STATES: right_lights, &e_LIGHT_STATES: back_lights) {
    new 
        lights, 
        e_LIGHT_STATES: lights_status[3],
        unnecessary_params;

    GetVehicleDamageStatus(vehicleid, unnecessary_params, unnecessary_params, lights, unnecessary_params);
    decode_lights(lights, lights_status[0], lights_status[1], lights_status[2]);
    
    left_lights = lights_status[0];
    right_lights = lights_status[1];
    back_lights = lights_status[2];

}

stock Vehicle_SetLightsRunState(vehicleid, e_LIGHT_RUN_STATE: light_state) {
        
    GetVehicleParamsEx(vehicleid, Vehicle_Params[vehicleid][PARAMS_ENGINE], Vehicle_Params[vehicleid][PARAMS_LIGHTS], Vehicle_Params[vehicleid][PARAMS_ALARM], Vehicle_Params[vehicleid][PARAMS_DOORS], Vehicle_Params[vehicleid][PARAMS_BONNET], Vehicle_Params[vehicleid][PARAMS_BOOT], Vehicle_Params[vehicleid][PARAMS_OBJECTIVE]);
    Vehicle_Params[vehicleid][PARAMS_LIGHTS] = _:light_state;
    new const ret = SetVehicleParamsEx(vehicleid, Vehicle_Params[vehicleid][PARAMS_ENGINE], Vehicle_Params[vehicleid][PARAMS_LIGHTS], Vehicle_Params[vehicleid][PARAMS_ALARM], Vehicle_Params[vehicleid][PARAMS_DOORS], Vehicle_Params[vehicleid][PARAMS_BONNET], Vehicle_Params[vehicleid][PARAMS_BOOT], Vehicle_Params[vehicleid][PARAMS_OBJECTIVE]);
    
    return ret;
}

stock e_LIGHT_RUN_STATE: Vehicle_GetLightsRunState(vehicleid) {
    GetVehicleParamsEx(vehicleid, Vehicle_Params[vehicleid][PARAMS_ENGINE], Vehicle_Params[vehicleid][PARAMS_LIGHTS], Vehicle_Params[vehicleid][PARAMS_ALARM], Vehicle_Params[vehicleid][PARAMS_DOORS], Vehicle_Params[vehicleid][PARAMS_BONNET], Vehicle_Params[vehicleid][PARAMS_BOOT], Vehicle_Params[vehicleid][PARAMS_OBJECTIVE]);
    return e_LIGHT_RUN_STATE: Vehicle_Params[vehicleid][PARAMS_LIGHTS];
}


// --
// Alarms
// --
enum e_ALARMS_STATE {
    E_ALARMS_OFF = 0,
    E_ALARMS_ON
};

stock Vehicle_SetAlarms(vehicleid, e_ALARMS_STATE: alarm_state) {
    GetVehicleParamsEx(vehicleid, Vehicle_Params[vehicleid][PARAMS_ENGINE], Vehicle_Params[vehicleid][PARAMS_LIGHTS], Vehicle_Params[vehicleid][PARAMS_ALARM], Vehicle_Params[vehicleid][PARAMS_DOORS], Vehicle_Params[vehicleid][PARAMS_BONNET], Vehicle_Params[vehicleid][PARAMS_BOOT], Vehicle_Params[vehicleid][PARAMS_OBJECTIVE]);
    Vehicle_Params[vehicleid][PARAMS_ALARM] = _:alarm_state;
   
    new const ret = SetVehicleParamsEx(vehicleid, Vehicle_Params[vehicleid][PARAMS_ENGINE], Vehicle_Params[vehicleid][PARAMS_LIGHTS], Vehicle_Params[vehicleid][PARAMS_ALARM], Vehicle_Params[vehicleid][PARAMS_DOORS], Vehicle_Params[vehicleid][PARAMS_BONNET], Vehicle_Params[vehicleid][PARAMS_BOOT], Vehicle_Params[vehicleid][PARAMS_OBJECTIVE]);
    return ret;
}

stock e_ALARMS_STATE: Vehicle_GetAlarms(vehicleid) {
    GetVehicleParamsEx(vehicleid, Vehicle_Params[vehicleid][PARAMS_ENGINE], Vehicle_Params[vehicleid][PARAMS_LIGHTS], Vehicle_Params[vehicleid][PARAMS_ALARM], Vehicle_Params[vehicleid][PARAMS_DOORS], Vehicle_Params[vehicleid][PARAMS_BONNET], Vehicle_Params[vehicleid][PARAMS_BOOT], Vehicle_Params[vehicleid][PARAMS_OBJECTIVE]);
    return e_ALARMS_STATE: Vehicle_Params[vehicleid][PARAMS_ALARM];
}

// ---
// Doors
// ---
enum e_DOOR_STATES {
    E_DOOR_STATE_OFF = 0,
    E_DOOR_STATE_ON
};

stock e_DOOR_STATES: Vehicle_GetDoorState(vehicleid) {

    GetVehicleParamsEx(vehicleid, Vehicle_Params[vehicleid][PARAMS_ENGINE], Vehicle_Params[vehicleid][PARAMS_LIGHTS], Vehicle_Params[vehicleid][PARAMS_ALARM], Vehicle_Params[vehicleid][PARAMS_DOORS], Vehicle_Params[vehicleid][PARAMS_BONNET], Vehicle_Params[vehicleid][PARAMS_BOOT], Vehicle_Params[vehicleid][PARAMS_OBJECTIVE]);
    
    return e_DOOR_STATES: Vehicle_Params[vehicleid][PARAMS_DOORS];
}

stock Vehicle_SetDoorState(vehicleid, e_DOOR_STATES: door_state) {
    GetVehicleParamsEx(vehicleid, Vehicle_Params[vehicleid][PARAMS_ENGINE], Vehicle_Params[vehicleid][PARAMS_LIGHTS], Vehicle_Params[vehicleid][PARAMS_ALARM], Vehicle_Params[vehicleid][PARAMS_DOORS], Vehicle_Params[vehicleid][PARAMS_BONNET], Vehicle_Params[vehicleid][PARAMS_BOOT], Vehicle_Params[vehicleid][PARAMS_OBJECTIVE]);
    Vehicle_Params[vehicleid][PARAMS_DOORS] = _:door_state;
    new const ret = SetVehicleParamsEx(vehicleid, Vehicle_Params[vehicleid][PARAMS_ENGINE], Vehicle_Params[vehicleid][PARAMS_LIGHTS], Vehicle_Params[vehicleid][PARAMS_ALARM], Vehicle_Params[vehicleid][PARAMS_DOORS], Vehicle_Params[vehicleid][PARAMS_BONNET], Vehicle_Params[vehicleid][PARAMS_BOOT], Vehicle_Params[PARAMS_OBJECTIVE]);
    return ret;
}

// ---
// Tires
// ---

enum e_TIRE_STATUS {
    E_TIRE_INFLATED = 0,
    E_TIRE_POPPED
};

stock Vehicle_SetTireState(vehicleid, e_TIRE_STATUS: rear_right_tire, e_TIRE_STATUS: front_right_tire, e_TIRE_STATUS: rear_left_tire, e_TIRE_STATUS: front_left_tire) {    
    new
        unnecessary_params[4],
        ret;

    GetVehicleDamageStatus(vehicleid, unnecessary_params[0], unnecessary_params[1], unnecessary_params[2], unnecessary_params[3]);
    ret = UpdateVehicleDamageStatus(vehicleid, unnecessary_params[0], unnecessary_params[1], unnecessary_params[2], encode_tires(_:rear_right_tire, _:front_right_tire, _:rear_left_tire, _:front_left_tire));

    return ret;
}

stock Vehicle_GetTireState(vehicleid, &e_TIRE_STATUS: rear_right_tire, &e_TIRE_STATUS: front_right_tire, &e_TIRE_STATUS: rear_left_tire, &e_TIRE_STATUS: front_left_tire) {    
    new 
        tires, 
        e_TIRE_STATUS: tires_status[4],
        unnecessary_params;

    GetVehicleDamageStatus(vehicleid, unnecessary_params, unnecessary_params, unnecessary_params, tires);
    decode_tires(tires, tires_status[0], tires_status[1], tires_status[2], tires_status[3]);
    
    rear_right_tire = tires_status[0];
    front_right_tire = tires_status[1];
    rear_left_tire = tires_status[2];
    front_left_tire = tires_status[3];

}

static stock encode_tires(rear_right_tire, front_right_tire, rear_left_tire, front_left_tire)
{
    return rear_right_tire | (front_right_tire << 1) | (rear_left_tire << 2) | (front_left_tire << 3);
}

static stock decode_tires(tires, &rear_right_tire, &front_right_tire, &rear_left_tire, &front_left_tire)
{
    rear_right_tire = tires & 1;
    front_right_tire = tires >> 1 & 1;
    rear_left_tire = tires >> 2 & 1;
    front_left_tire = tires >> 3 & 1;
}



// --
// Hooks
// --

