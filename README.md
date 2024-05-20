# ALARM SYSTEM Project

## Alarm System application main parts:

### 1. Login System

- Read userID from the user.
- Read password from the user.
- Check the password (hint: password is inverted userID).
    - User have only 3 attempts to write the password correct. Otherwise the system will not work.
    - if the user entered correct password. system will work.

### 2. Set time and date

- As it is the first time to work, the system will ask the user to enter the Time and date.

### 3. Display menu

- Then a menu will be displayed, and the user have to choose an option.(
    Set time and date/ show time and date/ set alarm (user have 5 alarms) / exit)
- system can suppot upto 5 alarms.


## Alarm system overview

- **PC** will be the user I/O interface.

- **Nucleo board STM32F446re** will handle the data displyed on pc terminal and the input data from user via ***USART***.

- **Nucleo board STM32F446re** will also handle the *RTC DS1307* module via ***I2C*** to get real time clock.

- The data will be transmitted from **nucleo board** to **STM32F103 bluebill** via ***SPI*** to display time on the CLCD.

- If alarm time is matched, **nucleo** will set a **STM32F103** pin ,which is *external interrupt*, to turn on buzzer and display message on CLCD.