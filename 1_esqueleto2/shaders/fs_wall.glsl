#version 410

in vec2 texture_coordinates;
uniform sampler2D basic_texture;








in vec3 position_eye, normal_eye;
uniform mat4 view;

// fixed point light properties
vec3 light_position_world  = vec3 (20.0, 20.0, 40.0);
vec3 Ls = vec3 (0.2, 0.2, 0.2); // white specular colour
vec3 Ld = vec3 (0.8, 0.8, 0.8); // dull white diffuse light colour
vec3 La = vec3 (0.2, 0.2, 0.2); // grey ambient colour
  
// surface reflectance
vec3 Ks = vec3 (1.0, 1.0, 1.0); // fully reflect specular light

//vec3 Kd = vec3 (0.856, 0.936, 0.992); // orange diffuse surface reflectance
//vec3 Kd = vec3 (0.696, 0.856, 0.964); // orange diffuse surface reflectance
vec3 Kd = vec3 (0.532, 0.772, 0.932); // orange diffuse surface reflectance

vec3 Ka = vec3 (0.5, 0.5, 0.5); // fully reflect ambient light
float specular_exponent = 100.0; // specular 'power'






out vec4 frag_colour;


void main() {
	vec4 texel = texture (basic_texture, texture_coordinates);
	frag_colour = texel;






	// ambient intensity
	vec3 Ia = La * Ka;

	// normalize in case interpolation has upset normals' lengths
	vec3 n_eye = normalize( normal_eye );

	// diffuse intensity
	// raise light position to eye space
	vec3 light_position_eye = vec3 (view * vec4 (light_position_world, 1.0));
	vec3 distance_to_light_eye = light_position_eye - position_eye;
	vec3 direction_to_light_eye = normalize (distance_to_light_eye);
	float dot_prod = dot (direction_to_light_eye, n_eye);
	dot_prod = max (dot_prod, 0.0);
	vec3 Id = Ld * Kd * dot_prod; // final diffuse intensity
	
	// specular intensity
	vec3 surface_to_viewer_eye = normalize (-position_eye);
	
	//vec3 reflection_eye = reflect (-direction_to_light_eye, n_eye);
	//float dot_prod_specular = dot (reflection_eye, surface_to_viewer_eye);
	//dot_prod_specular = max (dot_prod_specular, 0.0);
	//float specular_factor = pow (dot_prod_specular, specular_exponent);
	
	// blinn
	vec3 half_way_eye = normalize (surface_to_viewer_eye + direction_to_light_eye);
	float dot_prod_specular = max (dot (half_way_eye, n_eye), 0.0);
	float specular_factor = pow (dot_prod_specular, specular_exponent);
	
	vec3 Is = Ls * Ks * specular_factor; // final specular intensity
	
	// final colour
	frag_colour = vec4 (Is + Id + Ia, 1.0);
	
}
