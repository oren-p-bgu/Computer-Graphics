#include "scene.h"
#include "glad/include/glad/glad.h"
#include <iostream>
#include <fstream>
using namespace std;

	static void printMat(const glm::mat4 mat)
	{
		printf(" matrix: \n");
		for (int i = 0; i < 4; i++)
		{
			for (int j = 0; j < 4; j++)
				printf("%f ", mat[j][i]);
			printf("\n");
		}
	}

	Scene::Scene()
	{
		//
		glLineWidth(5);
	
		cameras.push_back(new Camera(60.0f,1.0,0.1f,100.0f));		
		pickedShape = -1;
		depth = 0;
		cameraIndx = 0;
		xold = 0;
		yold = 0;
		isActive = false;
	}

	Scene::Scene(float angle,float relationWH,float near1, float far1)
	{
		//glLineWidth(5);
		cameras.push_back(new Camera(angle,relationWH,near1,far1));
		pickedShape = -1;
		depth = 0;
		cameraIndx = 0;
		xold = 0;
		yold = 0;
		isActive = false;
	}

	void Scene::LoadSceneFile(const std::string& fileName) {
		ifstream sceneFile;

		AddTexture("../res/textures/box0.bmp", false);
		sceneFile.open(fileName);

		string eyeLine;
		float xEye, yEye, zEye, bonusEye;
		getline(sceneFile, eyeLine);
		sscanf(eyeLine.c_str(), "e %f %f %f %f", &xEye,&yEye, &zEye, &bonusEye);
		MoveCamera(0, xTranslate, xEye);
		MoveCamera(0, yTranslate, yEye);
		MoveCamera(0, zTranslate, zEye);

		string ambientLine;
		float rAmbient, gAmbient, bAmbient, aAmbient;
		getline(sceneFile, ambientLine);
		sscanf(ambientLine.c_str(), "a %f %f %f %f", &rAmbient,&gAmbient, &bAmbient, &aAmbient);
		// TODO

		string currentLine;
		char identifier;
		float arg1, arg2, arg3, arg4;
		int currentShape = -1;
		while (getline(sceneFile, currentLine)) {
			sscanf(currentLine.c_str(), "%c %f %f %f %f", &identifier, &arg1,&arg2, &arg3, &arg4);
			switch (identifier) {
			case 'd':
				// TODO
				break;
			case 'p':
				//TODO
				break;
			case 'i':
				//TODO
				break;
			case 'o':
				if (arg4 > 0) {
					// Sphere
					AddShape(Sphere,arg4, -1, TRIANGLES);
					currentShape++;
					pickedShape = currentShape;
					SetShapeTex(currentShape,0);
					ShapeTransformation(xTranslate, arg1);
					ShapeTransformation(yTranslate, arg2);
					ShapeTransformation(zTranslate, arg3);
					
					pickedShape = -1;
				}
				else {
					// Plane
				}
				break;
			case 'r':
				//TODO
				break;
			case 't':
				//TODO
				break;
			case 'c':
				//TODO
				break;
			default:
				cout << "Invalid scene line!" << endl;
			}
		}

		sceneFile.close();

	}

	void Scene::AddShapeFromFile(const std::string& fileName,int parent,unsigned int mode)
	{
		chainParents.push_back(parent);
		shapes.push_back(new Shape(fileName,mode));
	}

	void Scene::AddShape(int type, int parent,unsigned int mode)
	{
		chainParents.push_back(parent);
		shapes.push_back(new Shape(type,mode));
	}
	
	void Scene::AddShape(int type, float size ,int parent,unsigned int mode)
	{
		chainParents.push_back(parent);
		shapes.push_back(new Shape(type, size, mode));
	}

	void Scene::AddShapeCopy(int indx,int parent,unsigned int mode)
	{
		chainParents.push_back(parent);
		shapes.push_back(new Shape(*shapes[indx],mode));
	}

	void Scene::AddShader(const std::string& fileName)
	{
		shaders.push_back(new Shader(fileName));
	}

	void Scene::AddTexture(const std::string& textureFileName,bool for2D)
	{
		textures.push_back(new Texture(textureFileName));
	}

	void Scene::AddTexture(int width,int height, unsigned char *data)
	{
		textures.push_back(new Texture(width,height,data));
	}

	void Scene::AddCamera(glm::vec3& pos , float fov,float relationWH , float zNear, float zFar)
	{
		cameras.push_back(new Camera(fov,relationWH,zNear,zFar));
		cameras.back()->MyTranslate(pos,0);
	}

	void Scene::Draw(int shaderIndx,int cameraIndx,int buffer,bool toClear,bool debugMode)
	{
		glEnable(GL_DEPTH_TEST);
		glm::mat4 Normal = MakeTrans();
	
		glm::mat4 MVP = cameras[cameraIndx]->GetViewProjection()*glm::inverse(cameras[cameraIndx]->MakeTrans());
		int p = pickedShape;
		if(toClear)
		{
			if(shaderIndx>0)
				Clear(1,0,1,1);
			else
				Clear(0,0,0,0);
		}


		for (unsigned int i=0; i<shapes.size();i++)
		{
			if(shapes[i]->Is2Render())
			{
				glm::mat4 Model = Normal * shapes[i]->MakeTrans();
				
				if(shaderIndx > 0)
				{
					Update(MVP,Model,shapes[i]->GetShader());
					shapes[i]->Draw(shaders,textures,false);	
				}
				else 
				{ //picking
					Update(MVP,Model,0);
					shapes[i]->Draw(shaders,textures,true);
				}
			}
		}
		pickedShape = p;
	}

	// ASSIGNMENT 1 - Added version of draw with viewport index to allow drawing to different viewports
	void Scene::Draw(int shaderIndx, int cameraIndx, int buffer, bool toClear, bool debugMode,int viewport)
	{
		glEnable(GL_DEPTH_TEST);
		glm::mat4 Normal = MakeTrans();

		glm::mat4 MVP = cameras[cameraIndx]->GetViewProjection() * glm::inverse(cameras[cameraIndx]->MakeTrans());
		int p = pickedShape;
		if (toClear)
		{
			if (shaderIndx > 0)
				Clear(1, 0, 1, 1);
			else
				Clear(0, 0, 0, 0);
		}

		switch (viewport) {
		case 0:
			glViewport(0, 256, 256, 256);
			break;
		case 1:
			glViewport(256, 256, 256, 256);
			break;
		case 2:
			glViewport(0, 0, 256, 256);
			break;
		case 3:
			glViewport(256, 0, 256, 256);
			break;
		}
		for (unsigned int i = 0; i < shapes.size();i++)
		{
			if (shapes[i]->Is2Render())
			{
				glm::mat4 Model = Normal * shapes[i]->MakeTrans();

				if (shaderIndx > 0)
				{
					Update(MVP, Model, shapes[i]->GetShader());
					shapes[i]->Draw(shaders, textures, false);
				}
				else
				{ //picking
					Update(MVP, Model, 0);
					shapes[i]->Draw(shaders, textures, true);
				}
			}
		}
		pickedShape = p;
	}

	void Scene::MoveCamera(int cameraIndx,int type,float amt)
	{
		switch (type)
		{
			case xTranslate:
				cameras[cameraIndx]->MyTranslate(glm::vec3(amt,0,0),0);
			break;
			case yTranslate:
				cameras[cameraIndx]->MyTranslate(glm::vec3(0,amt,0),0);
			break;
			case zTranslate:
				cameras[cameraIndx]->MyTranslate(glm::vec3(0,0,amt),0);
			break;
			case xRotate:
				cameras[cameraIndx]->MyRotate(amt,glm::vec3(1,0,0),0);
			break;
			case yRotate:
				cameras[cameraIndx]->MyRotate(amt,glm::vec3(0,1,0),0);
			break;
			case zRotate:
				cameras[cameraIndx]->MyRotate(amt,glm::vec3(0,0,1),0);
			break;
			default:
				break;
		}
	}

	void Scene::ShapeTransformation(int type,float amt)
	{
		if(glm::abs(amt)>1e-5)
		{
			switch (type)
			{
			case xTranslate:
				shapes[pickedShape]->MyTranslate(glm::vec3(amt,0,0),0);
			break;
			case yTranslate:
				shapes[pickedShape]->MyTranslate(glm::vec3(0,amt,0),0);
			break;
			case zTranslate:
				shapes[pickedShape]->MyTranslate(glm::vec3(0,0,amt),0);
			break;
			case xRotate:
				shapes[pickedShape]->MyRotate(amt,glm::vec3(1,0,0),0);
			break;
			case yRotate:
				shapes[pickedShape]->MyRotate(amt,glm::vec3(0,1,0),0);
			break;
			case zRotate:
				shapes[pickedShape]->MyRotate(amt,glm::vec3(0,0,1),0);
			break;
			default:
				break;
			}
		}

	}

	void Scene::Resize(int width,int height)
	{
	
		cameras[0]->SetProjection(cameras[0]->GetAngle(),(float)width/height);
		glViewport(0,0,width,height);
		std::cout<<cameras[0]->GetRelationWH()<<std::endl;
	}

	float Scene::Picking(int x,int y)
	{
		
		
		return 0;
	}
	//return coordinates in global system for a tip of arm position is local system 
	void Scene::MouseProccessing(int button)
	{
		if(pickedShape == -1)
		{
			if(button == 1 )
			{				

				MyTranslate(glm::vec3(-xrel/20.0f,0,0),0);
				MyTranslate(glm::vec3(0,yrel/20.0f,0),0);
				WhenTranslate();
			}
			else
			{
				MyRotate(xrel/2.0f,glm::vec3(1,0,0),0);
				MyRotate(yrel/2.0f,glm::vec3(0,0,1),0);
				WhenRotate();
			}
		}
	}

	void Scene::ZeroShapesTrans()
	{
		for (unsigned int i = 0; i < shapes.size(); i++)
		{
			shapes[i]->ZeroTrans();
		}
	}

	void Scene::ReadPixel()
	{
		glReadPixels(1,1,1,1,GL_DEPTH_COMPONENT,GL_FLOAT,&depth);
	}

	void Scene::UpdatePosition(float xpos, float ypos)
	{
		xrel = xold - xpos;
		yrel = yold - ypos;
		xold = xpos;
		yold = ypos;
	}

	void Scene::HideShape(int shpIndx)
	{
			shapes[shpIndx]->Hide();
	}

	void Scene::UnhideShape(int shpIndx)
	{
			
			shapes[shpIndx]->Unhide();
	}

	void Scene::Clear(float r, float g, float b, float a)
	{
		glClearColor(r, g, b, a);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	}

	Scene::~Scene(void)
{
	for (Shape* shp : shapes)
		{
			delete shp;
		}
	for (Camera* cam : cameras)
		{
			delete cam;
		}
	for(Shader* sdr: shaders)
		{
			delete sdr;
		}
	for(Texture* tex: textures)
		{
			delete tex;
		}

}


	 
	