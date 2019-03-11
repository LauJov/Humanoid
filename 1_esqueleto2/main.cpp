//make && ./humanoid
/******************************************************************************\
| OpenGL 4 Example Code.                                                       |
| Accompanies written series "Anton's OpenGL 4 Tutorials"                      |
| Email: anton at antongerdelan dot net                                        |
| First version 27 Jan 2014                                                    |
| Copyright Dr Anton Gerdelan, Trinity College Dublin, Ireland.                |
| See individual libraries separate legal notices                              |
|******************************************************************************|                                             |
\******************************************************************************/
#include "gl_utils.h"		 // common opengl functions and small utilities like logs
#include "maths_funcs.h" // my maths functions
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"	// Sean Barrett's image loader - http://nothings.org/
#include "obj_parser.h"	// my little Wavefront .obj mesh loader
#include <GL/glew.h>		 // include GLEW and new version of GL on Windows
#include <GLFW/glfw3.h>	// GLFW helper library
#include <assert.h>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <iostream>
#include<time.h>
#include "humanoid.cpp"
#include "floor.cpp"
#include "wall.cpp"
#include "rocks.cpp"
#include "funciones.cpp"
using namespace std;

#define ONE_DEG_IN_RAD ( 2.0 * M_PI ) / 360.0 // 0.017444444
#define VERTEX_SHADER_FILE "shaders/test_vs.glsl"
#define FRAGMENT_SHADER_FILE "shaders/test_fs.glsl"
#define NUM_BONES  15
#define PI 3.141592653589793
#define LH_POS 5
#define RH_POS 8
#define LF_POS 11
#define RF_POS 14

#define MODO 1 //0=ordenado, 1=aleatorio, 2=fijo

// camera matrices. it's easier if they are global
mat4 view_mat;
mat4 proj_mat;
vec3 cam_pos( 0.0f, 0.0f, 8.0f );


int find_rock_to_hand(Rocks rocks, Bone bone, float & distance)
{
		float distance_aux = 0;
		int pos = -1;

		vec3 end_bone = bone.start_end[1];

		for(uint i=0; i <rocks.posRoks.size(); i++)
		{
			distance_aux = sqrt( pow( (rocks.posRoks[i].v[0] - end_bone.v[0]) ,2) +
								 pow( (rocks.posRoks[i].v[1] - end_bone.v[1]) ,2) +
								 pow( (rocks.posRoks[i].v[2] - end_bone.v[2]) ,2));
			
			
			{
				if(distance_aux < distance && rocks.posRoks[i].v[1]-0.25> end_bone.v[1])
				{
					distance = distance_aux;
					pos = i;
				}
			}
		}

		return pos;
}




int main() 
{
	srand(time(NULL));

	restart_gl_log();
	// start GL context and O/S window using the GLFW helper library
	start_gl();
	// set a function to be called when the mouse is clicked

	Skeleton skeleton(NUM_BONES);
	if(!skeleton.read_boneObj())
	{
		cout<<"Not read the bone objects in the correct way"<<endl;
		return -1;
	}
	skeleton.init_skeleton();
	


	// =============== HUMANOID SHADER =====================
	GLuint shader_programme =
		create_programme_from_files( VERTEX_SHADER_FILE, FRAGMENT_SHADER_FILE );
	int model_mat_location = glGetUniformLocation( shader_programme, "model" );
	int view_mat_location = glGetUniformLocation( shader_programme, "view" );
	int proj_mat_location = glGetUniformLocation( shader_programme, "proj" );


	// =============== END HUMANOID SHADER =====================	


	// ================ FLOOR SETTINGS ==========================	
	Floor floor;
	if(!floor.read_floorObj())
	{
		cout<<"Not read the bone objects in the correct way"<<endl;
		return -1;
	}

	floor.init_floor();
	floor.init_programe("shaders/vs_floor.glsl", "shaders/fs_floor.glsl");
	
	// ================ END FLOOR SETTINGS ==========================



	// ===================== WALL SETTINGS ==========================	
	Wall wall;
	if(!wall.read_wallObj())
	{
		cout<<"Not read the bone objects in the correct way"<<endl;
		return -1;
	}

	wall.init_wall();
	wall.init_programe("shaders/vs_wall.glsl", "shaders/fs_wall.glsl");
	
	// ================== END WALL SETTINGS ===========================

	// ===================== ROCKS SETTINGS ==========================	
	Rocks rocks(20, MODO);
	if(!rocks.read_rocksObj())
	{
		cout<<"Not read the bone objects in the correct way"<<endl;
		return -1;
	}

	rocks.init_rocks();
	rocks.init_programe("shaders/vs_rock.glsl", "shaders/fs_rock.glsl");
	
	// ================== END ROCKS SETTINGS ===========================
	



/*-------------------------------CREATE CAMERA--------------------------------*/

	// input variables
	float near = 0.1f;																		 // clipping plane
	float far = 100.0f;																		 // clipping plane
	float fovy = 67.0f;	// Angulo de apertura																	 // 67 degrees
	float aspect = (float)g_gl_width / (float)g_gl_height; // aspect ratio
	proj_mat = perspective( fovy, aspect, near, far );

	float cam_speed = 3.0f;					 // 1 unit per second
	float cam_heading = 0.0f;				 // y-rotation in degrees
	mat4 T = translate( identity_mat4(), vec3( -cam_pos.v[0], -cam_pos.v[1], -cam_pos.v[2] ) );
	mat4 R = rotate_y_deg( identity_mat4(), -cam_heading );
	versor q = quat_from_axis_deg( -cam_heading, 0.0f, 1.0f, 0.0f );
	view_mat = R * T;
	// keep track of some useful vectors that can be used for keyboard movement
	vec4 fwd( 0.0f, 0.0f, -1.0f, 0.0f );
	vec4 rgt( 1.0f, 0.0f, 0.0f, 0.0f );
	vec4 up( 0.0f, 1.0f, 0.0f, 0.0f );

	/*---------------------------SET RENDERING
	 * DEFAULTS---------------------------*/
	mat4 mat_rotarArreglar = rotate_y_deg( identity_mat4(), -90 );
	mat4 mat_scale_wall = scale(identity_mat4(), vec3(1.2, 1.57, 1));
	mat4 mat_scale_floor = scale(identity_mat4(), vec3(1.2, 1, 1.2));

	// ==================== FLOOR =================================
	floor.setMatrixs(view_mat, proj_mat, mat_scale_floor*mat_rotarArreglar);

	// ================= END FLOOR =================================

	// ==================== WALL =================================
	mat4 mat_traslate_wall = translate(identity_mat4(), vec3(0, 0, -1));
	wall.setMatrixs(view_mat, proj_mat, mat_traslate_wall * mat_scale_wall*mat_rotarArreglar);

	// ================= END WALL =================================

	// ==================== ROCKS =================================
	rocks.setMatrixs(view_mat, proj_mat, mat_rotarArreglar);

	// ================= END ROCKS =================================



	glUseProgram( shader_programme );
	glUniformMatrix4fv( view_mat_location, 1, GL_FALSE, view_mat.m );
	glUniformMatrix4fv( proj_mat_location, 1, GL_FALSE, proj_mat.m );



	
	
	glEnable( GL_DEPTH_TEST ); // enable depth-testing
	glDepthFunc( GL_LESS );		 // depth-testing interprets a smaller value as "closer"
	glEnable( GL_CULL_FACE );	// cull face
	glCullFace( GL_BACK );		 // cull back face
	glFrontFace( GL_CCW ); // set counter-clock-wise vertex order to mean the front
	glClearColor( 0.2, 0.2, 0.2, 1.0 ); // grey background to help spot mistakes
	glViewport( 0, 0, g_gl_width, g_gl_height );

	


	/*-------------------------------RENDERING LOOP-------------------------------*/
	float angle_scene_rotate = 0;
	mat4 rotate_scene = rotate_y_deg( identity_mat4(), angle_scene_rotate );


	/*--------------------------- FINDING ROCKS TO REACH-------------------------- */
	
	vec3 target, origen;
	vec3 pre_target;
	bool find_target = false;
	bool moveX = false, moveY = false, moveZ = false;

	float minDis = 1000000;
	Bone *currentBone;
	int posBoneAc = 0;
	int posRock = 0;

	int posBones[4] ={LH_POS, RH_POS, LF_POS, RF_POS};
	


	while ( !glfwWindowShouldClose( g_window ) ) 
	{
		// update timers
		static double previous_seconds = glfwGetTime();
		double current_seconds = glfwGetTime();
		double elapsed_seconds = current_seconds - previous_seconds;
		previous_seconds = current_seconds;
		_update_fps_counter( g_window );

		// wipe the drawing surface clear
		glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );

		
		//================== DRAWING START FLOOR =====================
		floor.draw();
		//================== DRAWING END FLOOR =======================

		//================== DRAWING START FLOOR =====================
		wall.draw();
		//================== DRAWING END FLOOR =======================

		//================== DRAWING START ROCKS =====================
		rocks.draw(rotate_scene);
		//================== DRAWING END ROCKS =======================

		
		//=========== START HUMANOID =====================

		if(find_target && moveX==false && moveY==false && moveZ==false)
		{	

			minDis = 1000000;
			
			if (posBoneAc>=4)
			{
				posBoneAc = 0;
			}




			int rock_pos = find_rock_to_hand(rocks, skeleton.bones[posBones[posBoneAc]], minDis);
			if (rock_pos!=-1)
			{
				posRock = rock_pos;
				//cout<<"posRock: "<<posRock<<"; Bone: "<<posBones[posBoneAc]<<endl;
				currentBone = &skeleton.bones[posBones[posBoneAc]];
				posBoneAc++;


				moveX = true;
				moveY = true;
				moveZ = true;
				origen = vec3(currentBone->start_end[1].v[0], currentBone->start_end[1].v[1], currentBone->start_end[1].v[2]);
				target = vec3(rocks.posRoks[posRock]);	
				pre_target = currentBone->start_end[1];	
				find_target = false;

				cout<<"Buscar"<<endl;
			}
			else
			{
				cout<<"============= FIN ==========="<<endl;
				find_target = false;
			}

			
		}

		if(moveX || moveY || moveZ)
		{

			if (moveX)
			{
				pre_target.v[0] = target.v[0]-origen.v[0];
				pre_target.v[0] *= elapsed_seconds*2;
				pre_target.v[0] += currentBone->start_end[1].v[0];
			}

			if (moveY)
			{
				pre_target.v[1] = target.v[1]-origen.v[1];
				pre_target.v[1] *= elapsed_seconds*2;
				pre_target.v[1] += currentBone->start_end[1].v[1];
			}

			if (moveZ)
			{
				pre_target.v[2] = target.v[2]-origen.v[2];
				pre_target.v[2] *= elapsed_seconds*2;
				pre_target.v[2] += currentBone->start_end[1].v[2];
			}

			currentBone->computeMotion(pre_target, currentBone->id);

			if( abs(target.v[0] - pre_target.v[0]) <0.05)
			{
				moveX = false;
			}

			if( abs(target.v[1] - pre_target.v[1]) <0.05)
			{
				moveY = false;
			}

			if( abs(target.v[2] - pre_target.v[2]) <0.05)
			{
				moveZ = false;
			}
			if (moveX==false && moveY==false && moveZ==false)
			{
				cout<<"Llego"<<endl;
				find_target = true;
			}
			/*
			moveX = false;
			moveY = false;
			moveZ = false;*/
		}

		glUseProgram( shader_programme );


		for (int i = 0; i < skeleton.num_bones; ++i)
		{
			glUniformMatrix4fv( model_mat_location, 1, GL_FALSE, (rotate_scene*skeleton.bones[i].mat_transformation*mat_rotarArreglar).m );

			glBindVertexArray( skeleton.vao[i] );		
			glDrawArrays( GL_TRIANGLES, 0, skeleton.bones[i].g_point_count );
		}



		//=========== END HUMANOID =====================

		
		// update other events like input handling
		glfwPollEvents();



		bool scena_moved = false;

		if ( glfwGetKey( g_window, GLFW_KEY_A ) ) {
			angle_scene_rotate -= 60 * elapsed_seconds;
			scena_moved = true;
		}
		if ( glfwGetKey( g_window, GLFW_KEY_D ) ) {
			angle_scene_rotate += 60 * elapsed_seconds;
			scena_moved = true;
		}

		if ( glfwGetKey( g_window, GLFW_KEY_T ) ) 
		{
			find_target = true;
		}

		if (scena_moved)
		{
			rotate_scene = rotate_y_deg( identity_mat4(), angle_scene_rotate );
		}




		// control keys
		bool cam_moved = false;
		vec3 move( 0.0, 0.0, 0.0 );




		if ( glfwGetKey( g_window, GLFW_KEY_W ) ) {
			move.v[2] -= cam_speed * elapsed_seconds;
			cam_moved = true;
		}
		if ( glfwGetKey( g_window, GLFW_KEY_S ) ) {
			move.v[2] += cam_speed * elapsed_seconds;
			cam_moved = true;
		}

		// update view matrix
		if ( cam_moved ) {
			// re-calculate local axes so can move fwd in dir cam is pointing
			R = quat_to_mat4( q );
			fwd = R * vec4( 0.0, 0.0, -1.0, 0.0 );
			rgt = R * vec4( 1.0, 0.0, 0.0, 0.0 );
			up = R * vec4( 0.0, 1.0, 0.0, 0.0 );

			cam_pos = cam_pos + vec3( fwd ) * -move.v[2];
			cam_pos = cam_pos + vec3( up ) * move.v[1];
			cam_pos = cam_pos + vec3( rgt ) * move.v[0];
			mat4 T = translate( identity_mat4(), vec3( cam_pos ) );

			view_mat = inverse( R ) * inverse( T );
			glUniformMatrix4fv( view_mat_location, 1, GL_FALSE, view_mat.m );
		}

		if ( GLFW_PRESS == glfwGetKey( g_window, GLFW_KEY_ESCAPE ) ) {
			glfwSetWindowShouldClose( g_window, 1 );
		}
		// put the stuff we've been drawing onto the display
		glfwSwapBuffers( g_window );
	}

	// close GL context and any other GLFW resources
	glfwTerminate();
	return 0;
}
