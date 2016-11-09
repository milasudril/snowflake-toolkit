function coords=mesh_rasterize(ice_file,grid,output_file,exepath,exename)
% function coords=mesh_rasterize(ice_file,grid,output_file,exepath,exename)
%
% Mesh rasterizer
%
% This function rasterizes ice_file, using defined grid [Nx,Ny,Nz]. For rasterizing of a
% sphere aggregate in a similar way, use sphere_aggregate_rasterize instead.
%
% IN
%	ice_file	File to rasterize
%	grid	The number of raster points in each direction. Zero can be used to infer a value
%		from the other values.
%	output_file	File to write the raster data to
%	exepath	The directory containeing the raseterizer executable
%	exename The name of the rasterizer executable
%
% OUT
%	coords a (Nx x Ny x Nz) x 3 matrix containing all raster points, in x, y, z order.
%
	prototype=['--prototype=',ice_file];
	sample_geometry='--sample-geometry=[';
	for k=1:numel(grid)
		sample_geometry=[sample_geometry,num2str(grid(k)),','];
	end
	sample_geometry=[sample_geometry,output_file,']'];

	n=nargin();
	cmd='';
	switch nargin()
		case 1
			cmd='snowflake_prototype-test';
		case 2
			cmd='snowflake_prototype-test';
		case 3
			cmd='snowflake_prototype-test';
		case 4
			cmd=[exepath,'/snowflake_prototype-test'];
		otherwise
			cmd=[exepath,'/',exefile];
	end

	system_wrapper({cmd,prototype,sample_geometry},nargout()>0);
	if nargout()>0
		tmp=importdata(output_file,' ');
		coords=tmp.data;
	end
end
