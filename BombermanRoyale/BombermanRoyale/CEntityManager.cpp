#include "CEntityManager.h"

CObject CEntityManager::CreateOBJFromTemplate(OBJLoadInfo loadInfo)
{
	CObject temp = CObject();
	TTransformComponent transform = TTransformComponent(loadInfo.position, loadInfo.forwardVec);
	temp.AddComponent((TComponent*)&transform);

	TRendererComponent renderer = TRendererComponent(loadInfo.usedVertex, loadInfo.usedPixel, loadInfo.usedGeo, loadInfo.usedInput);
	temp.AddComponent((TComponent*)&renderer);

	TMeshComponent mesh = TMeshComponent(v_tMeshTemplates.at(loadInfo.meshID));
	temp.AddComponent((TComponent*)&mesh);

	TTextureComponent tex;
	tex.p_d3dUsedDiffuse = &loadInfo.usedDiffuse;
	tex.p_d3dUsedEmissive = nullptr;
	tex.p_d3dUsedNormal = nullptr;
	tex.p_d3dUsedSpecular = nullptr;
	temp.AddComponent((TComponent*)&tex);

	return temp;
}
