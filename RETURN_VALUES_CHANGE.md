## Important change in return values within some functions and planned things to be done for 2.0.0

Guess who's back.
Long time no see with this repository due to my personal obligations.
It's been 3 months since this library got its last update and I decided it's time to refresh it with some new update or couple of them?

### Error handling

As of pre-release 2.0.0-beta.1 library will be adapting [pawn-errors](https://github.com/Southclaws/pawn-errors) as its error handler.
What does this mean? It means it will have aforementioned library included as a dependency. 
Yes, API changes, let's take a look how a function will look.

```pawn
stock Error: Vehicle_SetHealth(vehicleid, Float:health)
{
    if(health < 0)
    {
        return Error(1, "Invalid health amount.");
    }
    else if(!IsValidVehicle(vehicleid))
    {
        return Error(2, "Invalid vehicle.");
    }
    Vehicle_gsHealth[vehicleid] = health;
    SetVehicleHealth(vehicleid, health);
    return Ok();
}
```

Let's break the code above into smaller pieces.
`stock Error: Vehicle_SetHealth(vehicleid, Float:health)` as we can see, function is now tagged with `Error:` tag which indicates this function returns error code.
```c
if(health < 0)
{
    return Error(1, "Invalid health amount.");
}
```
This part checks if health is under 0 and then returns error code 1 with description. In this case its invalid health amoun.
```c
else if(!IsValidVehicle(vehicleid))
{
	return Error(2, "Invalid vehicle.");
}
```
This part pretty much same like the earlier one, but this time returned code is 2 with description of invalid vehicle.
```c
Vehicle_gsHealth[vehicleid] = health;
SetVehicleHealth(vehicleid, health);
return Ok();
```
Nothing interesting except the last line. `Ok()` returns zero value, which indicates there are no errors and function has been executed successfully.
Any value which is non-zero is treated as an error.

In order to handle error we'll call our function and get its return value with a variable.
```c
new Error: ret = Vehicle_SetHealth(INVALID_VEHICLE_ID, 30.0);
if(IsError(ret))
{
	print("There's error");
	Handled();
}
```
This basically checks if there's any error. To check for a specific error we could have done something like
```pawn
if(IsError(ret))
{
	print("There's error");
	if(ret == Error: 1)
	{
		print("error code 1 is invalid vehicleid");
		// do action you want.
		Handled();
	}
}
```
`Handled()` will erase stack of errors and indicates that script has returned to safe state.

In my personal opinion, this is great way of handling errors and much neater.
Look at [pawn-errors](https://github.com/Southclaws/pawn-errors) for more info.

### ENGINE_INVALID and DRIVE_MANUAL

Now `Vehicle_GetEngineType` won't return `ENGINE_INVALID` for invalid vehicles only but for bikes too. Bikes are as invalid vehicles, with no engine. 
Addition to `Vehicle_GetEngineType`, `Vehicle_GetDriveType` will return `DRIVE_MANUAL` for vehicles (bikes) human-powered.

### Planned API and features

Until main 2.0.0 release I will be releasing beta pre-releases.

##### Electric!

For sure one of the titles will be this. Electric vehicles will be implemented. You won't be able to use fuel functions for electric vehicles anymore but something like
`Vehicle_ChargeBattery` and the unit of measurement will be kWh. Using fuel functions on electric vehicles will result in an error. Electric vehicles will be predefined.

##### Ex functions for vehicle parts.

Some functions has its extended brothers, like `Vehicle_GetTyreCondition` has `Vehicle_GetTyreConditionEx`. Same but the latter one gets all tyres' status passed by reference.
Until main release, I hope I will have extended all functions.

##### Errors

As mentioned above, main subject of this topic has been return values. Yes, I hope all functions which need to be, will be ported to this neat error handling.

##### (What about tagged functions?)

Well, we will either be passing them by reference or we'll find other solution.
