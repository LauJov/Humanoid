#include "maths_funcs.h" // my maths functions
#include "obj_parser.h"	// my little Wavefront .obj mesh loader
#include <GL/glew.h>		 // include GLEW and new version of GL on Windows
#include <GLFW/glfw3.h>	// GLFW helper library
#include <assert.h>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <iostream>
#include <vector>

using namespace std;


vec2 buscarPiedraCercana(vector<vec3> posBones, vector<vec3> posRoks)
{
	float distancia =100000;
	float distanciaAux;
	vec2 posMover(-1, -1);
	for (uint i = 0; i < posBones.size(); ++i)
	{
		if (i==5 || i==8 || i==11 || i==14 )
		{
			/* code */
		
			for (uint j = 0; j < posRoks.size(); ++j)
			{
				distanciaAux = sqrt(
									pow(posBones[i].v[0]-posRoks[j].v[0], 2) + 
									pow(posBones[i].v[1]-posRoks[j].v[1], 2) + 
									pow(posBones[i].v[2]-posRoks[j].v[2], 2));
				if (  distanciaAux <  distancia && posBones[i].v[1]<posRoks[j].v[1])
				{
					distancia = distanciaAux;
					posMover.v[0] = i;
					posMover.v[1] = j;
				}
			}
		}
	}
	return posMover;
}