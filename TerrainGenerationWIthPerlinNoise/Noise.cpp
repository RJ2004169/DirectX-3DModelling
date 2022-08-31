#include "pch.h"
#include "Noise.h"

Noise::Noise() {
    for (int i = 0; i < 512; i++) {
        perm[i] = p[i & 255];
    }
}

Noise::~Noise() {
}


float Noise::generate_noise(float x, float y, float z) {
    int X = fastfloor(x);
    int Y = fastfloor(y);
    int Z = fastfloor(z);

    x = x - X;
    y = y - Y;
    z = z - Z;

    X = X & 255;
    Y = Y & 255;
    Z = Z & 255;

    int gi000 = perm[X + perm[Y + perm[Z]]] % 12;
    int gi001 = perm[X + perm[Y + perm[Z + 1]]] % 12;
    int gi010 = perm[X + perm[Y + 1 + perm[Z]]] % 12;
    int gi011 = perm[X + perm[Y + 1 + perm[Z + 1]]] % 12;
    int gi100 = perm[X + 1 + perm[Y + perm[Z]]] % 12;
    int gi101 = perm[X + 1 + perm[Y + perm[Z + 1]]] % 12;
    int gi110 = perm[X + 1 + perm[Y + 1 + perm[Z]]] % 12;
    int gi111 = perm[X + 1 + perm[Y + 1 + perm[Z + 1]]] % 12;

    float n000 = dot(grad3[gi000], x, y, z);
    float n100 = dot(grad3[gi100], x - 1, y, z);
    float n010 = dot(grad3[gi010], x, y - 1, z);
    float n110 = dot(grad3[gi110], x - 1, y - 1, z);
    float n001 = dot(grad3[gi001], x, y, z - 1);
    float n101 = dot(grad3[gi101], x - 1, y, z - 1);
    float n011 = dot(grad3[gi011], x, y - 1, z - 1);
    float n111 = dot(grad3[gi111], x - 1, y - 1, z - 1);

    float u = fade(x);
    float v = fade(y);
    float w = fade(z);
    // Interpolate along x the contributions from each of the corners
    float nx00 = mix(n000, n100, u);
    float nx01 = mix(n001, n101, u);
    float nx10 = mix(n010, n110, u);
    float nx11 = mix(n011, n111, u);
    // Interpolate the four results along y
    float nxy0 = mix(nx00, nx10, v);
    float nxy1 = mix(nx01, nx11, v);
    // Interpolate the two last results along z
    float nxyz = mix(nxy0, nxy1, w);

    return nxyz;
}

int Noise::fastfloor(float x) {
    return x > 0 ? (int)x : (int)-1;
}

float Noise::dot(int g[3], float x, float y, float z) {
    return (g[0] * x) + (g[1] * y) + (g[2] * z);
}

float Noise::mix(float a, float b, float t) {
    return ((1 - t) * a) + (t * b);
}

float Noise::fade(float t) {
    return t * t * t * (t * ((t * 6) - 15) + 10);
}