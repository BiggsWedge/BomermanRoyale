#include "CEntityManager.h"

CObject* CEntityManager::CreateOBJFromTemplate(OBJLoadInfo loadInfo)
{
	CObject* temp = new CObject();
	TTransformComponent* transform = new TTransformComponent(loadInfo.position, loadInfo.forwardVec, loadInfo.scale);
	temp->AddComponent((TComponent*)transform);

	TRendererComponent* renderer = new TRendererComponent(loadInfo.usedVertex, loadInfo.usedPixel, loadInfo.usedInput, loadInfo.usedGeo, loadInfo.LoadState);
	temp->AddComponent((TComponent*)renderer);

	TMeshComponent* mesh = new TMeshComponent(v_tMeshTemplates.at(loadInfo.meshID));
	temp->AddComponent((TComponent*)mesh);

	TTextureComponent* tex = new TTextureComponent(loadInfo.usedDiffuse);
	temp->AddComponent((TComponent*)tex);

	return temp;
}
