#include "m_nodes.h"
#include "m_parse.h"

/**
 */
void MN_DrawTBarNode (menuNode_t *node)
{
	/* dataStringOrImageOrModel is the texture name */
	float ps, shx;
	vec2_t nodepos;
	menu_t *menu = node->menu;
	const char* ref = MN_GetSaifeReferenceString(node->menu, node->dataStringOrImageOrModel);
	if (!ref) {
		return;
	}
	MN_GetNodeAbsPos(node, nodepos);

	if (node->pointWidth) {
		ps = MN_GetReferenceFloat(menu, node->abstractvalue.value) - MN_GetReferenceFloat(menu, node->abstractvalue.min);
		shx = node->texl[0] + round(ps * node->pointWidth) + (ps > 0 ? floor((ps - 1) / 10) * node->gapWidth : 0);
	} else
		shx = node->texh[0];
	R_DrawNormPic(nodepos[0], nodepos[1], node->size[0], node->size[1],
		shx, node->texh[1], node->texl[0], node->texl[1], node->align, node->blend, ref);
}

void MN_RegisterNodeTBar (nodeBehaviour_t *behaviour)
{
	behaviour->name = "tbar";
	behaviour->draw = MN_DrawTBarNode;
}
