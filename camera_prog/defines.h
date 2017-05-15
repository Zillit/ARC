#ifndef DEFINES_H
#define DEFINES_H
#include <string>
using namespace std;

namespace ARC_CAMERA
{
    const double HORIZONTAL_FOV = 62.2;     //Kamerans horisontella FOV
    const double VERTICAL_FOV = 48.8;       //Kamerans vertikala FOV
    
    const int ILOWH = 32;                   
    const int IHIGHH = 78;                  
    const int ILOWS = 30;                   //Bestämmer kontrastvillkoren för att vara ett hinder
    const int IHIGHS = 255;                 
    const int ILOWV = 30;                   
    const int IHIGHV = 255;                 
    
    const double HEIGHT_OF_CAMERA = 0.32;   //Kamerans höjd från marken
    const double ANGEL_OF_CAMERA = 20;      //Vinkeln kameran är vriden
    
    const int PIXEL_HEIGHT = 240;           //Bildskärmens upplösning  
    const int PIXEL_WIDTH = 320;            
    
    const int CANNY_THRE = 100;             //Sätter gränsen för hur skarp kontrastskillnader 
                                            // måste vara för att uppfattas som en linje
    
    const int MAX_NUM_OBJECTS = 10;         //Maximala antalet objekt som inte antas vara brus
    const int MIN_AREA = 100;               //Minsta arean för att kunna anses som ett objekt
    const int NUM_LAPS = 3;                 //Antal varv bilen ska köra
}

#endif
