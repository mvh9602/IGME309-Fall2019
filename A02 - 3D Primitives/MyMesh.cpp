#include "MyMesh.h"
void MyMesh::Init(void)
{
	m_bBinded = false;
	m_uVertexCount = 0;

	m_VAO = 0;
	m_VBO = 0;

	m_pShaderMngr = ShaderManager::GetInstance();
}
void MyMesh::Release(void)
{
	m_pShaderMngr = nullptr;

	if (m_VBO > 0)
		glDeleteBuffers(1, &m_VBO);

	if (m_VAO > 0)
		glDeleteVertexArrays(1, &m_VAO);

	m_lVertex.clear();
	m_lVertexPos.clear();
	m_lVertexCol.clear();
}
MyMesh::MyMesh()
{
	Init();
}
MyMesh::~MyMesh() { Release(); }
MyMesh::MyMesh(MyMesh& other)
{
	m_bBinded = other.m_bBinded;

	m_pShaderMngr = other.m_pShaderMngr;

	m_uVertexCount = other.m_uVertexCount;

	m_VAO = other.m_VAO;
	m_VBO = other.m_VBO;
}
MyMesh& MyMesh::operator=(MyMesh& other)
{
	if (this != &other)
	{
		Release();
		Init();
		MyMesh temp(other);
		Swap(temp);
	}
	return *this;
}
void MyMesh::Swap(MyMesh& other)
{
	std::swap(m_bBinded, other.m_bBinded);
	std::swap(m_uVertexCount, other.m_uVertexCount);

	std::swap(m_VAO, other.m_VAO);
	std::swap(m_VBO, other.m_VBO);

	std::swap(m_lVertex, other.m_lVertex);
	std::swap(m_lVertexPos, other.m_lVertexPos);
	std::swap(m_lVertexCol, other.m_lVertexCol);

	std::swap(m_pShaderMngr, other.m_pShaderMngr);
}
void MyMesh::CompleteMesh(vector3 a_v3Color)
{
	uint uColorCount = m_lVertexCol.size();
	for (uint i = uColorCount; i < m_uVertexCount; ++i)
	{
		m_lVertexCol.push_back(a_v3Color);
	}
}
void MyMesh::AddVertexPosition(vector3 a_v3Input)
{
	m_lVertexPos.push_back(a_v3Input);
	m_uVertexCount = m_lVertexPos.size();
}
void MyMesh::AddVertexColor(vector3 a_v3Input)
{
	m_lVertexCol.push_back(a_v3Input);
}
void MyMesh::CompileOpenGL3X(void)
{
	if (m_bBinded)
		return;

	if (m_uVertexCount == 0)
		return;

	CompleteMesh();

	for (uint i = 0; i < m_uVertexCount; i++)
	{
		//Position
		m_lVertex.push_back(m_lVertexPos[i]);
		//Color
		m_lVertex.push_back(m_lVertexCol[i]);
	}
	glGenVertexArrays(1, &m_VAO);//Generate vertex array object
	glGenBuffers(1, &m_VBO);//Generate Vertex Buffered Object

	glBindVertexArray(m_VAO);//Bind the VAO
	glBindBuffer(GL_ARRAY_BUFFER, m_VBO);//Bind the VBO
	glBufferData(GL_ARRAY_BUFFER, m_uVertexCount * 2 * sizeof(vector3), &m_lVertex[0], GL_STATIC_DRAW);//Generate space for the VBO

	// Position attribute
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 2 * sizeof(vector3), (GLvoid*)0);

	// Color attribute
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 2 * sizeof(vector3), (GLvoid*)(1 * sizeof(vector3)));

	m_bBinded = true;

	glBindVertexArray(0); // Unbind VAO
}
void MyMesh::Render(matrix4 a_mProjection, matrix4 a_mView, matrix4 a_mModel)
{
	// Use the buffer and shader
	GLuint nShader = m_pShaderMngr->GetShaderID("Basic");
	glUseProgram(nShader); 

	//Bind the VAO of this object
	glBindVertexArray(m_VAO);

	// Get the GPU variables by their name and hook them to CPU variables
	GLuint MVP = glGetUniformLocation(nShader, "MVP");
	GLuint wire = glGetUniformLocation(nShader, "wire");

	//Final Projection of the Camera
	matrix4 m4MVP = a_mProjection * a_mView * a_mModel;
	glUniformMatrix4fv(MVP, 1, GL_FALSE, glm::value_ptr(m4MVP));
	
	//Solid
	glUniform3f(wire, -1.0f, -1.0f, -1.0f);
	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	glDrawArrays(GL_TRIANGLES, 0, m_uVertexCount);  

	//Wire
	glUniform3f(wire, 1.0f, 0.0f, 1.0f);
	glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
	glEnable(GL_POLYGON_OFFSET_LINE);
	glPolygonOffset(-1.f, -1.f);
	glDrawArrays(GL_TRIANGLES, 0, m_uVertexCount);
	glDisable(GL_POLYGON_OFFSET_LINE);

	glBindVertexArray(0);// Unbind VAO so it does not get in the way of other objects
}
void MyMesh::AddTri(vector3 a_vBottomLeft, vector3 a_vBottomRight, vector3 a_vTopLeft)
{
	//C
	//| \
	//A--B
	//This will make the triangle A->B->C 
	AddVertexPosition(a_vBottomLeft);
	AddVertexPosition(a_vBottomRight);
	AddVertexPosition(a_vTopLeft);
}
void MyMesh::AddQuad(vector3 a_vBottomLeft, vector3 a_vBottomRight, vector3 a_vTopLeft, vector3 a_vTopRight)
{
	//C--D
	//|  |
	//A--B
	//This will make the triangle A->B->C and then the triangle C->B->D
	AddVertexPosition(a_vBottomLeft);
	AddVertexPosition(a_vBottomRight);
	AddVertexPosition(a_vTopLeft);

	AddVertexPosition(a_vTopLeft);
	AddVertexPosition(a_vBottomRight);
	AddVertexPosition(a_vTopRight);
}
void MyMesh::GenerateCube(float a_fSize, vector3 a_v3Color)
{
	if (a_fSize < 0.01f)
		a_fSize = 0.01f;

	Release();
	Init();

	float fValue = a_fSize * 0.5f;
	//3--2
	//|  |
	//0--1

	vector3 point0(-fValue,-fValue, fValue); //0
	vector3 point1( fValue,-fValue, fValue); //1
	vector3 point2( fValue, fValue, fValue); //2
	vector3 point3(-fValue, fValue, fValue); //3

	vector3 point4(-fValue,-fValue,-fValue); //4
	vector3 point5( fValue,-fValue,-fValue); //5
	vector3 point6( fValue, fValue,-fValue); //6
	vector3 point7(-fValue, fValue,-fValue); //7

	//F
	AddQuad(point0, point1, point3, point2);

	//B
	AddQuad(point5, point4, point6, point7);

	//L
	AddQuad(point4, point0, point7, point3);

	//R
	AddQuad(point1, point5, point2, point6);

	//U
	AddQuad(point3, point2, point7, point6);

	//D
	AddQuad(point4, point5, point0, point1);

	// Adding information about color
	CompleteMesh(a_v3Color);
	CompileOpenGL3X();
}
void MyMesh::GenerateCuboid(vector3 a_v3Dimensions, vector3 a_v3Color)
{
	Release();
	Init();

	vector3 v3Value = a_v3Dimensions * 0.5f;
	//3--2
	//|  |
	//0--1
	vector3 point0(-v3Value.x, -v3Value.y, v3Value.z); //0 - lower top left
	vector3 point1(v3Value.x, -v3Value.y, v3Value.z); //1 - lower top right
	vector3 point2(v3Value.x, v3Value.y, v3Value.z); //2 - upper top right
	vector3 point3(-v3Value.x, v3Value.y, v3Value.z); //3 - upper top left

	vector3 point4(-v3Value.x, -v3Value.y, -v3Value.z); //4 - lower bottom left
	vector3 point5(v3Value.x, -v3Value.y, -v3Value.z); //5 - lower bottom right
	vector3 point6(v3Value.x, v3Value.y, -v3Value.z); //6 - upper bottom right
	vector3 point7(-v3Value.x, v3Value.y, -v3Value.z); //7 - upper bottom left

	//F
	AddQuad(point0, point1, point3, point2);

	//B
	AddQuad(point5, point4, point6, point7);

	//L
	AddQuad(point4, point0, point7, point3);

	//R
	AddQuad(point1, point5, point2, point6);

	//U
	AddQuad(point3, point2, point7, point6);

	//D
	AddQuad(point4, point5, point0, point1);

	// Adding information about color
	CompleteMesh(a_v3Color);
	CompileOpenGL3X();
}
void MyMesh::GenerateCone(float a_fRadius, float a_fHeight, int a_nSubdivisions, vector3 a_v3Color)
{
	if (a_fRadius < 0.01f)
		a_fRadius = 0.01f;

	if (a_fHeight < 0.01f)
		a_fHeight = 0.01f;

	if (a_nSubdivisions < 3)
		a_nSubdivisions = 3;
	if (a_nSubdivisions > 360)
		a_nSubdivisions = 360;

	Release();
	Init();

	// Replace this with your code
	// vector for holding all circle vertices
	std::vector<vector3> vertexes;
	vector3 centerVertex(0, 0, 0);
	vector3 topVertex(0, a_fHeight, 0);
	float angle = (2 * PI) / a_nSubdivisions;
	// circle generation code
	for (int i = 0; i < a_nSubdivisions; i++)
	{
		float x1 = a_fRadius * cos(angle * i);
		float z1 = a_fRadius * sin(angle * i);
		float x2 = a_fRadius * cos(angle * (i + 1));
		float z2 = a_fRadius * sin(angle * (i + 1));
		vector3 firstVertex(x1, 0, z1);
		// only push first vertex or vector will contain multiple of the same vertex
		vertexes.push_back(firstVertex);
		vector3 secondVertex(x2, 0, z2);
		// create geometry of the circle
		AddTri(centerVertex, firstVertex, secondVertex);
	}

	// go around circle of vertices and connect up to the top vertex
	for (int i = 0; i < vertexes.size(); i++)
	{
		if(i < vertexes.size() - 1)
			AddTri(vertexes[i + 1], vertexes[i], topVertex);
		// last shapes in the circle wrap back to the first vertices
		else
			AddTri(vertexes[0], vertexes[i], topVertex);
	}
	// -------------------------------

	// Adding information about color
	CompleteMesh(a_v3Color);
	CompileOpenGL3X();
}
void MyMesh::GenerateCylinder(float a_fRadius, float a_fHeight, int a_nSubdivisions, vector3 a_v3Color)
{
	if (a_fRadius < 0.01f)
		a_fRadius = 0.01f;

	if (a_fHeight < 0.01f)
		a_fHeight = 0.01f;

	if (a_nSubdivisions < 3)
		a_nSubdivisions = 3;
	if (a_nSubdivisions > 360)
		a_nSubdivisions = 360;

	Release();
	Init();

	// Replace this with your code
	// vectors for holding separate circle vertices
	std::vector<vector3> topVertexes;
	std::vector<vector3> bottomVertexes;
	vector3 centerVertex(0, 0, 0);
	vector3 topVertex(0, a_fHeight, 0);
	float angle = (2 * PI) / a_nSubdivisions;
	// circle generation code
	for (int i = 0; i < a_nSubdivisions; i++)
	{
		float x1 = a_fRadius * cos(angle * i);
		float z1 = a_fRadius * sin(angle * i);
		float x2 = a_fRadius * cos(angle * (i + 1));
		float z2 = a_fRadius * sin(angle * (i + 1));
		vector3 bottomFirstVertex(x1, 0, z1);
		// only push first vertex or vector will contain multiple of the same vertex
		bottomVertexes.push_back(bottomFirstVertex);
		vector3 topFirstVertex(x1, a_fHeight, z1);
		// only push first vertex or vector will contain multiple of the same vertex
		topVertexes.push_back(topFirstVertex);
		vector3 bottomSecondVertex(x2, 0, z2);
		vector3 topSecondVertex(x2, a_fHeight, z2);
		// create geometry of the two circles
		AddTri(centerVertex, bottomFirstVertex, bottomSecondVertex);
		AddTri(topSecondVertex, topFirstVertex, topVertex);
	}

	// connect top and bottom circles together with quads
	for (int i = 0; i < bottomVertexes.size(); i++)
	{
		if (i < bottomVertexes.size() - 1)
			AddQuad(bottomVertexes[i + 1], bottomVertexes[i], topVertexes[i + 1], topVertexes[i]);
		// last shapes in the circle wrap back to the first vertices
		else
			AddQuad(bottomVertexes[0], bottomVertexes[i], topVertexes[0], topVertexes[i]);
	}
	// -------------------------------

	// Adding information about color
	CompleteMesh(a_v3Color);
	CompileOpenGL3X();
}
void MyMesh::GenerateTube(float a_fOuterRadius, float a_fInnerRadius, float a_fHeight, int a_nSubdivisions, vector3 a_v3Color)
{
	if (a_fOuterRadius < 0.01f)
		a_fOuterRadius = 0.01f;

	if (a_fInnerRadius < 0.005f)
		a_fInnerRadius = 0.005f;

	if (a_fInnerRadius > a_fOuterRadius)
		std::swap(a_fInnerRadius, a_fOuterRadius);

	if (a_fHeight < 0.01f)
		a_fHeight = 0.01f;

	if (a_nSubdivisions < 3)
		a_nSubdivisions = 3;
	if (a_nSubdivisions > 360)
		a_nSubdivisions = 360;

	Release();
	Init();

	// Replace this with your code
	// vectors for holding separate circle vertices
	std::vector<vector3> outerTopVertexes;
	std::vector<vector3> outerBottomVertexes;
	std::vector<vector3> innerTopVertexes;
	std::vector<vector3> innerBottomVertexes;
	vector3 centerVertex(0, 0, 0);
	vector3 topVertex(0, a_fHeight, 0);
	float angle = (2 * PI) / a_nSubdivisions;
	// outer circle generation code
	for (int i = 0; i < a_nSubdivisions; i++)
	{
		float x1 = a_fOuterRadius * cos(angle * i);
		float z1 = a_fOuterRadius * sin(angle * i);
		vector3 bottomFirstVertex(x1, 0, z1);
		outerBottomVertexes.push_back(bottomFirstVertex);
		vector3 topFirstVertex(x1, a_fHeight, z1);
		outerTopVertexes.push_back(topFirstVertex);
		// no geometry here this time, easier to handle later
	}
	// inner circle generation code
	for (int i = 0; i < a_nSubdivisions; i++)
	{
		float x1 = a_fInnerRadius * cos(angle * i);
		float z1 = a_fInnerRadius * sin(angle * i);
		vector3 bottomFirstVertex(x1, 0, z1);
		innerBottomVertexes.push_back(bottomFirstVertex);
		vector3 topFirstVertex(x1, a_fHeight, z1);
		innerTopVertexes.push_back(topFirstVertex);
		// no geometry here this time, easier to handle later
	}

	// connect inner bottom -> outer bottom   (bottom ring)
	// connect outer bottom -> outer top      (outside faces)
	// connect inner bottom -> inner top      (inside faces)
	// connect outer top -> inner top         (top ring)
	for (int i = 0; i < outerBottomVertexes.size(); i++)
	{
		if (i < outerBottomVertexes.size() - 1)
		{
			AddQuad(innerBottomVertexes[i + 1], innerBottomVertexes[i], outerBottomVertexes[i + 1], outerBottomVertexes[i]);
			AddQuad(outerBottomVertexes[i + 1], outerBottomVertexes[i], outerTopVertexes[i + 1], outerTopVertexes[i]);
			AddQuad(innerBottomVertexes[i], innerBottomVertexes[i + 1], innerTopVertexes[i], innerTopVertexes[i + 1]);
			AddQuad(outerTopVertexes[i + 1], outerTopVertexes[i], innerTopVertexes[i + 1], innerTopVertexes[i]);
		}
		// last shapes in the circle wrap back to the first vertices
		else
		{
			AddQuad(innerBottomVertexes[0], innerBottomVertexes[i], outerBottomVertexes[0], outerBottomVertexes[i]);
			AddQuad(outerBottomVertexes[0], outerBottomVertexes[i], outerTopVertexes[0], outerTopVertexes[i]);
			AddQuad(innerBottomVertexes[i], innerBottomVertexes[0], innerTopVertexes[i], innerTopVertexes[0]);
			AddQuad(outerTopVertexes[0], outerTopVertexes[i], innerTopVertexes[0], innerTopVertexes[i]);
		}
	}
	// -------------------------------

	// Adding information about color
	CompleteMesh(a_v3Color);
	CompileOpenGL3X();
}
void MyMesh::GenerateTorus(float a_fOuterRadius, float a_fInnerRadius, int a_nSubdivisionsA, int a_nSubdivisionsB, vector3 a_v3Color)
{
	if (a_fOuterRadius < 0.01f)
		a_fOuterRadius = 0.01f;

	if (a_fInnerRadius < 0.005f)
		a_fInnerRadius = 0.005f;

	if (a_fInnerRadius > a_fOuterRadius)
		std::swap(a_fInnerRadius, a_fOuterRadius);

	if (a_nSubdivisionsA < 3)
		a_nSubdivisionsA = 3;
	if (a_nSubdivisionsA > 360)
		a_nSubdivisionsA = 360;

	if (a_nSubdivisionsB < 3)
		a_nSubdivisionsB = 3;
	if (a_nSubdivisionsB > 360)
		a_nSubdivisionsB = 360;

	Release();
	Init();

	// Replace this with your code
	GenerateCube(a_fOuterRadius * 2.0f, a_v3Color);
	// -------------------------------

	// Adding information about color
	CompleteMesh(a_v3Color);
	CompileOpenGL3X();
}
void MyMesh::GenerateSphere(float a_fRadius, int a_nSubdivisions, vector3 a_v3Color)
{
	if (a_fRadius < 0.01f)
		a_fRadius = 0.01f;

	//Sets minimum and maximum of subdivisions
	if (a_nSubdivisions < 1)
	{
		GenerateCube(a_fRadius * 2.0f, a_v3Color);
		return;
	}
	if (a_nSubdivisions > 6)
		a_nSubdivisions = 6;

	Release();
	Init();

	// Replace this with your code

	
	glm::vec3 origin = vector3(0.0f, 0.0f, a_fRadius);
	glm::vec3 bottomVertex = vector3(0.0f, 0.0f, 0.0f);
	glm::vec3 topVertex = vector3(0.0f, 0.0f, 2 * a_fRadius);
	// vertical space between differential circles
	float circleSpacing = (2 * a_fRadius / 8);;
	float angle = (2 * PI) / a_nSubdivisions;
	// vectors for storing differential circle vertices
	std::vector<vector3> centerCircle;
	std::vector<vector3> topCircle1;
	std::vector<vector3> topCircle2;
	std::vector<vector3> topCircle3;
	std::vector<vector3> bottomCircle1;
	std::vector<vector3> bottomCircle2;
	std::vector<vector3> bottomCircle3;

	// Generates 7 differential circles of a sphere
	// I tried to come up with a way to make the number differentials match subdivisions, but it ended up being too dificult to calculate ratio for each differential
	// and I didn't have time to write a case for an even number of subdivisions
	for (int i = 0; i < a_nSubdivisions; i++)
	{
		centerCircle.push_back(vector3(a_fRadius * cos(i * angle), a_fRadius * sin(i * angle), a_fRadius));

		float dif1 = (9 * a_fRadius / 10); // Two circles closest to center circle
		topCircle1.push_back(vector3(dif1 * cos(i * angle), dif1 * sin(i * angle), circleSpacing * 5.5));
		bottomCircle1.push_back(vector3(dif1 * cos(i * angle), dif1 * sin(i * angle), circleSpacing * 2.5));

		float dif2 = (5 * a_fRadius / 7);
		topCircle2.push_back(vector3(dif2 * cos(i * angle), dif2 * sin(i * angle), circleSpacing * 6.5));
		bottomCircle2.push_back(vector3(dif2 * cos(i * angle), dif2 * sin(i * angle), circleSpacing * 1.5));

		float dif3 = (2 * a_fRadius / 5); // Two circles closest to ends of sphere
		topCircle3.push_back(vector3(dif3 * cos(i * angle), dif3 * sin(i * angle), circleSpacing * 7.5));
		bottomCircle3.push_back(vector3(dif3 * cos(i * angle), dif3 * sin(i * angle), circleSpacing * 0.5));
	}

	// generating geometry
	for (int i = 0; i < a_nSubdivisions; i++)
	{
		if (i < a_nSubdivisions - 1)
		{
			// top and bottom caps
			AddTri(topCircle3[i], topCircle3[i + 1], topVertex);
			AddTri(bottomCircle3[i + 1], bottomCircle3[i], bottomVertex);

			// quads between differential circles
			AddQuad(bottomCircle1[i], bottomCircle1[i + 1], centerCircle[i], centerCircle[i + 1]);
			AddQuad(centerCircle[i], centerCircle[i + 1], topCircle1[i], topCircle1[i + 1]);

			AddQuad(bottomCircle2[i], bottomCircle2[i + 1], bottomCircle1[i], bottomCircle1[i + 1]);
			AddQuad(topCircle1[i], topCircle1[i + 1], topCircle2[i], topCircle2[i + 1]);

			AddQuad(bottomCircle3[i], bottomCircle3[i + 1], bottomCircle2[i], bottomCircle2[i + 1]);
			AddQuad(topCircle2[i], topCircle2[i + 1], topCircle3[i], topCircle3[i + 1]);
		}
		// last shapes in the circle wrap back to the first vertices
		else
		{
			// top and bottom caps
			AddTri(topCircle3[i], topCircle3[0], topVertex);
			AddTri(bottomCircle3[0], bottomCircle3[i], bottomVertex);

			// quads between differential circles
			AddQuad(bottomCircle1[i], bottomCircle1[0], centerCircle[i], centerCircle[0]);
			AddQuad(centerCircle[i], centerCircle[0], topCircle1[i], topCircle1[0]);

			AddQuad(bottomCircle2[i], bottomCircle2[0], bottomCircle1[i], bottomCircle1[0]);
			AddQuad(topCircle1[i], topCircle1[0], topCircle2[i], topCircle2[0]);

			AddQuad(bottomCircle3[i], bottomCircle3[0], bottomCircle2[i], bottomCircle2[0]);
			AddQuad(topCircle2[i], topCircle2[0], topCircle3[i], topCircle3[0]);
		}
	}
	// -------------------------------

	// Adding information about color
	CompleteMesh(a_v3Color);
	CompileOpenGL3X();
}