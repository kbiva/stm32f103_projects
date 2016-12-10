# README #

* Nokia_6020_framebuffer

Various methods to access Nokia 6020 LCD.

LCD from Nokia 6020 mobile phone connected to HY-STM32 board.
LCD size is 132x132 pixels (visible resolution is 130x130) and the controller is PCF8833.
I am using 8bit color.

Nokia LCD uses 9-bit SPI mode, and STM32F103 has only 8 or 16 bit SPI.
To overcome this incompatibility, I used the idea of repacking nine "STM32 8bit SPI bytes"
to eigth "Nokia 9bit SPI bytes". 
I got this from http://we.easyelectronics.ru/lcd_gfx/stm32-monohromnyy-lcd-hozyayke-na-zametku.html

I overclocked STM32F103VET6 from 72Mhz to 128Mhz to speed-up things.
The SPI bus runs at 64Mhz, and the Nokia LCD handles it without a problem.
When MCU runs at default speed of 72Mhz, the results are slower by a factor ~1.8 (128/72=1.8).

I test different methods to output rotating 3D cube with checkerboard background 
to the LCD. Each time exact the same 200 frames are calculated and displayed 
using 6 diffent methods to access LCD:

1. Pixel,Bit Banging

  Calculated frame is drawn to LCD, pixel after pixel.
  Draw a checkerboard, and on top draw 3D cube, then repeat the process.
  This method is the slowest and 3D cube flickers a lot.
  When outputing a pixel, the bounds are set and then the color.
  This means 72 bits are clocked to the LCD for every pixel.

2. Pixel,SPI

  This is the same as the first method, but uses hardware SPI. It is a bit faster,
  but flickering is still visible. 

3. Framebuffer,Bit Banging

  Draw a checkerboard in the memory buffer, and on top draw 3D cube in the same memory buffer
  then output that buffer to the LCD using bit banging.
  The bounds are set at the begining, and then only color information is written to the LCD.
  There is no flickering using this method.
  This method is a lot of faster than the previous two, because only 
  9 bits are clocked to the LCD for every pixel.

4. Framebuffer,SPI

   This method is simmilar to the third method, but uses hardware SPI. It is a bit faster though.
 
5. Framebuffer,SPI,DMA

   This method is simmilar to the fourth method, but uses DMA controller to do hardware SPI.
   DMA is also used to clear memory buffer.
   
6. Double framebuffer,SPI,DMA

   This method is simmilar to the fifth method, but two buffers. When one buffer is sent to the LCD,
   calculation is started immediately in the second buffer, and then buffers are switched.
   This is the fastest method.

Summary of results:

| Method | FPS | Advantages | Disadvantages |
|--------|-----|-------------|---------------|
| 1 | 5.7 | yes | uses no  additional memory | slow, flickers |
| 2 | 11.0 | yes | uses no  additional memory | slow, flickers |
| 3 | 41.8 | no | fast, no flicker | uses ~20K memory for framebuffer |
| 4 | 71.1 | no | fast, no flicker | uses ~20K memory for framebuffer |
| 5 | 73.9 | no | fast, no flicker | uses ~20K memory for framebuffer |
| 6 | 89.2 | no | fastest, no flicker | uses ~40K memory for two framebuffers |

* More info at:

http://wp.me/p31f2I-1O

* Youtube video:

https://youtu.be/718ShQZI2rg
