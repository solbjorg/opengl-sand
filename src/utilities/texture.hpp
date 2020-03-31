#ifndef __TEXTURE_H_
#define __TEXTURE_H_

#include "utilities/imageLoader.hpp"
#include <glad/glad.h>

unsigned getTexture(PNGImage image, bool repeating, bool mipmapped = true);

#endif // __TEXTURE_H_
