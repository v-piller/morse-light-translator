# Morse Code Translator


## Sensor: Light Sensor (able to sense when there is light and when there isn't)

The sensor we were thinking on using is: https://shop.m5stack.com/products/light-sensor-unit?srsltid=AfmBOorAT_KwV2GzEu7Np9zr26Jv7H7u_yf2j1E82jheaLuKGSv_uy9e
We would also need LED light and we think a good option is this one: https://www.led1.de/shop/lng/en/standard-leds/led-5mm-ultra-bright/led-5mm-ultra-bright-white/nichia-led-5mm-white-44000mcd-nspw500gs-k1.html

Additionally, we need resistors and maybe a breadboard, it could be useful. If we are not mistaken, we would also need wires.


### Aditional sensor
We saw that a flashlight unit:  https://docs.m5stack.com/en/unit/FlashLight  /  https://www.bastelgarage.ch/m5stack-led-flashlight-unit-1-2643 , it could be an option for emiting the lights. However we do not
know if it will be powerful enough. It doesn't say the luminous instensity in mcd of the light, but it seems like it could work, we did our research and it seems like it has a luminous flux range of 100 to 150lm.

According to our calculations, the luminous intesity needed for the light sensor to sens the light would be more than 20'000mcd or around 100lm of luminous flux (may be wrong).

## Idea:

Make a server/web page where we can interact with the sensor (send messages that will be translated in morse code and receive messages in morse code that will be translated in a readable message). Two computers can interact with each other (send and receive).

We would use a LED light? to send the morse code from Computer 1 that will be sensed by Computer 2's sensor. The sesnor will have to understand the dot and dashes, and that information will be translated to a readable message and shown in the server/web page. Then, Computer 2 would be able to write a response and send it back to Computer 1, so that it can sens, understand, translate and show. Basically, a comunication system.

What do we do with the microprocessor?
We would use it as the translator.

