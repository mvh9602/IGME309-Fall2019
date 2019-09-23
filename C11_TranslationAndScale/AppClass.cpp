#include "AppClass.h"
void Application::InitVariables(void)
{
	//Row one
	for (int i = 0; i < 46; i++)
	{
		MyMesh* m_pMesh = new MyMesh();
		m_pMesh->GenerateCube(1.0f, C_BLACK);
		m_pMeshVector.push_back(m_pMesh);
	}

	//m_pMesh->GenerateSphere(1.0f, 5, C_WHITE);
}
void Application::Update(void)
{
	//Update the system so it knows how much time has passed since the last call
	m_pSystem->Update();

	//Is the arcball active?
	ArcBall();

	//Is the first person camera active?
	CameraRotation();
}
void Application::Display(void)
{
	// Clear the screen
	ClearScreen();

	matrix4 m4View = m_pCameraMngr->GetViewMatrix();
	matrix4 m4Projection = m_pCameraMngr->GetProjectionMatrix();
	
	matrix4 m4Scale = glm::scale(IDENTITY_M4, vector3(2.0f,2.0f,2.0f));
	static float value = 0.0f;
	static int sign = 1;
	//matrix4 m4Translate = glm::translate(IDENTITY_M4, vector3(value, 2.0f, 3.0f));

	//matrix4 m4Model = m4Translate * m4Scale;
	int index = 0;

	//row 1
	for (int i = 0; i < 2; i++)
	{
		matrix4 m4Translate = glm::translate(IDENTITY_M4, vector3((6 * i - 3) + value, 4, 0));
		matrix4 m4Model = m4Scale * m4Translate;
		m_pMeshVector[index]->Render(m4Projection, m4View, m4Model);
		index++;
	}
	//row 2
	for (int i = 0; i < 2; i++)
	{
		matrix4 m4Translate = glm::translate(IDENTITY_M4, vector3((4 * i - 2) + value, 3, 0));
		matrix4 m4Model = m4Scale * m4Translate;
		m_pMeshVector[index]->Render(m4Projection, m4View, m4Model);
		index++;
	}
	//row 3
	for (int i = 0; i < 7; i++)
	{
		matrix4 m4Translate = glm::translate(IDENTITY_M4, vector3((i - 3) + value, 2, 0));
		matrix4 m4Model = m4Scale * m4Translate;
		m_pMeshVector[index]->Render(m4Projection, m4View, m4Model);
		index++;
	}
	//row 4
	for (int i = 0; i < 9; i++)
	{
		if (i == 2 || i == 6) i++;
		matrix4 m4Translate = glm::translate(IDENTITY_M4, vector3((i - 4) + value, 1, 0));
		matrix4 m4Model = m4Scale * m4Translate;
		m_pMeshVector[index]->Render(m4Projection, m4View, m4Model);
		index++;
	}
	//row 5
	for (int i = 0; i < 11; i++)
	{
		matrix4 m4Translate = glm::translate(IDENTITY_M4, vector3((i - 5) + value, 0, 0));
		matrix4 m4Model = m4Scale * m4Translate;
		m_pMeshVector[index]->Render(m4Projection, m4View, m4Model);
		index++;
	}
	//row 6
	for (int i = 0; i < 11; i++)
	{
		if (i == 1 || i == 9) i++;
		matrix4 m4Translate = glm::translate(IDENTITY_M4, vector3((i - 5) + value, -1, 0));
		matrix4 m4Model = m4Scale * m4Translate;
		m_pMeshVector[index]->Render(m4Projection, m4View, m4Model);
		index++;
	}
	//row 7
	for (int i = 0; i < 11; i++)
	{
		if (i == 0 || i == 2 || i == 8 || i == 10)
		{
			matrix4 m4Translate = glm::translate(IDENTITY_M4, vector3((i - 5) + value, -2, 0));
			matrix4 m4Model = m4Scale * m4Translate;
			m_pMeshVector[index]->Render(m4Projection, m4View, m4Model);
			index++;
		}
	}
	//row 8
	for (int i = 0; i < 5; i++)
	{
		if (i == 2) i++;
		matrix4 m4Translate = glm::translate(IDENTITY_M4, vector3((i - 2) + value, -3, 0));
		matrix4 m4Model = m4Scale * m4Translate;
		m_pMeshVector[index]->Render(m4Projection, m4View, m4Model);
		index++;
	}

	if (value < -2 || value > 2)
		sign *= -1;
	value += 0.1f * sign;

	// draw a skybox
	m_pMeshMngr->AddSkyboxToRenderList();
	
	//render list call
	m_uRenderCallCount = m_pMeshMngr->Render();

	//clear the render list
	m_pMeshMngr->ClearRenderList();
	
	//draw gui
	DrawGUI();
	
	//end the current frame (internally swaps the front and back buffers)
	m_pWindow->display();
}
void Application::Release(void)
{
	for (int i = 0; i < m_pMeshVector.size(); i++)
	{
		SafeDelete(m_pMeshVector[i]);
	}

	//release GUI
	ShutdownGUI();
}