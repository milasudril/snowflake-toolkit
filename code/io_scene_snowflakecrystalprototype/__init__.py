# ##### BEGIN GPL LICENSE BLOCK #####
#
#  This program is free software; you can redistribute it and/or
#  modify it under the terms of the GNU General Public License
#  as published by the Free Software Foundation; either version 2
#  of the License, or (at your option) any later version.
#
#  This program is distributed in the hope that it will be useful,
#  but WITHOUT ANY WARRANTY; without even the implied warranty of
#  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
#  GNU General Public License for more details.
#
#  You should have received a copy of the GNU General Public License
#  along with this program; if not, write to the Free Software Foundation,
#  Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301, USA.
#
# ##### END GPL LICENSE BLOCK #####

# ##### Conditional license block #####
#
#  This script has been written to be used as a Blender plugin, and thus, it needs
#  to be licensed as a derived work of Blender. Only in the case anyone legally 
#  reimplements the plugin API defined by Blender (see Oracle vs Google case), and
#  the API used is accepted as a general purpose API for interacting with 3D softwar,
#  you may, in addition to the license required by Blender, also use this file under
#  the following terms
#
#  Copyright (c) 2015, Torbjörn Rathsman
#  All rights reserved.
#
#  Redistribution and use in source and binary forms, with or without modification,
#  are permitted provided that the following conditions are met:
#
#   1. Redistributions of source code must retain the above copyright notice, this 
#      list of conditions and the following disclaimer.
#
#   2. Redistributions in binary form must reproduce the above copyright notice, 
#      this list of conditions and the following disclaimer in the documentation 
#      and/or other materials provided with the distribution.
#
#   3. Neither the name of the copyright holder nor the names of its contributors
#      may be used to endorse or promote products derived from this software without
#      specific prior written permission.
#
# THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
# ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
# WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED.
# IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, 
# INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, 
# BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, 
# DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF
# LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE 
# OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED
# OF THE POSSIBILITY OF SUCH DAMAGE.
#
# ##### END ADDITIONAL PERMISSIONS #####

bl_info = {
	"name": "SnowflakeModel toolkit Prototype Crystal Exporter",
	"author": "Torbjörn Rathsman",
	"version": (1, 0, 0),
	"blender": (2, 74, 0),
	"location": "File > Import-Export",
	"description": "Exports all objects as Prototype Cystal file for the SnowflakeModel toolkit",
	"category": "Import-Export"};

if "bpy" in locals():
	import importlib;
	if "exporter" in locals():
		importlib.reload(exporter);

import bpy;
from bpy_extras.io_utils import ExportHelper;
from collections import defaultdict;

class SnowflakeExporter(bpy.types.Operator,ExportHelper):
	bl_idname="export_snowflakeproto.fmt";
	bl_label="Export";
	bl_options={"PRESET"};
	filename_ext=".ice";

	def execute(self,context):
		result={"FINISHED"};
		file_out = open(self.filepath, "w");
		file_out.write("#SnowflakeModel Ice crystal Prototype\n\n");
		file_out.write("#Scaling matrices\n\n");
		file_out.write(bpy.data.texts["matrices"].as_string());
		
		objects=bpy.context.scene.objects;
		file_out.write("\n\n#Mesh subvolumes. Each of these needs to be convex\n\n");

		for item in objects:
			if item.type=="MESH":
				file_out.write("volume()\n");
				mesh=item.data;
				group_dict=defaultdict(list);
				for vertex in mesh.vertices:
					file_out.write("\tvertex(%.7g,%.7g,%.7g)\n"%(vertex.co.x,vertex.co.y,vertex.co.z));
					for group in vertex.groups:
						group_dict[group.group].append(vertex.index);
				file_out.write("\n");
				
				for face in mesh.polygons:
					file_out.write("\tface(");
					for vert in face.vertices:
						file_out.write('%d,'%vert);
						vertex_first=False;
					if item.get("face_tags") is None:
						file_out.write("outside")
					else:
						file_out.write(item["face_tags"].get(str(face.index),"inside"))
					 
					file_out.write(")\n");
				file_out.write("\n");
				
				for vgroup,vertex_list in group_dict.items():
					file_out.write("\tgroup(%s"%item.vertex_groups[vgroup].name);
					for vertex in vertex_list:
						file_out.write(",%d"%vertex);
					file_out.write(")\n");		
		return result;

def menu_func(self,context):
	self.layout.operator(SnowflakeExporter.bl_idname, text="SnowflakeModel Prototype Crystal");

def register():
	bpy.utils.register_module(__name__);
	bpy.types.INFO_MT_file_export.append(menu_func);

def unregister():
	bpy.utils.unregister_module(__name__);
	bpy.types.INFO_MT_file_export.remove(menu_func);

if __name__ == "__main__":
	register();
