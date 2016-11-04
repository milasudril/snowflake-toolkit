function coords=sphere_aggregate_rasterize(aggregate,grid,output_file,exepath,exename)
% function coords=mesh_rasterize(aggregate,grid,output_file,exepath,exename)
%
% Rasterizes aggregate, using defined grid [Nx,Ny,Nz]
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
