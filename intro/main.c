/*-----------you_shouldnt_be_here---------------------------------------------//
 Purpose: This is the main C file for the animation

//----------------------------------------------------------------------------*/

#include <stdio.h>
#include "quibble.h"

#define TIME_A0 0.601114
#define TIME_A1 1.16201
#define TIME_A2 1.742658
#define TIME_A3 2.055067
#define TIME_A4 2.357713
#define TIME_A5 2.648156
#define TIME_A6 2.955684
#define TIME_A7 3.125765
#define TIME_A8 3.260771
#define TIME_A9 3.426739
#define TIME_B1 3.568617
#define TIME_B2 3.714796
#define TIME_B3 3.844095
#define TIME_B4 4.016495
#define TIME_B5 4.195051
#define TIME_B6 4.313063
#define TIME_B7 4.484436
#define TIME_B8 4.614952
#define TIME_C1 4.77459
#define TIME_C2 4.931338
#define TIME_D1 5.077914
#define TIME_D2 5.252063
#define TIME_D3 5.382815
#define TIME_D4 5.51743
#define TIME_D5 5.665852
#define TIME_D6 5.821177
#define TIME_E1 5.959243
#define TIME_E2 6.124923
#define TIME_E3 6.256087
#define TIME_E4 6.394153

void create_filename(char *buffer, int i){
    sprintf(buffer, "check%d%d%d%d.png",
            (i%10000 - i%1000)/1000,
            (i%1000 - i%100)/100,
            (i%100 - i%10)/10,
            i%10);
}

void find_platform_device(int *platform, int *device, int argc, char **argv){

    if (argc == 1){
        fprintf(stderr, "No platform or device provided, defaulting to 0 0!\n");
    }
    else if (argc == 2){
        fprintf(stderr, "Too few arguments provided. Defaulting to 0 0!\n");
    }
    else if (argc > 3){
        fprintf(stderr, "Too many arguments provided. Defaulting to 0 0!\n");
    }
    else {
        *platform = strtol(argv[1], NULL, 10);
        *device = strtol(argv[2], NULL, 10);
        printf("Platform %d\nDevice %d\n", *platform, *device);
    }
}

int main(int argc, char **argv){

    int platform = 0;
    int device = 0;

    find_platform_device(&platform, &device, argc, argv);

    quibble_program qp = qb_parse_program_file("main.qbl");

    qb_output_program_to_file(qp, "program.txt");

    qb_configure_program(&qp, platform, device);

    int width = 1920;
    int height = 1080;
    quibble_pixels qpix = qb_create_pixel_array(qp, width, height, PRGBA8888);

    float world_size_x = 6;
    float ppu = width / world_size_x;
    float world_size_y = height / ppu;

    float world_position_x = 0;
    float world_position_y = 0;

    quibble_simple_camera qcam = qb_create_simple_camera(ppu,
                                                         world_size_x,
                                                         world_size_y,
                                                         world_position_x,
                                                         world_position_y);
    float fps = 30;

    float start_time = 3.5;
    float final_time = 8;
    float curr_time = start_time;
    int i = curr_time * fps;

    qb_set_args(&qp, "bg_layer", 2,
                "quibble_pixels_prgba8888 qpix", qpix,
                "quibble_simple_camera qcam", &qcam);

    qb_set_args(&qp, "intro_sequence", 2,
                "quibble_pixels_prgba8888 qpix", qpix,
                "quibble_simple_camera qcam", &qcam);

    qb_set_args(&qp, "space_bulk", 2,
                "quibble_pixels_prgba8888 qpix", qpix,
                "quibble_simple_camera qcam", &qcam);

    qb_set_args(&qp, "space_rings", 2,
                "quibble_pixels_prgba8888 qpix", qpix,
                "quibble_simple_camera qcam", &qcam);

    while (curr_time < final_time){
        char filename[15] = {0};
        qb_set_arg(&qp, "intro_sequence", "float time", &curr_time);
        qb_set_arg(&qp, "bg_layer", "float time", &curr_time);

        qb_run(qp, "bg_layer", width*height, 256);
        qb_run(qp, "intro_sequence", 1024, 256);

        if (curr_time > TIME_C1 && curr_time <= TIME_D1){
            qb_set_arg(&qp, "space_bulk", "float time", &curr_time);
            qb_set_arg(&qp, "space_rings", "float time", &curr_time);

            qb_run(qp, "space_bulk", 1024, 256);
            qb_run(qp, "space_rings", 1024, 256);
        }

        qb_pixels_device_to_host(qpix);

        create_filename(filename, i);
        qb_write_png_file(filename, qpix);
        printf("%s\n", filename);

        curr_time += 1/fps;
        i++;
    }

    return 0;
}

