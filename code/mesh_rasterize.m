function coords=mesh_rasterize(ice_file,grid,output_file,exepath,exename)
% function coords=mesh_rasterize(ice_file,grid,output_file,exepath,exename)
%
% Rasterizes ice_file, using defined grid [Nx,Ny,Nz]
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
		coords=load(output_file);
	end
end
