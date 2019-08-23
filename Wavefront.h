#ifndef WAVEFRONT_H
#define WAVEFRONT_H

#include "cstdlib"
#include "TiposClasses.h"

void calculaWavefront(int objx, int objy, int objangulo, bool ignoraAdversario, std::vector<Estado> estado, int mapa[170][130]);

#endif /* WAVEFRONT_H */