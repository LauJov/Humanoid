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

#define PI 3.141592653589793

float getAngle(vec3 *target, vec3 *start_bone, int e1, int e2)
	{
		// Calculando la dirección y el ángulo
		vec3 direction = vec3(target->v[0] - start_bone->v[0], 
							  target->v[1] - start_bone->v[1], 
							  target->v[2] - start_bone->v[2]);
		float angle = 0;

		if(abs(start_bone->v[e1] - target->v[e1]) == 0)
		{
			if (start_bone->v[e2] <= target->v[e2])
				angle = 90;
			else
				angle = -90;		
		}
		else if(start_bone->v[e2] == target->v[e2])
		{
			if (start_bone->v[e1] < target->v[e1])
				angle = 0;
			else
				angle = 180;		
		}
		else
		{
			angle = atan(direction.v[e2]/direction.v[e1]);
			angle = angle*180/PI;

			if (start_bone->v[e1] > target->v[e1] && start_bone->v[e2] < target->v[e2])
			{
				angle += 180;
			}

			else if(start_bone->v[e1] > target->v[e1] && start_bone->v[e2] > target->v[e2])
			{
				angle += 180;
			}

			else if(start_bone->v[e1] < target->v[e1] && start_bone->v[e2] > target->v[e2])
			{
				angle += 360;
			}
		}
		return angle;
	}

class Bone
{
public:
	string name;
	//vec3 start_bone;	
	//vec3 end_bone;
	vector<vec3> start_end;	
	float angle_bone_z;
	float angle_bone_x;
	vector<Bone*>fathers;
	vector<Bone*>children;

	
	GLfloat *vp; 	// array of vertex points
	GLfloat *vn; 	// array of vertex normals
	GLfloat *vt; 	// array of texture coordinates
	int g_point_count = 0;

	mat4 mat_transformation;
	int id;

	friend class Skeleton;

	void drawBone();

	Bone()
	{
		name = "";

		this->vp = NULL; 	
		this->vn = NULL; 	
		this->vt = NULL; 	
		this->g_point_count = 0;

		mat_transformation = identity_mat4();

	}

	void setBone(vec3 start_bone, vec3 end_bone)
	{
		start_end.push_back(start_bone);
		start_end.push_back(end_bone);

		id = -1;

		angle_bone_z = 0;
		angle_bone_x = 0;


	}

	bool load_bone(string name)
	{
		this->name  = name;
		string path = "objs/" + name + ".obj";

		if ( !load_obj_file( path.c_str(), vp, vt, vn, g_point_count ) ) 
		{
			gl_log_err( "ERROR: loading mesh file\n" );
			return false;
		}  


		return true;
	}



	void computeMotion(vec3 target, int id)
	{
		vec3 *start_bone;
		vec3 *end_bone;
		uint pos_start;

		if(this->id == id)
		{
			start_bone = &start_end[0];
			end_bone = &start_end[1];
			pos_start = 0;
		}
		
		else
		{
			if (this->id==1)//condiciones para el pecho
			{
				if(id==3)//Origen mano Iz
				{
					start_bone = &start_end[2];
					end_bone = &start_end[0];
					pos_start = 2;
				}


				else if(id == 4)//Origen mano Der
				{
					start_bone = &start_end[2];
					end_bone = &start_end[1];
					pos_start = 2;
				}

				else if(id == 5)//Origen pie izq
				{
					start_bone = &start_end[0];
					end_bone = &start_end[2];
					pos_start = 0;
				}

				else if(id == 6)//Origen pie Der
				{
					start_bone = &start_end[1];
					end_bone = &start_end[2];
					pos_start = 1;
				}

			}


			else if (this->id==2)//condiciones para la pelvis
			{
				if(id==3)//Orige mano Iz
				{
					start_bone = &start_end[1];
					end_bone = &start_end[0];
					pos_start = 1;
				}

				else if(id == 4)//Origen mano Der
				{
					start_bone = &start_end[2];
					end_bone = &start_end[0];
					pos_start = 2;
				}

				else if(id == 5)//Origen pie izq
				{
					start_bone = &start_end[0];
					end_bone = &start_end[1];
					pos_start = 0;
				}

				else if(id == 6)//Origen pie der
				{
					start_bone = &start_end[0];
					end_bone = &start_end[2];
					pos_start = 0;
				}
			}
			
			else
			{
				start_bone = &start_end[1];
				end_bone = &start_end[0];
				pos_start = 1;
			}
		}


		mat4 mat_rotate_point_z = identity_mat4();
		float angle_z = 0;

		if(this->id != 0)
		{
			angle_bone_z = getAngle(end_bone, start_bone, 0, 1);
			angle_z = getAngle(&target, start_bone, 0, 1);
			angle_z = angle_z - angle_bone_z;
		}

		else 
		{
			angle_z = this->fathers[0]->angle_bone_z;
		}
		
		angle_bone_z = angle_z;

		//Rotate
		mat4 T1P = translate( identity_mat4(), (*start_bone)*(-1) );
		mat_rotate_point_z = rotate_z_deg( identity_mat4(), (angle_z) );
		mat4 T2P = translate( identity_mat4(), *start_bone );

		mat_rotate_point_z = T2P * mat_rotate_point_z * T1P;
			
		for(uint i=0; i<start_end.size(); i++)
		{
			if (i!=pos_start)
			{
				vec4 newEnd = mat_rotate_point_z*vec4(start_end[i], 1);
				start_end[i].v[0] = newEnd.v[0];
				start_end[i].v[1] = newEnd.v[1];
				start_end[i].v[2] = newEnd.v[2];
			}
			
		}



		mat4 mat_rotate_point_x = identity_mat4();
		float angle_x = 0;

		if(this->id != 0)
		{
			angle_bone_x = getAngle(end_bone, start_bone, 1, 2);
			angle_x = getAngle(&target, start_bone, 1, 2);
			angle_x = angle_x - angle_bone_x;
		}

		else 
		{
			angle_x = this->fathers[0]->angle_bone_x;
		}
		
		angle_bone_x = angle_x;

		//Rotate
		T1P = translate( identity_mat4(), (*start_bone)*(-1) );
		mat_rotate_point_x = rotate_x_deg( identity_mat4(), (angle_x) );
		T2P = translate( identity_mat4(), *start_bone );

		mat_rotate_point_x = T2P * mat_rotate_point_x * T1P;
			
		for(uint i=0; i<start_end.size(); i++)
		{
			if (i!=pos_start)
			{
				vec4 newEnd = mat_rotate_point_x*vec4(start_end[i], 1);
				start_end[i].v[0] = newEnd.v[0];
				start_end[i].v[1] = newEnd.v[1];
				start_end[i].v[2] = newEnd.v[2];
			}
			
		}
		

		//Traslate
		vec3 translate_value(target.v[0] - end_bone->v[0], 
								   target.v[1] - end_bone->v[1], 
								   target.v[2] - end_bone->v[2]);

		
		mat4 translate_dir = translate(identity_mat4(), translate_value);


		for (uint i = 0; i < start_end.size(); ++i)
		{
			vec4 newStart = translate_dir*vec4(start_end[i], 1);
			start_end[i].v[0] = newStart.v[0];
			start_end[i].v[1] = newStart.v[1];
			start_end[i].v[2] = newStart.v[2];
		}

		//mat_transformation = mat_rotate_point_x * mat_rotate_point_z * mat_transformation;
		mat_transformation = translate_dir * mat_rotate_point_x * mat_rotate_point_z * mat_transformation ;
		
		

		// ***************************************
		
		if(this->id == id)
		{
			if (fathers[0]!= NULL)
			{
				fathers[0]->computeMotion(*start_bone, id);
			}
		}
		
		else
		{
			if (this->id==1)//condiciones para el pecho
			{
				if(id==3)//Origen mano Iz
				{
					children[1]->computeMotion(start_end[1], id);
					children[2]->computeMotion(start_end[2], id);
					children[3]->computeMotion(start_end[3], id);
				}


				else if(id == 4)//Origen mano Der
				{
					children[0]->computeMotion(start_end[0], id);
					children[2]->computeMotion(start_end[2], id);
					children[3]->computeMotion(start_end[3], id);
				}

				else if(id == 5 || id == 6)//Origen legs
				{
					children[0]->computeMotion(start_end[0], id);
					children[1]->computeMotion(start_end[1], id);
					children[3]->computeMotion(start_end[3], id);
				}


			}


			else if (this->id==2)//condiciones para la pelvis
			{
				if(id==3 || id == 4)//Orige hands
				{
					children[0]->computeMotion(start_end[1], id);
					children[1]->computeMotion(start_end[2], id);
				}

				else if(id == 5)//Origen pie izq
				{
					fathers[0]->computeMotion(start_end[0], id);
					children[1]->computeMotion(start_end[2], id);
				}

				else if(id == 6)//Origen pie der
				{
					fathers[0]->computeMotion(start_end[0], id);
					children[0]->computeMotion(start_end[1], id);
				}
			}
			
			else
			{
				if(children[0] != NULL)
					children[0]->computeMotion(*start_bone, id);
			}
		}




		
		

	}

};



class Skeleton
{
public:
	int num_bones;
	vector<Bone> bones;
	GLuint *vao;

	Skeleton(int num_bones)
	{
		this->num_bones = num_bones;
		vao = new GLuint[num_bones];
		glGenVertexArrays( num_bones, vao ); //BE CAREFUL IT SHOULD BE &vao
	}


	bool read_boneObj()
	{
		string bone_names[] = {	"cabeza", 				"pecho", 				"pelvis",
								"antebrazoIzquierdo", 	"brazoIzquierdo", 		"manoIzquierda",
								"antebrazoDerecho", 	"brazoDerecho", 		"manoDerecha", 
								"musloIzquierdo", 		"pantorrillaIzquierda", "pieIzquierdo", 
								"musloDerecho", 		"pantorrillaDerecha", 	"pieDerecho"};
		
		/*float joinsX[] = { 	0		, 	0			, 	0			,
						   0.15			,  0.4994774	,  0.8253671	,
						  -0.15			, 	-0.4994774	, 	-0.8253671	,
						   0.0845678	,  0.0845678	,  0.0845678	,
						  -0.0845678	, 	-0.0845678	, 	-0.084567	};


		
		float joinsY[] = { 0.1993833	, 0			, -0.182307	,
							0			, 0			, 0			,
							0			, 0			, 0			,
							-0.525104	, -0.9931839	, -1.51095	,
							-0.525104	, -0.9931839	, -1.51095	};
		*/

		vec3 start_pos[] = {vec3(0,          0.1993833, 0), vec3(0.15	,    0,         0),vec3(0, 		   -0.182307, 0), 
							vec3(0.15,       0, 		0), vec3(0.4994774,  0,         0),vec3(0.8253671,         0, 0),
							vec3(-0.15,      0, 		0), vec3(-0.4994774, 0,         0),vec3(-0.8253671,        0, 0),
							vec3(0.0845678, -0.525104, 0),  vec3(0.0845678, -0.9931839, 0),vec3(0.0845678, -1.51095,  0),
							vec3(-0.0845678, -0.525104, 0), vec3(-0.0845678,-0.9931839, 0),vec3(-0.084567, -1.51095,  0),

							};


		vec3 end_pos[]	 = {vec3(0,           0.4493833, 0) , vec3(-0.15,     0,          0),	vec3(0.0845678, -0.525104, 0),
							vec3(0.4994774, 	      0, 0) , vec3(0.8253671, 0,          0),	vec3(1.0053671,  0,        0),
							vec3(-0.4994774, 	      0, 0) , vec3(-0.8253671, 0,         0),	vec3(-1.0053671, 0,        0),
							vec3(0.0845678,   -0.9931839, 0),vec3(0.0845678, -1.51095,   0),    vec3(0.0845678, -1.58095,  0),
							vec3(-0.0845678, -0.9931839, 0) , vec3(-0.084567, -1.51095,   0),    vec3(-0.084567, -1.58095,  0),

							};
		

		

		for(int i=0; i<num_bones; i++)
		{
			Bone bone;	
			bool found = bone.load_bone(bone_names[i]);

			if(found)
			{
				bone.setBone(start_pos[i], end_pos[i]);
				bones.push_back(bone);
			}

			else
			{
				cout<<"ERROR: Not found the bone"<<endl;
				return false;
			}

		}

		bones[1].start_end.push_back(vec3(0, -0.182307, 0));
		bones[1].start_end.push_back(vec3(0, 0.1993833, 0));
		bones[2].start_end.push_back(vec3(-0.0845678, -0.525104, 0));

			
		bones[0].fathers.push_back(&bones[1]);
		bones[0].children.push_back(NULL);
		bones[0].id = 0;

		bones[1].fathers.push_back(NULL);
		bones[1].children.push_back(&bones[3]);
		bones[1].children.push_back(&bones[6]);
		bones[1].children.push_back(&bones[2]);
		bones[1].children.push_back(&bones[0]);
		bones[1].id = 1;

		bones[2].fathers.push_back(&bones[1]);
		bones[2].children.push_back(&bones[9]);
		bones[2].children.push_back(&bones[12]);
		bones[2].id = 2;

		bones[3].fathers.push_back(&bones[1]);
		bones[3].children.push_back(&bones[4]);
		bones[3].id = 3;

		bones[4].fathers.push_back(&bones[3]);
		bones[4].children.push_back(&bones[5]);
		bones[4].id = 3;

		bones[5].fathers.push_back(&bones[4]);
		bones[5].children.push_back(NULL);
		bones[5].id = 3;

		bones[6].fathers.push_back(&bones[1]);
		bones[6].children.push_back(&bones[7]);
		bones[6].id = 4;

		bones[7].fathers.push_back(&bones[6]);
		bones[7].children.push_back(&bones[8]);
		bones[7].id = 4;

		bones[8].fathers.push_back(&bones[7]);
		bones[8].children.push_back(NULL);
		bones[8].id = 4;

		bones[9].fathers.push_back(&bones[2]);
		bones[9].children.push_back(&bones[10]);
		bones[9].id = 5;

		bones[10].fathers.push_back(&bones[9]);
		bones[10].children.push_back(&bones[11]);
		bones[10].id = 5;

		bones[11].fathers.push_back(&bones[10]);
		bones[11].children.push_back(NULL);
		bones[11].id = 5;

		bones[12].fathers.push_back(&bones[2]);
		bones[12].children.push_back(&bones[13]);
		bones[12].id = 6;

		bones[13].fathers.push_back(&bones[12]);
		bones[13].children.push_back(&bones[14]);
		bones[13].id = 6;

		bones[14].fathers.push_back(&bones[13]);
		bones[14].children.push_back(NULL);
		bones[14].id = 6;

		return true;
	}





	void init_skeleton()
	{

		for (int i = 0; i < num_bones; ++i)
		{
			glBindVertexArray(vao[i]);

			GLuint points_vbo;
			if ( NULL != bones[i].vp ) 
			{
				glGenBuffers( 1, &points_vbo );
				glBindBuffer( GL_ARRAY_BUFFER, points_vbo );
				glBufferData( GL_ARRAY_BUFFER, 3 * bones[i].g_point_count * sizeof( GLfloat ), bones[i].vp,
											GL_STATIC_DRAW );
				glVertexAttribPointer( 0, 3, GL_FLOAT, GL_FALSE, 0, NULL );
				glEnableVertexAttribArray( 0 );
			}

			GLuint normal_vbo;
			if ( NULL != bones[i].vn ) 
			{
				glGenBuffers( 1, &normal_vbo );
				glBindBuffer( GL_ARRAY_BUFFER, normal_vbo );
				glBufferData( GL_ARRAY_BUFFER, 3 * bones[i].g_point_count * sizeof( GLfloat ), bones[i].vn,
											GL_STATIC_DRAW );
				glVertexAttribPointer( 1, 3, GL_FLOAT, GL_FALSE, 0, NULL );
				glEnableVertexAttribArray( 1 );
			}
		}
	}
};


