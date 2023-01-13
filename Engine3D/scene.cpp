#include "scene.h"
#include "glad/include/glad/glad.h"
#include <iostream>
#include <glm/gtc/matrix_transform.hpp>

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

	Scene::Scene() : direction(Clockwise)
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

	Scene::Scene(float angle,float relationWH,float near1, float far1): direction(Clockwise)
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
				Clear(1,1,1,1);
			else
				Clear(0,0,0,0);
		}


		for (unsigned int i=0; i<shapes.size();i++)
		{
			if(shapes[i]->Is2Render())
			{
				glm::mat4 Model = Normal * shapes[i]->MakeTrans();
				
				if(p != i)
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
				Clear(1, 1, 1, 1);
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
		ReadPixel(x, 512 -y -1);
		glm::vec4 viewport = glm::vec4(0, 0, 512, 512);
		glm::vec3 wincoord = glm::vec3(x, 512 - y - 1, depth);
		glm::mat4 projection = cameras[0]->GetViewProjection();
		glm::mat4 view = cameras[0]->MakeTrans();
		view[3][0] = -1 * view[3][0];
		view[3][1] = -1 * view[3][1];
		view[3][2] = -1 * view[3][2];
		glm::vec3 objcoord = glm::unProject(wincoord,view*MakeTrans(), projection, viewport);

		printf("Coordinates in object space: %f, %f, %f\n",
			objcoord.x, objcoord.y, objcoord.z);

		for (int i = 2; i < shapes.size(); i++) {
			Shape* shape = shapes[i];
			if (glm::length(shape->GetOrigin() - objcoord) <= shape->GetScale()) {
				pickedShape = i;
				return i;
			}
		}
		pickedShape = -1;
		return -1;
	}
	//return coordinates in global system for a tip of arm position is local system 
	void Scene::MouseProccessing(int button)
	{
		if(pickedShape == -1)
		{
			if(button == 1 )
			{				

				MoveCamera(0, xTranslate, xrel/60.0f);
				MoveCamera(0, yTranslate,-yrel/60.0f);
				WhenTranslate();
			}
			else
			{
				RotateRelativeGlobal (- xrel / 2.0f,glm::vec3(0, 1, 0));
				RotateRelativeGlobal (- yrel / 2.0f,glm::vec3(1, 0, 0));
				WhenRotate();
			}
		}
		else {
			// picked control point
			glm::vec4 viewport = glm::vec4(0, 0, 512, 512);
			glm::vec3 wincoord = glm::vec3(xold+xrel, 512 - (yold+yrel) - 1, depth);
			glm::mat4 projection = cameras[0]->GetViewProjection();
			glm::mat4 view = cameras[0]->MakeTrans() * MakeTrans();
			view[3][0] = -1 * view[3][0];
			view[3][1] = -1 * view[3][1];
			view[3][2] = -1 * view[3][2];
			glm::vec3 newPos = glm::unProject(wincoord,view, projection, viewport);
			wincoord = glm::vec3(xold, 512 - (yold) - 1, depth);
			glm::vec3 oldPos = glm::unProject(wincoord,view, projection, viewport);


			glm::vec3 diff = oldPos - newPos;
			shapes[pickedShape]->MyTranslate(diff, 0);
			float dx = diff[0];
			float dy = diff[1];
			float dz = diff[2];
			bez->CurveUpdate(pickedShape - 2, dx,dy,dz, continuity);

		}
	}

	void Scene::ScrollProcessing(double xoffest, double yoffset) {
		if (pickedShape == -1) {
			MoveCamera(0, 2, -yoffset);
		}
		else {
			glm::vec3 delta = glm::vec3(glm::vec4(0, 0, -yoffset/3,1) * cameras[0]->MakeTrans()*MakeTrans());
			shapes[pickedShape]->MyTranslate(delta, 0);
			bez->CurveUpdate(pickedShape - 2, delta[0], delta[1], delta[2], continuity);
		}
	}

	void Scene::RotateCube(float amount, glm::vec3 direction) {
		std::vector<int> indexes;
		for (auto const& imap : rubiksCube.Cubes)
			indexes.push_back(imap.first);

		for (int index : indexes) {
			shapes[index]->RotateRelativeOrigin(amount, direction, rubiksCube);
		}
		rubiksCube.RotateRelativeGlobal(amount, direction);
		// shapes[0]->RotateRelativeGlobal(amount, direction); // Debug plane
	}

	// ASSIGNMENT 3
	void Scene::RotateCubeWall(WallType type)
	{
		std::vector<int> wallIndexes = rubiksCube.GetWall(type);
		glm::vec3 rotationVector;
		int dir;
		if (direction == Clockwise) {
			dir = 1;
		}
		else {
			dir = -1;
		}

		switch (type) {
		case (Right):
			rotationVector = glm::vec3(dir * -1, 0, 0);
			break;
		case (Left):
			rotationVector = glm::vec3(dir * 1, 0, 0);
			break;
		case (Up):
			rotationVector = glm::vec3( 0, dir * -1, 0);
			break;
		case (Down):
			rotationVector = glm::vec3( 0, dir *1, 0);
			break;
		case (Front):
			rotationVector = glm::vec3( 0, 0, dir *-1);
			break;
		case (Back):
			rotationVector = glm::vec3( 0, 0, dir *1);
			break;
		}
		for (int index : wallIndexes) {
			shapes[index]->RotateRelative(90, rotationVector, rubiksCube);
		}
		rubiksCube.RotateWall(type, direction);
	}

	void Scene::ZeroShapesTrans()
	{
		for (unsigned int i = 0; i < shapes.size(); i++)
		{
			shapes[i]->ZeroTrans();
		}
	}

	void Scene::ReadPixel(int x, int y)
	{
		glReadPixels(x,y,1,1,GL_DEPTH_COMPONENT,GL_FLOAT,&depth);
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


	 
	