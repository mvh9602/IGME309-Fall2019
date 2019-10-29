#include "MyEntityManager.h"
using namespace Simplex;
//  MyEntityManager
MyEntityManager* MyEntityManager::m_pInstance = nullptr;
void MyEntityManager::Init(void)
{
}
void MyEntityManager::Release(void)
{
}
MyEntityManager* MyEntityManager::GetInstance()
{
	if (m_pInstance == nullptr)
	{
		m_pInstance = new MyEntityManager();
	}
	return m_pInstance;
}
void MyEntityManager::ReleaseInstance()
{
	if (m_pInstance != nullptr)
	{
		delete m_pInstance;
		m_pInstance = nullptr;
	}
}
int Simplex::MyEntityManager::GetEntityIndex(String a_sUniqueID)
{
	for (int i = 0; i < m_uEntityCount; i++)
	{
		if (a_sUniqueID == m_entityList[i]->GetUniqueID())
			return i;
	}
	//Only reaches this return if it was not found in vector
	return -1;
}
//Accessors
Model* Simplex::MyEntityManager::GetModel(uint a_uIndex)
{
	if (m_uEntityCount == 0)
	{
		return nullptr;
	}

	if (a_uIndex >= m_uEntityCount)
	{
		a_uIndex = m_uEntityCount - 1;
	}

	return m_entityList[a_uIndex]->GetModel();
}
Model* Simplex::MyEntityManager::GetModel(String a_sUniqueID)
{
	MyEntity* entity = MyEntity::GetEntity(a_sUniqueID);
	if (entity != nullptr)
	{
		return entity->GetModel();
	}
	return nullptr;
}
RigidBody* Simplex::MyEntityManager::GetRigidBody(uint a_uIndex)
{
	if (m_uEntityCount == 0)
	{
		return nullptr;
	}

	if (a_uIndex >= m_uEntityCount)
	{
		a_uIndex = m_uEntityCount - 1;
	}

	return m_entityList[a_uIndex]->GetRigidBody();
}
RigidBody* Simplex::MyEntityManager::GetRigidBody(String a_sUniqueID)
{
	MyEntity* entity = MyEntity::GetEntity(a_sUniqueID);
	if (entity != nullptr)
	{
		return entity->GetRigidBody();
	}
	return nullptr;
}
matrix4 Simplex::MyEntityManager::GetModelMatrix(uint a_uIndex)
{
	if (m_uEntityCount == 0)
	{
		return matrix4();
	}

	if (a_uIndex >= m_uEntityCount)
	{
		a_uIndex = m_uEntityCount - 1;
	}

	return m_entityList[a_uIndex]->GetModelMatrix();
}
matrix4 Simplex::MyEntityManager::GetModelMatrix(String a_sUniqueID)
{
	MyEntity* entity = MyEntity::GetEntity(a_sUniqueID);
	if (entity != nullptr)
	{
		return entity->GetModelMatrix();
	}
	return matrix4();
}
void Simplex::MyEntityManager::SetModelMatrix(matrix4 a_m4ToWorld, String a_sUniqueID)
{
	MyEntity* entity = MyEntity::GetEntity(a_sUniqueID);
	if (entity != nullptr)
	{
		entity->SetModelMatrix(a_m4ToWorld);
	}
}
void Simplex::MyEntityManager::SetModelMatrix(matrix4 a_m4ToWorld, uint a_uIndex)
{
	if (m_uEntityCount == 0)
	{
		return;
	}

	if (a_uIndex >= m_uEntityCount)
	{
		a_uIndex = m_uEntityCount - 1;
	}

	m_entityList[a_uIndex]->SetModelMatrix(a_m4ToWorld);
}
//The big 3
MyEntityManager::MyEntityManager(){Init();}
MyEntityManager::MyEntityManager(MyEntityManager const& other){ }
MyEntityManager& MyEntityManager::operator=(MyEntityManager const& other) { return *this; }
MyEntityManager::~MyEntityManager(){Release();};
// other methods
void Simplex::MyEntityManager::Update(void)
{
	m_uEntityCount = m_entityList.size();

	for (int i = 0; i < m_uEntityCount; i++)
	{
		m_entityList[i]->GetRigidBody()->ClearCollidingList();
	}

	for (int i = 0; i < m_uEntityCount - 1; i++)
	{
		for (int j = i + 1; j < m_uEntityCount; j++)
		{
			m_entityList[i]->IsColliding(m_entityList[j]);
		}
	}
}
void Simplex::MyEntityManager::AddEntity(String a_sFileName, String a_sUniqueID)
{
	MyEntity* entity = new MyEntity(a_sFileName, a_sUniqueID);
	if (entity->IsInitialized())
	{
		m_entityList.push_back(entity);
	}
}
void Simplex::MyEntityManager::RemoveEntity(uint a_uIndex)
{
	if (m_uEntityCount == 0)
	{
		return;
	}

	if (a_uIndex >= m_uEntityCount)
	{
		a_uIndex = m_uEntityCount - 1;
	}

	m_entityList.erase(m_entityList.begin() + a_uIndex - 1);
}
void Simplex::MyEntityManager::RemoveEntity(String a_sUniqueID)
{
	int index = GetEntityIndex(a_sUniqueID);
	RemoveEntity((uint)index);
}
String Simplex::MyEntityManager::GetUniqueID(uint a_uIndex)
{
	if (m_uEntityCount == 0)
	{
		return "";
	}

	if (a_uIndex >= m_uEntityCount)
	{
		a_uIndex = m_uEntityCount - 1;
	}

	return m_entityList[a_uIndex]->GetUniqueID();
}
MyEntity* Simplex::MyEntityManager::GetEntity(uint a_uIndex)
{
	if (m_uEntityCount == 0)
	{
		return nullptr;
	}

	if (a_uIndex >= m_uEntityCount)
	{
		a_uIndex = m_uEntityCount - 1;
	}

	return m_entityList[a_uIndex];
}
void Simplex::MyEntityManager::AddEntityToRenderList(uint a_uIndex, bool a_bRigidBody)
{
	if (m_uEntityCount == 0)
	{
		return;
	}

	if (a_uIndex >= m_uEntityCount)
	{
		a_uIndex = m_uEntityCount - 1;
	}

	m_entityList[a_uIndex]->AddToRenderList(a_bRigidBody);
}
void Simplex::MyEntityManager::AddEntityToRenderList(String a_sUniqueID, bool a_bRigidBody)
{
	MyEntity* entity = MyEntity::GetEntity(a_sUniqueID);
	if (entity != nullptr)
	{
		entity->AddToRenderList(a_bRigidBody);
	}
}