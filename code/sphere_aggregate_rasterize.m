function coords=sphere_aggregate_rasterize(aggregate,grid,output_file,exepath,exename)
% function coords=mesh_rasterize(aggregate,grid,output_file,exepath,exename)
%
% Sphere aggregate rasterizer
%
% Rasterizes aggregate, using defined grid [Nx,Ny,Nz] For rasterizing of a
% mesh aggregate in a similar way, use mesh_rasterize instead.
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
	prototype=['--source=',aggregate];
	sample_geometry='--grid=[';
	for k=1:numel(grid)
		sample_geometry=[sample_geometry,num2str(grid(k)),','];
	end
	sample_geometry=[sample_geometry,output_file,']'];

	n=nargin();
	cmd='';
	switch nargin()
		case 1
			cmd='sphere_aggregate_rasterize';
		case 2
			cmd='sphere_aggregate_rasterize';
		case 3
			cmd='sphere_aggregate_rasterize';
		case 4
			cmd=[exepath,'/sphere_aggregate_rasterize'];
		otherwise
			cmd=[exepath,'/',exefile];
	end

	system_wrapper({cmd,prototype,sample_geometry},nargout()>0);
	if nargout()>0
		tmp=importdata(output_file,' ');
		coords=tmp.data;
	end
end
