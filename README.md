# LELIT MaraX Pressure Mod

This mod can create pressure profiling by reading the current pressure and adjusting the pump over PFC accordingly.

![Thumbnail](./assets/thumbnail.jpg)

## Demo Videos

- [😁 Meme Version](https://www.youtube.com/watch?v=6iu-tRzNN24) <- Recommended
- [🤏 Shot Version](https://www.youtube.com/watch?v=Ix4moGTFoKE)

## Features

- 📊 Pressure Profiling
- 📝 Touchscreen and visualization
- 📲 Publish Temperatures over MQTT
- 📋 Pressure Readout
- 🛁 Cleaning Mode

![BrewGraph](./assets/brew_graph.png)

## Overview


### Diagramm

![Wiring Overview](./assets/circuit.svg)

# 🧑🏼‍🏭👩🏼‍💻🧑🏼‍🔧 [More build informations](./doc/README.md) 👩🏼‍🔬👩🏼‍🔧👨🏼‍🏭

### planned todos

| todo                                 | status  | description                                                                                                                                                    |
| ------------------------------------ | ------- | -------------------------------------------------------------------------------------------------------------------------------------------------------------- |
| document more about the mod          | 80%     | -                                                                                                                                                              |
| add a prober wiring diagram          | started | replace the current diagram with a proper one using a suitable program                                                                                     |
| better way of the hardware packaging | planned | this mod was started as a PoC and therefor has a lot of room for improvements regarding resulting hardware packaging / smaller components or even a custom PCB |

## Important notes

- This Project is in no way or form related to LELIT! All trademarks and logos used in this project are for illustrative purposes only and are not intended to indicate any affiliation with any organization or company.
- This Project is for educational purposes only!
- This Mod is not recommended, and this repository is for documentary purpose only! Do not try this mod on your machine

## The goal

My goal with this mod was to try and explore the capability's of pressure profiling espresso and I can for sure say that the end result paid off. Even if the development of this mod was quite challenging, I really improved my knowledge about embedded systems and electrics in geneal. 

The mod is now in place for more then 3 Months(11.03.23) don't doing good in the "long" run and I certainly had my fun with some espresso recipes.

## Further ideas
I played a bit with creating the pressure graph in react and then sending the graph points over mqtt to the controller. This step up is definitely interesting to look into a bit more.

If some day, I magically have another machine as "backup" and a lot of free time. I might build all electronics inside the machine from the ground up my self with a proper PID and ...

### Some more details 
 - the control of the pressure is only possible due to the vibration pump which can be controlled by an AC dimmer (Imported if you want to mod other machines)
 - all informations are transmitted via MQTT and can then be integrated in for example HomeAssistant
 - 

### Similar projects 
- [Gaggiuino](https://github.com/Zer0-bit/gaggiuino)
- [MaraX Shot Timer](https://github.com/alexrus/marax_timer)

## License
Apache License Version 2.0