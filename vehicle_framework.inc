#include <a_samp>
#include <YSI_Data\y_foreach>

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
    Vehicle_Handler[MAX_VEHICLES],
    Vehicle_NumberPlate[MAX_VEHICLES][MAX_NUMBER_PLATE],
    Vehicle_RespawnDelay[MAX_VEHICLES],
    Vehicle_DimensionInfo[MAX_VEHICLES][2],
    Vehicle_ColorInfo[MAX_VEHICLES][2],
    Vehicle_Handler[MAX_VEHICLES],
    Vehicle_Iter<MAX_VEHICLES>,
    
    Float: Vehicle_Health[MAX_VEHICLES],
    Float: Vehicle_Position[4];
    
/*
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
    LinkVehicleToInterior(Vehicle_Handler[idx], interiorid);
}

/*
  * <param name="Float: health">vehicle health (damage)</param>
  * <param name="plate">the text that will appear on the vehicle plates</param>
*/
stock Vehicle_CreateEx(modelid, Float:x, Float:y, Float:z, Float:rotation, color1, color2, respawn_delay, interior, virtual_world, Float: health, const plate[MAX_NUMBER_PLATE]) {
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
    
    SetVehicleHealth(Vehicle_Handler[idx], health)
    SetVehicleNumberPlate(Vehicle_Handler[idx], plate);
    SetVehicleVirtualWorld(Vehicle_Handler[idx], virtual_world);
    LinkVehicleToInterior(Vehicle_Handler[idx], interiorid);
}

// -- 
// Engine 
// --

enum E_ENGINE_STATES {
    E_ENGINE_STATE_OFF = 0,
    E_ENGINE_STATE_ON
};

stock E_LIGHT_RUN_STATE: Vehicle_GetEngineState(vehicleid) {
    new 
        engine_state,
        unnecessary;
    
    GetVehicleParamsEx(vehicleid, engine_state, unnecessary, unnecessary, unnecessary, unnecessary, unnecessary, unnecessary);
    
    return E_ENGINE_STATES: engine_state;
}

stock Vehicle_SetEngineState(vehicleid, E_ENGINE_STATES: engine_state) {
    new
        unnecessary_params[7];
        
    GetVehicleParamsEx(vehicleid, unnecessary_params[0], unnecessary_params[1], unnecessary_params[2], unnecessary_params[3], unnecessary_params[4], unnecessary_params[5], unnecessary_params[6]);
    new const ret = SetVehicleParamsEx(vehicleid, _:engine_state, unnecessary_params[1], unnecessary_params[2], unnecessary_params[3], unnecessary_params[4], unnecessary_params[5], unnecessary_params[6]);
    
    return ret;
}

// --
// Lights
// --

enum E_LIGHT_STATES {
    E_LIGHT_ON = 0,
    E_LIGHT_OFF
};

enum E_LIGHT_RUN_STATE {
    E_LIGHTS_OFF,
    E_LIGHTS_ON    
};

stock encode_lights(front_left_light, front_right_light, back_lights) {
    return front_left_light | (front_right_light << 2) | (back_lights << 6);
}

stock Vehicle_SetLightsState(vehicleid, E_LIGHT_STATES: left_lights, E_LIGHT_STATES: right_lights, E_LIGHT_STATES: back_lights) {
    new 
        lights, 
        unnecessary_params;
        
    GetVehicleDamageStatus(vehicleid, unnecessary_params, unnecessary_params, lights, unnecessary_params);
    new const ret = UpdateVehicleDamageStatus(vehicleid, unnecessary_params, unnecessary_params, encode_lights(_:left_lights, _:right_lights, _:back_lights), unnecessary_params);
    
    return ret;
}

stock Vehicle_SetLightsRunState(vehicleid, E_LIGHT_RUN_STATE: light_state) {
    new
        unnecessary_params[7];
        
    GetVehicleParamsEx(vehicleid, unnecessary_params[0], unnecessary_params[1], unnecessary_params[2], unnecessary_params[3], unnecessary_params[4], unnecessary_params[5], unnecessary_params[6]);
    new const ret = SetVehicleParamsEx(vehicleid, unnecessary_params[0], _:light_state, unnecessary_params[2], unnecessary_params[3], unnecessary_params[4], unnecessary_params[5], unnecessary_params[6]);
    
    return ret;
}

stock E_LIGHT_RUN_STATE: Vehicle_GetLightsRunState(vehicleid) {
    new
        light_state,
        unnecessary;
        
    GetVehicleParamsEx(vehicleid, unnecessary, light_state, unnecessary, unnecessary, unnecessary, unnecessary, unnecessary);
    
    return E_LIGHT_RUN_STATE: light_state;
}

//--
// Alarms
// --
