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

class Floor
{
public:

	GLfloat *vp = NULL; // array of vertex points
	GLfloat *vn = NULL; // array of vertex normals
	GLfloat *vt = NULL; // array of texture coordinates
	int g_point_count;


	GLuint vao;
	GLuint shader_programme;
	int model_mat_location;
	int view_mat_location;
	int proj_mat_location;

	Floor()
	{
		g_point_count = 0;
	}


	bool read_floorObj()
	{
		string path = "objs/piso.obj";

		if ( !load_obj_file( path.c_str(), vp, vt, vn, g_point_count ) ) 
		{
			gl_log_err( "ERROR: loading mesh floor\n" );
			return false;
		}
		return true;
	}

	void init_floor()
	{
		GLfloat texcoords[] = { 0.0f, 0.0f, 1.0f, 1.0f, 0.0f, 1.0f, 
						0.0f, 0.0f, 1.0f, 0.0f, 1.0f, 1.0f};


		glGenVertexArrays( 1, &vao );
		glBindVertexArray( vao );

		GLuint points_vbo;
		GLuint texcoords_vbo;
		if ( NULL != vp ) 
		{
			glGenBuffers( 1, &points_vbo );
			glBindBuffer( GL_ARRAY_BUFFER, points_vbo );
			glBufferData( GL_ARRAY_BUFFER, 3 * g_point_count * sizeof( GLfloat ), vp,
										GL_STATIC_DRAW );
			glVertexAttribPointer( 0, 3, GL_FLOAT, GL_FALSE, 0, NULL );
			glEnableVertexAttribArray( 0 );





			glGenBuffers( 1, &texcoords_vbo );
			glBindBuffer( GL_ARRAY_BUFFER, texcoords_vbo );
			glBufferData( GL_ARRAY_BUFFER, 12 * sizeof( GLfloat ), texcoords,
										GL_STATIC_DRAW );
			glVertexAttribPointer( 1, 2, GL_FLOAT, GL_FALSE, 0, NULL ); // normalise!
			glEnableVertexAttribArray( 1 );
		}

		GLuint normal_vbo;
		if ( NULL != vn ) 
		{
			glGenBuffers( 1, &normal_vbo );
			glBindBuffer( GL_ARRAY_BUFFER, normal_vbo );
			glBufferData( GL_ARRAY_BUFFER, 3 * g_point_count * sizeof( GLfloat ), vn,
												GL_STATIC_DRAW );
			glVertexAttribPointer( 2, 3, GL_FLOAT, GL_FALSE, 0, NULL );
			glEnableVertexAttribArray( 2 );
		}
	}

	bool load_texture( const char *file_name, GLuint *tex ) 
	{
		int x, y, n;
		int force_channels = 4;
		// the following function call flips the image
		// needs to be called before each stbi_load(...);
		stbi_set_flip_vertically_on_load(true);
		unsigned char *image_data = stbi_load( file_name, &x, &y, &n, force_channels );
		if ( !image_data ) 
		{
			fprintf( stderr, "ERROR: could not load %s\n", file_name );
			return false;
		}
		// NPOT check
		if ( ( x & ( x - 1 ) ) != 0 || ( y & ( y - 1 ) ) != 0 ) 
		{
			fprintf( stderr, "WARNING: texture %s is not power-of-2 dimensions\n",
							 file_name );
		}

		glGenTextures( 1, tex );
		glActiveTexture( GL_TEXTURE0 );
		glBindTexture( GL_TEXTURE_2D, *tex );
		glTexImage2D( GL_TEXTURE_2D, 0, GL_RGBA, x, y, 0, GL_RGBA, GL_UNSIGNED_BYTE,
									image_data );
		glGenerateMipmap( GL_TEXTURE_2D );
		glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE );
		glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE );
		glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR );
		glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR );
		GLfloat max_aniso = 0.0f;
		glGetFloatv( GL_MAX_TEXTURE_MAX_ANISOTROPY_EXT, &max_aniso );
		// set the maximum!
		glTexParameterf( GL_TEXTURE_2D, GL_TEXTURE_MAX_ANISOTROPY_EXT, max_aniso );


		

		return true;
	}

	void init_programe(const char * vertex, const char * fragment)
	{
		shader_programme = create_programme_from_files( vertex, fragment );
		model_mat_location = glGetUniformLocation( shader_programme, "model" );
		view_mat_location = glGetUniformLocation( shader_programme, "view" );
		proj_mat_location = glGetUniformLocation( shader_programme, "proj" );
		// load texture
		//glUseProgram( shader_programme );
		GLuint tex;
		( load_texture( "img/pisoTextura3.png", &tex ) );
	}

	void setMatrixs(mat4 view_mat, mat4 proj_mat, mat4 mat_model)
	{
		glUseProgram( shader_programme );
		glUniformMatrix4fv( view_mat_location, 1, GL_FALSE, view_mat.m );
		glUniformMatrix4fv( proj_mat_location, 1, GL_FALSE, proj_mat.m );
		glUniformMatrix4fv( model_mat_location, 1, GL_FALSE, mat_model.m );
	}
	void draw()
	{
		glUseProgram( shader_programme);
		glBindVertexArray(vao);		
		glDrawArrays( GL_TRIANGLES, 0, g_point_count );
	}
};