cmake_minimum_required(VERSION 3.0)
project(Tetris)

include_directories("$ENV{HOME}/Software/common/ATMEGA328/PortPin")
add_avr_executable(${PROJECT_NAME}
    main.cpp
    matrixdisplay.cpp
    tetromino.cpp
    tetris.cpp
    display.cpp
    menu_sm.cpp
    spi.cpp
    timing.c
    text.cpp
    snake.cpp
    game.cpp
    position.cpp
    defaults.h
    event.cpp
    list.h
    timer.cpp
    button.cpp
    dodge.cpp
    staticlist.h
    spaceinvaders.cpp
    buttonautotrigger.cpp
    trigger.cpp
    )
