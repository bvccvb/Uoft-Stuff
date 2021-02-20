#include <stdio.h>
#include <string.h>
#include <stdbool.h>
#include "utilities.h"  // DO NOT REMOVE this line
#include "implementation_reference.h"   // DO NOT REMOVE this line
#include <stdlib.h>
#pragma GCC optimize ("Ofast")

void calculate_x_y(int * array, int width, int height, int *x, int *y);

/***********************************************************************************************************************
 * WARNING: Do not modify the implementation_driver and team info prototype (name, parameter, return value) !!!
 *          Do not forget to modify the team_name and team member information !!!
 **********************************************************************************************************************/
void print_team_info(){
    // Please modify this field with something interesting
    char team_name[] = "Jong Mo Park";

    // Please fill in your information
    char student_first_name[] = "Lam Fung";
    char student_last_name[] = "Ng";
    char student_student_number[] = "1003435525";

    // Printing out team information
    printf("*******************************************************************************************************\n");
    printf("Team Information:\n");
    printf("\tteam_name: %s\n", team_name);
    printf("\tstudent_first_name: %s\n", student_first_name);
    printf("\tstudent_last_name: %s\n", student_last_name);
    printf("\tstudent_student_number: %s\n", student_student_number);
}


void calculate_x_y(int * array, int width, int height, int *x, int *y){
    //printf("x is %d, y is %d\n", *x, *y);
    int _x = *x;
    int _y = *y;
    //printf("x is %d, y is %d\n", _x, _y);

    // 1 = w. 2 = a, 3 = s. 4 = d
    // 5 = cw. 6 = ccw. 7 = mx, 8 = my
    for(int i = 0; i < 50; i+=2){
        // printf("\n #%d instruction %d, offset %d\n", i/2, array[i], array[i+1]);
        //printf("Begin x is %d, y is %d\n", _x, _y);
        switch(array[i]){
            case 1:
                //printf("Begin x is %d, y is %d\n", _x, _y);
                _y -= array[i+1];
                //printf("End x is %d, y is %d\n", _x, _y);
                break;
            case 2:
                _x -= array[i+1];
                break;
            case 3:
                //printf("Begin x is %d, y is %d\n", _x, _y);
                _y += array[i+1];
                //printf("End x is %d, y is %d\n", _x, _y);
                break;
            case 4:
                _x += array[i+1];
                break;
            case 5:
                if(array[i+1] == 1 || array[i+1] == -3){ //Clockwise direction
                    int temp_x = _x;
                    _x = height - _y - 1;
                    _y = temp_x;
                } else if (array[i+1] == -1 || array[i+1] == 3) { //Counterclockwise direction
                    int temp_y = _y;
                    _y = width - _x - 1;
                    _x = temp_y;
                } else if (array[i+1] == 2 || array[i+1] == -2) { // Treat as a flip in x and flip in y
                    _y = height - _y - 1;
                    _x = width - _x - 1;
                }
                // If 0 just break
                break;
            case 6:
                break;

            case 7:
                _y = height - _y - 1;
                break;
            case 8:
                _x = width - _x - 1;
                break;
        }
        //printf("End x is %d, y is %d\n", _x, _y);
    }

    *x = _x;
    *y = _y;

    //printf("x is %d, y is %d\n", *x, *y);

    return;
}


/***********************************************************************************************************************
 * WARNING: Do not modify the implementation_driver and team info prototype (name, parameter, return value) !!!
 *          You can modify anything else in this file
 ***********************************************************************************************************************
 * @param sensor_values - structure stores parsed key value pairs of program instructions
 * @param sensor_values_count - number of valid sensor values parsed from sensor log file or commandline console
 * @param frame_buffer - pointer pointing to a buffer storing the imported  24-bit bitmap image
 * @param width - width of the imported 24-bit bitmap image
 * @param height - height of the imported 24-bit bitmap image
 * @param grading_mode - turns off verification and turn on instrumentation
 ***********************************************************************************************************************
 *
 **********************************************************************************************************************/
void implementation_driver(struct kv *sensor_values, int sensor_values_count, unsigned char *frame_buffer,
                           unsigned int width, unsigned int height, bool grading_mode) {

    // 1 = w. 2 = a, 3 = s. 4 = d
    // 5 = cw. 6 = ccw. 7 = mx, 8 = my

    int * action_array = (int*)malloc( 50 * sizeof(int) ); 

    int action_index = 0;

    int top_left_corner_x = 0;
    int top_right_corner_x = 0;
    int bottom_left_corner_x = 0;
    int bottom_right_corner_x = 0;

    int top_left_corner_y = 0;
    int top_right_corner_y = 0;
    int bottom_left_corner_y = 0;
    int bottom_right_corner_y = 0;

    int most_top_row = height;
    int most_bottom_row = 0;
    int most_left_col = width;
    int most_right_col = 0;

    // One time cost, find the corners and box the image that is not 0
    for (int i = 0; i < height; i++){
        for (int j = 0; j < width ; j++){
            int position_buffer_frame = i * width * 3 + j * 3;

            if(frame_buffer[position_buffer_frame] != 255 &&
               frame_buffer[position_buffer_frame + 1] !=255 &&
               frame_buffer[position_buffer_frame + 2] !=255 ){
                //if this is the top 
                if(i<most_top_row)
                    most_top_row = i;
                if(i>most_bottom_row)
                    most_bottom_row = i;
                if(j<most_left_col)
                    most_left_col = j;
                if(j>most_right_col)
                    most_right_col = j;
            }
        }
    }

    // printf("top %d bottom %d left %d right %d\n", most_top_row, most_bottom_row, most_left_col, most_right_col);
    //top 560 bottom 711 left 566 right 716

    int boxed_image_height = 1 + most_bottom_row - most_top_row;
    int boxed_image_width = 1 + most_right_col - most_left_col;

    // printf("Boxed image height is %d width %d\n", boxed_image_height, boxed_image_width);

    // If not perfect square, add extra rows to bottom and left
    if(boxed_image_height > boxed_image_width){
        most_right_col += boxed_image_height - boxed_image_width;
        boxed_image_width = boxed_image_height;
    }else if(boxed_image_width > boxed_image_height){
        most_bottom_row += boxed_image_width - boxed_image_height;
        boxed_image_height = boxed_image_width;
    }

    top_left_corner_x = most_left_col;
    top_right_corner_x = most_right_col;
    bottom_left_corner_x = most_left_col;
    bottom_right_corner_x = most_right_col;

    top_left_corner_y = most_top_row;
    top_right_corner_y = most_top_row;
    bottom_left_corner_y = most_bottom_row;
    bottom_right_corner_y = most_bottom_row;


    unsigned char boxed_image[boxed_image_height][boxed_image_width * 3];

    // printf("initial\n");
    // printf("top left (%d,%d) top right (%d,%d) bottom left (%d,%d) bottom right (%d,%d)\n", 
    //         top_left_corner_x, top_left_corner_y, top_right_corner_x, top_right_corner_y,
    //         bottom_left_corner_x, bottom_left_corner_y, bottom_right_corner_x, bottom_right_corner_y);

    // printBMP(width, height, frame_buffer);

    for (int i = 0; i < boxed_image_height; i++){
        for (int j = 0; j < boxed_image_width ; j++){
            int position_buffer_frame = (i+most_top_row) * width * 3 + (j+most_left_col) * 3;
            boxed_image[i][3*j] = frame_buffer[position_buffer_frame];
            boxed_image[i][3*j+1] = frame_buffer[position_buffer_frame + 1];
            boxed_image[i][3*j+2] = frame_buffer[position_buffer_frame + 2];
        }
    }
    // printf("Boxed Image\n");
    // printf("*******************************************************************************************************\n");
    // for (int i = 0; i < boxed_image_height; i++) {
    //     for (int j = 0; j < boxed_image_width; j++) {
    //         printf("[%d,%d,%d]", boxed_image[i][3*j], boxed_image[i][3*j + 1], boxed_image[i][3*j + 2]);
    //     }
    //     printf("\n");
    // }
    // printf("*******************************************************************************************************\n");

            // printf("Before\n");
            // printBMP(width, height, frame_buffer);

    int processed_frames = 0;

    for (int sensorValueIdx = 0; sensorValueIdx < sensor_values_count; sensorValueIdx++) {
//        printf("Processing sensor value #%d: %s, %d\n", sensorValueIdx, sensor_values[sensorValueIdx].key,
//               sensor_values[sensorValueIdx].value);
        if (!strcmp(sensor_values[sensorValueIdx].key, "W")) {
            if(sensor_values[sensorValueIdx].value < 0){
                action_array[action_index] = 3;
                action_array[action_index + 1] = -1*sensor_values[sensorValueIdx].value;
                //offset_down += sensor_values[sensorValueIdx].value;
            } else {
                action_array[action_index] = 1;
                action_array[action_index + 1] = sensor_values[sensorValueIdx].value;
                //offset_up += sensor_values[sensorValueIdx].value;
            }
//            printBMP(width, height, frame_buffer);
        } else if (!strcmp(sensor_values[sensorValueIdx].key, "A")) {
            if(sensor_values[sensorValueIdx].value < 0){
                action_array[action_index] = 4;
                action_array[action_index + 1] = -1*sensor_values[sensorValueIdx].value;
                //offset_right += sensor_values[sensorValueIdx].value;
            } else {
                action_array[action_index] = 2;
                action_array[action_index + 1] = sensor_values[sensorValueIdx].value;
                //offset_left += sensor_values[sensorValueIdx].value;
            }
//            printBMP(width, height, frame_buffer);
        } else if (!strcmp(sensor_values[sensorValueIdx].key, "S")) {
            if(sensor_values[sensorValueIdx].value < 0){
                action_array[action_index] = 1;
                action_array[action_index + 1] = -1*sensor_values[sensorValueIdx].value;
                //offset_up += sensor_values[sensorValueIdx].value;
            } else {
                action_array[action_index] = 3;
                action_array[action_index + 1] = sensor_values[sensorValueIdx].value;
                //offset_down += sensor_values[sensorValueIdx].value;
            }
//            printBMP(width, height, frame_buffer);
        } else if (!strcmp(sensor_values[sensorValueIdx].key, "D")) {
            if(sensor_values[sensorValueIdx].value < 0){
                action_array[action_index] = 2;
                action_array[action_index + 1] = -1*sensor_values[sensorValueIdx].value;
                //offset_left += sensor_values[sensorValueIdx].value;
            } else {
                action_array[action_index] = 4;
                action_array[action_index + 1] = sensor_values[sensorValueIdx].value;
                //offset_right += sensor_values[sensorValueIdx].value;
            }
//            printBMP(width, height, frame_buffer);
        } else if (!strcmp(sensor_values[sensorValueIdx].key, "CW")) {
                action_array[action_index] = 5;
                action_array[action_index + 1] = sensor_values[sensorValueIdx].value % 4;

//            printBMP(width, height, frame_buffer);
        } else if (!strcmp(sensor_values[sensorValueIdx].key, "CCW")) {
                action_array[action_index] = 5;
                action_array[action_index + 1] =  (-1*sensor_values[sensorValueIdx].value) % 4;

//            printBMP(width, height, frame_buffer);
        } else if (!strcmp(sensor_values[sensorValueIdx].key, "MX")) {
            action_array[action_index] = 7;
            action_array[action_index + 1] = sensor_values[sensorValueIdx].value;
            // (width - x - 1, y)
            //offset_x = -1*offset_x;  
//            printBMP(width, height, frame_buffer);
        } else if (!strcmp(sensor_values[sensorValueIdx].key, "MY")) {
            action_array[action_index] = 8;
            action_array[action_index + 1] = sensor_values[sensorValueIdx].value;
            //offset_y = -1*offset_y;
            //xy need to lok at (y, 1 o– x)
//            printBMP(width, height, frame_buffer);
        }

        action_index+=2;
        processed_frames += 1;
        // We need to remake the image
        if (processed_frames % 25 == 0){

            // printf("Before\n");
            // printBMP(width, height, frame_buffer);

            // Fill previous box in frame buffer with 255
            int count = 0;
            for (int i = most_top_row; i <= most_bottom_row; i++){
                for (int j = most_left_col; j <= most_right_col ; j++){
                    count++;
                    int position_buffer_frame = i * width * 3 + j * 3;
                    frame_buffer[position_buffer_frame] = 255;
                    frame_buffer[position_buffer_frame + 1] = 255;
                    frame_buffer[position_buffer_frame + 2] = 255;
                }
            }


            calculate_x_y(action_array, width, height, &top_left_corner_x, &top_left_corner_y);
            calculate_x_y(action_array, width, height, &top_right_corner_x, &top_right_corner_y);
            calculate_x_y(action_array, width, height, &bottom_left_corner_x, &bottom_left_corner_y);
            calculate_x_y(action_array, width, height, &bottom_right_corner_x, &bottom_right_corner_y);

            // printf("after_transforms\n");
            // printf("top left (%d,%d) top right (%d,%d) bottom left (%d,%d) bottom right (%d,%d)\n", 
            // top_left_corner_x, top_left_corner_y, top_right_corner_x, top_right_corner_y,
            // bottom_left_corner_x, bottom_left_corner_y, bottom_right_corner_x, bottom_right_corner_y);

            // Figure out orientation of our box after transforms
            // 1 = original, all 4 corners are in place
            // 2 = upsidedown, bottom 2 corners higher than top 2
            // 3 = flipped on y axis, left 2 corners are greater than right 2
            // 4 = rotated 90 degrees cw
            // 5 = rotated 90 degrees ccw
            // 6 = rotated 180 degrees
            // 7 = ccw then flip x
            // 8 = cw then flip x

            int orientation = 0;

            if(top_left_corner_y < bottom_left_corner_y &&
               top_left_corner_x < top_right_corner_x &&
               bottom_left_corner_x < bottom_right_corner_x &&
               top_right_corner_y < bottom_right_corner_y){
                most_top_row = top_left_corner_y;
                most_bottom_row = bottom_right_corner_y;
                most_left_col = top_left_corner_x;
                most_right_col = bottom_right_corner_x;
                orientation = 1;
            } else if (top_left_corner_y > bottom_left_corner_y &&
                       top_left_corner_x < top_right_corner_x &&
                       bottom_left_corner_x < bottom_right_corner_x &&
                       top_right_corner_y > bottom_right_corner_y){

                most_top_row = bottom_left_corner_y;
                most_bottom_row = top_right_corner_y;
                most_left_col = bottom_left_corner_x;
                most_right_col = bottom_right_corner_x;

                // reorient
                top_left_corner_y = most_top_row;
                top_right_corner_y = most_top_row;
                bottom_left_corner_y = most_bottom_row;
                bottom_right_corner_y = most_bottom_row;
                orientation = 2;

            } else if (top_left_corner_y < bottom_left_corner_y &&
                       top_left_corner_x > top_right_corner_x &&
                       bottom_left_corner_x > bottom_right_corner_x &&
                       top_right_corner_y < bottom_right_corner_y){

                most_top_row = top_left_corner_y;
                most_bottom_row = bottom_right_corner_y;
                most_left_col = top_right_corner_x;
                most_right_col = bottom_left_corner_x;

                // reorient
                top_left_corner_x = most_left_col;
                top_right_corner_x = most_right_col;
                bottom_left_corner_x = most_left_col;
                bottom_right_corner_x = most_right_col;

                orientation = 3;
            } else if (top_left_corner_y == bottom_left_corner_y &&
                       top_left_corner_x == top_right_corner_x &&
                       bottom_left_corner_x == bottom_right_corner_x &&
                       top_right_corner_y == bottom_right_corner_y &&
                       top_right_corner_x > bottom_right_corner_x &&
                       top_left_corner_x > bottom_left_corner_x &&
                       top_right_corner_y > top_left_corner_y &&
                       bottom_right_corner_y > bottom_left_corner_y){

                most_top_row = top_left_corner_y;
                most_bottom_row = bottom_right_corner_y;
                most_left_col = bottom_right_corner_x;
                most_right_col = top_left_corner_x;

                // reorient
                top_left_corner_x = most_left_col;
                top_right_corner_x = most_right_col;
                bottom_left_corner_x = most_left_col;
                bottom_right_corner_x = most_right_col;

                top_left_corner_y = most_top_row;
                top_right_corner_y = most_top_row;
                bottom_left_corner_y = most_bottom_row;
                bottom_right_corner_y = most_bottom_row;

                orientation = 4;
            } else if (top_left_corner_y == bottom_left_corner_y &&
                       top_left_corner_x == top_right_corner_x &&
                       bottom_left_corner_x == bottom_right_corner_x &&
                       top_right_corner_y == bottom_right_corner_y &&
                       top_right_corner_x < bottom_right_corner_x &&
                       top_left_corner_x < bottom_left_corner_x &&
                       top_right_corner_y < top_left_corner_y &&
                       bottom_left_corner_y > bottom_right_corner_y){

                most_top_row = top_right_corner_y;
                most_bottom_row = bottom_left_corner_y;
                most_left_col = top_right_corner_x;
                most_right_col = bottom_left_corner_x;

                // reorient
                top_left_corner_x = most_left_col;
                top_right_corner_x = most_right_col;
                bottom_left_corner_x = most_left_col;
                bottom_right_corner_x = most_right_col;

                top_left_corner_y = most_top_row;
                top_right_corner_y = most_top_row;
                bottom_left_corner_y = most_bottom_row;
                bottom_right_corner_y = most_bottom_row;

                orientation = 5;
            } else if (top_left_corner_y > bottom_left_corner_y &&
                       top_left_corner_x > top_right_corner_x &&
                       bottom_left_corner_x > bottom_right_corner_x &&
                       top_right_corner_y > bottom_right_corner_y &&
                       top_right_corner_x == bottom_right_corner_x &&
                       top_left_corner_x == bottom_left_corner_x){

                most_top_row = bottom_right_corner_y;
                most_bottom_row = top_left_corner_y;
                most_left_col = bottom_right_corner_x;
                most_right_col = top_left_corner_x;

                // reorient
                top_left_corner_x = most_left_col;
                top_right_corner_x = most_right_col;
                bottom_left_corner_x = most_left_col;
                bottom_right_corner_x = most_right_col;

                top_left_corner_y = most_top_row;
                top_right_corner_y = most_top_row;
                bottom_left_corner_y = most_bottom_row;
                bottom_right_corner_y = most_bottom_row;

                orientation = 6;
            } else if (top_left_corner_y == bottom_left_corner_y &&
                       top_left_corner_x == top_right_corner_x &&
                       bottom_left_corner_x == bottom_right_corner_x &&
                       top_right_corner_y == bottom_right_corner_y &&
                       top_right_corner_x < bottom_right_corner_x &&
                       top_left_corner_x < bottom_left_corner_x &&
                       top_right_corner_y > top_left_corner_y &&
                       bottom_left_corner_y < bottom_right_corner_y){

                most_top_row = top_left_corner_y;
                most_bottom_row = bottom_right_corner_y;
                most_left_col = top_right_corner_x;
                most_right_col = bottom_left_corner_x;

                // reorient
                top_left_corner_x = most_left_col;
                top_right_corner_x = most_right_col;
                bottom_left_corner_x = most_left_col;
                bottom_right_corner_x = most_right_col;

                top_left_corner_y = most_top_row;
                top_right_corner_y = most_top_row;
                bottom_left_corner_y = most_bottom_row;
                bottom_right_corner_y = most_bottom_row;

                orientation = 7;
            } else if (top_left_corner_y == bottom_left_corner_y &&
                       top_left_corner_x == top_right_corner_x &&
                       bottom_left_corner_x == bottom_right_corner_x &&
                       top_right_corner_y == bottom_right_corner_y &&
                       top_right_corner_x > bottom_right_corner_x &&
                       top_left_corner_x > bottom_left_corner_x &&
                       top_right_corner_y < top_left_corner_y &&
                       bottom_right_corner_y < bottom_left_corner_y){

                most_top_row = top_right_corner_y;
                most_bottom_row = bottom_left_corner_y;
                most_left_col = bottom_right_corner_x;
                most_right_col = top_left_corner_x;

                // reorient
                top_left_corner_x = most_left_col;
                top_right_corner_x = most_right_col;
                bottom_left_corner_x = most_left_col;
                bottom_right_corner_x = most_right_col;

                top_left_corner_y = most_top_row;
                top_right_corner_y = most_top_row;
                bottom_left_corner_y = most_bottom_row;
                bottom_right_corner_y = most_bottom_row;

                orientation = 8;
            }

            // printf("Orientation is %d\n", orientation);

            switch (orientation){
                case 1:
                    // Draw it in normally
                    for (int i = 0; i < boxed_image_height; i++){
                        for (int j = 0; j < boxed_image_width ; j++){
                            // printf("[%d,%d,%d]\n", boxed_image[i][3*j], boxed_image[i][3*j + 1], boxed_image[i][3*j + 2]);
                            int position_buffer_frame = (i+most_top_row) * width * 3 + (j+most_left_col) * 3;
                            frame_buffer[position_buffer_frame] = boxed_image[i][3*j] ;
                            frame_buffer[position_buffer_frame+1] = boxed_image[i][3*j+1];
                            frame_buffer[position_buffer_frame+2] = boxed_image[i][3*j+2];
                        }
                    }
                    break;
                case 2:
                    for (int i = 0; i < boxed_image_height; i++){
                        for (int j = 0; j < boxed_image_width ; j++){
                            // printf("[%d,%d,%d]\n", boxed_image[i][3*j], boxed_image[i][3*j + 1], boxed_image[i][3*j + 2]);
                            int position_buffer_frame = (i+most_top_row) * width * 3 + (j+most_left_col) * 3;
                            frame_buffer[position_buffer_frame] = boxed_image[boxed_image_height-1-i][3*j] ;
                            frame_buffer[position_buffer_frame+1] = boxed_image[boxed_image_height-1-i][3*j+1];
                            frame_buffer[position_buffer_frame+2] = boxed_image[boxed_image_height-1-i][3*j+2];
                        }
                    }
                    break;
                case 3:
                    for (int i = 0; i < boxed_image_height; i++){
                        for (int j = 0; j < boxed_image_width ; j++){
                            // printf("[%d,%d,%d]\n", boxed_image[i][3*j], boxed_image[i][3*j + 1], boxed_image[i][3*j + 2]);
                            int position_buffer_frame = (i+most_top_row) * width * 3 + (j+most_left_col) * 3;
                            int index = (boxed_image_width - 1) * 3;
                            frame_buffer[position_buffer_frame] = boxed_image[i][index - (3*j)] ;
                            frame_buffer[position_buffer_frame+1] = boxed_image[i][index - (3*j) + 1];
                            frame_buffer[position_buffer_frame+2] = boxed_image[i][index - (3*j) + 2];
                        }
                    }
                    break;
                case 4: 
                    for (int i = 0; i < boxed_image_height; i++){
                        for (int j = 0; j < boxed_image_width ; j++){
                            // printf("[%d,%d,%d]\n", boxed_image[i][3*j], boxed_image[i][3*j + 1], boxed_image[i][3*j + 2]);
                            int position_buffer_frame = (i+most_top_row) * width * 3 + (j+most_left_col) * 3;
                            frame_buffer[position_buffer_frame] = boxed_image[boxed_image_height-1-j][3*i] ;
                            frame_buffer[position_buffer_frame+1] = boxed_image[boxed_image_height-1-j][3*i + 1];
                            frame_buffer[position_buffer_frame+2] = boxed_image[boxed_image_height-1-j][3*i + 2];
                        }
                    }
                    break;
                case 5: 
                    for (int i = 0; i < boxed_image_height; i++){
                        for (int j = 0; j < boxed_image_width ; j++){
                            // printf("[%d,%d,%d]\n", boxed_image[i][3*j], boxed_image[i][3*j + 1], boxed_image[i][3*j + 2]);
                            int position_buffer_frame = (i+most_top_row) * width * 3 + (j+most_left_col) * 3;
                            int index = (boxed_image_width - 1) * 3;
                            frame_buffer[position_buffer_frame] = boxed_image[j][index - (3*i)] ;
                            frame_buffer[position_buffer_frame+1] = boxed_image[j][index - (3*i) + 1];
                            frame_buffer[position_buffer_frame+2] = boxed_image[j][index - (3*i) + 2];
                        }
                    }
                    break;
                case 6:
                    for (int i = 0; i < boxed_image_height; i++){
                        for (int j = 0; j < boxed_image_width ; j++){
                            int index = (boxed_image_height - 1) * 3;
                            // printf("[%d,%d,%d]\n", boxed_image[ boxed_image_height-1- (i)][index-j*3], boxed_image[ boxed_image_height-1- (i)][index-j*3 +2], boxed_image[ boxed_image_height-1- (i)][index-(j*3)+2]);
                            int position_buffer_frame = (i+most_top_row) * width * 3 + (j+most_left_col) * 3;
                            frame_buffer[position_buffer_frame] = boxed_image[boxed_image_height-1-i][index - (3*j)] ;
                            frame_buffer[position_buffer_frame+1] = boxed_image[boxed_image_height-1-i][index - (3*j) + 1];
                            frame_buffer[position_buffer_frame+2] = boxed_image[boxed_image_height-1-i][index - (3*j) + 2];
                        }
                    }
                    break;
                case 7:
                    for (int i = 0; i < boxed_image_height; i++){
                        for (int j = 0; j < boxed_image_width ; j++){
                            int index = (boxed_image_height - 1) * 3;
                            // printf("[%d,%d,%d]\n", boxed_image[ boxed_image_height-1- (i)][index-j*3], boxed_image[ boxed_image_height-1- (i)][index-j*3 +2], boxed_image[ boxed_image_height-1- (i)][index-(j*3)+2]);
                            int position_buffer_frame = (i+most_top_row) * width * 3 + (j+most_left_col) * 3;
                            frame_buffer[position_buffer_frame] = boxed_image[j][(3*i)] ;
                            frame_buffer[position_buffer_frame+1] = boxed_image[j][(3*i) + 1];
                            frame_buffer[position_buffer_frame+2] = boxed_image[j][(3*i) + 2];
                        }
                    }
                    break;
                case 8:
                    for (int i = 0; i < boxed_image_height; i++){
                        for (int j = 0; j < boxed_image_width ; j++){
                            int index = (boxed_image_height - 1) * 3;
                            // printf("[%d,%d,%d]\n", boxed_image[ boxed_image_height-1- (i)][index-j*3], boxed_image[ boxed_image_height-1- (i)][index-j*3 +2], boxed_image[ boxed_image_height-1- (i)][index-(j*3)+2]);
                            int position_buffer_frame = (i+most_top_row) * width * 3 + (j+most_left_col) * 3;
                            frame_buffer[position_buffer_frame] = boxed_image[boxed_image_height-1-j][index - (3*i)] ;
                            frame_buffer[position_buffer_frame+1] = boxed_image[boxed_image_height-1-j][index - (3*i) + 1];
                            frame_buffer[position_buffer_frame+2] = boxed_image[boxed_image_height-1-j][index - (3*i) + 2];
                        }
                    }
                    break;

            }

            // replace with new box image
            for (int i = 0; i < boxed_image_height; i++){
                for (int j = 0; j < boxed_image_width ; j++){
                    int position_buffer_frame = (i+most_top_row) * width * 3 + (j+most_left_col) * 3;
                    boxed_image[i][3*j] = frame_buffer[position_buffer_frame];
                    boxed_image[i][3*j+1] = frame_buffer[position_buffer_frame + 1];
                    boxed_image[i][3*j+2] = frame_buffer[position_buffer_frame + 2];
                }
            }

            // printf("After\n");
            // printBMP(width, height, frame_buffer);

            for (int i = 0; i < 50; i++){
                action_array[i] = 0;
            }

            action_index = 0;
        }

        if (processed_frames % 25 == 0) {
            verifyFrame(frame_buffer, width, height, grading_mode);
        }
    }

    free(action_array);

    return;
}