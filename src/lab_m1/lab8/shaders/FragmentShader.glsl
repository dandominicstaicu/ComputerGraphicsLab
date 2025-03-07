#version 330

// Input
in vec3 world_position;
in vec3 world_normal;

// Uniforms for light properties
uniform vec3 light_direction[2];
uniform vec3 light_position[2];
uniform vec3 eye_position;

uniform float material_kd;
uniform float material_ks;
uniform int material_shininess;

// TODO(student): Declare any other uniforms

uniform vec3 object_color;
uniform int light_type;
uniform float cut_off_angle;

// Output
layout(location = 0) out vec4 out_color;


void main()
{
    // TODO(student): Define ambient, diffuse and specular light components
    vec3 N1 = normalize(world_normal);
	vec3 L1 = normalize(light_position[0] - world_position);
	vec3 V1 = normalize(eye_position - world_position);
	vec3 H1 = normalize(L1 + V1);
	vec3 R1 = normalize(reflect(-L1, world_normal));

    vec3 N2 = normalize(world_normal);
	vec3 L2 = normalize(light_position[1] - world_position);
	vec3 V2 = normalize(eye_position - world_position);
	vec3 H2 = normalize(L2 + V2);
	vec3 R2 = normalize(reflect(-L2, world_normal));

    float ambient_light = 0.25;
    float diffuse_light = material_kd * max(dot(normalize(N1), L1), 0.f);
    float specular_light = 0.f;

    float ambient_light2 = 0.25;
    float diffuse_light2 = material_kd * max(dot(normalize(N2), L2), 0.f);
    float specular_light2 = 0.f;

    if (diffuse_light > 0.f)
	{
		specular_light = material_ks * pow(max(dot(N1, H1), 0), material_shininess);
	}

    if (diffuse_light2 > 0.f) {
        specular_light2 = material_ks * pow(max(dot(N2, H2), 0), material_shininess);
    }

    // It's important to distinguish between "reflection model" and
    // "shading method". In this shader, we are experimenting with the Phong
    // (1975) and Blinn-Phong (1977) reflection models, and we are using the
    // Phong (1975) shading method. Don't mix them up!


    // TODO(student): If (and only if) the light is a spotlight, we need to do
    // some additional things.

    // TODO(student): Compute the total light. You can just add the components
    // together, but if you're feeling extra fancy, you can add individual
    // colors to the light components. To do that, pick some vec3 colors that
    // you like, and multiply them with the respective light components.

    float light = 0.f;
    float light2 = 0.f;


	if (light_type == 1) {
		float cut_off_rad		= radians(cut_off_angle);
		float spot_light		= dot(-L1, light_direction[0]);
		float spot_light_limit	= cos(cut_off_rad);
		
		if (spot_light > spot_light_limit) {	 
			// Quadratic attenuation
			float linear_att		= (spot_light - spot_light_limit) / (1.f - spot_light_limit);
			float light_att_factor	= linear_att * linear_att;
			light					= ambient_light + light_att_factor * (diffuse_light + specular_light);
		} else {
			light = ambient_light;  // There is no spot light, but there is light from other objects
		}
	} else {
		float d						= distance(light_position[0], world_position);
		float attenuation_factor	= 1.f / max(d * d, 1.f);
		light						= ambient_light + attenuation_factor * (diffuse_light + specular_light);
	}

    if (light_type == 1) {
		float cut_off_rad		= radians(cut_off_angle);
		float spot_light		= dot(-L2, light_direction[1]);
		float spot_light_limit	= cos(cut_off_rad);
		
		if (spot_light > spot_light_limit) {	 
			// Quadratic attenuation
			float linear_att		= (spot_light - spot_light_limit) / (1.f - spot_light_limit);
			float light_att_factor	= linear_att * linear_att;
			light2					= ambient_light2 + light_att_factor * (diffuse_light2 + specular_light2);
		} else {
			light2 = ambient_light2;  // There is no spot light, but there is light from other objects
		}
	} else {
		float d						= distance(light_position[1], world_position);
		float attenuation_factor	= 1.f / max(d * d, 1.f);
		light2						= ambient_light2 + attenuation_factor * (diffuse_light2 + specular_light2);
	}


    // TODO(student): Write pixel out color
    vec3 colour = object_color * (light + light2);

	out_color = vec4(colour, 1.f);

}