# FDRM-kl25z

## Board connectors
![alt text](http://aaronclarke.com/blog.embeddedcoding.com/images/kl25-slide2.jpg)

## RGB LED
PTD1 is also connected to the I/O header on J2 pin 10 (also known as D13)
![alt text](http://visualgdb.com/w/wp-content/uploads/tutorials/arm/kinetis/frdm-kl25z/led-schematics.png)


## Steps for using Segger tool

⋅⋅* Take the eval board which supports OpenSDA and connect it to the PC via USB while keeping the reset button of the board pushed. This will     start the OpenSDA platform in MSD mode.
⋅⋅* The OpenSDA platform will show up as mass storage device (MSD) on the PC just like an USB stick.
⋅⋅* Download and extract the appropriate OpenSDA image
⋅⋅* Load the firmware by simply drag & drop the *.bin file on the device.
⋅⋅* After power-cycling the hardware, the OpenSDA platform will show up as J-Link on the PC.

