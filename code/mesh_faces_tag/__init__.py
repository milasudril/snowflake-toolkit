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
	"name": "Face tagger",
	"author": "Torbjörn Rathsman",
	"version": (1, 0, 0),
	"blender": (2, 74, 0),
	"description": "Makes it possible to assign a tag to faces",
	"category": "Mesh"};


import bpy
from bpy.props import StringProperty

class FaceTag(bpy.types.Operator):
	bl_label = "Face tag"
	bl_idname = "mesh.face_tag"
	bl_description = "Assign a tag to selected faces"
	bl_options = {"REGISTER","UNDO"}
	tag=StringProperty(name="Tag",description="Face tag")

	def execute(self, context):
		obj=bpy.context.scene.objects.active
		if obj.type!="MESH":
			self.report({"ERROR"},"Selected object is not a mesh")
			return {"ERROR"}
        
		obj.update_from_editmode()
		if obj.get("face_tags") is None:
			obj["face_tags"]={}
        
		for face in obj.data.polygons:
			if face.select:
				obj["face_tags"][str(face.index)]=self.tag
     
		return {"FINISHED"}
 

class FaceTagPanel(bpy.types.Panel):
	bl_label = "Face tag"
	bl_space_type = "VIEW_3D"
	bl_region_type = "TOOLS"
	bl_context = "mesh_edit"
 
	is_left = True
    
	def draw(self, context):
		obj=bpy.context.scene.objects.active
		if obj.type!="MESH":
			self.report({"ERROR"},"Selected object is not a mesh")
			return {"ERROR"}
		
		obj.update_from_editmode()
        
		layout = self.layout
		row = layout.row()
        
		if obj.get("face_tags") is None:
			row.operator("mesh.face_tag",text="Set tag").tag=""
		else:
			selected=obj.data.polygons.active
			val=obj["face_tags"].get(str(selected),"")
			row.operator("mesh.face_tag",text="Face tag: %s"%(val)).tag=val
            

 
def register():
    bpy.utils.register_module(__name__)
 
def unregister():
    bpy.utils.unregister_module(__name__)
 
if __name__ == "__main__":
    register()
