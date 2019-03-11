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


class Rock
{
public:
	
	vec3 position;  		// x_i
	
	
	GLfloat *vp; 	// array of vertex points
	GLfloat *vn; 	// array of vertex normals
	GLfloat *vt; 	// array of texture coordinates
	int g_point_count = 0;
	friend class Skeleton;

	void drawRock();

	Rock()
	{
		position.v[0] = 0;
		position.v[1] = 0;
		position.v[2] = 0;

		vp = NULL; 	
		vn = NULL; 	
		vt = NULL; 	
		g_point_count = 0;

	}

	bool load_rock()
	{
		string path = "objs/piedra.obj";

		if ( !load_obj_file( path.c_str(), vp, vt, vn, g_point_count ) ) 
		{
			gl_log_err( "ERROR: loading mesh file\n" );
			return false;
		}


		return true;
	}


};



class Rocks
{
public:
	int num_rocks;
	vector<Rock> rocks;
	vector<vec3> posRoks;
	
	GLuint *vao;
	GLuint shader_programme;
	int model_mat_location;
	int view_mat_location;
	int proj_mat_location;

	mat4 mat_model;

	Rocks(int num_rocks, int modo)
	{
		float moverX;
		float moverY;
		float moverZ = -0.5;
		
		if(modo==0)//ordenado
		{
			moverX = 0.5;
			moverY=0;
			for (int i = 0; i < num_rocks; ++i)
			{
				if (i%2==0 && i>1)
				{
					moverY += 0.5;
				}	

				if (i%2==0)
				{
					posRoks.push_back(vec3(-moverX, moverY-1, moverZ));
				}
				else
				{
					posRoks.push_back(vec3(moverX, moverY-1, moverZ));
				}					
			}
		}

		else if(modo==1)//aleatorio
		{
			moverX = 0.5;
			moverY=0;
			for (int i = 0; i < num_rocks; ++i)
			{
				if (i%2==0 && i>1)
				{
					moverY += 0.6;
				}	

				if (i%2==0)
				{
					posRoks.push_back(vec3(-moverX + (rand()%(100)/100.0) , moverY-1, moverZ));
				}
				else
				{
					posRoks.push_back(vec3(moverX, moverY-1 +(rand()%(100)/100.0), moverZ));
				}					
			}
		}

		else if(modo==2)//fijo
		{
			posRoks.push_back(vec3(-0.5, -1, moverZ));
			posRoks.push_back(vec3(0.5, -1, moverZ));
			posRoks.push_back(vec3(0.3, -0.5, moverZ));
			posRoks.push_back(vec3(-0.4, -0.5, moverZ));
			posRoks.push_back(vec3(-0.5, -0.8, moverZ));

			posRoks.push_back(vec3(-0.4, 0.5, moverZ));
			posRoks.push_back(vec3(0.5, 0.7, moverZ));
			posRoks.push_back(vec3(-0.5, 1, moverZ));
			posRoks.push_back(vec3(-0.55, 1.34, moverZ));
			posRoks.push_back(vec3(0.45, 1.7, moverZ));

			posRoks.push_back(vec3(1.5, 1.25, moverZ));
			posRoks.push_back(vec3(0.63, 1.42, moverZ));
			posRoks.push_back(vec3(1.75, 2, moverZ));
			posRoks.push_back(vec3(2.4, 2.5, moverZ));
			posRoks.push_back(vec3(1.9, 3, moverZ));

			posRoks.push_back(vec3(0.9, 2.15, moverZ));
			posRoks.push_back(vec3(0.2, 2.95, moverZ));
			posRoks.push_back(vec3(0.55, 2.7, moverZ));
			posRoks.push_back(vec3(1.25, 3.2, moverZ));
			posRoks.push_back(vec3(0.5, 0.2, moverZ));

			posRoks.push_back(vec3(0.7, 3.6, moverZ));
			posRoks.push_back(vec3(0, 4.6, moverZ));
								
			num_rocks = 22;
		}



		this->num_rocks = num_rocks;
		vao = new GLuint[num_rocks];


		glGenVertexArrays( num_rocks, vao ); //BE CAREFUL IT SHOULD BE &vao
	}


	bool read_rocksObj()
	{
		for (int i = 0; i < num_rocks; ++i)
		{
			Rock rock;
			bool found = rock.load_rock();
			

			if(found)
			{
				rock.position=posRoks[i];
				rocks.push_back(rock);
			}

			else
			{
				cout<<"Not Exist Bone in the position: "<< i << endl;
				return false;
			}
		}

		return true;


	}
	
	void init_rocks()
	{

		for (int i = 0; i < num_rocks; ++i)
		{
			glBindVertexArray(vao[i]);

			GLuint points_vbo;
			if ( NULL != rocks[i].vp ) 
			{
				glGenBuffers( 1, &points_vbo );
				glBindBuffer( GL_ARRAY_BUFFER, points_vbo );
				glBufferData( GL_ARRAY_BUFFER, 3 * rocks[i].g_point_count * sizeof( GLfloat ), rocks[i].vp,
											GL_STATIC_DRAW );
				glVertexAttribPointer( 0, 3, GL_FLOAT, GL_FALSE, 0, NULL );
				glEnableVertexAttribArray( 0 );
			}

			GLuint normal_vbo;
			if ( NULL != rocks[i].vn ) 
			{
				glGenBuffers( 1, &normal_vbo );
				glBindBuffer( GL_ARRAY_BUFFER, normal_vbo );
				glBufferData( GL_ARRAY_BUFFER, 3 * rocks[i].g_point_count * sizeof( GLfloat ), rocks[i].vn,
											GL_STATIC_DRAW );
				glVertexAttribPointer( 1, 3, GL_FLOAT, GL_FALSE, 0, NULL );
				glEnableVertexAttribArray( 1 );
			}
		}
	}

	void init_programe(const char * vertex, const char * fragment)
	{
		shader_programme = create_programme_from_files( vertex, fragment );
		model_mat_location = glGetUniformLocation( shader_programme, "model" );
		view_mat_location = glGetUniformLocation( shader_programme, "view" );
		proj_mat_location = glGetUniformLocation( shader_programme, "proj" );
		
	}

	void setMatrixs(mat4 view_mat, mat4 proj_mat, mat4 mat_model)
	{
		this->mat_model = mat_model;
		glUseProgram( shader_programme );
		glUniformMatrix4fv( view_mat_location, 1, GL_FALSE, view_mat.m );
		glUniformMatrix4fv( proj_mat_location, 1, GL_FALSE, proj_mat.m );
		glUniformMatrix4fv( model_mat_location, 1, GL_FALSE, mat_model.m );
	}
	void draw(mat4 mat_rotate)
	{
		mat4 translate_rock;
		glUseProgram( shader_programme);
		for (int i = 0; i < num_rocks; ++i)
		{
			translate_rock = translate   ( identity_mat4(), rocks[i].position);
			glUniformMatrix4fv( model_mat_location, 1, GL_FALSE, (mat_rotate*translate_rock*mat_model).m );
			glBindVertexArray(vao[i]);		
			glDrawArrays( GL_TRIANGLES, 0, rocks[i].g_point_count );
		}
		
	}
};


