function coords=mesh_rasterize_2(ice_file,n_points,ratio,output_file,exepath,exename)
% function coords=mesh_rasterize_2(ice_file,n_points,ratio,output_file,exepath,exename)
%
% Rasterizes ice_file, using N points, with ratio [Rx,Ry,Rz]
%
	prototype=['--prototype=',ice_file];
	sample_geometry=['--sample-geometry-2=[',num2str(n_points),','];
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
		coords=load(output_file);
	end
end
