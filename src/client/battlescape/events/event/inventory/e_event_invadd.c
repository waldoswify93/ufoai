/**
 * @file e_event_invadd.c
 */

/*
Copyright (C) 2002-2009 UFO: Alien Invasion.

This program is free software; you can redistribute it and/or
modify it under the terms of the GNU General Public License
as published by the Free Software Foundation; either version 2
of the License, or (at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.

See the GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program; if not, write to the Free Software
Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.

*/

#include "../../../../client.h"
#include "../../../cl_localentity.h"
#include "../../../../cl_inventory.h"
#include "e_event_invadd.h"

/**
 * @sa G_WriteItem
 * @sa G_ReadItem
 * @note The amount of the item_t struct should not be needed here - because
 * the amount is only valid for idFloor and idEquip
 */
static void CL_NetReceiveItem (struct dbuffer *buf, item_t *item, int *container, int *x, int *y)
{
	const eventRegister_t *eventData = CL_GetEvent(EV_INV_TRANSFER);

	/* reset */
	int t, m;
	item->t = item->m = NULL;
	item->a = NONE_AMMO;
	NET_ReadFormat(buf, eventData->formatString, &t, &item->a, &m, container, x, y, &item->rotated, &item->amount);

	item->t = INVSH_GetItemByIDX(t);
	item->m = INVSH_GetItemByIDX(m);
}

/**
 * @sa CL_InvDel
 * @sa G_SendInventory
 * @sa EV_INV_ADD
 */
void CL_InvAdd (const eventRegister_t *self, struct dbuffer *msg)
{
	int container, x, y;
	item_t item;
	const int number = NET_ReadShort(msg);
	le_t *le = LE_Get(number);
	int nr = NET_ReadShort(msg) / INV_INVENTORY_BYTES;

	if (!le)
		Com_Error(ERR_DROP, "InvAdd message ignored... LE not found\n");

	le->removeNextFrame = qfalse;

	for (; nr-- > 0;) {
		CL_NetReceiveItem(msg, &item, &container, &x, &y);
		Com_AddToInventory(&le->i, item, &csi.ids[container], x, y, item.amount);

		if (container == csi.idRight)
			le->right = item.t->idx;
		else if (container == csi.idLeft)
			le->left = item.t->idx;
		else if (container == csi.idExtension)
			le->extension = item.t->idx;
		else if (container == csi.idHeadgear)
			le->headgear = item.t->idx;
	}

	switch (le->type) {
	case ET_ACTOR:
	case ET_ACTOR2x2:
		LE_SetThink(le, LET_StartIdle);
		break;
	case ET_ITEM:
		LE_PlaceItem(le);
		break;
	}
}

