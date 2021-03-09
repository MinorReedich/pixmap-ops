#include <iostream>
#include "ppm_image.h"
using namespace std;
using namespace agl;

int main(int argc, char** argv)
{
    ppm_image soup;
    soup.load("../images/soup-ascii.ppm");

    ppm_image earth;
    earth.load("../images/earth-ascii.ppm");

    // TESTS 
    /*
    // border
    ppm_image border = soup.border();
    border.save("soup-border.ppm");

    // swirl colors
    ppm_image swirl = soup.swirl();
    swirl.save("soup-swirl.ppm");

    // invert colors
    ppm_image invert = soup.invert();
    invert.save("soup-invert.ppm");

    // extract red
    ppm_image red = soup.extract_red();
    red.save("soup-red.ppm");

    // randomize colors
    ppm_image random = soup.randomize();
    random.save("soup-random.ppm"); 
 
    // box blur
    ppm_image blur = soup.box_blur();
    blur.save("soup-boxblur.ppm");
    */
    
    ppm_image art1 = soup.invert().extract_red().swirl().border();
    art1.save("../images/art1.ppm");

    ppm_image art2 = earth.invert().extract_red().swirl().swirl().gammaCorrect(0.6);
    art2.save("../images/art2.ppm");

    ppm_image art3 = earth.randomize().box_blur().border().box_blur().box_blur();
    art3.save("../images/art3.ppm");

    //these art pieces are in the images folder
}

