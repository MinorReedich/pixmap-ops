#include "ppm_image.h"
#include <string>
#include <fstream>
#include <iostream>
#include <math.h>
#include <stdlib.h>

using namespace agl;
using namespace std;

ppm_image::ppm_image() 
{
}

ppm_image::ppm_image(int w, int h) 
{
   ht = h;
   wdth = w;
}

ppm_image::ppm_image(const ppm_image& orig)
{
   ht = orig.ht;
   wdth = orig.wdth;
   ary = orig.ary;
}

ppm_image& ppm_image::operator=(const ppm_image& orig)
{
   if (&orig == this) // protect against self-assignment
   {
      return *this;
   }
   ht = orig.ht;
   wdth = orig.wdth;
   ary = orig.ary;
   return *this;   
}

ppm_image::~ppm_image()
{
   //cout << ("Destroyed\n");
}

bool ppm_image::load(const std::string& filename)
{
   ifstream file(filename);
   if (!file) // true if the file is valid
   {
	   return false;
   }
   else {
      // remove the headers and read w and h
      string p3;
      int colormax;
      file >> p3;
      file >> ht;
      file >> wdth;
      file >> colormax;

      // create the frame for the array
      ary = new ppm_pixel*[wdth];
      for(int i = 0; i < wdth; ++i) {
         ary[i] = new ppm_pixel[ht];
      }

      // read the rgb values into the array
      for(int i = 0; i<wdth; i++){  
         for(int j = 0; j<ht; j++){
            int red, green, blue;
            file >> red;
            file >> green;
            file >> blue;
            ary[i][j] = ppm_pixel{red, green, blue};
         }
      }

      return true; 
   }
}

bool ppm_image::save(const std::string& filename) const
{
   ifstream file(filename);
   if(file){
      return false;
   }
   else{
      ofstream myFile;
      myFile.open(filename);
   
      // input the headers and w and h
      myFile << "P3" << "\n" << ht << " " << wdth << "\n" << "255" << "\n";

      // input the rgb values into the file
      for (int i = 0; i < wdth; i++) {
         for (int j = 0; j < ht; j++) {
            myFile << ary[i][j].r << "\n" << ary[i][j].g << "\n" << ary[i][j].b << "\n";
         }
      }
      myFile.close();
      return true;
   }
}

 ppm_image ppm_image::resize(int h, int w) const
{
   ppm_image result(w, h);

   // create new array frame
   result.ary = new ppm_pixel*[w];
   for(int i = 0; i < w; ++i) {
      result.ary[i] = new ppm_pixel[h];
   }

   for(int i = 0; i<w; i++){
      for(int j = 0; j<h; j++){ 
         int old_x = floor(i/(double)(w-1) * (wdth-1));
         int old_y = floor(j/(double)(h-1) * (ht-1)); 
         result.ary[i][j] = ary[old_x][old_y]; 
      }
   }

   return result;
}

ppm_image ppm_image::flip_horizontal() const
{
   ppm_image result(wdth, ht);
   
   // create new array frame
   result.ary = new ppm_pixel*[wdth];
   for(int i = 0; i < wdth; ++i) {
      result.ary[i] = new ppm_pixel[ht];
   }

   for(int i = 0; i<wdth; i++){
      for(int j = 0; j<ht; j++){
         result.ary[wdth-i-1][ht-j-1] = ary[i][j]; // horizontally flip eaach pixel 
      }
   }
   return result;
}

ppm_image ppm_image::subimage(int startx, int starty, int w, int h) const
{
   ppm_image result(w, h);  

   // create new array frame
   result.ary = new ppm_pixel*[w];
   for(int i = 0; i < w; ++i) {
      result.ary[i] = new ppm_pixel[h];
   }

   for(int i = 0; i<min(w, (wdth-startx)); i++){
      for(int j = 0; j<min(h, (ht-starty)); j++){ 
         result.ary[i][j] = ary[i+startx][j+starty];
      }
   }

   return result;
}

void ppm_image::replace(const ppm_image& image, int startx, int starty)
{
   for(int i = 0; i<min(wdth, image.width()); i++){
      for(int j = 0; j<min(ht, image.height()); j++){
         ary[i+startx][j+starty] = image.ary[i][j]; 
      }
   }
}

ppm_image ppm_image::alpha_blend(const ppm_image& other, float alpha) const
{
   ppm_image result(wdth, ht);

   // create new array frame
   result.ary = new ppm_pixel*[wdth];
   for(int i = 0; i < wdth; ++i) {
      result.ary[i] = new ppm_pixel[ht];
   }

   for(int i = 0; i<wdth; i++){
      for(int j = 0; j<ht; j++){
         result.ary[i][j].r = min((int)(ary[i][j].r*(1-alpha) + other.ary[i][j].r*alpha), 255); 
         result.ary[i][j].g = min((int)(ary[i][j].g*(1-alpha) + other.ary[i][j].g*alpha), 255); 
         result.ary[i][j].b = min((int)(ary[i][j].b*(1-alpha) + other.ary[i][j].b*alpha), 255);    
      }
   }
   return result;
}

ppm_image ppm_image::gammaCorrect(float gamma) const
{
   ppm_image result(wdth, ht);

   // create new array frame
   result.ary = new ppm_pixel*[wdth];
   for(int i = 0; i < wdth; ++i) {
      result.ary[i] = new ppm_pixel[ht];
   }

   for(int i = 0; i<wdth; i++){
      for(int j = 0; j<ht; j++){
         result.ary[i][j].r = min((int)(255*pow(ary[i][j].r/255.0f, (1.0f/gamma))), 255); 
         result.ary[i][j].g = min((int)(255*pow(ary[i][j].g/255.0f, (1.0f/gamma))), 255); 
         result.ary[i][j].b = min((int)(255*pow(ary[i][j].b/255.0f, (1.0f/gamma))), 255);    
      }
   }

   return result;
}

ppm_image ppm_image::grayscale() const
{
   ppm_image result(wdth, ht);

   // create new array frame
   result.ary = new ppm_pixel*[wdth];
   for(int i = 0; i < wdth; ++i) {
      result.ary[i] = new ppm_pixel[ht];
   }

   for(int i = 0; i<wdth; i++){
      for(int j = 0; j<ht; j++){
         int gray = (int)round((ary[i][j].r + ary[i][j].g + ary[i][j].b)/3.0f);
         result.ary[i][j].r = gray;
         result.ary[i][j].g = gray;
         result.ary[i][j].b = gray;
      }
   }
   return result;
}

ppm_pixel ppm_image::get(int row, int col) const
{
   return ary[row][col];
}

void ppm_image::set(int row, int col, const ppm_pixel& c)
{
   ary[row][col] = c;
}

int ppm_image::height() const
{
   return ht;
}

int ppm_image::width() const
{
   return wdth;
}

// MY 6 METHODS

ppm_image ppm_image::border() const // places border around image
{
   ppm_image result(wdth, ht);

   // create new array frame
   result.ary = new ppm_pixel*[wdth];
   for(int i = 0; i < wdth; ++i) {
      result.ary[i] = new ppm_pixel[ht];
   }

   // load in
   for(int i = 0; i<wdth; i++){
      for(int j = 0; j<ht; j++){ 
         result.ary[i][j] = ary[i][j];
      }
   }

   for(int i = 0; i<(int)(wdth/50.0f); i++){
      for(int j = 0; j<ht; j++){
         result.ary[i][j] = {128, 0, 128};
      }
   }
   for(int i = (int)(wdth-wdth/50.0f); i<wdth; i++){
      for(int j = 0; j<ht; j++){
         result.ary[i][j] = {128, 0, 128};
      }
   }
   for(int i = 0; i<wdth; i++){
      for(int j = 0; j<(int)(wdth/50.0f); j++){
         result.ary[i][j] = {128, 0, 128};
      }
   }
   for(int i = 0; i<wdth; i++){
      for(int j = (int)(ht-wdth/50.0f); j<ht; j++){
         result.ary[i][j] = {128, 0, 128};
      }
   }

   return result;
}

ppm_image ppm_image::swirl() const{
   ppm_image result(wdth, ht);

   // create new array frame
   result.ary = new ppm_pixel*[wdth];
   for(int i = 0; i < wdth; ++i) {
      result.ary[i] = new ppm_pixel[ht];
   }
   
   for(int i = 0; i<wdth; i++){
      for(int j = 0; j<ht; j++){
         result.ary[i][j].r = ary[i][j].b;
         result.ary[i][j].g = ary[i][j].r;
         result.ary[i][j].b = ary[i][j].g;
         
          
      }
   }

   return result;
}

ppm_image ppm_image::invert() const{
   ppm_image result(wdth, ht);

   // create new array frame
   result.ary = new ppm_pixel*[wdth];
   for(int i = 0; i < wdth; ++i) {
      result.ary[i] = new ppm_pixel[ht];
   }

   for(int i = 0; i<wdth; i++){
      for(int j = 0; j<ht; j++){
         result.ary[i][j].r = 255-ary[i][j].r;
         result.ary[i][j].g = 255-ary[i][j].g;
         result.ary[i][j].b = 255-ary[i][j].b; 
      }
   } 

   return result;
}

ppm_image ppm_image::extract_red() const{
   ppm_image result(wdth, ht);

   // create new array frame
   result.ary = new ppm_pixel*[wdth];
   for(int i = 0; i < wdth; ++i) {
      result.ary[i] = new ppm_pixel[ht];
   }

   for(int i = 0; i<wdth; i++){
      for(int j = 0; j<ht; j++){
         result.ary[i][j].r = ary[i][j].r;
         result.ary[i][j].g = 0;
         result.ary[i][j].b = 0; 
      }
   } 

   return result;
}

ppm_image ppm_image::randomize() const{
   ppm_image result(wdth, ht);

   // create new array frame
   result.ary = new ppm_pixel*[wdth];
   for(int i = 0; i < wdth; ++i) {
      result.ary[i] = new ppm_pixel[ht];
   }

   for(int i = 0; i<wdth; i++){
      for(int j = 0; j<ht; j++){
         result.ary[i][j].r = rand()%255;
         result.ary[i][j].g = rand()%255;
         result.ary[i][j].b = rand()%255;
      }
   }  
    
   return result;
}

ppm_image ppm_image::box_blur() const{
   ppm_image result(wdth, ht);

   // create new array frame
   result.ary = new ppm_pixel*[wdth];
   for(int i = 0; i < wdth; ++i) {
      result.ary[i] = new ppm_pixel[ht];
   }

   for(int i = 1; i<wdth-1; i++){
      for(int j = 1; j<ht-1; j++){
         int mean_r = (int)round((ary[i][j].r + ary[i-1][j-1].r + 
            ary[i-1][j].r + ary[i][j-1].r +
            ary[i-1][j+1].r + ary[i+1][j-1].r +
            ary[i+1][j].r + ary[i][j+1].r +
            ary[i+1][j+1].r)/9.0f);
         int mean_g = (int)round((ary[i][j].g + ary[i-1][j-1].g + 
            ary[i-1][j].g + ary[i][j-1].g +
            ary[i-1][j+1].g + ary[i+1][j-1].g +
            ary[i+1][j].g + ary[i][j+1].g +
            ary[i+1][j+1].g)/9.0f);
         int mean_b = (int)round((ary[i][j].b + ary[i-1][j-1].b + 
            ary[i-1][j].b + ary[i][j-1].b +
            ary[i-1][j+1].b + ary[i+1][j-1].b +
            ary[i+1][j].b + ary[i][j+1].b +
            ary[i+1][j+1].b)/9.0f);
         result.ary[i][j].r = mean_r;
         result.ary[i][j].g = mean_g;
         result.ary[i][j].b = mean_b;
      }
   }  
    
   return result;
}