/*---------------------------------------------------------------------
*
* Copyright © 2016  Minsi Chen
* E-mail: m.chen@derby.ac.uk
*
* The source is written for the Graphics I and II modules. You are free
* to use and extend the functionality. The code provided here is functional
* however the author does not guarantee its performance.
---------------------------------------------------------------------*/
#include "Scene.h"
#include "Sphere.h"
#include "Plane.h"
#include "Box.h"

Scene::Scene()
{
	InitDefaultScene();
}


Scene::~Scene()
{
	CleanupScene();
}

void Scene::InitDefaultScene()
{
	//Create a box and its material
	Primitive* newobj = new Box(Vector3(-4.0, 4.0, -20.0), 10.0, 15.0, 4.0);
	Material* newmat = new Material();
	//mat for the box1
	newmat->SetAmbientColour(0.0, 0.0, 0.0);
	newmat->SetDiffuseColour(1.0, 0.0, 0.0);
	newmat->SetSpecularColour(1.0, 1.0, 1.0);
	newmat->SetSpecPower(20);
	newobj->SetMaterial(newmat);
	m_sceneObjects.push_back(newobj);
	m_objectMaterials.push_back(newmat);

	newobj = new Box(Vector3(4.0, 4.0, -15.0), 4.0, 20.0, 4.0);
	newmat = new Material();
	//mat for the box2
	newmat->SetAmbientColour(0.0, 0.0, 0.0);
	newmat->SetDiffuseColour(0.8, 0.8, 0.8);
	newmat->SetSpecularColour(1.0, 1.0, 1.0);
	newmat->SetSpecPower(20);
	newobj->SetMaterial(newmat);
	m_sceneObjects.push_back(newobj);
	m_objectMaterials.push_back(newmat);
	
	//Create sphere 1 and its material
	newobj = new Sphere(3.0, 2, -3.5, 2.0); //sphere 2
	newmat = new Material();
	newmat->SetAmbientColour(0.0, 0.0, 0.0);
	newmat->SetDiffuseColour(0.0, 0.8, 0.0);
	newmat->SetSpecularColour(1.0, 1.0, 1.0);
	newmat->SetSpecPower(2);
	newobj->SetMaterial(newmat);
	m_sceneObjects.push_back(newobj);
	m_objectMaterials.push_back(newmat);
	
	//Create sphere 2 and its material
	newobj = new Sphere(-2.0, 3.0, -5.0, 3.0); //sphere 3
	newmat = new Material();
	newmat->SetAmbientColour(0.0, 0.0, 0.0);
	newmat->SetDiffuseColour(0.0, 0.0, 0.9);
	newmat->SetSpecularColour(1.0, 1.0, 1.0);
	newmat->SetSpecPower(20);
	newobj->SetMaterial(newmat);
	m_sceneObjects.push_back(newobj);
	m_objectMaterials.push_back(newmat);


	newobj = new Plane(); //an xz plane at the origin, floor
	static_cast<Plane*>(newobj)->SetPlane(Vector3(0.0, 1.0, 0.0), 0.0);
	newmat = new Material();
	newmat->SetAmbientColour(0.0, 0.0, 0.0);
	newmat->SetDiffuseColour(1.0, 0.0, 0.0);
	newmat->SetSpecularColour(0.0, 0.0, 0.0);
	newmat->SetSpecPower(10);
	newmat->SetCastShadow(false);
	newobj->SetMaterial(newmat);
	m_sceneObjects.push_back(newobj);
	m_objectMaterials.push_back(newmat);
	
	newobj = new Plane(); //an xz plane 40 units above, ceiling
	static_cast<Plane*>(newobj)->SetPlane(Vector3(0.0, -1.0, 0.0), -40.0);
	newobj->SetMaterial(newmat);
	m_sceneObjects.push_back(newobj);
	

	newmat = new Material();
	newmat->SetAmbientColour(0.0, 0.0, 0.0);
	newmat->SetDiffuseColour(0.0, 1.0, 0.0);
	newmat->SetSpecularColour(0.0, 0.0, 0.0);
	newmat->SetSpecPower(10);
	newmat->SetCastShadow(false);
	newobj->SetMaterial(newmat);
	
	newobj = new Plane(); //an xy plane 40 units along -z axis, 
	static_cast<Plane*>(newobj)->SetPlane(Vector3(0.0, 0.0, 1.0), -40.0);
	m_sceneObjects.push_back(newobj);
	newobj->SetMaterial(newmat);
	//m_objectMaterials.push_back(newmat);
	
	newobj = new Plane(); //an xy plane 40 units along the z axis
	static_cast<Plane*>(newobj)->SetPlane(Vector3(0.0, 0.0, -1.0), -40.0);
	newobj->SetMaterial(newmat);
	m_sceneObjects.push_back(newobj);
	
	newobj = new Plane(); //an yz plane 20 units along -x axis
	static_cast<Plane*>(newobj)->SetPlane(Vector3(1.0, 0.0, 0.0), -20.0);
	newmat = new Material();
	newmat->SetAmbientColour(0.0, 0.0, 0.0);
	newmat->SetDiffuseColour(0.0, 0.0, 1.0);
	newmat->SetSpecularColour(0.0, 0.0, 0.0);
	newmat->SetSpecPower(10);
	newmat->SetCastShadow(false);
	newobj->SetMaterial(newmat);
	m_sceneObjects.push_back(newobj);
	m_objectMaterials.push_back(newmat);

	newobj = new Plane(); //an yz plane 20 units along +x axis
	static_cast<Plane*>(newobj)->SetPlane(Vector3(-1.0, 0.0, 0.0), -20.0);
	newobj->SetMaterial(newmat);
	m_sceneObjects.push_back(newobj);

	//Create one light source for the scene
	Light *newlight = new Light();
	newlight->SetLightPosition(0.0, 15.0, 10.0);
	m_lights.push_back(newlight);
	
	//A blue background
	m_background.SetVector(0.25, 0.6, 1.0);

	//default scene width and height;
	m_sceneWidth = 1.33333333;
	m_sceneHeight = 1.0;

	//default camera position and look at
	m_activeCamera.SetPositionAndLookAt(Vector3(0.0, 10.0, 13.0), Vector3(0.0, 7.0, 0.0));
}

void Scene::CleanupScene()
{
	//Cleanup object list
	std::vector<Primitive*>::iterator prim_iter = m_sceneObjects.begin();

	while(prim_iter != m_sceneObjects.end())
	{
		delete *prim_iter;
		prim_iter++;
	}

	m_sceneObjects.clear();

	//Cleanup material list
	std::vector<Material*>::iterator mat_iter = m_objectMaterials.begin();

	while (mat_iter != m_objectMaterials.end())
	{
		delete *mat_iter;
		mat_iter++;
	}
	m_objectMaterials.clear();

	//cleanup light list
	std::vector<Light*>::iterator lit_iter = m_lights.begin();

	while (lit_iter != m_lights.end())
	{
		delete *lit_iter;
		lit_iter++;
	}

	m_lights.clear();
}

RayHitResult Scene::IntersectByRay(Ray& ray, bool shadowRay)
{
	RayHitResult result = Ray::s_defaultHitResult;

	Colour surfaceColour;

	//create an iterator to go through each primitive in the list of primitives
	std::vector<Primitive*>::iterator primList = m_sceneObjects.begin();

	if (!shadowRay) 
	{
		//if the intersection is not in shadow
		while (primList != m_sceneObjects.end())
		{
			//While we are not at the end of the list...
			RayHitResult currentResult;

			currentResult = (*primList)->IntersectByRay(ray);  //get current primitive that is hit by dereferencing the primList

			if (currentResult.t < 0.0)
			{
				//reject the result if t is negative
				currentResult.data = NULL;
			}
			else if (currentResult.t < result.t)
			{
				//swap the current hitresult with the latest hit result
				result = currentResult;
			}
			primList++;
		}
	}
	else 
	{
		while (primList != m_sceneObjects.end())
		{
			//While we are not at the end of the list...
			RayHitResult current = result;
			bool castShadow = (*primList)->GetMaterial()->CastShadow();   //see if current primitive can cast a shadow i.e. blocking the ray to the light source

			current = (*primList)->IntersectByRay(ray);    //Get current primitive that is hit by dereferencing primList

			if (castShadow && current.t > 0.0 && current.t < result.t)
			{
				//if can cast shadow, t value is negative and current t is greater than result t...
				//take the closest(lowest) point of intersection

				std::vector<Light*>::iterator iterator = m_lights.begin();   //iterator to go through all light sources

				while (iterator != m_lights.end())
				{
					//while we have not gone through all light sources...

					Vector3 r = ray.GetRay();   //get the ray which is currently being tested
					double light_t = (*iterator)->GetLightPosition().DotProduct(r);   //get the ray from surface point to the light
					double nearest_t = 1;   //the 'max' value before nothing intersects the ray to the light

					if (light_t > nearest_t)
					{
						//anything higher than one means not in shadow
						result = current;
					}

					iterator++;
				}

			}

			primList++;
		}
	}
	

	
	//TODO: Implement ray to scene intersection
	//All geometric objects are stored in m_sceneObjects.
	//To find out which object intersects with the ray
	//1. Enumerate through each primitive in m_sceneObjects
	//2. For each primitive, there is an IntersectByRay method which can be called to test for intersection
	//
	//Remember: a ray can potentially intersect with more than one object
	//We are only interested in the closest one, i.e. one with the smallest positive t
	//Check result.t for the parametric result of the interesection

	return result;
}
