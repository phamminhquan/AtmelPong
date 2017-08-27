# AtmelPong
2 player, 1 dimensional, with LED strips and 2 tactile switch

|||
|-|-|
| Processor | ATXMEGA 128A1u |
| Microcontroller | uPAD 1.4 |
| Accessories | LED & Switches Backpack |
| Peripherals | Timer/Counter, External Interrupts |

Setup:
PORTC[7:0] = LEDs (low-true)
PORTF[2:3] = Tactile Switches (low-true)
  
Instruction:
Upon reset, LEDs sequentially light up from Pin 0 to Pin 7. 
While LED on Pin 7 is lighting up, Player 2 must hit tactile switch connected to Pin 3.
If Player 2 fail to hit the switch during that duration, all 7 LED will light up signaling that the game is over.
If Player 2 successfully hit the switch durring that duration, LEDs will sequentially light up in reverse order.
While LED on Pin 0 is lighting up, Player 1 must hit tactile switch connected to Pin 2.
If Player 1 fail to hit the switch during that duration, all 7 LED will light up signaling that the game is over.
If Player 1 successfully hit the switch durring that duration, LEDs will sequentially light up in reverse order.
...
The game goes on until one Player win.
The duration of LED sequence decreases linearly in a piece-wise function fashion.
  
  
Have fun.
