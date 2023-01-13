#pragma once   //maybe should be static class
#include "display.h"
#include "game.h"


	void mouse_callback(GLFWwindow* window,int button, int action, int mods)
	{	
		if(action == GLFW_PRESS )
		{
			Game *scn = (Game*)glfwGetWindowUserPointer(window);
			double x2,y2;
			glfwGetCursorPos(window,&x2,&y2);
			scn->Picking((int)x2,(int)y2);
		}
	}
	
	void scroll_callback(GLFWwindow* window, double xoffset, double yoffset)
	{
		Game *scn = (Game*)glfwGetWindowUserPointer(window);
		scn->ScrollProcessing(xoffset, yoffset);
		
	}
	
	void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods)
	{
		Game *scn = (Game*)glfwGetWindowUserPointer(window);

		if(action == GLFW_PRESS || action == GLFW_REPEAT)
		{
			switch (key)
			{
				case GLFW_KEY_ESCAPE:			
					glfwSetWindowShouldClose(window,GLFW_TRUE);
				break;
				case GLFW_KEY_C:
					scn->ToggleContinuity();
					break;
				case GLFW_KEY_SPACE:
					scn->ToggleActive();
					break;
				case GLFW_KEY_UP:
					scn->RotateRelativeGlobal(4, glm::vec3(1, 0, 0));
					break;
				case GLFW_KEY_LEFT:
					scn->RotateRelativeGlobal(4, glm::vec3(0, 1, 0));
					break;
				case GLFW_KEY_RIGHT:
					scn->RotateRelativeGlobal(4, glm::vec3(0, -1, 0));
					break;

				case GLFW_KEY_DOWN:
					//scn->shapeTransformation(scn->xGlobalRotate,-5.f);
					//cout<< "down: "<<endl;
					scn->RotateRelativeGlobal(4, glm::vec3(-1, 0, 0));
					break;

				case GLFW_KEY_R:
					scn->MoveCamera(0, 0, 0.1);
					break;
				case GLFW_KEY_L:
					scn->MoveCamera(0, 0, -0.1);
					break;
				case GLFW_KEY_U:
					scn->MoveCamera(0, 1, 0.1);
					break;
				case GLFW_KEY_D:
					scn->MoveCamera(0, 1, -0.1);
					break;
				case GLFW_KEY_B:
					scn->MoveCamera(0, 2, 0.1);
					break;
				case GLFW_KEY_F:
					scn->MoveCamera(0, 2, -0.1);
					break;
				case GLFW_KEY_2:
					scn->AddBezierCurve(2);
					break;
				case GLFW_KEY_3:
					scn->AddBezierCurve(3);
					break;
				case GLFW_KEY_4:
					scn->AddBezierCurve(4);
					break;
				case GLFW_KEY_5:
					scn->AddBezierCurve(5);
					break;
				case GLFW_KEY_6:
					scn->AddBezierCurve(6);
					break;

			default:
				break;
			}
		}
	}
	
	void cursor_position_callback(GLFWwindow* window, double xpos, double ypos)
	{
		Game *scn = (Game*)glfwGetWindowUserPointer(window);

		scn->UpdatePosition((float)xpos,(float)ypos);
		if(glfwGetMouseButton(window,GLFW_MOUSE_BUTTON_RIGHT) == GLFW_PRESS)
		{
			scn->MouseProccessing(GLFW_MOUSE_BUTTON_RIGHT);
		}
		else if(glfwGetMouseButton(window,GLFW_MOUSE_BUTTON_LEFT) == GLFW_PRESS)
		{
			scn->MouseProccessing(GLFW_MOUSE_BUTTON_LEFT);
		}

	}

	void window_size_callback(GLFWwindow* window, int width, int height)
	{
		Game *scn = (Game*)glfwGetWindowUserPointer(window);

		scn->Resize(width,height);
		
	}

	void Init(Display &display)
	{
		display.AddKeyCallBack(key_callback);
		display.AddMouseCallBacks(mouse_callback,scroll_callback,cursor_position_callback);
		display.AddResizeCallBack(window_size_callback);
	}
