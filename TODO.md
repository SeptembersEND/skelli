- https://www.lucavall.in/blog/how-to-structure-c-projects-my-experience-best-practices
- https://viewsourcecode.org/snaptoken/kilo/
- https://gist.github.com/ConnerWill/d4b6c776b509add763e17f9f113fd25b

- [ ] `menu.h`
    - [x] Make menu
    - [ ] console
        - [ ] `menu_appendconsole();` have a variable list of `char**` check every
          `console()` if one equals NULL, and remove it if it does.
            - when destination cha[x] is NULL dont print and increase counter
            - LinkedList?
            - or Wait till certain percentage of array is NULL and rearrange
              array? May not be good, if want to reenable/set char.
- [ ] `run.c`
    - [ ] Make buffer to write menu to
        - [ ] Change MenuList > MenuScreen, MenuScreen hold buffer
    - [ ] `check()`
        - [ ] check if file needs to be created
        - [ ] check if librray needs to be downloaded
    - [ ] `make()`
    - [ ] ``run()`
- [ ] Windows Compatibility Termios

# Later

- [ ] refactor `menu.h`
- hold global variable showing what OS is in use
- [ ] console print differently if menu not initalized
