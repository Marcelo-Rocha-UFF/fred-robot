# FRED - Friendly Robot for EDucation and Healthcare
## FRED's Arduino Control Software

A arquitetura de software do robô FRED pode ser dividida em duas camadas, de alto nível e de baixo nível. A camada de baixo nível é implementada nas placas Arduino UNO e ESP8266 (NodeMCU) que ficam dentro do robô. O software de controle dessa camada é dividido nessas duas placas. O software que roda na placa Arduino é o responsável pelo controle e pela comunicação com os atuadores do robô, como: as três matrizes de leds, que representam a expressão facial do robô, a fita de leds RGB no tórax do robô e os quatro servomotores que compõem as pernas e os pés do FRED.

</br>
<p align="center">
<img src="img-fred-arch-layers.jpg" width="45%"> </br></br>
<strong>Figure 1.</strong> FRED Robot - Software Archtecture (LAYERS).
</br></br></br>

The FRED firmware for the Arduino board depends on two libraries, **VarSpeedServo (4)**, **LedControl (3)** and **Adafruit_NeoPixel (2)**. They are, respectively, the libraries that control the servomotors and the LED ring on the robot's chest. These libraries must be inside the **libraries folder (1)** of the Arduino IDE. These libraries are available in this directory in a compressed form. You must download the three compressed files and unzip them in the IDE's **libraries folder (1)**, as you can see in Figure 2.

</br>
<p align="center">
<img src="img-libraries-folder.png" width="80%"> </br></br>
<strong>Figure 2.</strong> Copying and uncompressing the libraries do the Arduino libraries folder.

</br></br>
After that, the **FredControl.ino** file can be loaded into the Arduino IDE, compiled and uploaded to the Arduino UNO board.

As you can see in Figure 3, the necessary libraries are included right at the beginning of the code (1). In (2) there is the inclusion of the files **FredSymbols.h**, which contain the **array of bits** that define the design of FRED's gaze, and the files **FredControlBoy.h** and **FredControlGirl.h**, which contain some adjustments to specific parameters of the FRED and Frida robots.

</br>
<p align="center">
<img src="img-arduino-include.png" width="75%"> </br></br>
<strong>Figure 3.</strong> Files used in Arduino's #include section.
