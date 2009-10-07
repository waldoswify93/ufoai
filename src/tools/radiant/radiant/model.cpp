/*
 Copyright (C) 2001-2006, William Joseph.
 All Rights Reserved.

 This file is part of GtkRadiant.

 GtkRadiant is free software; you can redistribute it and/or modify
 it under the terms of the GNU General Public License as published by
 the Free Software Foundation; either version 2 of the License, or
 (at your option) any later version.

 GtkRadiant is distributed in the hope that it will be useful,
 but WITHOUT ANY WARRANTY; without even the implied warranty of
 MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 GNU General Public License for more details.

 You should have received a copy of the GNU General Public License
 along with GtkRadiant; if not, write to the Free Software
 Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA
 */

#include <stdio.h>
#include "picomodel.h"
typedef unsigned char byte;
#include <stdlib.h>
#include <algorithm>
#include <list>

#include "AutoPtr.h"
#include "iscenegraph.h"
#include "irender.h"
#include "iselection.h"
#include "iimage.h"
#include "imodel.h"
#include "igl.h"
#include "ifilesystem.h"
#include "iundo.h"
#include "ifiletypes.h"
#include "preferencesystem.h"
#include "stringio.h"
#include "iarchive.h"

#include "modulesystem/singletonmodule.h"
#include "stream/textstream.h"
#include "string/string.h"
#include "stream/stringstream.h"
#include "typesystem.h"

#include "commands.h"
#include "gtkutil/widget.h"
#include "picomodel/model.h"

bool g_showModelNormals = false;
bool g_showModelBoundingBoxes = false;

void pico_toggleShowModelNormals (void)
{
	g_showModelNormals ^= true;
	SceneChangeNotify();
}

void pico_toggleShowModelBoundingBoxes (void)
{
	g_showModelBoundingBoxes ^= true;
	SceneChangeNotify();
}

bool ShowModelNormals (void)
{
	return g_showModelNormals;
}

bool ShowModelBoundingBoxes (void)
{
	return g_showModelBoundingBoxes;
}

void ShowModelNormalsExport (const BoolImportCallback& importCallback)
{
	importCallback(g_showModelNormals);
}
void ShowModelBoundingBoxesExport (const BoolImportCallback& importCallback)
{
	importCallback(g_showModelBoundingBoxes);
}

typedef FreeCaller1<const BoolImportCallback&, ShowModelNormalsExport> ShowModelNormalsApplyCaller;
ShowModelNormalsApplyCaller g_showModelNormals_button_caller;
BoolExportCallback g_showModelNormals_button_callback (g_showModelNormals_button_caller);
ToggleItem g_showModelNormals_button (g_showModelNormals_button_callback);

typedef FreeCaller1<const BoolImportCallback&, ShowModelBoundingBoxesExport> ShowModelBoundingBoxApplyCaller;
ShowModelBoundingBoxApplyCaller g_showModelBB_button_caller;
BoolExportCallback g_showModelBB_button_callback (g_showModelBB_button_caller);
ToggleItem g_showModelBB_button (g_showModelBB_button_callback);

void Model_RegisterToggles (void)
{
	GlobalToggles_insert("ToggleShowModelBoundingBox", FreeCaller<pico_toggleShowModelBoundingBoxes> (),
			ToggleItem::AddCallbackCaller(g_showModelBB_button), Accelerator('B'));
	GlobalToggles_insert("ToggleShowModelNormals", FreeCaller<pico_toggleShowModelNormals> (),
			ToggleItem::AddCallbackCaller(g_showModelNormals_button), Accelerator('N'));
}

static void PicoPrintFunc (int level, const char *str)
{
	if (str == 0)
		return;
	switch (level) {
	case PICO_NORMAL:
		g_message("%s\n", str);
		break;

	case PICO_VERBOSE:
		g_message("PICO_VERBOSE: %s\n", str);
		break;

	case PICO_WARNING:
		g_warning("PICO_WARNING: %s\n", str);
		break;

	case PICO_ERROR:
		g_warning("PICO_ERROR: %s\n", str);
		break;

	case PICO_FATAL:
		g_critical("PICO_FATAL: %s\n", str);
		break;
	}
}

static void PicoLoadFileFunc (char *name, byte **buffer, int *bufSize)
{
	*bufSize = vfsLoadFile(name, (void**) buffer);
}

static void PicoFreeFileFunc (void* file)
{
	vfsFreeFile(file);
}

static void pico_initialise (void)
{
	PicoInit();
	PicoSetMallocFunc(malloc);
	PicoSetFreeFunc(free);
	PicoSetPrintFunc(PicoPrintFunc);
	PicoSetLoadFileFunc(PicoLoadFileFunc);
	PicoSetFreeFileFunc(PicoFreeFileFunc);
}

class PicoModelLoader: public ModelLoader
{
		const picoModule_t* m_module;
	public:
		PicoModelLoader (const picoModule_t* module) :
			m_module(module)
		{
		}
		scene::Node& loadModel (ArchiveFile& file)
		{
			return loadPicoModel(m_module, file);
		}

		// Load the given model from the VFS path
		model::IModelPtr loadModelFromPath (const std::string& name)
		{
			// Open an ArchiveFile to load
			AutoPtr<ArchiveFile> file(GlobalFileSystem().openFile(name));
			if (file) {
				// Load the model and return the RenderablePtr
				return loadIModel(m_module, *file);
			}
			return model::IModelPtr();
		}
};

class ModelPicoDependencies: public GlobalFileSystemModuleRef,
		public GlobalOpenGLModuleRef,
		public GlobalUndoModuleRef,
		public GlobalSceneGraphModuleRef,
		public GlobalShaderCacheModuleRef,
		public GlobalSelectionModuleRef,
		public GlobalFiletypesModuleRef,
		public GlobalPreferenceSystemModuleRef
{
};

class ModelPicoAPI: public TypeSystemRef
{
		PicoModelLoader m_modelLoader;
	public:
		typedef ModelLoader Type;

		ModelPicoAPI (const char* extension, const picoModule_t* module) :
			m_modelLoader(module)
		{
			StringOutputStream filter(128);
			filter << "*." << extension;
			GlobalFiletypesModule::getTable().addType(Type::Name(), extension, filetype_t(module->displayName,
					filter.c_str()));

			GlobalPreferenceSystem().registerPreference("ShowModelNormals", BoolImportStringCaller(g_showModelNormals),
					BoolExportStringCaller(g_showModelNormals));
			GlobalPreferenceSystem().registerPreference("ShowModelBoundingBoxes", BoolImportStringCaller(
					g_showModelBoundingBoxes), BoolExportStringCaller(g_showModelBoundingBoxes));
		}
		ModelLoader* getTable ()
		{
			return &m_modelLoader;
		}
};

class PicoModelAPIConstructor
{
		std::string m_extension;
		const picoModule_t* m_module;
	public:
		PicoModelAPIConstructor (const char* extension, const picoModule_t* module) :
			m_extension(extension), m_module(module)
		{
		}
		const char* getName ()
		{
			return m_extension.c_str();
		}
		ModelPicoAPI* constructAPI (ModelPicoDependencies& dependencies)
		{
			return new ModelPicoAPI(m_extension.c_str(), m_module);
		}
		void destroyAPI (ModelPicoAPI* api)
		{
			delete api;
		}
};

typedef SingletonModule<ModelPicoAPI, ModelPicoDependencies, PicoModelAPIConstructor> PicoModelModule;

void ModelModules_Init (void)
{
	pico_initialise();
	const picoModule_t** modules = PicoModuleList(0);
	while (*modules != 0) {
		const picoModule_t* module = *modules++;
		if (module->canload && module->load) {
			for (const char* const * ext = module->defaultExts; *ext != 0; ++ext) {
				PicoModelModule *picomodule = new PicoModelModule(PicoModelAPIConstructor(*ext, module));
				StaticModuleRegistryList().instance().addModule(*picomodule);
				/** @todo do we have to delete these PicoModelModules anywhere? */
			}
		}
	}
}
