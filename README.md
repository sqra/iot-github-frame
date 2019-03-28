# IoT GitHub Frame

![Front view](/docs/photos/ready_5.jpg)

IoT GitHub Frame is a photo frame with built-in device based on the ESP32 module. This project will help you track the popularity of selected GitHub repository. You will be able to follow live the number of stargazers, watchers and forks. The frame can also display the number of daily stargazers. Simple configuration, automatic updates via HTTPS OTA, stability of operation. These are just some of the advantages of this project. The frame will look great on the wall in your home or office. 

![Cutting holes](/docs/photos/photo_7.jpg)

## BUILD GUIDE

### What you will need

![Necessary parts](/docs/photos/photo_1.jpg)

1. ESP-32 WROOM DevKit 1.0 – 1 pc.
2. 4-digit display TM1637 0.56" - white color– 1 pc.
3. 4-digit display TM1637 0.36" – 1 pc.
4. 4 Pin F/F  200mm Cable – 4 pcs.
5. JST 1.25 2-Pin Male - Female Connector – 2 pcs.
6. Micro JST 1.25 2-Pin Male - Female Connector – 2 pcs.
7. Universal PCB stripboard 50x70mm PI-01 – 1 pc.
8. Carbon Film Resistor – 220 ohm – 5 pcs.
9. Carbon Film Resistor – 2,2k ohm – 3 pcs.
10. Breadboard Jumpers - 1 set
11. Pin Header Right Angle Male – 1 pcs.
12. LED Diode 5mm RGB with common anode – 1 pc.
13. LED Diode 3mm white – 3 pcs.
14. Round Push Button - momentary – 10mm – 2 pcs.
15. ON-OFF switch – 1 pc.
16. Cable USB A – USB micro – 1 pc.
17. Phone charger with usb output 5V  – 1 pc.
18. Heat Shrink Tubing - 1 pc.
19. IKEA RIBBA photo frame – 21x30cm ( it must be thick  ) – 1 pc.
20. Self-adhesive paper for the printer – 	1 pc.

### Tools
1. Soldering iron
2. Solder
3. Drill or screwdriver
4. Drills: 7, 10, 13
5. Hot glue gun
6. Knife
7. Printer

## STEP 1
Solder electronic components to the PCB as in the picture below. Soldering start from the lowest elements. For connecting displays and LEDs use angled connectors.

![Stripboard prototype](/docs/IoTGitHubFrame_stripboard.png)

After finishing soldering, your board should look similar to this one

![Soldered stripboard](/docs/photos/photo_2.jpg)

## STEP 2

In the [printouts folder](https://github.com/sqra/iot-github-frame/tree/master/printouts) you will find print templates that will help you drill holes:
- [back template](https://github.com/sqra/iot-github-frame/blob/master/printouts/drilling-template-back.pdf)
- [side template](https://github.com/sqra/iot-github-frame/blob/master/printouts/drilling-template-side-buttons.pdf)

Print them and then tape back template to the back of the frame and side template on the right edge of the frame. Use the appropriate drills to drill the holes. Cut the holes for the displays and the button with a knife.

![Cutting holes](/docs/photos/photo_5.jpg)

Insert the displays and diodes in the holes using hot glue.

![Assembly of parts](/docs/photos/photo_3.jpg)

That's how it should look from the front

![Front view](/docs/photos/photo_4.jpg)

## STEP 3

In this step you need to adapt the graphics to your repository. To do this, edit the [main PSD file](https://github.com/sqra/iot-github-frame/blob/master/printouts/main-graphics-template.psd) and then print it on a regular A4 sheet. You can use the free online editor [Photopea](https://www.photopea.com/). If you want to, you can also print a [label](https://github.com/sqra/iot-github-frame/blob/master/printouts/side-buttons-labels.psd) with the description of the side buttons. The self-adhesive paper works best for this.

## STEP 4

Now you have to program ESP32 module. Download the latest [Arduino IDE](https://www.arduino.cc/en/main/software) and the necessary libraries. Clone repository. Connect the ESP32 via the USB cable to the computer and upload the code.

## STEP 5

After upload firmware, unplug and plug again USB cable to your device. Status led should glow in red. After few seconds should change color to blue. It means that device is in AP mode. Using a computer or phone scan nearby networks and find **GITHUB-FRAME** ssid. 
Once you connect to the selected access point you will be automatically redirected to the configuration page. Here you enter the name and password for the network with which your frame will connect. 
Save credentials and reset ESP.

If everything has been done correctly, the status LED should be green and the display should show ```SEt```  ```rEPO```  ```IP```  ```192.168.xxx.xxx```. This means that we must now select the repository. Go to computer or phone and connect to the same WiFi network as ESP. Then in browser url placeholder type ip address of your ESP. You will be redirected to the page with the configuration of the repository. In example: 
For repository url: ```https://github.com/DivanteLtd/vue-storefront```

![Front view](/docs/photos/photo_6.jpg)

Fill form fields and click save configuration. 



That’s all :)

Now you can follow the popularity of the selected repository. 
Have a nice building !

| ![Front view](/docs/photos/ready_1.jpg) | ![Front view](/docs/photos/ready_2.jpg) |
|--|--|
| ![Front view](/docs/photos/ready_3.jpg) | ![Front view](/docs/photos/ready_4.jpg) |


[YT - Watch "IoT GitHub Frame" in action](https://youtu.be/0hm3B-3gIzI)

## FAQ

#### Led status is glow in red or blue and display zero.
- Check if your router is connected to the internet. If there is a temporary network loss, the device will automatically connect to the network after a while. You can check also your WiFi credentials and repository configuration.

#### How to set automatic updates ?
- Edit the update.h file
Edit the url under which the latest version of the .bin file will be available:
`const char* versionFileUrlBase`
`const char* binFileUrlStart`
`const char* binFileUrlEnd`
and change version by increasing number of 
`FW_VERSION variable`. 
Then export from Arduino IDE [binary](https://github.com/sqra/iot-github-frame/blob/master/OTA/IoTGithubFrame.ino.esp32.bin) file and place on your server. The last thing you have to do is update version number in [IoTGithubFrame.version](https://github.com/sqra/iot-github-frame/blob/master/OTA/IoTGithubFrame.version) file to be the same as `FW_VERSION`. By default, the update check is set to 00:00 and each time the frame is started.

#### What the colors of the status LED mean ?
- Red - no connection with network
Green - everything works well
Blue - AP mode - ssid: **GITHUB-FRAME**

#### How to check the ip address of my frame ?
- Just press and hold the **SHOW IP** button until the address appears on the display

#### We have hackaton day. I'd like to know how many new stargazers will get my repository today
- Just press and hold the **RESET TODAY'S STARS** button until the zero digit appears on the smaller display. From now until midnight you will receive the number of new stargazers.

#### My device is connected to the network but when I send the request and got a 400 error
- You have to check certificate of the Root CA. It is very possible that it has already expired. Go to [this](https://techtutorialsx.com/2017/11/18/esp32-arduino-https-get-request/) page and update key in your [cacert.h](https://github.com/sqra/iot-github-frame/blob/master/cacert.h) file.
**Important!** If you have an anti-virus installed, check in its settings whether the SSL protocol [filtering mode is disabled](https://support.eset.com/kb3126/?locale=en_US&viewlocale=en_US). If it is enabled, you can disable it temporarily and save the key correctly. Then restore the settings of the antivirus. 


### Useful links

- [User guide for TM1637 4 digits display](https://halckemy.s3.amazonaws.com/uploads/attachments/257650/user_guide_for_tm1637_4_digits_display_Yr8LQNg7dV.pdf)
- [First Use of ESP32 Devkit Board](https://startingelectronics.org/articles/ESP32-WROOM-testing/)





