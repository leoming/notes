make kernel support LKM
--- Enable loadable module support ---                                                                   
[*]   Forced module loading                                                                          
[*]   Module unloading                                                                               
[*]     Forced module unloading

reflash boot partion

emulator driver for keyboard and touch screen, weidognshan's android video tutorial.  
  
compile:  
make   
  
use:  
  
insmod InputEmulator-yf.ko   
  
and then use sendevent to write the /dev/input/eventX  
sendevent /dev/input/event6 1 2 1 
sendevent /dev/input/event6 1 2 0 
sendevent /dev/input/event6 0 0 0 


