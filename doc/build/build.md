# Build Info


## General MaraX Infos
### Overview of  the OPV area


![OPVarea](/assets/info_opv_area.jpg)


### Marax GiCard MCU

![MCU](/assets/mcu_marax.jpg)

Pinout of the MCU of the MaraX(GiCar) from center to edge
| Vcc | Gnd | RX | TX | RTS | CT |


## Build Information
```
Do not take this as a build guide, just a documentation of this mod!
```

This is the overall circuit of the mod. The Nano was soldered to PCB with some plugs for each components. 
![TWiring](/assets/circuit.svg)

## Pressure Sensor

![TFitting](/assets/t_fitting_mount.jpg)
The T fitting is mounted between the pump and the pressure gauge T fitting.
On the other side of the T Fitting the pressure sensor is mounted. 

> Note: Lelit does use 5mm fittings these are found on high pressure systems and can to ~18bars
> They can be hard to find but Festo also does use 5mm hosing.
> 
> NOT 4mm NOT 6mm but 5mm! I ordered a lot of wrong parts because of this.

I have also added some extra foam for the pressure sensor to keep it from making noise.

The pressure senor works with max 4,5 Volts for 12 Bar. This is to much for the Nano which works with 3,3Volts for this I have added a voltage divider with two resistors.
>0.3V for 0 Bars
>3V for 12 Bars


![Pumpwires](/assets/pump_wires.jpg)

The AC dimmer will be connected between the pump and the cables from the MCU of the MaraX.
>Remember the pump runs on mains 230V power!!
The AC dimmer will always allow 100% of the power to the pump if no brew is active, this way the MCU of the Marax can still control the pump to fill the boiler etc.

![Brewswitch](/assets/brew_switch_wires.jpg)
I have connected the Nano to the original brew switch, this way the Nano can detect if a brew is active. I have used a relay to still "switch" the brew switch for the MCU of the Marax. This way the MCU of the Marax still thinks is stock and will also know if a brew is active and heat the boiler accordingly 
> Note, the MCU of the Marax will switch ground over this switch!


# The good the bad and the ugly

### good 
 - fun to play around with 
 - really good espresso 
 - learned a lot about the machine 
 - learned a lot about embedded systems and electrics
 - The touch screen makes the machine look way more modern
 - the readout of the temps over mqtt is really nice
  
### the bad
- As I am not an really no expert when it comes to embedded systems I choose to go with an arduino step to quickly prototype if this project would work. However there are definitely some drawbacks. I would now choose a STM controller and a proper development environment, with a CI, package management etc.. and maybe even develop my own pcb with all the components needed. But this would have taken me even more time to finish this project and I don't have a backup espresso machine.. 
- The Code of the mod is ... lets just say it involved..  
- As you can see in the diagram the 5v rail of the Nano is used for the power of the lcd, this is **not** best practice and will reduce the live time of the Nano. However it has been working great so far.  

### the ugly
- I have mounted most of the components at the back of the machine inside small electrical boxes, even if you cant see them I really think this can be improved in the feature. Lelit has build a compact machine so I think a custom pcb is needed to keep everything inside. 


## Modding other Machines
I have revived some messages asking about this mod for other machines. Technically this is possible with every machine that has a vibration pump.
 However:
- the brew switch will be different
- the hosing used after the pump will be different
- you will not be able to use the temperature read outs.
  


