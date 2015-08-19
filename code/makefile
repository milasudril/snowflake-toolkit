all: __wand_targets/io_scene_snowflakecrystalprototype.zip __wand_targets/mesh_faces_tag.zip __wand_targets
.PHONY: __wand_targets
.PHONY: dox
__wand_targets:
	wand 'profile[release]'

__wand_targets/io_scene_snowflakecrystalprototype.zip: io_scene_snowflakecrystalprototype/__init__.py | __wand_targets
	rm -f __wand_targets/io_scene_snowflakecrystalprototype.zip
	zip __wand_targets/io_scene_snowflakecrystalprototype.zip io_scene_snowflakecrystalprototype/*.py
	rm -rf __wand_targets/io_scene_snowflakecrystalprototype

__wand_targets/mesh_faces_tag.zip: mesh_faces_tag/__init__.py | __wand_targets
	rm -f __wand_targets/mesh_faces_tag.zip
	zip __wand_targets/mesh_faces_tag.zip mesh_faces_tag/*.py
	rm -rf __wand_targets/mesh_faces_tag

dox:
	rm -rf html
	rm -rf latex
	doxygen

clean:
	rm -rf __wand_targets
	rm -rf html
	rm -rf latex
