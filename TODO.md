# Version 1.0

- [ ] Bootstrap v0.1 -- stage1
    - [ ] Build system
        - [x] Commands
            - [x] `run`         download, make and run stage2
            - [x] `make`        compile stage2
            - [x] `download`    download dependencies for stage2
            - [x] `clean`       remove all created files
        - [x] Check if in correct directory
        - [ ] Work with Windows
    - [x] Input format specification
    - [x] Plan v0.2
    - Goals
        - Only Dependency: Compatible C Compiler
        - Graphics Cross-Platform Compatible
        - Supported Platforms: Windows, Linux
    - Output
        - C, C stdlib, git
        - No graphics
        - Linux
    - Tested
        - Alpine Linux Base
            - Dependencies: `apk add clang libc-dev git`

- [ ] Menu v0.2 -- stage2
    - [ ] Menu System
        - [ ] Data Structure
        - [ ] Anbui
            - [ ] Render Menus
            - [ ] Read Structure
    - [ ] Baked In Menu
    - [ ] Plan v0.3
    - Goals
        - Only Dependency: run stage1


# Sources

- [Project Structure](https://www.lucavall.in/blog/how-to-structure-c-projects-my-experience-best-practices)
- [ANSI Escape Sequences](https://gist.github.com/ConnerWill/d4b6c776b509add763e17f9f113fd25b)
- [Find Location of Executable](https://linuxvox.com/blog/how-do-i-find-the-location-of-the-executable-in-c/)
- [win32ports: unistd.h](https://github.com/win32ports/unistd_h)
