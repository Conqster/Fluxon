#version 450

layout(location = 0) out vec4 outColour;

layout(location = 0) in vec3 vFragColour;
layout(location = 1) in vec2 vFragTexCoord;
in VS_OUT
{
	layout(location = 2) vec3 fragPos;
	layout(location = 3) vec3 viewPos;
	layout(location = 4) vec3 normal;
}fs_in;

layout(set = 1, binding = 1) uniform sampler2D inTexSampler;

layout(set = 0, binding = 0) uniform sampler2D albedoMap;
layout(set = 0, binding = 1) uniform sampler2D normalMap;
layout(set = 0, binding = 2) uniform sampler2D aoRMMap;

layout(set = 0, binding = 3) uniform MatHack
{
	vec3 colour;
	float metallic;
	float roughness;
	float ao;
}uMatHack;




//extract into a struct later 
layout(set = 1, binding = 2) uniform DirectionalLight
{
	vec3 direction;
	bool enable;
	vec3 colour;
	int shinness;
}uDirLight;


layout(set = 2, binding = 0) uniform samplerCube uSkyboxTex;
layout(set = 2, binding = 1) uniform Skybox
{
	float skyboxInfluencity;
	float objectReflectivity;
	bool enable;
	int _padding;
}uSkybox;




layout(push_constant) uniform CameraPos{
	layout(offset = 80) vec3 position;   //<offset based on vert Model push_constant
	bool useMaterialSystem;
}uCam;


//Constants 
const float PI = 3.141592653589793238462643f;

///////////////////////////////
//FUNCTIONS DECLARATIONs
///////////////////////////////

vec3 ReflectedSkybox(samplerCube skybox_map, vec3 vdir, vec3 nor, float object_reflectivity);
vec3 ComputeDirectionalLight(vec3 N, vec3 V, vec3 base_colour);

// GGX Normal Distribution Function
float D_GGX(float NoH, float roughness);
//Fresenel Scklicjk Approxiamtion
vec3 FrenselSchlick(float cos_theta, vec3 F0);
//Smith Geometry Function (Schlick-GGX)
float G_Smith(float NoV, float NoL, float roughness);
// Cook-Torrance Specular BRDF
vec3 CookTorranceBRDF(vec3 N, vec3 V, vec3 L, float roughness, vec3 F0);

vec3 RetriveNormalFromMap(sampler2D map, vec2 coord, vec3 frag_world_pos, vec3 frag_nor);


void main()
{

	vec3 base_colour = vFragColour;
	
	//cam view direction
	vec3 V = normalize(fs_in.viewPos - fs_in.fragPos);
	vec3 N = normalize(fs_in.normal);
	
	vec3 commulated_lighting = vec3(0.0f);
	
	if(uCam.useMaterialSystem)
	{
		vec3 albedo = pow(texture(albedoMap, vFragTexCoord).rgb, vec3(2.2f));
		vec3 ao_rough_metallic = texture(aoRMMap, vFragTexCoord).rgb;
		float ao = ao_rough_metallic.r;
		float roughness = ao_rough_metallic.g;
		float metallic = ao_rough_metallic.b;

		ao = 1.0f;
		
		//N = normalize(texture(normalMap, vFragTexCoord).xyz);
		//N = N * 2.0 - 1.0; //[0,1] >> [-1, 1]
		//vec3 L  = normalize(uDirLight.direction);
		//vec3 radiance = uDirLight.colour;
		//
		////Reflectance at normal incidence
		//vec3 F0 = vec3(0.04f);
		//F0 = mix(F0, albedo, metallic);
		//
		////Cook-Torrance specular BRDF
		//vec3 F = CookTorranceBRDF(N, V, L, roughness, F0);
		//float NoL = max(dot(N, L), 0.0f); 
		//
		////Lambertian diffuse
		//vec3 kD = (1.0f - F) * (1.0f - metallic);
		//vec3 diffuse = kD * albedo / PI;
		//
		////Final colour 
		//commulated_lighting = (diffuse + F) * radiance * NoL;
		//
		////Apply ambient + ao
		//vec3 ambient = vec3(0.03f) * albedo * ao;
		//commulated_lighting += ambient;

		vec3 Lo = vec3(0.0f);
		//override normal with texture map normal 
		//N = RetriveNormalFromMap(normalMap, vFragTexCoord, fs_in.fragPos, fs_in.normal);
		for(int i = 0; i < 4; ++i)
		{
			//compute light per radiance
			vec3 L = normalize(uDirLight.direction);
			vec3 H = normalize(V+L);
			vec3 radiance = uDirLight.colour;

			//cook-torrance brdf 
			float NoV = max(dot(N, V), 0.0f);
			float NoL = max(dot(N, L), 0.0f);
			float NoH = max(dot(N, H), 0.0f);
			float VoH = max(dot(V, H), 0.0f);
	
			float D = D_GGX(NoH, roughness);
			float G = G_Smith(NoV, NoL, roughness);
			vec3 F0 = mix(vec3(0.04f), albedo, metallic);
			vec3 F = FrenselSchlick(VoH, F0);

			vec3 kS = F;
			vec3 kD = vec3(1.0f) - kS;
			kD *= 1.0f - metallic; 

			vec3 num = D * G * F;
			float deno = 4.0f * NoV * NoL + 0.0001f;
			vec3 specular = num / deno;

			//add to outgoing radiance Lo
			Lo += (kD * albedo / PI + specular) * radiance * NoL;
		}


		//Apply ambient + ao
		vec3 ambient = vec3(0.03f) * albedo * ao;
		commulated_lighting = ambient + Lo;
	}
	else
	{
		//base_colour *= pow(texture(inTexSampler, vFragTexCoord).rgb, vec3(2.2f));
		vec3 ao_rough_metallic = pow(texture(aoRMMap, vFragTexCoord).rgb, vec3(2.2f));
		
		if(uSkybox._padding == 0)
		{
			base_colour *= pow(texture(inTexSampler, vFragTexCoord).rgb, vec3(2.2f));
		}
		else if(uSkybox._padding == 1)
		{
			base_colour *= pow(texture(albedoMap, vFragTexCoord).rgb, vec3(2.2f));
		}
		else if(uSkybox._padding == 2)
		{
			base_colour *= pow(texture(normalMap, vFragTexCoord).rgb, vec3(2.2f));
		}
		else if(uSkybox._padding == 3)
		{
			base_colour *= vec3(ao_rough_metallic.r);
		}
		else if(uSkybox._padding == 4)
		{
			base_colour *= vec3(ao_rough_metallic.g);
		}
		else if(uSkybox._padding == 5)
		{
			base_colour *= vec3(ao_rough_metallic.b);
		}
		else
			base_colour *= ao_rough_metallic;
		
		if(uDirLight.enable)
			commulated_lighting += ComputeDirectionalLight(N, V, base_colour);
		else 
			commulated_lighting += base_colour;
	}
		
	

	
	

		
	//with sky box reflection 
	if(uSkybox.enable)
	{
		//commulated_lighting += ReflectedSkybox(uSkyboxTex, V, N, uSkybox.objectReflectivity);
		commulated_lighting += mix(vec3(0.0f), ReflectedSkybox(uSkyboxTex, V, N, uSkybox.objectReflectivity), uSkybox.skyboxInfluencity);
	}

	//Gamma correction 
	commulated_lighting = pow(commulated_lighting, vec3(1.0f/2.2));
	outColour = vec4(commulated_lighting, 1.0f);
}


vec3 ComputeDirectionalLight(vec3 N, vec3 V, vec3 base_colour)
{
	vec3 ambient = 0.2f * uDirLight.colour * base_colour;

	//diffuse component
	vec3 Ld = normalize(uDirLight.direction);//light direction
	//Lambert cosine law
	float factor = max(dot(N, Ld), 0.0f);
	vec3 diffuse = 0.7f * uDirLight.colour * factor * base_colour; 
	
	
	
	float specularity = 0.0f;
	//specular component (Blinn-Phong)
	//halfway 
	vec3 H = normalize(Ld + V);
	specularity = pow(max(dot(N, H), 0.0f), uDirLight.shinness);

	vec3 specular = 0.7f * uDirLight.colour * specularity;
	
	//return diffuse + specular;
	return ambient + diffuse + specular;
}

vec3 ReflectedSkybox(samplerCube skybox_map, vec3 vdir, vec3 nor, float object_reflectivity)
{
	vec3 R = reflect(-vdir, nor);
	
	//return (texture(skybox_map, R).rgb) * skyIntensity;
	//return (texture(skybox_map, R).rgb);
	return (texture(skybox_map, R).rgb) * object_reflectivity;
}


// GGX Normal Distribution Function
float D_GGX(float NoH, float roughness)
{
	float a = roughness * roughness;
	float a2 = a * a;
	float denom = NoH * NoH * (a2 - 1.0f) + 1.0f;
	return a2 / (PI * denom * denom);
}

//Fresenel Scklicjk Approxiamtion
vec3 FrenselSchlick(float cos_theta, vec3 F0)
{
	return F0 + (1.0f - F0) * pow(1.0f - cos_theta, 5.0f);
}


//Smith Geometry Function (Schlick-GGX)
float G_Smith(float NoV, float NoL, float roughness)
{
	float r = roughness + 1.0f;
	float k = (r * r) / 8.0f;
	
	float G_V = NoV / (NoV * (1.0f - k) + k);
	float G_L = NoL / (NoL * (1.0f - k) + k);
	
	return G_V * G_L;
}

// Cook-Torrance Specular BRDF
vec3 CookTorranceBRDF(vec3 N, vec3 V, vec3 L, float roughness, vec3 F0)
{
	vec3 H = normalize(V+L);
	
	float NoV = max(dot(N, V), 0.0f);
	float NoL = max(dot(N, L), 0.0f);
	float NoH = max(dot(N, H), 0.0f);
	float VoH = max(dot(V, H), 0.0f);
	
	float D = D_GGX(NoH, roughness);
	float G = G_Smith(NoV, NoL, roughness);
	vec3 F = FrenselSchlick(VoH, F0);
	
	vec3 numerator = D * G * F;
	float denominator = 4.0f * NoL * NoL + 0.001f;
	
	return numerator / denominator;
}



vec3 RetriveNormalFromMap(sampler2D map, vec2 coords, vec3 frag_world_pos, vec3 frag_nor)
{
	vec3 tangent_nor = texture(map, coords).xyz * 2.0f - 1.0f;

	vec3 Q1 = dFdx(frag_world_pos);
	vec3 Q2 = dFdy(frag_world_pos);
	vec2 st1 = dFdx(coords);
	vec2 st2 = dFdy(coords);

	vec3 N = normalize(frag_nor);
	vec3 T = normalize(Q1 * st2.t - Q2 * st1.t);
	vec3 B = normalize(cross(N,T));
	mat3 TBN = mat3(T, B, N);

	return normalize(TBN * tangent_nor);
}