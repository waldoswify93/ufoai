/**
 * @file m_node_abstractvalue.c
 */

/*
Copyright (C) 2002-2007 UFO: Alien Invasion team.

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

#include "m_nodes.h"
#include "m_parse.h"
#include "m_node_abstractvalue.h"
#include "m_input.h"

static const value_t properties[] = {
	{"current", V_FLOAT|V_MENU_COPY, offsetof(menuNode_t, u.abstractvalue.value), 0},
	{"delta", V_FLOAT|V_MENU_COPY, offsetof(menuNode_t, u.abstractvalue.delta), 0},
	{"max", V_FLOAT|V_MENU_COPY, offsetof(menuNode_t, u.abstractvalue.max), 0},
	{"min", V_FLOAT|V_MENU_COPY, offsetof(menuNode_t, u.abstractvalue.min), 0},
	{"lastdiff", V_FLOAT, offsetof(menuNode_t, u.abstractvalue.lastdiff), MEMBER_SIZEOF(menuNode_t, u.abstractvalue.lastdiff)},
	{NULL, V_NULL, 0, 0}
};

void MN_RegisterAbstractValueNode (nodeBehaviour_t *behaviour)
{
	behaviour->properties = properties;
}
