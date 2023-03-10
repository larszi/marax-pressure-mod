# Build Info


Overview of  the OPV area

![OPVarea](/assets/info_opv_area.jpg)

![TFitting](/assets/t_fitting_mount.jpg)
The T fitting is mounted between the pump and the pressure gauge T fitting

> Note: Lelit does use 5mm fittings these are found on high pressure systems and can to ~18bars
> They can be hard to find but Festo also does use 5mm hosing.
> 
> NOT 4mm NOT 6mm but 5mm! I ordered a lot of wrong parts because of this.

I have also added some extra foam for the pressure sensor to keep it from making noise.

![Pumpwires](/assets/pump_wires.jpg)

The AC dimmer will be connected between the pump and the cables from the MCU of the MaraX.
>Remember the pump runs on mains 230V power!!
The AC dimmer will always allow 100% off the power to the pump if now brew is active, this way the MCU of the Marax can still control the pump to fill the boiler etc.

![Brewswitch](/assets/brew_switch_wires.jpg)
I have connected the Nano to the original brew switch, this way the Nano can detect if a brew is active. I have used a relay to still "switch" the brew switch for the MCU of the Marax. This way the MCU of the Marax still thinks is stock and will also know if a brew is active and heat the boiler accordingly 
> Note, the MCU of the Marax will switch ground over this switch!