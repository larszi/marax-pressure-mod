
# A overview of the different UI pages

## Home page

![HomePage](/assets/home_page_with_details.jpg)


## Brew graph page

![BrewGraph](/assets/brew_graph_with_details.jpg)

The brew graph page will automatically be shown if the brew leaver it switched to the "on/top" postion

## Cleaning Page

![CleaningPage](/assets/cleaning_page_with_details.jpg)

If the cleaning page is active, it will count the shot time of the cleaning flush as well as the current amount of flushes that have been done.

## Settings Page

![Settings](/assets/settings_page_with_details.jpg)

\* Read `Further ideas`, (This is implanted in the current version and will taken 50 points in `;` separated values.)
When Pressure Profiling is disabled the machine will do a normal shot, but will show a graph and infos about the brew.

## Pressure Profile Page

![PP_Page](/assets/pp_page_with_details.jpg)

The Pressure Profile that can be set. There a four time slots which can each have different pressures set. 
Example Shot -> 5 Bar for 5 Sec. then 6 Bar for 6 Sec. then 7 Bar for 5 Sec. then 8 Bar for 3 Sec.

## Splash Screen/Page

![Boot_Page](/assets/boot_screen.jpg)
LELIT with added <3

## Temperature imported in Home Assistant

![HA](/assets/ha_temps.png)

# Building the UI 

- The UI design was build using Figma and by using [feathericons](https://github.com/feathericons/feather)
- The Screen then was then "programmed" with the Nexion editor.
- The Screen does have its known controller and communicates with the Nano over a serial connection 
- The Nexion Display file can be found under `/display`