/*
 * ***** BEGIN GPL LICENSE BLOCK *****
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 2
 * of the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software Foundation,
 * Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301, USA.
 *
 * The Original Code is Copyright (C) 2016 Blender Foundation.
 * All rights reserved.
 *
 * Contributor(s): none yet.
 *
 * ***** END GPL LICENSE BLOCK *****
 */

/** \file BKE_layer.h
 *  \ingroup bke
 */

#ifndef __BKE_LAYER_H__
#define __BKE_LAYER_H__

struct bContext;
struct uiLayout;

#include "DNA_space_types.h"


/* -------------------------------------------------------------------- */
/* Layer Tree */

typedef bool (*LayerTreeIterFunc)(LayerTreeItem *, void *);

/**
 * LayerTree.type
 * Defines the type used for the layer tree.
 */
typedef enum eLayerTree_Type {
	LAYER_TREETYPE_OBJECT,
//	LAYER_TREETYPE_GPENCIL,
//	LAYER_TREETYPE_ARMATURE,
//	...
} eLayerTree_Type;

LayerTree *BKE_layertree_new(const eLayerTree_Type type);
LayerTree *BKE_layertree_copy(const LayerTree *original_tree);
void BKE_layertree_delete(LayerTree *ltree);

bool BKE_layertree_iterate(const LayerTree *ltree, LayerTreeIterFunc foreach, void *customdata, const bool inverse);
int  BKE_layertree_get_totitems(const LayerTree *ltree);

/**
 * Macro to iterate over all layer items of a tree.
 * Don't call #BKE_layeritem_remove inside, it will mess up iteration.
 */
#define BKE_LAYERTREE_ITER_START(ltree, start_at, idx_name, litem_name) \
	for (int idx_name = start_at; idx_name < BKE_layertree_get_totitems(ltree); idx_name++) { \
		LayerTreeItem *litem_name = ltree->items_all[idx_name];
#define BKE_LAYERTREE_ITER_END } (void)0

/* -------------------------------------------------------------------- */
/* Layer Types */

typedef enum eLayerTreeItem_Type {
	LAYER_ITEMTYPE_LAYER = 0,
	LAYER_ITEMTYPE_GROUP, /* layer group */
	LAYER_ITEMTYPE_COMP,  /* compositing layer (wireframes, SSAO, blending type, etc) */

	LAYER_ITEMTYPE_TOT,   /* always last! */
} eLayerTreeItem_Type;

typedef struct LayerType {
	/* idname is needed to support reading custom (.py defined) layers. During normal runtime
	 * using type below should be preferred though, avoids slow string comparisons */
	const char *idname;
	eLayerTreeItem_Type type;

	/* drawing of the item in the list */
	void (*draw)(const struct bContext *, struct LayerTreeItem *, struct uiLayout *); /* LayerItemDrawFunc */
	/* drawing of the expanded layer settings (gear wheel icon) */
	void (*draw_settings)(const struct bContext *, struct LayerTreeItem *, struct uiLayout *); /* LayerItemDrawSettingsFunc */

	/* Optional callback called when duplicating a layer */
	void (*copy)(struct LayerTreeItem *copied_item, const struct LayerTreeItem *original_item);
	/* Optional free callback. Don't free item itself! */
	void (*free)(struct LayerTreeItem *);

	/* rna for properties */
	struct StructRNA *srna;
} LayerType;

void BKE_layertype_append(void (*ltfunc)(LayerType *));
void BKE_layertypes_free(void);
LayerType *BKE_layertype_find(const char *idname);

/* -------------------------------------------------------------------- */
/* Layer Tree Item */

typedef void  (*LayerItemDrawFunc)(const struct bContext *, struct LayerTreeItem *, struct uiLayout *layout);
typedef void  (*LayerItemDrawSettingsFunc)(const struct bContext *, struct LayerTreeItem *, struct uiLayout *layout);

LayerTreeItem *BKE_layeritem_add(
        LayerTree *tree, LayerTreeItem *parent,
        const eLayerTreeItem_Type type, const char *name);
void BKE_layeritem_register(
        LayerTree *tree, LayerTreeItem *litem, LayerTreeItem *parent,
        const eLayerTreeItem_Type type, const char *name);
void BKE_layeritem_remove(LayerTreeItem *litem, const bool remove_children);

void BKE_layeritem_move(LayerTreeItem *litem, const int newidx, const bool with_childs);
void BKE_layeritem_group_assign(LayerTreeItem *group, LayerTreeItem *item);

bool BKE_layeritem_iterate_childs(
        LayerTreeItem *litem, LayerTreeIterFunc foreach, void *customdata,
        const bool inverse);

bool BKE_layeritem_is_visible(LayerTreeItem *litem);

#endif  /* __BKE_LAYER_H__ */
