#ifndef __TEXTURE_H_
#define __TEXTURE_H_

#include <glad/glad.h>
#include "utilities/imageLoader.hpp"

unsigned getTexture(PNGImage image, bool repeating);

#endif // __TEXTURE_H_
