#pragma once
#include "game.h"
#include "position.h"
#include "tetromino.h" // only for combined position

class RunningMan : public Game
{
public:
    RunningMan(Display *display);

    // Game interface
public:
    void start() override;
    bool process() override;
    void clear() override;
    void reset() override;
    bool right() override;
    bool left() override;
    bool up() override;
    bool down() override;
    unsigned int points() override;
    char *row(byte n);

private:
    void newHind();
    bool isValid(Pos pos);
protected:
    static unsigned int highscore_;
    void render() override;
    bool man_state_;
    Pos man_pos_;
    byte next_hind_;
    char * field_;
    char current_field_start_;

    const Pos man_points[6] = {Pos(0,0),Pos(0,2),Pos(1,1),Pos(1,2),Pos(1,3),Pos(2,2)};
    const Pos man_moving_points[2] = {Pos(0,1),Pos(0,2)};

    const Pos hinds_[7][4] = {{Pos(0,1), Pos(1,0),Pos(1,1), Pos(2,1)},
                              {Pos(0,0), Pos(0,1),Pos(0,2), Pos(0,0)},
                              {Pos(0,0), Pos(0,1),Pos(1,0), Pos(1,1)},
                              {Pos(0,0), Pos(4,1),Pos(0,0), Pos(0,0)},
                              {Pos(0,0), Pos(1,0),Pos(1,1), Pos(2,0)},
                              {Pos(0,0), Pos(0,1),Pos(0,2), Pos(0,0)},
                              {Pos(0,0), Pos(0,0),Pos(0,0), Pos(0,0)}};
};
