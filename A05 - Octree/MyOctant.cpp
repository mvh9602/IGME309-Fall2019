#include "MyOctantExample.h"
using namespace Simplex;

// non-init() variables
uint MyOctant::m_uOctantCount = 0;
uint MyOctant::m_uMaxLevel = 3;
uint MyOctant::m_uIdealEntityCount = 5;

// PUBLIC METHODS
// Constructor
MyOctant::MyOctant(uint a_MaxLevel, uint a_IdealEntity)
{
	// set up all defaults
	Init();
	m_uOctantCount = 0;
	m_uMaxLevel = a_MaxLevel;
	m_uIdealEntityCount = a_IdealEntity;
	m_uID = m_uOctantCount;

	// start as root and clear all children
	m_pRoot = this;
	m_lChild.clear();

	// vector for mins and maxes as max,min
	std::vector<vector3> minMaxVector;

	// number of objects in scene
	int objectCount = m_pEntityMngr->GetEntityCount();

	// find mins and maxes by looping through all objects
	for (int i = 0; i < objectCount; i++)
	{
		// find entity, then rigidbody
		MyRigidBody* rb = m_pEntityMngr->GetEntity(i)->GetRigidBody();

		// add the max then min to vector
		minMaxVector.push_back(rb->GetMaxGlobal());
		minMaxVector.push_back(rb->GetMinGlobal());
	}

	// use half-widths to determine the max
	MyRigidBody* rigidBody = new MyRigidBody(minMaxVector);
	vector3 halfWidth = rigidBody->GetHalfWidth();
	float max = halfWidth.x;
	for (int i = 1; i < 3; i++)
	{
		if (max < halfWidth[i])
		{
			max = halfWidth[i];
		}
	}

	// clear vector now that we've found the max
	minMaxVector.clear();

	// setting values since checks are done
	m_v3Center = rigidBody->GetCenterLocal();
	m_fSize = max * 2.0f;
	m_v3Min = m_v3Center - (vector3(max));
	m_v3Max = m_v3Center + (vector3(max));
	m_uOctantCount++;

	// construct the octree
	ConstructTree(m_uMaxLevel);

	// clean up
	SafeDelete(rigidBody);
}

MyOctant::MyOctant(vector3 a_v3Center, float a_fSize)
{
	// initalize defaults
	Init();
	m_v3Center = a_v3Center;
	m_fSize = a_fSize;
	m_v3Max = m_v3Center + (vector3(m_fSize) / 2.0f);
	m_v3Min = m_v3Center - (vector3(m_fSize) / 2.0f);

	m_uOctantCount++;
}

// copy constructor, copy assignment operator, deconstructor
MyOctant::MyOctant(MyOctant const& other)
{
	m_uID = other.m_uID;
	m_uLevel = other.m_uLevel;
	m_uChildren = other.m_uChildren;
	m_fSize = other.m_fSize;
	m_pMeshMngr = MeshManager::GetInstance();
	m_pEntityMngr = MyEntityManager::GetInstance();
	m_v3Center = other.m_v3Center;
	m_v3Min = other.m_v3Min;
	m_v3Max = other.m_v3Max;
	m_pParent = other.m_pParent;

	for (uint i = 0; i < 8; i++)
	{
		m_pChild[i] = other.m_pChild[i];
	}

	m_pRoot = other.m_pRoot;
	m_lChild = other.m_lChild;
}
MyOctant& MyOctant::operator=(MyOctant const& other)
{
	if (this != &other)
	{
		Release();
		Init();
		MyOctant temp(other);
		Swap(temp);
	}
	return *this;
}
MyOctant::~MyOctant() { Release(); }

// swap defaults into second octant
void MyOctant::Swap(MyOctant& other)
{
	std::swap(m_uID, other.m_uID);
	std::swap(m_uLevel, other.m_uLevel);
	std::swap(m_uChildren, other.m_uChildren);
	std::swap(m_fSize, other.m_fSize);

	m_pMeshMngr = MeshManager::GetInstance();
	m_pEntityMngr = MyEntityManager::GetInstance();

	std::swap(m_v3Center, other.m_v3Center);
	std::swap(m_v3Max, other.m_v3Max);
	std::swap(m_v3Min, other.m_v3Min);

	std::swap(m_pParent, other.m_pParent);
	for (uint i = 0; i < 8; i++)
	{
		std::swap(m_pChild[i], other.m_pChild[i]);
	}
	std::swap(m_pRoot, other.m_pRoot);
	std::swap(m_lChild, other.m_lChild);
}

// get-variable functions
float MyOctant::GetSize(void) { return m_fSize; }
vector3 MyOctant::GetCenterGlobal(void) { return m_v3Center; }
vector3 MyOctant::GetMinGlobal(void) { return m_v3Min; }
vector3 MyOctant::GetMaxGlobal(void) { return m_v3Max; }

// all singleton functions
bool MyOctant::IsColliding(uint a_uIndex)
{
	int objCount = m_pEntityMngr->GetEntityCount();

	// no collision if index is larger than number of objects
	if (a_uIndex >= objCount)
	{
		return false;
	}

	// AABB code
	MyRigidBody* rigidBody = m_pEntityMngr->GetEntity(a_uIndex)->GetRigidBody();
	vector3 globalMin = rigidBody->GetMinGlobal();
	vector3 globalMax = rigidBody->GetMaxGlobal();

	// X
	if (m_v3Max.x < globalMin.x)
	{
		return false;
	}
	if (m_v3Min.x > globalMax.x)
	{
		return false;
	}
	// Y
	if (m_v3Max.y < globalMin.y)
	{
		return false;
	}
	if (m_v3Min.y > globalMax.y)
	{
		return false;
	}
	// Z
	if (m_v3Max.z < globalMin.z)
	{
		return false;
	}
	if (m_v3Min.z > globalMax.z)
	{
		return false;
	}

	return true;
}
void MyOctant::Display(uint a_Index, vector3 a_v3Color)
{
	if (m_uID == a_Index)
	{
		m_pMeshMngr->AddWireCubeToRenderList(glm::translate(IDENTITY_M4, m_v3Center) * glm::scale(vector3(m_fSize)), a_v3Color, RENDER_WIRE);
	}

	else
	{
		for (int i = 0; i < m_uChildren; i++)
		{
			m_pChild[i]->Display(a_Index);
		}
	}
}
void MyOctant::Display(vector3 a_v3Color)
{
	for (int i = 0; i < m_uChildren; i++)
	{
		m_pChild[i]->Display(a_v3Color);
	}
	m_pMeshMngr->AddWireCubeToRenderList(glm::translate(IDENTITY_M4, m_v3Center) * glm::scale(vector3(m_fSize)), a_v3Color, RENDER_WIRE);
}
void MyOctant::DisplayLeafs(vector3 a_v3Color)
{
	int leaves = m_lChild.size();

	for (int i = 0; i < leaves; i++)
	{
		m_lChild[i]->DisplayLeafs(a_v3Color);
	}

	m_pMeshMngr->AddWireCubeToRenderList(glm::translate(IDENTITY_M4, m_v3Center) * glm::scale(vector3(m_fSize)), a_v3Color, RENDER_WIRE);
}
void MyOctant::ClearEntityList(void)
{
	for (int i = 0; i < m_uChildren; i++)
	{
		m_pChild[i]->ClearEntityList();
	}

	m_EntityList.clear();
}
void MyOctant::Subdivide(void)
{
	// return if already at max
	if (m_uLevel >= m_uMaxLevel) { return; }

	// return if children have been divided already
	if (m_uChildren != 0) { return; }

	m_uChildren = 8;
	float size = m_fSize / 4.0f;
	float doubleSize = size * 2.0f;
	vector3 center;

	// octants created by moving center in the order drawn below
	//
	//			5-----6
	//		   /|    /|
	//		  4-+---7 |
	//		  | 0---+-1
	//		  |/    |/
	//		  3-----2

	// octant 0
	center = m_v3Center;
	center.x -= size;
	center.y -= size;
	center.z -= size;
	m_pChild[0] = new MyOctant(center, doubleSize);

	// octant 1
	center.x += doubleSize;
	m_pChild[1] = new MyOctant(center, doubleSize);

	// octant 2
	center.z += doubleSize;
	m_pChild[2] = new MyOctant(center, doubleSize);

	// octant 3
	center.x -= doubleSize;
	m_pChild[3] = new MyOctant(center, doubleSize);

	// octant 4
	center.y += doubleSize;
	m_pChild[4] = new MyOctant(center, doubleSize);

	// octant 5
	center.z -= doubleSize;
	m_pChild[5] = new MyOctant(center, doubleSize);

	// octant 6
	center.x += doubleSize;
	m_pChild[6] = new MyOctant(center, doubleSize);

	// octant 7
	center.z += doubleSize;
	m_pChild[7] = new MyOctant(center, doubleSize);

	// loop through children and set new level, parent, and root
	for (int i = 0; i < 8; i++)
	{
		m_pChild[i]->m_uLevel = m_uLevel + 1;
		m_pChild[i]->m_pParent = this;
		m_pChild[i]->m_pRoot = m_pRoot;

		// subdivide again if theres too many objects
		if (m_pChild[i]->ContainsMoreThan(m_uIdealEntityCount))
		{
			m_pChild[i]->Subdivide();
		}
	}

}
MyOctant* MyOctant::GetChild(uint a_child)
{
	if (a_child > 7) 
	{ 
		return nullptr; 
	
	}
	return m_pChild[a_child];
}
MyOctant* MyOctant::GetParent(void) { return m_pParent; }

bool MyOctant::IsLeaf(void) { return m_uChildren == 0; }
bool MyOctant::ContainsMoreThan(uint a_Entities)
{
	int count = 0;
	int objCount = m_pEntityMngr->GetEntityCount();

	for (int i = 0; i < objCount; i++)
	{
		if (IsColliding(i)) 
		{ 
			count++; 
		}
		if (count > a_Entities) 
		{ 
			return true; 
		}
	}

	return false;
}
void MyOctant::KillBranches(void)
{
	// recursively deletes children, moving backwards from end of tree to the root
	for (int i = 0; i < m_uChildren; i++)
	{
		m_pChild[i]->KillBranches();
		delete m_pChild[i];
		m_pChild[i] = nullptr;
	}

	m_uChildren = 0;
}
void MyOctant::ConstructTree(uint a_MaxLevel)
{
	// return if not the root
	if (m_uLevel != 0) 
	{ 
		return; 
	}

	m_uMaxLevel = a_MaxLevel;
	m_uOctantCount = 1;

	m_EntityList.clear();
	KillBranches();
	m_lChild.clear();

	if (ContainsMoreThan(m_uIdealEntityCount))
	{
		Subdivide();
	}

	AssignIDtoEntity();
	ConstructList();
}
void MyOctant::AssignIDtoEntity(void)
{
	// recursively assign to all branches
	for (int i = 0; i < m_uChildren; i++)
	{
		m_pChild[i]->AssignIDtoEntity();
	}

	if (m_uChildren == 0)
	{
		int entitieCount = m_pEntityMngr->GetEntityCount();
		for (int i = 0; i < entitieCount; i++)
		{
			if (IsColliding(i))
			{
				m_EntityList.push_back(i);
				m_pEntityMngr->AddDimension(i, m_uID);
			}
		}
	}
}
uint MyOctant::GetOctantCount(void) { return m_uOctantCount; }

// PRIVATE METHODS
// deconstructor but with simplex name
void MyOctant::Release(void)
{
	if (m_uLevel == 0)
	{
		KillBranches();
	}

	// reset variables
	m_uChildren = 0;
	m_fSize = 0.0f;
	m_EntityList.clear();
	m_lChild.clear();
}

// initialization for defaults
void MyOctant::Init(void)
{
	m_uID = m_uOctantCount;
	m_uLevel = 0;
	m_uChildren = 0;
	m_fSize = 0.0f;

	// managers from scene
	m_pMeshMngr = MeshManager::GetInstance();
	m_pEntityMngr = MyEntityManager::GetInstance();

	m_v3Center = vector3(0.0f);
	m_v3Max = vector3(0.0f);
	m_v3Min = vector3(0.0f);

	m_pParent = nullptr;
	for (uint i = 0; i < 8; i++)
	{
		m_pChild[i] = nullptr;
	}
	m_pRoot = nullptr;
}

void MyOctant::ConstructList(void)
{
	// recursively construct list for all branches until back to root
	for (uint i = 0; i < m_uChildren; i++)
	{
		m_pChild[i]->ConstructList();
	}

	if (m_EntityList.size() > 0)
	{
		m_pRoot->m_lChild.push_back(this);
	}
}