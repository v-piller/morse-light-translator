# Morse Code Translator


## Sensor: Light Sensor (able to sense when there is light and when there isn't)
We are not sure what sensor to use as there is 2 different light sensors available, ** SKU: U021 and U136 ** . As, we did not find precise specification
for the first one.  
### Aditional sensor
We saw that a flashlight unit exits ** SKU: U152 ** , it could be an option for emiting the lights. However we do not
know if it will be powerful enought.

## Idea:

Make a server/web page where we can interact with the sensor (send messages that will be translated in morse code and receive messages in morse code that will be translated in a readable message). Two computers can interact with each other (send and receive).

We would use a LED light? to send the morse code from Computer 1 that will be sensed by Computer 2's sensor. The sesnor will have to understand the dot and dashes, and that information will be translated to a readable message and shown in the server/web page. Then, Computer 2 would be able to write a response and send it back to Computer 1, so that it can sens, understand, translate and show. Basically, a comunication system.

What do we do with the microprocessor?
We would use it as the translator.

