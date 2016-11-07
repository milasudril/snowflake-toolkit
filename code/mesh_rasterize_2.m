function coords=mesh_rasterize_2(ice_file,N,ratio,output_file,exepath,exename)
% function coords=mesh_rasterize_2(ice_file,N,ratio,output_file,exepath,exename)
%
% Rasterizes ice_file, using N points, with ratio [Rx,Ry,Rz]. For rasterizing a
% sphere aggregate in a similar way, use sphere_aggregate_rasterize_2 instead.
%
% IN
%	ice_file	File to rasterize
%	N	The number of raster points. The actual number M, of output points may, be less
%		than, or greater than, this value, depending on the geometry.
%	ratio	A vector of three elements with the aspect ratio of the grid. The values
%		are normailzed automatically
%	output_file	File to write the raster data to
%	exepath	The directory containeing the raseterizer executable
%	exename The name of the rasterizer executable
%
% OUT
%	coords a M x 3 matrix containing all raster points, in x, y, z order.
%
	prototype=['--prototype=',ice_file];
	sample_geometry=['--sample-geometry-2=[',num2str(N),','];
	for k=1:numel(ratio)
		sample_geometry=[sample_geometry,num2str(ratio(k))...
			,ternary(@()(k==numel(ratio)), @()(','),@()(':')) ];
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
			cmd='snowflake_prototype-test';
		case 5
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
