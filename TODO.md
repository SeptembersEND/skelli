# Version 1.0

- [ ] Bootstrap v0.1 -- stage1
    - [x] Build system
        - [x] Commands
            - [x] `run`         download, make and run stage2
            - [x] `make`        compile stage2
            - [x] `download`    download dependencies for stage2
            - [x] `clean`       remove all created files
        - [x] Change directory to where executable is
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
