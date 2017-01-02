/*---------------------------------------------------------------------
*
* Copyright © 2015  Minsi Chen
* E-mail: m.chen@derby.ac.uk
*
* The source is written for the Graphics I and II modules. You are free
* to use and extend the functionality. The code provided here is functional
* however the author does not guarantee its performance.
---------------------------------------------------------------------*/
#include <math.h>
#include <stdlib.h>
#include <stdio.h>
#include <time.h>


#if defined(WIN32) || defined(_WINDOWS)
#include <Windows.h>
#include <gl/GL.h>
#endif

#ifdef __APPLE__
#include <OpenGL/gl.h>
#endif

#include "RayTracer.h"
#include "Ray.h"
#include "Scene.h"
#include "Camera.h"
#include "perlin.h"

RayTracer::RayTracer()
{
	m_buffHeight = m_buffWidth = 0.0;
	m_renderCount = 0;
	SetTraceLevel(5);
	m_traceflag = (TraceFlags)(TRACE_AMBIENT | TRACE_DIFFUSE_AND_SPEC |
		TRACE_SHADOW | TRACE_REFLECTION | TRACE_REFRACTION);

}

RayTracer::RayTracer(int Width, int Height)
{
	m_buffWidth = Width;
	m_buffHeight = Height;
	m_renderCount = 0;
	SetTraceLevel(5);

	m_framebuffer = new Framebuffer(Width, Height);

	//default set default trace flag, i.e. no lighting, non-recursive
	m_traceflag = (TraceFlags)(TRACE_AMBIENT);
}

RayTracer::~RayTracer()
{
	delete m_framebuffer;
}

void RayTracer::DoRayTrace( Scene* pScene )
{
	Camera* cam = pScene->GetSceneCamera();
	
	Vector3 camRightVector = cam->GetRightVector();
	Vector3 camUpVector = cam->GetUpVector();
	Vector3 camViewVector = cam->GetViewVector();
	Vector3 centre = cam->GetViewCentre();
	Vector3 camPosition = cam->GetPosition();

	double sceneWidth = pScene->GetSceneWidth();
	double sceneHeight = pScene->GetSceneHeight();

	double pixelDX = sceneWidth / m_buffWidth;
	double pixelDY = sceneHeight / m_buffHeight;
	
	int total = m_buffHeight*m_buffWidth;
	int done_count = 0;
	
	Vector3 start;

	start[0] = centre[0] - ((sceneWidth * camRightVector[0])
		+ (sceneHeight * camUpVector[0])) / 2.0;
	start[1] = centre[1] - ((sceneWidth * camRightVector[1])
		+ (sceneHeight * camUpVector[1])) / 2.0;
	start[2] = centre[2] - ((sceneWidth * camRightVector[2])
		+ (sceneHeight * camUpVector[2])) / 2.0;
	
	Colour scenebg = pScene->GetBackgroundColour();

	if (m_renderCount == 0)
	{
		fprintf(stdout, "Trace start.\n");

		Colour colour;
//TinyRay on multiprocessors using OpenMP!!!
#pragma omp parallel for schedule (dynamic, 1) private(colour)
		for (int i = 0; i < m_buffHeight; i+=1) {
			for (int j = 0; j < m_buffWidth; j+=1) {

				//calculate the metric size of a pixel in the view plane (e.g. framebuffer)
				Vector3 pixel;

				pixel[0] = start[0] + (i + 0.5) * camUpVector[0] * pixelDY
					+ (j + 0.5) * camRightVector[0] * pixelDX;
				pixel[1] = start[1] + (i + 0.5) * camUpVector[1] * pixelDY
					+ (j + 0.5) * camRightVector[1] * pixelDX;
				pixel[2] = start[2] + (i + 0.5) * camUpVector[2] * pixelDY
					+ (j + 0.5) * camRightVector[2] * pixelDX;

				/*
				* setup first generation view ray
				* In perspective projection, each view ray originates from the eye (camera) position 
				* and pierces through a pixel in the view plane
				*/
				Ray viewray;
				viewray.SetRay(camPosition,	(pixel - camPosition).Normalise());
				
				double u = (double)j / (double)m_buffWidth;
				double v = (double)i / (double)m_buffHeight;

				scenebg = pScene->GetBackgroundColour();

				//trace the scene using the view ray
				//default colour is the background colour, unless something is hit along the way
				colour = this->TraceScene(pScene, viewray, scenebg, m_traceLevel);

				/*
				* Draw the pixel as a coloured rectangle
				*/
				m_framebuffer->WriteRGBToFramebuffer(colour, j, i);
			}
		}

		fprintf(stdout, "Done!!!\n");
		m_renderCount++;
	}
}

Colour RayTracer::TraceScene(Scene* pScene, Ray& ray, Colour incolour, int tracelevel, bool shadowray)
{
	RayHitResult result;

	Colour outcolour = incolour; //the output colour based on the ray-primitive intersection
	
	std::vector<Light*> *light_list = pScene->GetLightList();
	Vector3 cameraPosition = pScene->GetSceneCamera()->GetPosition();


	//Intersect the ray with the scene
	//TODO: Scene::IntersectByRay needs to be implemented first
	result = pScene->IntersectByRay(ray, shadowray);

	if (result.data) //the ray has hit something
	{
		Vector3 start = ray.GetRayStart();

		if (!shadowray) 
		{
			//if not in shadow, calculate normal lighting
			outcolour = CalculateLighting(light_list, &start, &result);
		}
		else 
		{
			//return shadow colour
			return outcolour * 0.3f;
		}


		//TODO: 2. The following conditions are for implementing recursive ray tracing
		if (m_traceflag & TRACE_REFLECTION)
		{
			//TODO: trace the reflection ray from the intersection point			
			//it is not perfect, the reflection on the boxes is not completely right, and it looks too 'grainy' compared to the example executable
			if (((Primitive*)result.data)->m_primtype != Primitive::PRIMTYPE_Plane)
			{
				//if we intersect a shape on the scene... (that is not the plane, you do not want to reflect on the plane!!)

				Vector3 cameraView = pScene->GetSceneCamera()->GetViewVector();  //get the vector of camera
				float value = -3.5f * (cameraPosition.DotProduct(cameraView));   //'arbitrary' reflection value - changing the float value changes the intensity of reflection
				Vector3 viewDir = cameraPosition;   //get the position of the camera
				Vector3 reflectDir = viewDir + ((ray.GetRay().Reflect(result.normal) * value));   //get the reflection ray

				ray.SetRay(result.point, reflectDir); //set the new ray from intersection point to reflected direction
				outcolour = outcolour * TraceScene(pScene, ray, outcolour, tracelevel--, shadowray);  //trace scene again with reflections
			}						
		}

		if (m_traceflag & TRACE_REFRACTION)
		{
			//TODO: trace the refraction ray from the intersection point
		}

		if (m_traceflag & TRACE_SHADOW)
		{
			//TODO: trace the shadow ray from the intersection point
			std::vector<Light*>::iterator lit_iter = light_list->begin();   //light source iterator

			while (lit_iter != light_list->end())
			{
				//while not at end...
				Vector3 direction = ((*lit_iter)->GetLightPosition() - result.point).Normalise();   //get the direction of the shadow ray as unit vector
				Vector3 l = result.point + (direction * 0.0001);   //get the position of the shadow
				ray.SetRay(l, direction);   //set each ray in the right place

				outcolour = TraceScene(pScene, ray, outcolour, tracelevel--, true); //trace the scene again with shadowRay bool as true

				lit_iter++;
			}
		}
	}
		
	return outcolour;
}

Colour RayTracer::CalculateLighting(std::vector<Light*>* lights, Vector3* campos, RayHitResult* hitresult)
{
	Colour outcolour;
	std::vector<Light*>::iterator lit_iter = lights->begin();

	//get the material(colour) of the primitive that is hit
	Primitive* prim = (Primitive*)hitresult->data;
	Material* mat = prim->GetMaterial();

	//default colour
	outcolour = mat->GetAmbientColour();
	
	//Generate the grid pattern on the plane
	if (((Primitive*)hitresult->data)->m_primtype == Primitive::PRIMTYPE_Plane)
	{
		int dx = hitresult->point[0]/2.0;
		int dy = hitresult->point[1]/2.0;
		int dz = hitresult->point[2]/2.0;

		if (dx % 2 || dy % 2 || dz % 2 )
		{
			outcolour = Vector3(0.1, 0.1, 0.1);
		}
		else
		{
			outcolour = mat->GetDiffuseColour();
		}
		return outcolour;   //remove this if you want to see attenuation across the whole image, and not just on the shapes

	}
	
	////Go through all lights in the scene
	////Note the default scene only has one light source
	if (m_traceflag & TRACE_DIFFUSE_AND_SPEC)
	{
		while (lit_iter != lights->end()) 
		{
			//while we have not gone through all the light sources...
			
			Vector3 lightPosition = (*lit_iter)->GetLightPosition();   //position of the current light

			Light currentLight;   //Light source

			Vector3 surfaceNormal = hitresult->normal;    //surface normal at intersection

			Vector3 surfaceIntersect = hitresult->point;  //location of intersection

			//compute the diffuse of the current light source and primitive
			outcolour = outcolour + CalculateDiffuse(surfaceNormal, (lightPosition - surfaceIntersect).Normalise(), currentLight, *mat);

			//compute the specular of the current light source and primitive
			outcolour = outcolour + CalculateSpecular(surfaceNormal, surfaceIntersect, lightPosition, currentLight, *mat);

			double distance = (lightPosition - surfaceIntersect).Norm();   //direction of light source to surface point
			double attenuation = 1.0 / (1.0 + (0 * distance) + (0.002 * distance * distance)); //Attenuation (loss of light intensity over distance)
																							   //(0 * distance) is the arbitrary "attenuation factor" - smaller value = more light lost
																							   //(0.002 * dis * dis) is the distance (bigger = lower intensity)															
																							   //formula = att = 1 / 1 + att factor * distance squared

			//outcolour = outcolour * attenuation;  //uncomment this to see attenuation - disabled as it makes reflections harder to see
			lit_iter++;    //Go to next primitive in list
		}
	}

	return outcolour;
}


//Diffuse calculation method
Colour RayTracer::CalculateDiffuse(Vector3 &surfaceNorm, Vector3 &lightDirection, Light &light, Material &material) 
{
	//get the cosine theta value (angle) by doing dot product of surface normal and light direction
	double angle = surfaceNorm.DotProduct(lightDirection);   //angle between the light source and the surface normal

	
	Colour diff;   //Colour object to be passed to CalculateLighting

	//compute the colour
	//incoming light source intensity * surface diffuse colour * cos theta value (reflection)
	diff = (light.GetLightColour() * material.GetDiffuseColour() * angle);
	
	
	
	return diff;   //return colour
}

//Specular calculation method
Colour RayTracer::CalculateSpecular(Vector3 &surfaceNorm, Vector3 &surfaceIntersect, Vector3 &lightPos, Light &light, Material &material) 
{
	
	Vector3 reflectVect = lightPos.Reflect(surfaceNorm).Normalise();   //reflected direction vector by using Reflect method
	double angle = reflectVect.DotProduct(surfaceNorm);    //cosine theta value used to get shininess
	double specCoefficient = pow(angle, material.GetSpecPower());   //specular coefficient (shininess)

	Colour spec;   //Colour object

	//compute the new colour
	//incoming light intensity * surface specular colour * shininess
	spec = (light.GetLightColour() * material.GetSpecularColour() * specCoefficient);

	return spec;   //return colour
}

