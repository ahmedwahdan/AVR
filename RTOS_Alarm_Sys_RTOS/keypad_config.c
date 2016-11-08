/*
 * keypad_config.c
 *
 * Created: 10/11/2016 11:30:42 AM
 *  Author: Master
 */ 
 #include "keypad_config.h"


 const Keypad_config_t g_Keypad_Col_Config[Col_numbers] =
 {
	 {PT4,0},
	 {PT4,1},
	 {PT4,2},
	 {PT4,3}
 };

 const Keypad_config_t g_Keypad_Row_Config[Row_numbers] =
 {
	 {PT4,4},
	 {PT4,5},
	 {PT4,6},
	 {PT4,7}
 };
 /*
-----------------
| 1 | 2 | 3 | A |
-----------------
| 4 | 5 | 6 | B |
-----------------
| 7 | 8 | 9 | C |
-----------------
| * | 0 | # | D |
-----------------
 */

 const uint8_t g_Keypad_Map[Row_numbers][Col_numbers] =
 {
 {'7','8','9','A'},
  {'4','5','6','B'},
 {'1','2','3','C'},
 {'*','0','#','D'}
 /*
	{'1','2','3','A'},
	{'4','5','6','B'},
	{'7','8','9','C'},
	{'*','0','#','D'}
*/
 };