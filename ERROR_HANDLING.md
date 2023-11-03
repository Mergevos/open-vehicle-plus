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