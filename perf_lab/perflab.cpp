#include <stdio.h>
#include "cs1300bmp.h"
#include <iostream>
#include <fstream>
#include "Filter.h"

using namespace std;

#include "rdtsc.h"

//
// Forward declare the functions
//
Filter * readFilter(string filename);
float applyFilter(Filter *filter, cs1300bmp *input, cs1300bmp *output);

int
main(int argc, char **argv)
{

  if ( argc < 2) {
    fprintf(stderr,"Usage: %s filter inputfile1 inputfile2 .... \n", argv[0]);
  }

  //
  // Convert to C++ strings to simplify manipulation
  //
  string filtername = argv[1];

  //
  // remove any ".filter" in the filtername
  //
  string filterOutputName = filtername;
  string::size_type loc = filterOutputName.find(".filter");
  if (loc != string::npos) {
    //
    // Remove the ".filter" name, which should occur on all the provided filters
    //
    filterOutputName = filtername.substr(0, loc);
  }

  Filter *filter = readFilter(filtername);

  float sum = 0.0;
  int samples = 0;

  for (int inNum = 2; inNum < argc; inNum++) {
    string inputFilename = argv[inNum];
    string outputFilename = "filtered-" + filterOutputName + "-" + inputFilename;
    struct cs1300bmp *input = new struct cs1300bmp;
    struct cs1300bmp *output = new struct cs1300bmp;
    int ok = cs1300bmp_readfile( (char *) inputFilename.c_str(), input);

    if ( ok ) {
      float sample = applyFilter(filter, input, output);
      sum += sample;
      samples++;
      cs1300bmp_writefile((char *) outputFilename.c_str(), output);
    }
    delete input;
    delete output;
  }
  fprintf(stdout, "Average cycles per sample is %f\n", sum / samples);

}

struct Filter *
readFilter(string filename)
{
  ifstream input(filename.c_str());

  if ( ! input.bad() ) {
    int size = 0;
    input >> size;
    Filter *filter = new Filter(size);
    int div;
    input >> div;
    filter -> setDivisor(div);
    for (int i=0; i < size; i++) {
      for (int j=0; j < size; j++) {
	int value;
	input >> value;
	filter -> set(i,j,value);
      }
    }
    return filter;
  } else {
    cerr << "Bad input in readFilter:" << filename << endl;
    exit(-1);
  }
}

float
applyFilter(struct Filter *filter, cs1300bmp *input, cs1300bmp *output)
{

    float cycStart, cycStop;
    //unsigned short cycStart, cycStop;

  cycStart = rdtscll();

  output -> width = input -> width;
  output -> height = input -> height;

    unsigned short filter_size =  filter -> getSize();
    short iw = input -> width -1; //declare new variabl for input->width
    short ih = input ->height - 1; //declare new variabl for input->height
    
    short get_division = filter -> getDivisor();
    
    for(short plane = 0; plane < 3; plane++) {
       for(short row = 1; row < ih; row++) {
            for(short col = 1; col < iw; col++) {
                output -> color[plane][row][col] = 0;
                short * sum = &output -> color[plane][row][col];   
            
            //for (int j = 0; j < filter -> getSize(); j++) {
              //for (int i = 0; i < filter -> getSize(); i++) {	
                //output -> color[plane][row][col]
                  //= output -> color[plane][row][col]
                  //+ (input -> color[plane][row + i - 1][col + j - 1] 
                 //* filter -> get(i, j) );
                  //}
                //}
             
           for (short i= filter_size-1; (i>-1); i--) {	 
              for(short j = filter_size-1; (j>-1); j--) {
            //for (unsigned int i = 0; i < filter_size; i++) {	 
              //for (unsigned int j = 0; j <filter_size; j++) {
                 
                short filter_get = filter -> get(i, j);
                //short input = input->color[plane][row + i - 1][col + j - 1];
                 *sum  += (input -> color[plane][row + i - 1][col + j - 1] * filter_get);
                  //*sum += (input * filter_get);
              }
           }
	
        //output -> color[plane][row][col] = 	
         //output -> color[plane][row][col] / get_division;
             *sum /= (get_division);
            if ( *sum  < 0 ) {
                 *sum= 0;
            }

            if (*sum  > 255 ) { 
                *sum = 255;
            }
          }
        }
      }
  cycStop = rdtscll();
  float diff = cycStop - cycStart;
  //double diffPerPixel = diff / (output -> width * output -> height);
    float diffPerPixel = diff / (iw * ih);
  fprintf(stderr, "Took %f cycles to process, or %f cycles per pixel\n",
	 diff, diff / (iw * ih));
      
  return diffPerPixel;
}

