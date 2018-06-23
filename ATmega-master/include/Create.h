/*
 * Create.h
 *
 *  Created on: Apr 4, 2012
 *      Author: Clark Scheff
 */

#ifndef CREATE_H_
#define CREATE_H_

#define COMMAND_START					128
#define COMMAND_BAUD					129
#define COMMAND_CONTROL					130
#define COMMAND_SAFE					131
#define COMMAND_FULL					132
#define COMMAND_SPOT					134
#define COMMAND_COVER					135
#define COMMAND_DEMO					136
#define COMMAND_DRIVE					137
#define COMMAND_LOW_SIDE_DRIVERS		138
#define COMMAND_LEDS					139
#define COMMAND_SONG					140
#define COMMAND_PLAY					141
#define COMMAND_SENSORS					142
#define COMMAND_COVER_AND_DOCK			143
#define COMMAND_PWM_LOW_SIDE			144
#define COMMAND_DRIVE_DIRECT			145
#define COMMAND_DIGITAL_OUTPUTS			147
#define COMMAND_STREAM					148
#define COMMAND_QUERY_LIST				149
#define COMMAND_PAUSE_RESUME_STREAM		150
#define COMMAND_SEND_IR					151
#define COMMAND_SCRIPT					152
#define COMMAND_PLAY_SCRIPT				153
#define COMMAND_SHOW_SCRIPT				154
#define COMMAND_WAIT_TIME				155
#define COMMAND_WAIT_DISTANCE			156
#define COMMAND_WAIT_ANGLE				157
#define COMMAND_WAIT_EVENT				158

void Create_Init( void );
void Create_Drive( int16_t velocity, int16_t radius);
void Create_DriveDirect( int16_t rightVelocity, int16_t leftVelocity);
void Create_SetHorn( void );
void Create_PlayHorn( void );

#endif /* CREATE_H_ */
