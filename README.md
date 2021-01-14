# samp-vehicle-framework

[![Mergevos](https://img.shields.io/badge/Mergevos-samp--vehicle--framework-2f2f2f.svg?style=for-the-badge)](https://github.com/Mergevos/samp-vehicle-framework)

## Installation

Simply install to your project:

```bash
sampctl package install Mergevos/samp-vehicle-framework
```

Include in your code and begin using the library:

```pawn
#include <vehicle-framework>
```

## Usage

The library is pretty currently, pretty well, undocumented. It tries to offers as much possibilities it could. The point is to simplify usage of vehicles' functions and try to give additional features as getting vehicle informations like respawn delay, enabled or disabled vehicle's parts, etc...
Feel free in any way to contribute.

# Examples
## Create vehicle
```c
stock Vehicle_Create(modelid, Float: x, Float: y, Float: z, Float: rotation, color1, color2, respawn_delay, interior, virtual_world)
stock Vehicle_CreateEx(modelid, Float: x, Float: y, Float: z, Float: rotation, color1, color2, respawn_delay, interior, virtual_world, Float: health, const plate[MAX_NUMBER_PLATE])

CMD:veh(playerid, const params[])
{
    extract params -> new vehID, color1, color2; else {
        return SendClientMessage(
            playerid, -1, "USAGE: /veh [model] [color1] [color2]");
        )
    }
    
    static
        x,
        y,
        z,
        rotation;
        
    GetPlayerPos(playerid, Float: x, Float: y, Float: z);
    GetPlayerFacingAngle(playerid, Float: rotation);
    
    Vehicle_Create(vehID, Float: x, Float: y, Float: z, Float: rotation, color1, color2, 0, 0, 0);
    // Or
    Vehicle_CreateEx(vehID, Float: x, Float: y, Float: z, Float: rotation, color1, color2, 0, 0, 0, Float: 999.0, "My Vehicle");

    return 1;
}
```
## Engine
```c
stock e_ENGINE_STATES: Vehicle_GetEngineState(vehicleid)
stock Vehicle_SetEngineState(vehicleid, E_ENGINE_STATES: engine_state)

CMD:engine(playerid, const params[])
{
    Vehicle_SetEngineState(GetPlayerVehicleID(playerid),
        Vehicle_GetEngineState(GetPlayerVehicleID(playerid)) ? (E_ENGINE_STATE_OFF) : (E_ENGINE_STATE_ON)
    );
    
    // output > 0 or 1
    va_SendClientMessage(playerid, -1, "Engine status: %d", Vehicle_GetEngineState(GetPlayerVehicleID(playerid)));

    return 1;
}
```
## Lights
```c
stock Vehicle_SetLightsState(vehicleid, E_LIGHT_STATES: left_lights, E_LIGHT_STATES: right_lights, E_LIGHT_STATES: back_lights)
stock Vehicle_SetLightsRunState(vehicleid, E_LIGHT_RUN_STATE: light_state)
stock e_LIGHT_RUN_STATE: Vehicle_GetLightsRunState(vehicleid)

CMD:lights(playerid, const params[])
{
    Vehicle_SetLightsRunState(GetPlayerVehicleID(playerid),
        Vehicle_GetLightsRunState(GetPlayerVehicleID(playerid)) ? (E_LIGHTS_OFF) : (E_LIGHTS_ON)
    );
  
    // Output > 0 or 1
    va_SendClientMessage(playerid, -1, "Lights status: %d", Vehicle_GetLightsRunState(GetPlayerVehicleID(playerid)));

    return 1;
}
```

## Testing

To test, simply run the package:

```bash
sampctl package run
```
